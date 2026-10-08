#include "ActivityAnalyzer.h"

#include <algorithm>

std::vector<SignalActivity> ActivityAnalyzer::analyze(const TraceData& trace) const {
    std::vector<SignalActivity> activities;

    for (std::size_t signal_index = 0; signal_index < trace.signal_names.size();
         ++signal_index) {
        const std::vector<int>& values = trace.signal_values[signal_index];
        SignalActivity activity;
        activity.signal_name = trace.signal_names[signal_index];

        // Index 1 is the first sample that has a previous sample to compare.
        for (std::size_t sample_index = 1; sample_index < values.size(); ++sample_index) {
            const int previous = values[sample_index - 1];
            const int current = values[sample_index];

            if (previous != current) {
                ++activity.transitions;
                if (previous == 0 && current == 1) {
                    ++activity.rising_transitions;
                } else if (previous == 1 && current == 0) {
                    ++activity.falling_transitions;
                }
            }
        }

        if (values.size() > 1) {
            const std::size_t possible_transitions = values.size() - 1;
            // Floating-point division keeps ratios such as 2 / 5 equal to 0.4.
            activity.activity_ratio = static_cast<double>(activity.transitions)
                                      / static_cast<double>(possible_transitions);
        }

        activities.push_back(activity);
    }

    return activities;
}

std::vector<SignalActivity> ActivityAnalyzer::rankByActivity(
    const std::vector<SignalActivity>& activities) const {
    std::vector<SignalActivity> ranked_activities = activities;

    std::stable_sort(ranked_activities.begin(), ranked_activities.end(),
                     [](const SignalActivity& left, const SignalActivity& right) {
                         return left.activity_ratio > right.activity_ratio;
                     });

    return ranked_activities;
}
