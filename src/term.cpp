#include "term.h"

#include <cctype>

#include "exceptions.h"

Term Term::parse(const std::string& text) {
     bool valid = true;

    // Must be YYYY-S
    if (text.size() != 6)
        valid = false;

    if (valid && text[4] != '-')
        valid = false;

    // First 4 chars must be digits
    for (int i = 0; valid && i < 4; ++i) {
        if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
            valid = false;
        }
    }

    // Semester must be 1, 2, or 3
    if (valid && text[5] != '1' &&
        text[5] != '2' &&
        text[5] != '3') {
        valid = false;
    }

    if (!valid) {
        throw ParseException("bad term '" + text + "'");
    }

    return Term(
        std::stoi(text.substr(0, 4)),
        text[5] - '0'
    );

}

std::string Term::toString() const {
    std::string year = std::to_string(year_);
    if (year.size() < 4) {
        year.insert(0, 4 - year.size(), '0');   // pad on the left: 24 -> "0024"
    }
    return year + "-" + std::to_string(semester_);
}