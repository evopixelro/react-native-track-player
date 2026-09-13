// Native queue and AVPlayer command tests; run with npm run test:macos.
import Foundation
import AppKit
import MediaPlayer
import React

@main
struct PlayerTests {
    private static var checks = 0

    private static func expect(_ value: @autoclosure () -> Bool, _ message: String) {
        precondition(value(), message)
        checks += 1
    }

    private static func track(_ name: String) -> [String: Any] {
        return ["url": "https://example.com/\(name).mp3", "title": name]
    }

    private static func rejects(_ code: String, _ action: () throws -> Void) {
        do {
            try action()
            preconditionFailure("Expected \(code)")
        } catch let error as TrackPlayerError {
            expect(error.code == code, "Unexpected rejection: \(error.code)")
        } catch {
            preconditionFailure(error.localizedDescription)
        }
    }

    private static func queueChecks() throws {
        var queue = TrackQueue()
        expect(queue.current == nil && queue.index == nil, "Empty queue has no current track")
        _ = try queue.add([track("a"), track("b"), track("c")], before: -1)
        try queue.select(1)
        let identity = queue.current?.identity
        _ = try queue.add([track("inserted")], before: 0)
        expect(queue.index == 2 && queue.current?.identity == identity, "Insertion preserves current track")
        try queue.move(from: 2, to: 0)
        expect(queue.index == 0 && queue.current?.identity == identity, "Moving current track preserves identity")
        try queue.remove([1, 1, 2])
        expect(queue.tracks.count == 2 && queue.current?.identity == identity, "Duplicate removals are applied once")
        try queue.remove([0])
        expect(queue.current?.dictionary["title"] as? String == "c", "Removing current selects next survivor")
        let saved = queue.current?.identity
        rejects("invalid_track_object") { try queue.set([track("valid"), ["url": "javascript:bad"]]) }
        expect(queue.current?.identity == saved, "Invalid replacement is atomic")
        rejects("invalid_track_object") { _ = try queue.add([track("valid"), [:]], before: -1) }
        expect(queue.tracks.count == 1, "Invalid insertion is atomic")
        for invalid in [Double.nan, Double.infinity, -1, 0.5, Double.greatestFiniteMagnitude] {
            rejects("index_out_of_bounds") { try queue.select(invalid) }
        }
        rejects("index_out_of_bounds") { try queue.remove([0, 8]) }
        expect(queue.tracks.count == 1, "Invalid removal does not partially remove tracks")
        try queue.update(0, metadata: ["title": "Și музыка", "url": "https://other.example/changed"])
        expect(queue.current?.dictionary["title"] as? String == "Și музыка", "Unicode metadata survives")
        expect(queue.current?.url.absoluteString == "https://example.com/c.mp3", "Metadata cannot replace media URL")
        _ = try queue.add([track("d"), track("e")], before: -1)
        queue.removeUpcoming()
        expect(queue.tracks.count == 1, "Removing upcoming retains current")
        try queue.load(track("replacement"))
        expect(queue.current?.dictionary["title"] as? String == "replacement", "Load replaces active track")
        try queue.remove([0])
        expect(queue.index == nil && queue.current == nil, "Removing last track clears current index")
        try queue.set([["url": ["uri": "file:///tmp/sample.mp3"], "artwork": ["uri": "file:///tmp/art.png"]]])
        expect(queue.current?.url.isFileURL == true, "Resolved bundled URLs are accepted")
        rejects("unsupported_format") { try queue.set([["url": "https://example.com/file.mpd", "type": "dash"]]) }
        expect(queue.current?.url.isFileURL == true, "Unsupported formats preserve existing queue")
    }

