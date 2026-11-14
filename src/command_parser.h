#ifndef ACADERP_COMMAND_PARSER_H
#define ACADERP_COMMAND_PARSER_H

#include <memory>
#include <string>

#include "command.h"

// Turns one script line into a Command object.
class CommandParser {
public:
    // Returns nullptr for blank and comment lines. Throws ParseException for
    // an unknown command, wrong token count or a malformed value. Never
    // touches any program state, so a failed parse changes nothing.
    std::unique_ptr<Command> parse(const std::string& line) const;
};

#endif  // ACADERP_COMMAND_PARSER_H