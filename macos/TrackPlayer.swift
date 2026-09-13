// TrackPlayer.swift
// Copyright 2026 EvoPixel. Licensed under the Apache License, Version 2.0.
import Foundation
import AVFoundation
import AppKit
import MediaPlayer
import React

@objc(NativeTrackPlayerImpl)
public class NativeTrackPlayerImpl: NSObject {
    @objc public weak var delegate: NativeTrackPlayerImplDelegate?

    // MARK: - Attributes

    private let player = AVPlayer()
    private var queue = TrackQueue()
    private var hasInitialized = false
    private var playWhenReady = false
    private var playbackRate: Float = 1
    private var repeatMode = 0
    private var state = "none"
    private var playbackError: [String: String]?
    private var generation = UUID()
    private var observations: [NSKeyValueObservation] = []
    private var timeObservation: NSKeyValueObservation?
    private var endObserver: NSObjectProtocol?
    private var failureObserver: NSObjectProtocol?
    private var progressObserver: Any?
    private var remoteTargets: [(MPRemoteCommand, Any)] = []
    private var automaticMetadata = true
    private var metadata: [String: Any] = [:]
    private var artworkTask: URLSessionDataTask?
    private var artworkGeneration = UUID()
    private var bufferDuration: Double = 0
    private var pendingStart: Double?
    private var starting = false
    private var ended = false
    private var stoppedPosition: Double = 0

    deinit {
        artworkTask?.cancel()
        if let observer = progressObserver { player.removeTimeObserver(observer) }
        if let observer = endObserver { NotificationCenter.default.removeObserver(observer) }
        if let observer = failureObserver { NotificationCenter.default.removeObserver(observer) }
        for (command, target) in remoteTargets { command.removeTarget(target) }
        player.pause()
    }

    // MARK: - Events and State

    private func emit(_ name: String, _ body: Any? = nil) {
        delegate?.sendEvent(name: name, body: body)
    }

    private func changeState(_ next: String) {
        guard state != next else { return }
        state = next
        emit("playback-state", playbackState())
        updatePlaybackInfo()
    }

    private func playbackState() -> [String: Any] {
        var result: [String: Any] = ["state": state]
        if let error = playbackError { result["error"] = error }
        return result
    }

    private func progress() -> [String: Double] {
        let seconds = player.currentTime().seconds
        let duration = player.currentItem?.duration.seconds ?? 0
        let buffered = player.currentItem?.loadedTimeRanges.map { CMTimeRangeGetEnd($0.timeRangeValue).seconds }.filter { $0.isFinite }.max() ?? 0
        return [
            "position": player.currentItem == nil ? stoppedPosition : (seconds.isFinite ? max(0, seconds) : 0),
            "duration": duration.isFinite ? max(0, duration) : 0,
            "buffered": max(0, buffered),
        ]
    }

    private func setIntent(_ value: Bool) {
        guard playWhenReady != value else { return }
        playWhenReady = value
        emit("playback-play-when-ready-changed", ["playWhenReady": value])
    }

    private func fail(_ error: Error?) {
        playbackError = ["code": "playback_error", "message": error?.localizedDescription ?? "The media could not be played."]
        changeState("error")
        emit("playback-error", playbackError)
    }

    // KVO may arrive on an AVFoundation thread; queue mutations stay on the bridge's main queue.
    private func observeItem(_ item: AVPlayerItem) {
        observations = [item.observe(\.status, options: [.new]) { [weak self, weak item] _, _ in
            DispatchQueue.main.async {
                guard let self = self, let item = item, self.player.currentItem === item else { return }
                if item.status == .failed {
                    self.fail(item.error)
                } else if item.status == .readyToPlay {
                    self.startWhenReady()
                }
            }
        }]
        endObserver = NotificationCenter.default.addObserver(forName: .AVPlayerItemDidPlayToEndTime, object: item, queue: .main) { [weak self, weak item] _ in
            guard let self = self, let item = item, self.player.currentItem === item else { return }
            self.finishTrack()
        }
        failureObserver = NotificationCenter.default.addObserver(forName: .AVPlayerItemFailedToPlayToEndTime, object: item, queue: .main) { [weak self, weak item] notification in
            guard let self = self, let item = item, self.player.currentItem === item else { return }
            self.fail(notification.userInfo?[AVPlayerItemFailedToPlayToEndTimeErrorKey] as? Error)
        }
    }

