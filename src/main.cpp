#include "TraceParser.h"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <trace.csv>\n";
        return 1;
    }

    try {
        const TraceParser parser;
        const TraceData trace = parser.parse(argv[1]);

        std::cout << "Trace loaded successfully\n"
                  << "Samples: " << trace.cycles.size() << '\n'
                  << "Signals: " << trace.signal_names.size() << "\n\n"
                  << "Signals:\n";

        for (const std::string& signal_name : trace.signal_names) {
            std::cout << signal_name << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
