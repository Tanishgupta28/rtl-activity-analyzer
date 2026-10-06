#ifndef TRACE_PARSER_H
#define TRACE_PARSER_H

#include "TraceData.h"

#include <string>

class TraceParser {
public:
    // Return the complete trace, or throw std::runtime_error for invalid input.
    TraceData parse(const std::string& file_path) const;
};

#endif