    private func clearItem() {
        generation = UUID()
        observations.removeAll()
        if let observer = endObserver { NotificationCenter.default.removeObserver(observer) }
        if let observer = failureObserver { NotificationCenter.default.removeObserver(observer) }
        endObserver = nil
        failureObserver = nil
        pendingStart = nil
        starting = false
        player.replaceCurrentItem(with: nil)
    }

    // Reuse one AVPlayer and keep remote command registrations across track changes.
    private func activate(previous: Track?, previousIndex: Int?, previousPosition: Double, position: Double = 0, force: Bool = false) {
        let track = queue.current
        let changed = previous?.identity != track?.identity
        if changed || force {
            clearItem()
            playbackError = nil
            ended = false
            stoppedPosition = 0
            if let track = track {
                var options: [String: Any] = [:]
                if let headers = track.dictionary["headers"] as? [String: String] {
                    options["AVURLAssetHTTPHeaderFieldsKey"] = headers
                }
                if let agent = track.dictionary["userAgent"] as? String {
                    options[AVURLAssetHTTPUserAgentKey] = agent
                }
                let asset = AVURLAsset(url: track.url, options: options)
                let item = AVPlayerItem(asset: asset)
                item.preferredForwardBufferDuration = bufferDuration
                let pitch = track.dictionary["pitchAlgorithm"] as? Int ?? 2
                item.audioTimePitchAlgorithm = pitch == 1 ? .varispeed : (pitch == 3 ? .timeDomain : .spectral)
                pendingStart = max(0, position)
                changeState("loading")
                observeItem(item)
                player.replaceCurrentItem(with: item)
                if item.status == .readyToPlay { startWhenReady() }
            } else {
                setIntent(false)
                changeState("none")
            }
            if automaticMetadata { replaceMetadata(track?.dictionary ?? [:]) }
            if track == nil { replaceMetadata([:]) }
        }
        if changed || previousIndex != queue.index {
            var body: [String: Any] = ["lastPosition": previousPosition]
            if let previous = previous { body["lastTrack"] = previous.dictionary }
            if let previousIndex = previousIndex { body["lastIndex"] = previousIndex }
            if let track = track { body["track"] = track.dictionary }
            if let index = queue.index { body["index"] = index }
            emit("playback-active-track-changed", body)
        }
        updatePlaybackInfo()
    }

    private func startWhenReady() {
        guard player.currentItem?.status == .readyToPlay, !starting else { return }
        if let position = pendingStart, position > 0, player.currentItem?.duration.isIndefinite == false {
            starting = true
            let token = generation
            player.seek(to: CMTime(seconds: position, preferredTimescale: 1000), toleranceBefore: .zero, toleranceAfter: .zero) { [weak self] finished in
                DispatchQueue.main.async {
                    guard let self = self, self.generation == token else { return }
                    self.starting = false
                    self.pendingStart = nil
                    if finished, self.playbackError == nil { self.applyIntent() }
                }
            }
        } else {
            pendingStart = nil
            applyIntent()
        }
    }

    private func applyIntent() {
        if playWhenReady {
            player.playImmediately(atRate: playbackRate)
        } else {
            player.pause()
            changeState("paused")
        }
        updatePlaybackInfo()
    }

