// Modified by EvoPixel for the current queue API and checked index handling.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <vector>

namespace winrt::RNTrackPlayer {
    // Queue operations are independent of the media engine so they can be tested
    // without starting a React instance or opening an audio device.
    template <typename Track>
    struct Playback {
        std::vector<Track> queue;
        std::optional<size_t> currentTrack;

        static size_t Index(double value, size_t size, bool allowEnd = false) {
            if (!std::isfinite(value) || value < 0 || std::floor(value) != value ||
                value > static_cast<double>(size) || (!allowEnd && value == size)) {
                throw std::out_of_range("The track index is out of bounds");
            }
            return static_cast<size_t>(value);
        }

        std::optional<size_t> Add(std::vector<Track> tracks, double before) {
            const auto index = before == -1 ? queue.size() : Index(before, queue.size(), true);
            if (tracks.empty()) return std::nullopt;
            const auto count = tracks.size();
            queue.insert(queue.begin() + index,
                std::make_move_iterator(tracks.begin()), std::make_move_iterator(tracks.end()));
            if (currentTrack && index <= *currentTrack) *currentTrack += count;
            return index;
        }

        // Return true only when the active source must be replaced.
        bool Remove(const std::vector<double>& indexes) {
            std::vector<size_t> sorted;
            for (const auto index : indexes) sorted.push_back(Index(index, queue.size()));
            std::sort(sorted.begin(), sorted.end());
            sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
            const auto previous = currentTrack;
            const bool removedCurrent = previous && std::binary_search(sorted.begin(), sorted.end(), *previous);
            size_t removedBefore = 0;
            if (previous) removedBefore = std::lower_bound(sorted.begin(), sorted.end(), *previous) - sorted.begin();
            for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) queue.erase(queue.begin() + *it);
            if (queue.empty()) currentTrack.reset();
            else if (previous) {
                auto next = *previous - removedBefore;
                currentTrack = next < queue.size() ? next : 0;
            }
            return removedCurrent;
        }

        void Move(double from, double to) {
            const auto source = Index(from, queue.size());
            if (!std::isfinite(to) || to < 0 || std::floor(to) != to)
                throw std::out_of_range("The destination index is invalid");
            const auto destination = to >= queue.size() ? queue.size() - 1 : static_cast<size_t>(to);
            if (source == destination) return;
            auto track = std::move(queue[source]);
            queue.erase(queue.begin() + source);
            queue.insert(queue.begin() + destination, std::move(track));
            if (currentTrack) {
                if (*currentTrack == source) currentTrack = destination;
                else if (source < *currentTrack && *currentTrack <= destination) --*currentTrack;
                else if (destination <= *currentTrack && *currentTrack < source) ++*currentTrack;
            }
        }

        void RemoveUpcomingTracks() {
            if (currentTrack) queue.erase(queue.begin() + *currentTrack + 1, queue.end());
        }

        void Reset() {
            queue.clear();
            currentTrack.reset();
        }
    };
}
