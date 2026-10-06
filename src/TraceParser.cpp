#include "TraceParser.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string trim(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }

    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::vector<std::string> splitCsvRow(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream row(line);
    std::string field;

    while (std::getline(row, field, ',')) {
        fields.push_back(trim(field));
    }

    // getline does not return the empty field after a final comma.
    if (!line.empty() && line.back() == ',') {
        fields.push_back("");
    }

    return fields;
}

} // namespace

TraceData TraceParser::parse(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open trace file '" + file_path + "'.");
    }

    std::string line;
    if (!std::getline(file, line)) {
        if (file.bad()) {
            throw std::runtime_error("Could not read trace file '" + file_path + "'.");
        }
        throw std::runtime_error("Trace file is empty.");
    }

    const std::vector<std::string> header = splitCsvRow(line);
    if (header.size() < 2) {
        throw std::runtime_error(
            "Line 1: header must contain 'cycle' and at least one signal name.");
    }
    if (header[0] != "cycle") {
        throw std::runtime_error("Line 1: first header column must be 'cycle'.");
    }

    TraceData trace;
    for (std::size_t column = 1; column < header.size(); ++column) {
        const std::string& name = header[column];
        if (name.empty()) {
            throw std::runtime_error("Line 1, column " + std::to_string(column + 1)
                                     + ": signal name must not be empty.");
        }
        if (name.find('"') != std::string::npos) {
            throw std::runtime_error("Line 1: quoted signal names are not supported.");
        }
        if (std::find(trace.signal_names.begin(), trace.signal_names.end(), name)
            != trace.signal_names.end()) {
            throw std::runtime_error("Line 1: duplicate signal name '" + name + "'.");
        }
        trace.signal_names.push_back(name);
    }
    trace.signal_values.resize(trace.signal_names.size());

    std::size_t line_number = 1;
    while (std::getline(file, line)) {
        ++line_number;
        const std::vector<std::string> fields = splitCsvRow(line);
        const std::string location = "Line " + std::to_string(line_number);

        if (fields.size() != header.size()) {
            throw std::runtime_error(location + ": expected "
                                     + std::to_string(header.size()) + " columns, found "
                                     + std::to_string(fields.size()) + ".");
        }

        const std::string& cycle_text = fields[0];
        if (cycle_text.empty()
            || cycle_text.find_first_not_of("0123456789") != std::string::npos) {
            throw std::runtime_error(location + ": invalid cycle number '" + cycle_text
                                     + "'; expected a non-negative decimal integer.");
        }

        unsigned long long cycle;
        try {
            cycle = std::stoull(cycle_text);
        } catch (const std::out_of_range&) {
            throw std::runtime_error(location + ": cycle number '" + cycle_text
                                     + "' is too large.");
        }

        for (std::size_t column = 1; column < fields.size(); ++column) {
            if (fields[column] != "0" && fields[column] != "1") {
                throw std::runtime_error(location + ": invalid value '" + fields[column]
                                         + "' for signal '" + trace.signal_names[column - 1]
                                         + "'; expected 0 or 1.");
            }
        }

        // Store a sample only after every field in the row has been checked.
        trace.cycles.push_back(cycle);
        for (std::size_t column = 1; column < fields.size(); ++column) {
            trace.signal_values[column - 1].push_back(fields[column] == "1" ? 1 : 0);
        }
    }

    if (file.bad()) {
        throw std::runtime_error("Could not read trace file '" + file_path + "'.");
    }
    if (trace.cycles.empty()) {
        throw std::runtime_error("Trace contains a header but no data rows.");
    }

    return trace;
}