    private func finishTrack() {
        guard let index = queue.index else { return }
        let previous = queue.current
        let position = progress()["position"] ?? 0
        var next: Int?
        if repeatMode == 1 {
            next = index
        } else if index + 1 < queue.tracks.count {
            next = index + 1
        } else if repeatMode == 2 {
            next = 0
        }
        if let next = next {
            try? queue.select(Double(next))
            activate(previous: previous, previousIndex: index, previousPosition: position, force: true)
        } else {
            ended = true
            changeState("ended")
            emit("playback-queue-ended", ["track": index, "position": position])
        }
    }

    // MARK: - Promise Validation

    private func perform(_ resolve: RCTPromiseResolveBlock, _ reject: RCTPromiseRejectBlock, _ action: () throws -> Any?) {
        guard hasInitialized else {
            reject("player_not_initialized", "The player is not initialized. Call setupPlayer first.", nil)
            return
        }
        do {
            resolve(try action() ?? NSNull())
        } catch let error as TrackPlayerError {
            reject(error.code, error.message, nil)
        } catch {
            reject("player_error", error.localizedDescription, error)
        }
    }

    private func mutate(_ action: () throws -> Void) rethrows {
        let previous = queue.current
        let index = queue.index
        let position = progress()["position"] ?? 0
        try action()
        activate(previous: previous, previousIndex: index, previousPosition: position)
    }

    private func requireNumber(_ value: Double, minimum: Double, maximum: Double = Double.greatestFiniteMagnitude) throws {
        guard value.isFinite, value >= minimum, value <= maximum else {
            throw TrackPlayerError(code: "invalid_parameter", message: "The numeric value is outside its supported range.")
        }
    }

    // MARK: - Bridged Methods

    @objc(setupPlayer:resolver:rejecter:)
    public func setupPlayer(config: [String: Any], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        guard !hasInitialized else {
            reject("player_already_initialized", "The player has already been initialized via setupPlayer.", nil)
            return
        }
        let buffer = config["minBuffer"] as? Double ?? 0
        guard buffer.isFinite, buffer >= 0 else {
            reject("invalid_parameter", "minBuffer must be a finite non-negative number.", nil)
            return
        }
        bufferDuration = buffer
        automaticMetadata = config["autoUpdateMetadata"] as? Bool ?? true
        hasInitialized = true
        player.automaticallyWaitsToMinimizeStalling = true
        timeObservation = player.observe(\.timeControlStatus, options: [.new]) { [weak self] _, _ in
            DispatchQueue.main.async {
                guard let self = self, self.player.currentItem != nil, self.playbackError == nil else { return }
                if self.player.timeControlStatus == .playing {
                    self.changeState("playing")
                } else if self.playWhenReady, self.state != "ended", self.state != "loading" {
                    self.changeState("buffering")
                }
            }
        }
        registerRemoteCommands()
        resolve(NSNull())
    }

