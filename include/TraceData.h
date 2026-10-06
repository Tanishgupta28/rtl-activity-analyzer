#ifndef TRACE_DATA_H
#define TRACE_DATA_H

#include <string>
#include <vector>

struct TraceData {
    std::vector<unsigned long long> cycles;
    std::vector<std::string> signal_names;

    // signal_values[signal_index][sample_index] belongs to that signal and cycle.
    std::vector<std::vector<int>> signal_values;
};

#endif
