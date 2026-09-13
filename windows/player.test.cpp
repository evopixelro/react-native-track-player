#include "RNTrackPlayer/Players/Playback.h"
#include <iostream>
#include <limits>
#include <string>

using Queue = winrt::RNTrackPlayer::Playback<std::string>;

// Fail in Release builds too; standard assert can be compiled out by NDEBUG.
void Check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

template <typename Callback>
void Rejects(Callback callback, const char* message) {
    try { callback(); } catch (const std::logic_error&) { return; }
    throw std::runtime_error(message);
}

int main() {
    try {
        Queue queue;
        Check(queue.Add({"A", "B", "C", "D"}, -1) == 0, "First inserted index");
        Check(!queue.currentTrack, "Queue edits must not start playback themselves");
        queue.currentTrack = 1;
        Check(queue.Add({"X"}, 0) == 0, "Insertion position");
        Check(queue.currentTrack == 2 && queue.queue[*queue.currentTrack] == "B", "Preserve active source when inserting");

        const auto unchanged = queue.queue;
        for (const auto index : {-2.0, 0.5, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()}) {
            Rejects([&] { queue.Add({"invalid"}, index); }, "Invalid insertion was accepted");
            Rejects([&] { queue.Remove({index}); }, "Invalid removal was accepted");
            Check(queue.queue == unchanged, "Invalid indexes mutated the queue");
        }
        Rejects([&] { queue.Remove({0, 500}); }, "Partially invalid removal was accepted");
        Check(queue.queue == unchanged, "Removal must validate all indexes before mutating");

        Check(!queue.Remove({1, 0, 1}), "Removing other tracks must not reload active audio");
        Check(queue.currentTrack == 0 && queue.queue == std::vector<std::string>({"B", "C", "D"}), "Unsorted duplicate removal");
        Check(queue.Remove({0}), "Removing current track must replace the source");
        Check(queue.currentTrack == 0 && queue.queue[0] == "C", "Select successor after removing current track");

        queue.currentTrack = 1;
        Check(queue.Remove({1}), "Remove final active track");
        Check(queue.currentTrack == 0 && queue.queue[0] == "C", "Wrap to first after removing final track");
        Check(queue.Remove({0}), "Remove only active track");
        Check(queue.queue.empty() && !queue.currentTrack, "Clear active index for empty queue");

        queue.Add({"A", "B", "C", "D"}, -1);
        queue.currentTrack = 1;
        queue.Move(0, 3);
        Check(queue.currentTrack == 0 && queue.queue[*queue.currentTrack] == "B", "Move across active track forward");
        queue.Move(3, 0);
        Check(queue.currentTrack == 1 && queue.queue[*queue.currentTrack] == "B", "Move across active track backward");
        queue.Move(1, 1000);
        Check(queue.currentTrack == 3 && queue.queue[*queue.currentTrack] == "B", "Move active track to end");
        queue.Move(3, 0);
        Check(queue.currentTrack == 0 && queue.queue[0] == "B", "Move active track to start");
        Rejects([&] { queue.Move(0, -1); }, "Negative destination accepted");
        Rejects([&] { queue.Move(0, 0.5); }, "Fractional destination accepted");

        queue.currentTrack = 1;
        const auto active = queue.queue[1];
        queue.RemoveUpcomingTracks();
        Check(queue.queue.size() == 2 && queue.queue[1] == active, "RemoveUpcomingTracks removed active track");
        queue.Reset();
        Check(queue.queue.empty() && !queue.currentTrack, "Reset must clear all queue state");
        Rejects([&] { queue.Move(0, 0); }, "Move in empty queue was accepted");
        Check(!queue.Add({}, -1), "Empty add must have no inserted index");
        std::cout << "Windows queue regression tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