    // Exercise the real backend without requiring a network stream or an audible output device.
    private static func nativeChecks() {
        _ = NSApplication.shared
        let player = NativeTrackPlayerImpl()
        let events = EventRecorder()
        player.delegate = events
        var result: Any?
        var failure: String?
        let resolve: RCTPromiseResolveBlock = { result = $0 }
        let reject: RCTPromiseRejectBlock = { code, _, _ in failure = code }
        player.play(resolve: resolve, reject: reject)
        expect(failure == "player_not_initialized", "Commands reject before setup")
        failure = nil
        player.setupPlayer(config: ["autoUpdateMetadata": true], resolve: resolve, reject: reject)
        expect(failure == nil, "Native setup succeeds")
        player.setupPlayer(config: [:], resolve: resolve, reject: reject)
        expect(failure == "player_already_initialized", "Duplicate setup rejects")
        failure = nil
        player.updateOptions(options: ["capabilities": [1, 4, 5, 6, 8, 9, 10, 11], "progressUpdateEventInterval": 0], resolve: resolve, reject: reject)
        let commands = MPRemoteCommandCenter.shared()
        expect(commands.nextTrackCommand.isEnabled && commands.previousTrackCommand.isEnabled, "Both navigation commands enabled")
        expect(commands.playCommand.isEnabled && commands.pauseCommand.isEnabled, "Play and Pause enabled")
        player.setQueue(trackDicts: [["url": "file:///nonexistent-test-a.mp3", "title": "A"], ["url": "file:///nonexistent-test-b.mp3", "title": "B"]], resolve: resolve, reject: reject)
        player.skipToNext(initialTime: -1, resolve: resolve, reject: reject)
        player.getActiveTrackIndex(resolve: resolve, reject: reject)
        expect(result as? Int == 1, "Native Next selects second track")
        player.skipToPrevious(initialTime: -1, resolve: resolve, reject: reject)
        player.getActiveTrackIndex(resolve: resolve, reject: reject)
        expect(result as? Int == 0, "Native Previous selects first track")
        expect(commands.nextTrackCommand.isEnabled, "Track changes preserve remote commands")
        player.pause(resolve: resolve, reject: reject)
        player.updateMetadataFor(index: 0, metadata: ["title": "Updated while paused"], resolve: resolve, reject: reject)
        expect(MPNowPlayingInfoCenter.default().nowPlayingInfo?[MPMediaItemPropertyTitle] as? String == "Updated while paused", "Paused metadata updates Now Playing")
        player.setRepeatMode(repeatMode: 2, resolve: resolve, reject: reject)
        player.getRepeatMode(resolve: resolve, reject: reject)
        expect(result as? Int == 2, "Native repeat mode round-trip")
        player.setVolume(level: 0.4, resolve: resolve, reject: reject)
        player.getVolume(resolve: resolve, reject: reject)
        expect(abs((result as? Float ?? -1) - 0.4) < 0.001, "Native volume round-trip")
        player.setRate(rate: 1.5, resolve: resolve, reject: reject)
        player.getRate(resolve: resolve, reject: reject)
        expect(result as? Float == 1.5, "Native rate round-trip")
        player.setRate(rate: Double.nan, resolve: resolve, reject: reject)
        expect(failure == "invalid_parameter", "Non-finite rate rejected")
        player.seekTo(time: Double.greatestFiniteMagnitude, resolve: resolve, reject: reject)
        expect(failure == "invalid_parameter", "Unrepresentable seek time rejected")
        failure = nil
        player.stop(resolve: resolve, reject: reject)
        player.getQueue(resolve: resolve, reject: reject)
        expect((result as? [[String: Any]])?.count == 2, "Stop retains queue")
        player.reset(resolve: resolve, reject: reject)
        player.getActiveTrackIndex(resolve: resolve, reject: reject)
        expect(result is NSNull, "Reset clears active track")
        expect(MPNowPlayingInfoCenter.default().nowPlayingInfo == nil, "Reset removes stale system metadata")
        expect(events.names.contains("playback-active-track-changed"), "Queue transitions emit track events")
        expect(failure == nil, "Valid native commands resolve")
    }

    private static func waitFor(_ message: String, _ predicate: () -> Bool) {
        let deadline = Date().addingTimeInterval(8)
        while !predicate(), Date() < deadline {
            RunLoop.current.run(until: Date().addingTimeInterval(0.01))
        }
        expect(predicate(), message)
    }

