#include "ActivityAnalyzer.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace {

int checks_run = 0;
int failed_checks = 0;

void check(bool condition, const std::string& message) {
    ++checks_run;
    if (condition) {
        std::cout << "PASS: " << message << '\n';
    } else {
        ++failed_checks;
        std::cout << "FAIL: " << message << '\n';
    }
}

void checkTransitionTotal(const SignalActivity& activity) {
    check(activity.transitions
              == activity.rising_transitions + activity.falling_transitions,
          activity.signal_name + ": total transitions equal rising plus falling");
}

void testSignal(const std::string& name, const std::vector<int>& values,
                std::size_t expected_transitions, std::size_t expected_rising,
                std::size_t expected_falling, double expected_activity) {
    TraceData trace;
    trace.signal_names = {name};
    trace.signal_values = {values};
    for (std::size_t sample_index = 0; sample_index < values.size(); ++sample_index) {
        trace.cycles.push_back(static_cast<unsigned long long>(sample_index));
    }

    const ActivityAnalyzer analyzer;
    const std::vector<SignalActivity> activities = analyzer.analyze(trace);
    check(activities.size() == 1, name + ": one result returned");
    if (activities.size() != 1) {
        return;
    }

    const SignalActivity& activity = activities[0];
    check(activity.signal_name == name, name + ": signal name preserved");
    check(activity.transitions == expected_transitions, name + ": transition count");
    check(activity.rising_transitions == expected_rising, name + ": rising count");
    check(activity.falling_transitions == expected_falling, name + ": falling count");
    // Floating-point calculations can differ slightly; allow a small tolerance.
    check(std::fabs(activity.activity_ratio - expected_activity) < 1e-9,
          name + ": activity ratio");
    checkTransitionTotal(activity);
}

void testRankingOrder() {
    TraceData trace;
    trace.cycles = {0, 1, 2, 3};
    trace.signal_names = {"medium", "low", "high"};
    trace.signal_values = {
        {0, 0, 1, 1}, // Activity: 1/3.
        {0, 0, 0, 0}, // Activity: 0.
        {0, 1, 0, 1}  // Activity: 1.
    };

    const ActivityAnalyzer analyzer;
    const std::vector<SignalActivity> activities = analyzer.analyze(trace);
    check(activities.size() == 3, "ranking: three analysis results returned");
    if (activities.size() != 3) {
        return;
    }

    const std::vector<SignalActivity> ranked = analyzer.rankByActivity(activities);
    check(ranked.size() == 3, "ranking: three ranked results returned");
    if (ranked.size() != 3) {
        return;
    }

    check(ranked[0].signal_name == "high", "ranking: highest activity comes first");
    check(ranked[1].signal_name == "medium", "ranking: medium activity comes second");
    check(ranked[2].signal_name == "low", "ranking: lowest activity comes last");
    check(ranked[0].activity_ratio > ranked[1].activity_ratio
              && ranked[1].activity_ratio > ranked[2].activity_ratio,
          "ranking: activity ratios are strictly descending");
    check(activities[0].signal_name == "medium"
              && activities[1].signal_name == "low"
              && activities[2].signal_name == "high",
          "ranking: original analysis order is preserved");

    for (const SignalActivity& activity : ranked) {
        checkTransitionTotal(activity);
    }
}

void testStableRanking() {
    TraceData trace;
    trace.cycles = {0, 1, 2, 3, 4, 5};
    // Tied signals are separated in the input and are not in alphabetical order.
    trace.signal_names = {"z_first", "low", "a_second", "high", "m_third"};
    trace.signal_values = {
        {0, 0, 1, 1, 0, 1}, // Activity: 0.6.
        {0, 0, 0, 0, 0, 0}, // Activity: 0.
        {1, 1, 0, 0, 1, 0}, // Activity: 0.6.
        {0, 1, 0, 1, 0, 1}, // Activity: 1.
        {0, 1, 1, 0, 0, 1}  // Activity: 0.6.
    };

    const ActivityAnalyzer analyzer;
    const std::vector<SignalActivity> activities = analyzer.analyze(trace);
    check(activities.size() == 5, "stable ranking: five analysis results returned");
    if (activities.size() != 5) {
        return;
    }

    const std::vector<SignalActivity> ranked = analyzer.rankByActivity(activities);
    check(ranked.size() == 5, "stable ranking: five ranked results returned");
    if (ranked.size() != 5) {
        return;
    }

    const std::vector<std::string> expected_order = {
        "high", "z_first", "a_second", "m_third", "low"
    };
    for (std::size_t index = 0; index < ranked.size(); ++index) {
        check(ranked[index].signal_name == expected_order[index],
              "stable ranking: position " + std::to_string(index + 1)
                  + " contains " + expected_order[index]);
        check(activities[index].signal_name == trace.signal_names[index],
              "stable ranking: original position " + std::to_string(index + 1)
                  + " is preserved");
        checkTransitionTotal(ranked[index]);
    }
    check(std::fabs(ranked[1].activity_ratio - 0.6) < 1e-9
              && ranked[1].activity_ratio == ranked[2].activity_ratio
              && ranked[2].activity_ratio == ranked[3].activity_ratio,
          "stable ranking: all three tied signals have activity 0.6");
}

} // namespace

int main() {
    testSignal("constant low", {0, 0, 0, 0}, 0, 0, 0, 0.0);
    testSignal("constant high", {1, 1, 1, 1}, 0, 0, 0, 0.0);
    testSignal("alternating", {0, 1, 0, 1}, 3, 2, 1, 1.0);
    testSignal("mixed", {0, 0, 1, 1, 0, 1}, 3, 2, 1, 0.6);
    testSignal("single low sample", {0}, 0, 0, 0, 0.0);
    testSignal("single high sample", {1}, 0, 0, 0, 0.0);
    testRankingOrder();
    testStableRanking();

    if (failed_checks != 0) {
        std::cout << '\n' << failed_checks << " of " << checks_run << " checks failed.\n";
        return 1;
    }

    std::cout << "\nAll " << checks_run << " checks passed.\n";
    return 0;
}
