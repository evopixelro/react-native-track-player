// TrackQueue.swift
// Copyright 2026 EvoPixel. Licensed under the Apache License, Version 2.0.
import Foundation

struct TrackPlayerError: Error {
    let code: String
    let message: String
}

struct Track {
    let identity = UUID()
    let url: URL
    var dictionary: [String: Any]

    init(dictionary: [String: Any]) throws {
        guard let url = Track.mediaURL(dictionary["url"]) else {
            throw TrackPlayerError(code: "invalid_track_object", message: "Track requires a file, HTTP or HTTPS URL.")
        }
        if dictionary["type"] as? String == "dash" || dictionary["type"] as? String == "smoothstreaming" {
            throw TrackPlayerError(code: "unsupported_format", message: "macOS supports AVPlayer formats, including HLS, but not DASH or SmoothStreaming.")
        }
        self.url = url
        self.dictionary = dictionary
    }

    // Accept resolved React Native assets without changing their original metadata.
    static func mediaURL(_ value: Any?) -> URL? {
        let source = (value as? String) ?? (value as? [String: Any])?["uri"] as? String
        guard let text = source, !text.isEmpty else { return nil }
        if text.hasPrefix("/") { return URL(fileURLWithPath: text) }
        guard let url = URL(string: text), let scheme = url.scheme?.lowercased() else { return nil }
        if scheme == "file" { return url }
        return ["http", "https"].contains(scheme) && url.host != nil ? url : nil
    }
}

struct TrackQueue {
    private(set) var tracks: [Track] = []
    private(set) var index: Int?

    var current: Track? {
        guard let index = index else { return nil }
        return tracks[index]
    }

    // Validate before converting bridged doubles so NaN and overflow cannot trap.
    func checkedIndex(_ value: Double, inserting: Bool = false) throws -> Int {
        let limit = tracks.count + (inserting ? 1 : 0)
        guard value.isFinite, value.rounded(.towardZero) == value, value >= 0, value < Double(limit) else {
            throw TrackPlayerError(code: "index_out_of_bounds", message: "The track index is out of bounds.")
        }
        return Int(value)
    }

    mutating func set(_ dictionaries: [[String: Any]]) throws {
        let replacement = try dictionaries.map { try Track(dictionary: $0) }
        tracks = replacement
        index = tracks.isEmpty ? nil : 0
    }

    mutating func add(_ dictionaries: [[String: Any]], before value: Double) throws -> Int {
        let position = try checkedIndex(value == -1 ? Double(tracks.count) : value, inserting: true)
        let additions = try dictionaries.map { try Track(dictionary: $0) }
        tracks.insert(contentsOf: additions, at: position)
        if let active = index, position <= active {
            index = active + additions.count
        } else if index == nil, !tracks.isEmpty {
            index = 0
        }
        return position
    }

    mutating func load(_ dictionary: [String: Any]) throws {
        let track = try Track(dictionary: dictionary)
        if let active = index {
            tracks[active] = track
        } else {
            tracks = [track]
            index = 0
        }
    }

    mutating func select(_ value: Double) throws {
        index = try checkedIndex(value)
    }

    mutating func move(from: Double, to: Double) throws {
        let start = try checkedIndex(from)
        let end = try checkedIndex(to)
        let identity = current?.identity
        let track = tracks.remove(at: start)
        tracks.insert(track, at: end)
        index = tracks.firstIndex { $0.identity == identity }
    }

    // Remove duplicates once; preserve the playing item or choose its next survivor.
    mutating func remove(_ values: [Double]) throws {
        let removed = Set(try values.map { try checkedIndex($0) })
        guard !removed.isEmpty else { return }
        let previous = index ?? 0
        let identity = current?.identity
        let remainingBefore = tracks.indices.filter { $0 < previous && !removed.contains($0) }.count
        tracks = tracks.enumerated().filter { !removed.contains($0.offset) }.map { $0.element }
        index = tracks.firstIndex { $0.identity == identity }
        if index == nil, !tracks.isEmpty {
            index = min(remainingBefore, tracks.count - 1)
        }
    }

    mutating func removeUpcoming() {
        guard let active = index else { return }
        tracks.removeSubrange((active + 1)..<tracks.count)
    }

    mutating func update(_ value: Double, metadata: [String: Any]) throws {
        let position = try checkedIndex(value)
        let keys: Set<String> = ["title", "artist", "album", "artwork", "duration", "genre", "date", "description", "isLiveStream", "rating"]
        for (key, item) in metadata where keys.contains(key) {
            tracks[position].dictionary[key] = item
        }
    }
}
