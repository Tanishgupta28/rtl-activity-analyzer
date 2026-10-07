#ifndef ACTIVITY_ANALYZER_H
#define ACTIVITY_ANALYZER_H

#include "TraceData.h"

#include <cstddef>
#include <string>
#include <vector>

struct SignalActivity {
    std::string signal_name;
    std::size_t transitions = 0;
    std::size_t rising_transitions = 0;
    std::size_t falling_transitions = 0;
    double activity_ratio = 0.0;
};

class ActivityAnalyzer {
public:
    // Expects a trace validated by TraceParser; preserves the signal order.
    std::vector<SignalActivity> analyze(const TraceData& trace) const;
};

#endif