    @objc(addWithTrackDicts:before:resolve:reject:)
    public func add(trackDicts: [[String: Any]], before: NSNumber, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            var index = 0
            try mutate { index = try queue.add(trackDicts, before: before.doubleValue) }
            return index
        }
    }

    @objc(loadWithTrackDict:resolve:reject:)
    public func load(trackDict: [String: Any], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try mutate { try queue.load(trackDict) }
            return queue.index
        }
    }

    @objc(setQueueWithTrackDicts:resolve:reject:)
    public func setQueue(trackDicts: [[String: Any]], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try mutate { try queue.set(trackDicts) }
            return nil
        }
    }

    @objc(moveFromIndex:toIndex:resolve:reject:)
    public func move(from: Double, to: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try mutate { try queue.move(from: from, to: to) }
            return nil
        }
    }

    @objc(removeWithTracks:resolve:reject:)
    public func remove(tracks: [NSNumber], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try mutate { try queue.remove(tracks.map { $0.doubleValue }) }
            return nil
        }
    }

    @objc(removeUpcomingTracksWithResolve:reject:)
    public func removeUpcomingTracks(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            queue.removeUpcoming()
            updatePlaybackInfo()
            return nil
        }
    }

    @objc(resetWithResolve:reject:)
    public func reset(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            setIntent(false)
            try mutate { try queue.set([]) }
            return nil
        }
    }

    @objc(skipTo:initialTime:resolve:reject:)
    public func skip(to value: Double, initialTime: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try requireNumber(initialTime, minimum: -1, maximum: Double(Int64.max) / 1000)
            let previous = queue.current
            let index = queue.index
            let position = progress()["position"] ?? 0
            try queue.select(value)
            activate(previous: previous, previousIndex: index, previousPosition: position, position: max(0, initialTime), force: true)
            return nil
        }
    }

    @objc(skipToNextWithInitialTime:resolve:reject:)
    public func skipToNext(initialTime: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        skip(to: Double((queue.index ?? -1) + 1), initialTime: initialTime, resolve: resolve, reject: reject)
    }

    @objc(skipToPreviousWithInitialTime:resolve:reject:)
    public func skipToPrevious(initialTime: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        skip(to: Double((queue.index ?? 0) - 1), initialTime: initialTime, resolve: resolve, reject: reject)
    }

    @objc(playWithResolve:reject:)
    public func play(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            setIntent(true)
            if queue.current == nil { return nil }
            if player.currentItem == nil || state == "error" || ended {
                activate(previous: queue.current, previousIndex: queue.index, previousPosition: stoppedPosition, position: ended ? 0 : stoppedPosition, force: true)
            } else if player.currentItem?.status == .readyToPlay, pendingStart == nil {
                applyIntent()
            }
            return nil
        }
    }

    @objc(pauseWithResolve:reject:)
    public func pause(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            setIntent(false)
            player.pause()
            if queue.current != nil, state != "error", state != "stopped" { changeState("paused") }
            return nil
        }
    }

    @objc(stopWithResolve:reject:)
    public func stop(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            setIntent(false)
            stoppedPosition = progress()["position"] ?? 0
            clearItem()
            changeState("stopped")
            return nil
        }
    }

    @objc(setPlayWhenReadyWithPlayWhenReady:resolve:reject:)
    public func setPlayWhenReady(playWhenReady value: Bool, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        if value {
            play(resolve: { _ in resolve(true) }, reject: reject)
        } else {
            pause(resolve: { _ in resolve(false) }, reject: reject)
        }
    }

    @objc(seekToTime:resolve:reject:)
    public func seekTo(time: Double, resolve: @escaping RCTPromiseResolveBlock, reject: @escaping RCTPromiseRejectBlock) {
        guard hasInitialized else {
            reject("player_not_initialized", "The player is not initialized. Call setupPlayer first.", nil)
            return
        }
        guard time.isFinite, time >= 0, time < Double(Int64.max) / 1000, queue.current != nil else {
            reject("invalid_parameter", "Seeking requires a track and a finite non-negative position.", nil)
            return
        }
        if player.currentItem == nil {
            stoppedPosition = time
            updatePlaybackInfo()
            resolve(NSNull())
            return
        }
        pendingStart = nil
        let token = generation
        player.seek(to: CMTime(seconds: time, preferredTimescale: 1000), toleranceBefore: .zero, toleranceAfter: .zero) { [weak self] finished in
            DispatchQueue.main.async {
                guard let self = self, self.generation == token, finished else {
                    reject("seek_cancelled", "The seek was superseded by another command.", nil)
                    return
                }
                if self.ended {
                    self.ended = false
                    self.applyIntent()
                }
                self.updatePlaybackInfo()
                resolve(NSNull())
            }
        }
    }

    @objc(seekByOffset:resolve:reject:)
    public func seekBy(offset: Double, resolve: @escaping RCTPromiseResolveBlock, reject: @escaping RCTPromiseRejectBlock) {
        guard offset.isFinite else {
            reject("invalid_parameter", "The seek offset must be finite.", nil)
            return
        }
        seekTo(time: max(0, (progress()["position"] ?? 0) + offset), resolve: resolve, reject: reject)
    }

    @objc(retryWithResolve:reject:)
    public func retry(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            let position = progress()["position"] ?? 0
            activate(previous: queue.current, previousIndex: queue.index, previousPosition: position, position: position, force: true)
            return nil
        }
    }

    @objc(setVolumeWithLevel:resolve:reject:)
    public func setVolume(level: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try requireNumber(level, minimum: 0, maximum: 1)
            player.volume = Float(level)
            return nil
        }
    }

    @objc(setRateWithRate:resolve:reject:)
    public func setRate(rate: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try requireNumber(rate, minimum: 0.1, maximum: 4)
            playbackRate = Float(rate)
            if player.timeControlStatus == .playing { player.rate = playbackRate }
            updatePlaybackInfo()
            return nil
        }
    }

    @objc(setRepeatModeWithRepeatMode:resolve:reject:)
    public func setRepeatMode(repeatMode value: NSNumber, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            guard [0.0, 1.0, 2.0].contains(value.doubleValue) else {
                throw TrackPlayerError(code: "invalid_parameter", message: "Unknown repeat mode.")
            }
            repeatMode = value.intValue
            return repeatMode
        }
    }

    @objc(getQueueWithResolve:reject:)
    public func getQueue(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { queue.tracks.map { $0.dictionary } }
    }

    @objc(getTrackWithIndex:resolve:reject:)
    public func getTrack(index: Double, resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            guard let index = try? queue.checkedIndex(index) else { return nil }
            return queue.tracks[index].dictionary
        }
    }

    @objc(getActiveTrackWithResolve:reject:)
    public func getActiveTrack(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { queue.current?.dictionary }
    }

    @objc(getActiveTrackIndexWithResolve:reject:)
    public func getActiveTrackIndex(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { queue.index }
    }

    @objc(getPlayWhenReadyWithResolve:reject:)
    public func getPlayWhenReady(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { playWhenReady }
    }

    @objc(getPlaybackStateWithResolve:reject:)
    public func getPlaybackState(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { playbackState() }
    }

    @objc(getProgressWithResolve:reject:)
    public func getProgress(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { progress() }
    }

    @objc(getVolumeWithResolve:reject:)
    public func getVolume(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { player.volume }
    }

    @objc(getRateWithResolve:reject:)
    public func getRate(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { playbackRate }
    }

    @objc(getRepeatModeWithResolve:reject:)
    public func getRepeatMode(resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) { repeatMode }
    }
}

