#include "ActivityAnalyzer.h"
#include "TraceParser.h"

#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <trace.csv>\n";
        return 1;
    }

    try {
        const TraceParser parser;
        const TraceData trace = parser.parse(argv[1]);
        const ActivityAnalyzer analyzer;
        const std::vector<SignalActivity> activities = analyzer.analyze(trace);

        std::cout << "Trace loaded successfully\n"
                  << "Samples: " << trace.cycles.size() << '\n'
                  << "Signals: " << trace.signal_names.size() << "\n\n";

        std::size_t signal_width = 16;
        for (const std::string& signal_name : trace.signal_names) {
            if (signal_name.size() > signal_width) {
                signal_width = signal_name.size();
            }
        }

        std::cout << "Switching Activity\n"
                  << std::left << std::setw(static_cast<int>(signal_width)) << "Signal"
                  << "  " << std::setw(11) << "Transitions"
                  << "  " << std::setw(6) << "Rising"
                  << "  " << std::setw(7) << "Falling"
                  << "  Activity\n";

        std::cout << std::fixed << std::setprecision(3);
        for (const SignalActivity& activity : activities) {
            std::cout << std::setw(static_cast<int>(signal_width)) << activity.signal_name
                      << "  " << std::setw(11) << activity.transitions
                      << "  " << std::setw(6) << activity.rising_transitions
                      << "  " << std::setw(7) << activity.falling_transitions
                      << "  " << activity.activity_ratio << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
