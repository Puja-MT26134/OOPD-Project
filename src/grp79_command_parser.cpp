// Group ID: 79
#include "grp79_command_parser.h"

#include <cctype>
#include <map>
#include <utility>
#include <vector>

#include "grp79_exceptions.h"

namespace {

using Tokens = std::vector<std::string>;

// Splits on spaces, tabs and '\r' (so CRLF files work).
Tokens tokenize(const std::string& line) {
    Tokens tokens;
    std::string current;
    for (char ch : line) {
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(ch);
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

bool allDigits(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    for (char ch : text) {
        if (std::isdigit(static_cast<unsigned char>(ch)) == 0) {
            return false;
        }
    }
    return true;
}

bool startsWith(const std::string& text, const std::string& prefix) {
    return text.compare(0, prefix.size(), prefix) == 0;
}

Tokens split(const std::string& text, char separator) {
    Tokens parts;
    std::string current;
    for (char ch : text) {
        if (ch == separator) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    parts.push_back(current);
    return parts;
}

void requireCount(const Tokens& tokens, std::size_t count) {
    if (tokens.size() != count) {
        throw ParseException(tokens[0] + " expects " + std::to_string(count - 1) +
                             " argument(s), got " +
                             std::to_string(tokens.size() - 1));
    }
}

// Optional minus sign, then 1 to 9 digits.
int parseInteger(const std::string& text, const std::string& what) {
    const std::size_t start = (!text.empty() && text[0] == '-') ? 1 : 0;
    const std::string digits = text.substr(start);
    if (!allDigits(digits) || digits.size() > 9) {
        throw ParseException(what + " must be an integer, got '" + text + "'");
    }
    return std::stoi(text);
}

int parseNonNegative(const std::string& text, const std::string& what) {
    const int value = parseInteger(text, what);
    if (value < 0) {
        throw ParseException(what + " must not be negative");
    }
    return value;
}

// "5", "5.0" or "5.00" -> 500; at most 10.00. Integer arithmetic only.
int parseHundredths(const std::string& text, const std::string& what) {
    const std::size_t dot = text.find('.');
    const std::string whole = text.substr(0, dot);
    const std::string fraction =
        dot == std::string::npos ? "" : text.substr(dot + 1);
    const bool shapeOk =
        allDigits(whole) && whole.size() <= 2 && fraction.size() <= 2 &&
        (dot == std::string::npos || !fraction.empty()) &&
        (fraction.empty() || allDigits(fraction));
    if (!shapeOk) {
        throw ParseException(what + " must look like d.dd, got '" + text + "'");
    }
    int value = std::stoi(whole) * 100;
    if (fraction.size() == 1) {
        value += (fraction[0] - '0') * 10;
    } else if (fraction.size() == 2) {
        value += std::stoi(fraction);
    }
    if (value > 1000) {
        throw ParseException(what + " must be at most 10.00");
    }
    return value;
}

// ------------------------------------------------------------- builders

std::unique_ptr<Command> buildProgram(const Tokens& tokens) {
    ProgramRules rules;
    for (std::size_t i = 1; i < tokens.size(); ++i) {
        const std::size_t eq = tokens[i].find('=');
        if (eq == std::string::npos || eq == 0 || eq + 1 == tokens[i].size()) {
            throw ParseException("PROGRAM expects key=value, got '" + tokens[i] +
                                 "'");
        }
        const std::string key = tokens[i].substr(0, eq);
        const std::string value = tokens[i].substr(eq + 1);
        if (key == "total") {
            rules.totalCredits = parseNonNegative(value, key);
        } else if (key == "elective") {
            rules.electiveCredits = parseNonNegative(value, key);
        } else if (key == "mingpa") {
            rules.minGpaHundredths = parseHundredths(value, key);
        } else if (key == "thesis_min_grade") {
            if (!Grade::isKnownSymbol(value) ||
                !Grade::fromSymbol(value).isLetter()) {
                throw ParseException("thesis_min_grade must be a letter grade");
            }
            rules.thesisMinGrade = Grade::fromSymbol(value);
        } else if (key == "thesis_min_credits") {
            rules.thesisMinCredits = parseNonNegative(value, key);
        } else if (key == "thesis_min_gpa") {
            rules.thesisMinGpaHundredths = parseHundredths(value, key);
        } else if (key == "maxload") {
            rules.maxLoad = parseNonNegative(value, key);
        } else {
            throw ParseException("bad PROGRAM key '" + key + "'");
        }
    }
    return std::make_unique<ProgramCommand>(rules);
}

CourseType parseCourseType(const std::string& text) {
    if (text == "REGULAR") return CourseType::Regular;
    if (text == "SU") return CourseType::Su;
    if (text == "THESIS") return CourseType::Thesis;
    throw ParseException("unknown course type '" + text + "'");
}

Category parseCategory(const std::string& text) {
    if (text == "CORE") return Category::Core;
    if (text == "ELECTIVE") return Category::Elective;
    if (text == "MANDATORY_SU") return Category::MandatorySu;
    if (text == "OPEN") return Category::Open;
    throw ParseException("unknown category '" + text + "'");
}

// "CS101:C,CS102" -> [{CS101,C}, {CS102,""}]
std::vector<PrerequisiteSpec> parsePrerequisites(const std::string& value) {
    std::vector<PrerequisiteSpec> result;
    for (const std::string& item : split(value, ',')) {
        const std::size_t colon = item.find(':');
        const std::string code = item.substr(0, colon);
        const std::string minimum =
            colon == std::string::npos ? "" : item.substr(colon + 1);
        if (code.empty() || (colon != std::string::npos && minimum.empty())) {
            throw ParseException("bad PRE entry '" + item + "'");
        }
        result.push_back(PrerequisiteSpec{code, minimum});
    }
    return result;
}

std::vector<std::string> parseAntiRequisites(const std::string& value) {
    std::vector<std::string> result = split(value, ',');
    for (const std::string& code : result) {
        if (code.empty()) {
            throw ParseException("empty code in ANTI list");
        }
    }
    return result;
}

std::unique_ptr<Command> buildCourse(const Tokens& tokens) {
    if (tokens.size() < 5 || tokens.size() > 7) {
        throw ParseException("COURSE expects 4 to 6 arguments, got " +
                             std::to_string(tokens.size() - 1));
    }
    CourseSpec spec{tokens[1],
                    parseCourseType(tokens[2]),
                    parseInteger(tokens[3], "credits"),
                    parseCategory(tokens[4]),
                    {},
                    {}};
    bool sawPre = false;
    bool sawAnti = false;
    for (std::size_t i = 5; i < tokens.size(); ++i) {
        if (startsWith(tokens[i], "PRE=") && !sawPre) {
            sawPre = true;
            spec.prerequisites = parsePrerequisites(tokens[i].substr(4));
        } else if (startsWith(tokens[i], "ANTI=") && !sawAnti) {
            sawAnti = true;
            spec.antiRequisites = parseAntiRequisites(tokens[i].substr(5));
        } else {
            throw ParseException("unexpected COURSE option '" + tokens[i] + "'");
        }
    }
    return std::make_unique<CourseCommand>(std::move(spec));
}

std::unique_ptr<Command> buildStudent(const Tokens& tokens) {
    requireCount(tokens, 3);
    return std::make_unique<StudentCommand>(tokens[1], tokens[2]);
}

std::unique_ptr<Command> buildRegister(const Tokens& tokens) {
    requireCount(tokens, 4);
    return std::make_unique<RegisterCommand>(tokens[1], tokens[2],
                                             Term::parse(tokens[3]));
}

std::unique_ptr<Command> buildGrade(const Tokens& tokens) {
    requireCount(tokens, 5);
    return std::make_unique<GradeCommand>(tokens[1], tokens[2],
                                          Term::parse(tokens[3]), tokens[4]);
}

   std::unique_ptr<Command> buildConvert(const Tokens& tokens) {
       requireCount(tokens, 4);
       return std::make_unique<ConvertCommand>(tokens[1], tokens[2], tokens[3]);
   }

std::unique_ptr<Command> buildEligible(const Tokens& tokens) {
    requireCount(tokens, 4);
    return std::make_unique<EligibleCommand>(tokens[1], tokens[2],
                                             Term::parse(tokens[3]));
}

std::unique_ptr<Command> buildGrad(const Tokens& tokens) {
    requireCount(tokens, 2);
    return std::make_unique<GradCommand>(tokens[1]);
}

std::unique_ptr<Command> buildTranscript(const Tokens& tokens) {
    requireCount(tokens, 2);
    return std::make_unique<TranscriptCommand>(tokens[1]);
}


std::unique_ptr<Command> buildGpa(const Tokens& tokens) {
    requireCount(tokens, 2);
    return std::make_unique<GpaCommand>(tokens[1]);
}

std::unique_ptr<Command> buildAvg(const Tokens& tokens) {
    requireCount(tokens, 2);
    return std::make_unique<AvgCommand>(tokens[1]);
}

}  // namespace

std::unique_ptr<Command> CommandParser::parse(const std::string& line) const {
    const Tokens tokens = tokenize(line);
    if (tokens.empty() || tokens[0][0] == '#') {
        return nullptr;   // blank or comment line
    }
    using Builder = std::unique_ptr<Command> (*)(const Tokens&);
    // Adding a command = one builder function + one entry in this table.
    static const std::map<std::string, Builder> kBuilders = {
        {"PROGRAM", buildProgram},   {"COURSE", buildCourse},
        {"STUDENT", buildStudent},   {"REGISTER", buildRegister},
        {"GRADE", buildGrade},       {"CONVERT", buildConvert},
        {"ELIGIBLE", buildEligible}, {"GRAD", buildGrad},
        {"GPA", buildGpa},           {"AVG", buildAvg},
        {"TRANSCRIPT", buildTranscript},
    };
    const auto it = kBuilders.find(tokens[0]);
    if (it == kBuilders.end()) {
        throw ParseException("unknown command '" + tokens[0] + "'");
    }
    return it->second(tokens);
}