extension NativeTrackPlayerImpl {

    // MARK: - Now Playing

    private func replaceMetadata(_ value: [String: Any]) {
        metadata = value
        artworkTask?.cancel()
        artworkGeneration = UUID()
        let token = artworkGeneration
        var info: [String: Any] = [:]
        let names = [
            "title": MPMediaItemPropertyTitle,
            "artist": MPMediaItemPropertyArtist,
            "album": MPMediaItemPropertyAlbumTitle,
            "genre": MPMediaItemPropertyGenre,
        ]
        for (key, target) in names {
            if let text = value[key] as? String { info[target] = text }
        }
        if let live = value["isLiveStream"] as? Bool { info[MPNowPlayingInfoPropertyIsLiveStream] = live }
        MPNowPlayingInfoCenter.default().nowPlayingInfo = value.isEmpty ? nil : info
        updatePlaybackInfo()
        guard let url = Track.mediaURL(value["artwork"]) else { return }
        if url.isFileURL {
            if let image = NSImage(contentsOf: url) { installArtwork(image, token: token) }
        } else {
            var request = URLRequest(url: url)
            request.timeoutInterval = 15
            artworkTask = URLSession.shared.dataTask(with: request) { [weak self] data, response, error in
                guard error == nil, let response = response as? HTTPURLResponse, (200..<300).contains(response.statusCode), let data = data, data.count <= 8 * 1024 * 1024 else { return }
                DispatchQueue.main.async {
                    guard let self = self, self.artworkGeneration == token, let image = NSImage(data: data) else { return }
                    self.installArtwork(image, token: token)
                }
            }
            artworkTask?.resume()
        }
    }

