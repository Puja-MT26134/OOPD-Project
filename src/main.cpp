#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "command_parser.h"
#include "exceptions.h"
#include "registrar.h"

namespace {

// Reads one command per line. Every ErpException is caught by reference
// here, reported as "ERROR <line> <CODE>" on out and as a message on err,
// and the loop carries on with the next line.
int runScript(std::istream& in, std::ostream& out, std::ostream& err) {
    Registrar registrar;
    const CommandParser parser;
    std::string line;
    int lineNumber = 0;
    while (std::getline(in, line)) {
        ++lineNumber;   // blank and comment lines still count
        try {
            const std::unique_ptr<Command> command = parser.parse(line);
            if (command == nullptr) {
                continue;
            }
            const std::optional<std::string> result = command->execute(registrar);
            if (result.has_value()) {
                out << *result << '\n';
            }
        } catch (const ErpException& error) {
            out << "ERROR " << lineNumber << ' ' << error.code() << '\n';
            err << "line " << lineNumber << ": " << error.what() << '\n';
        }
    }
    return 0;
}

}  // namespace

int main() {
    try {
        return runScript(std::cin, std::cout, std::cerr);
    } catch (...) {   // the single outer guard allowed by the spec
        std::cerr << "FATAL\n";
        return 2;
    }
}