    // Generate silent PCM locally so playback tests do not depend on public media URLs.
    private static func playbackChecks() throws {
        let directory = FileManager.default.temporaryDirectory.appendingPathComponent("rntp-audio-\(UUID().uuidString)")
        try FileManager.default.createDirectory(at: directory, withIntermediateDirectories: false)
        defer { try? FileManager.default.removeItem(at: directory) }
        let url = directory.appendingPathComponent("silence.wav")
        var wav = Data()
        func text(_ value: String) { wav.append(contentsOf: value.utf8) }
        func number<T: FixedWidthInteger>(_ value: T) {
            var littleEndian = value.littleEndian
            withUnsafeBytes(of: &littleEndian) { wav.append(contentsOf: $0) }
        }
        text("RIFF")
        number(UInt32(32036))
        text("WAVEfmt ")
        number(UInt32(16))
        number(UInt16(1))
        number(UInt16(1))
        number(UInt32(8000))
        number(UInt32(16000))
        number(UInt16(2))
        number(UInt16(16))
        text("data")
        number(UInt32(32000))
        wav.append(Data(count: 32000))
        try wav.write(to: url)

        let player = NativeTrackPlayerImpl()
        var result: Any?
        var failure: String?
        let resolve: RCTPromiseResolveBlock = { result = $0 }
        let reject: RCTPromiseRejectBlock = { code, _, _ in failure = code }
        player.setupPlayer(config: [:], resolve: resolve, reject: reject)
        player.play(resolve: resolve, reject: reject)
        player.getPlayWhenReady(resolve: resolve, reject: reject)
        expect(result as? Bool == true, "Play before adding tracks retains autoplay intent")
        player.setQueue(trackDicts: [["url": url.absoluteString, "title": "First"], ["url": url.absoluteString, "title": "Second"]], resolve: resolve, reject: reject)
        waitFor("Local AVPlayer playback starts and advances") {
            player.getProgress(resolve: resolve, reject: reject)
            return ((result as? [String: Double])?["position"] ?? 0) > 0.1
        }
        player.pause(resolve: resolve, reject: reject)
        var sought = false
        player.seekTo(time: 0.5, resolve: { _ in sought = true }, reject: reject)
        waitFor("Paused seek completes") { sought }
        player.getProgress(resolve: resolve, reject: reject)
        expect(abs(((result as? [String: Double])?["position"] ?? -1) - 0.5) < 0.1, "Paused seek updates position")
        player.getPlayWhenReady(resolve: resolve, reject: reject)
        expect(result as? Bool == false, "Seeking while paused does not resume playback")
        player.skipToNext(initialTime: -1, resolve: resolve, reject: reject)
        player.getActiveTrackIndex(resolve: resolve, reject: reject)
        expect(result as? Int == 1, "Next works after actual playback")
        player.skipToPrevious(initialTime: -1, resolve: resolve, reject: reject)
        player.setRepeatMode(repeatMode: 0, resolve: resolve, reject: reject)
        player.play(resolve: resolve, reject: reject)
        waitFor("Natural end automatically advances to next track") {
            player.getActiveTrackIndex(resolve: resolve, reject: reject)
            return result as? Int == 1
        }
        waitFor("Queue end reaches ended state") {
            player.getPlaybackState(resolve: resolve, reject: reject)
            return (result as? [String: Any])?["state"] as? String == "ended"
        }
        player.pause(resolve: resolve, reject: reject)
        player.play(resolve: resolve, reject: reject)
        waitFor("Play after pausing an ended queue restarts playback") {
            player.getPlaybackState(resolve: resolve, reject: reject)
            return (result as? [String: Any])?["state"] as? String == "playing"
        }
        player.stop(resolve: resolve, reject: reject)
        player.play(resolve: resolve, reject: reject)
        waitFor("Stop followed by Play reloads the retained source") {
            player.getPlaybackState(resolve: resolve, reject: reject)
            return (result as? [String: Any])?["state"] as? String == "playing"
        }
        player.setRepeatMode(repeatMode: 2, resolve: resolve, reject: reject)
        player.skip(to: 1, initialTime: 1.5, resolve: resolve, reject: reject)
        waitFor("Repeat Queue wraps to the first track") {
            player.getActiveTrackIndex(resolve: resolve, reject: reject)
            return result as? Int == 0
        }
        player.setRepeatMode(repeatMode: 1, resolve: resolve, reject: reject)
        player.skip(to: 0, initialTime: 1.5, resolve: resolve, reject: reject)
        waitFor("An initial playback position is applied") {
            player.getProgress(resolve: resolve, reject: reject)
            return ((result as? [String: Double])?["position"] ?? 0) > 1.4
        }
        waitFor("Repeat Track restarts the current item") {
            player.getProgress(resolve: resolve, reject: reject)
            return ((result as? [String: Double])?["position"] ?? 2) < 0.5
        }
        player.getActiveTrackIndex(resolve: resolve, reject: reject)
        expect(result as? Int == 0, "Repeat Track retains the active index")
        player.reset(resolve: resolve, reject: reject)
        expect(failure == nil, "Local playback commands complete without rejection")
    }

    static func main() throws {
        try queueChecks()
        nativeChecks()
        try playbackChecks()
        print("macOS: \(checks) native checks passed")
    }
}

private class EventRecorder: NSObject, NativeTrackPlayerImplDelegate {
    var names: [String] = []

    func sendEvent(name: String, body: Any?) {
        names.append(name)
    }
}