    // Discard old artwork responses after Next, reset or a metadata replacement.
    private func installArtwork(_ image: NSImage, token: UUID) {
        guard token == artworkGeneration, image.size.width > 0, image.size.height > 0 else { return }
        var info = MPNowPlayingInfoCenter.default().nowPlayingInfo ?? [:]
        info[MPMediaItemPropertyArtwork] = MPMediaItemArtwork(boundsSize: image.size) { _ in image }
        MPNowPlayingInfoCenter.default().nowPlayingInfo = info
    }

    private func updatePlaybackInfo() {
        let center = MPNowPlayingInfoCenter.default()
        guard queue.current != nil else {
            center.nowPlayingInfo = nil
            center.playbackState = .stopped
            return
        }
        var info = center.nowPlayingInfo ?? [:]
        let progress = self.progress()
        let duration = metadata["duration"] as? Double ?? progress["duration"] ?? 0
        if duration.isFinite, duration > 0 {
            info[MPMediaItemPropertyPlaybackDuration] = duration
        } else {
            info.removeValue(forKey: MPMediaItemPropertyPlaybackDuration)
        }
        info[MPNowPlayingInfoPropertyElapsedPlaybackTime] = progress["position"] ?? 0
        info[MPNowPlayingInfoPropertyPlaybackRate] = state == "playing" ? playbackRate : 0
        info[MPNowPlayingInfoPropertyDefaultPlaybackRate] = playbackRate
        info[MPNowPlayingInfoPropertyPlaybackQueueIndex] = queue.index ?? 0
        info[MPNowPlayingInfoPropertyPlaybackQueueCount] = queue.tracks.count
        center.nowPlayingInfo = info
        center.playbackState = state == "playing" ? .playing : (state == "stopped" || state == "ended" || state == "error" ? .stopped : .paused)
    }

    @objc(updateMetadataFor:metadata:resolve:reject:)
    public func updateMetadataFor(index: Double, metadata: [String: Any], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            try queue.update(index, metadata: metadata)
            if let active = queue.index, Double(active) == index, automaticMetadata {
                replaceMetadata(queue.current?.dictionary ?? [:])
            }
            return nil
        }
    }

    @objc(updateNowPlayingMetadataWithMetadata:resolve:reject:)
    public func updateNowPlayingMetadata(metadata value: [String: Any], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            guard queue.current != nil else {
                throw TrackPlayerError(code: "no_current_item", message: "There is no current track.")
            }
            replaceMetadata(metadata.merging(value) { _, new in new })
            return nil
        }
    }

    // MARK: - Remote Commands

    private func bind(_ command: MPRemoteCommand, event: String, body: @escaping (MPRemoteCommandEvent) -> [String: Any] = { _ in [:] }) {
        command.isEnabled = false
        let target = command.addTarget { [weak self] input in
            guard let self = self else { return .commandFailed }
            let payload = body(input)
            DispatchQueue.main.async { [weak self] in
                guard let self = self, self.hasInitialized else { return }
                self.emit(event, payload)
            }
            return .success
        }
        remoteTargets.append((command, target))
    }

    private func registerRemoteCommands() {
        let commands = MPRemoteCommandCenter.shared()
        bind(commands.playCommand, event: "remote-play")
        bind(commands.pauseCommand, event: "remote-pause")
        bind(commands.togglePlayPauseCommand, event: "remote-play-pause")
        bind(commands.stopCommand, event: "remote-stop")
        bind(commands.nextTrackCommand, event: "remote-next")
        bind(commands.previousTrackCommand, event: "remote-previous")
        bind(commands.changePlaybackPositionCommand, event: "remote-seek") { input in
            ["position": (input as? MPChangePlaybackPositionCommandEvent)?.positionTime ?? 0]
        }
        bind(commands.skipForwardCommand, event: "remote-jump-forward") { input in
            ["interval": (input as? MPSkipIntervalCommandEvent)?.interval ?? 15]
        }
        bind(commands.skipBackwardCommand, event: "remote-jump-backward") { input in
            ["interval": (input as? MPSkipIntervalCommandEvent)?.interval ?? 15]
        }
        bind(commands.likeCommand, event: "remote-like")
        bind(commands.dislikeCommand, event: "remote-dislike")
        bind(commands.bookmarkCommand, event: "remote-bookmark")
    }

    @objc(updateOptions:resolver:rejecter:)
    public func updateOptions(options: [String: Any], resolve: RCTPromiseResolveBlock, reject: RCTPromiseRejectBlock) {
        perform(resolve, reject) {
            let interval = options["progressUpdateEventInterval"] as? Double
            let forward = options["forwardJumpInterval"] as? Double
            let backward = options["backwardJumpInterval"] as? Double
            for number in [interval, forward, backward].compactMap({ $0 }) {
                try requireNumber(number, minimum: 0, maximum: 86400)
            }
            let commands = MPRemoteCommandCenter.shared()
            if let capabilities = options["capabilities"] as? [Int] {
                let supported: [(Int, MPRemoteCommand)] = [
                    (1, commands.playCommand),
                    (4, commands.pauseCommand),
                    (5, commands.stopCommand),
                    (6, commands.changePlaybackPositionCommand),
                    (8, commands.nextTrackCommand),
                    (9, commands.previousTrackCommand),
                    (10, commands.skipForwardCommand),
                    (11, commands.skipBackwardCommand),
                ]
                for (capability, command) in supported { command.isEnabled = capabilities.contains(capability) }
                commands.togglePlayPauseCommand.isEnabled = capabilities.contains(1) && capabilities.contains(4)
            }
            if let forward = forward { commands.skipForwardCommand.preferredIntervals = [NSNumber(value: forward)] }
            if let backward = backward { commands.skipBackwardCommand.preferredIntervals = [NSNumber(value: backward)] }
            if commands.skipForwardCommand.preferredIntervals.isEmpty { commands.skipForwardCommand.preferredIntervals = [15] }
            if commands.skipBackwardCommand.preferredIntervals.isEmpty { commands.skipBackwardCommand.preferredIntervals = [15] }
            for (key, command) in [("likeOptions", commands.likeCommand), ("dislikeOptions", commands.dislikeCommand), ("bookmarkOptions", commands.bookmarkCommand)] {
                if let value = options[key] as? [String: Any] {
                    command.isEnabled = true
                    command.isActive = value["isActive"] as? Bool ?? false
                    command.localizedTitle = value["title"] as? String ?? ""
                }
            }
            if let interval = interval {
                if let observer = progressObserver { player.removeTimeObserver(observer) }
                progressObserver = nil
                if interval > 0 {
                    progressObserver = player.addPeriodicTimeObserver(forInterval: CMTime(seconds: max(0.1, interval), preferredTimescale: 1000), queue: .main) { [weak self] _ in
                        guard let self = self, let index = self.queue.index else { return }
                        var body: [String: Any] = self.progress()
                        body["track"] = index
                        self.emit("playback-progress-updated", body)
                        self.updatePlaybackInfo()
                    }
                }
            }
            return nil
        }
    }

    @objc(supportedEvents)
    public static var supportedEvents: [String] {
        return [
            "remote-play", "remote-pause", "remote-play-pause", "remote-stop",
            "remote-next", "remote-previous", "remote-seek",
            "remote-jump-forward", "remote-jump-backward", "remote-like",
            "remote-dislike", "remote-bookmark", "remote-duck",
            "playback-state", "playback-error", "playback-queue-ended",
            "playback-active-track-changed", "playback-progress-updated",
            "playback-play-when-ready-changed", "metadata-common-received",
            "metadata-timed-received", "metadata-chapter-received",
        ]
    }
}

@objc public protocol NativeTrackPlayerImplDelegate {
    @objc(sendEvent:body:)
    func sendEvent(name: String, body: Any?)
}
