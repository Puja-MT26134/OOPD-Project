#include "grade.h"

#include <utility>

#include "exceptions.h"

namespace {

struct Entry {
    const char* symbol;
    int points;
    Grade::Kind kind;
    bool pass;
};

// One table = one source of truth for every symbol the system knows.
const Entry kEntries[] = {
    {"A", 10, Grade::Kind::Letter, true},
    {"A-", 9, Grade::Kind::Letter, true},
    {"B", 8, Grade::Kind::Letter, true},
    {"B-", 7, Grade::Kind::Letter, true},
    {"C", 6, Grade::Kind::Letter, true},
    {"C-", 5, Grade::Kind::Letter, true},
    {"D", 4, Grade::Kind::Letter, true},
    {"E", 2, Grade::Kind::Letter, false},
    {"F", 0, Grade::Kind::Letter, false},
    {"I", 0, Grade::Kind::Incomplete, false},
    {"S", 0, Grade::Kind::Satisfactory, true},
    {"X", 0, Grade::Kind::Unsatisfactory, false},
    
};

}  // namespace

Grade::Grade(std::string symbol, int points, Kind kind, bool pass)
    : symbol_(std::move(symbol)), points_(points), kind_(kind), pass_(pass) {}

Grade Grade::fromSymbol(const std::string& symbol) {

    for (const auto& entry : kEntries) {
        if (symbol == entry.symbol) {
            return Grade(entry.symbol, entry.points, entry.kind, entry.pass);
        }
    }
    
     throw InvalidGradeSymbolException(
        "Invalid grade symbol: " + symbol
    );
}

bool Grade::isKnownSymbol(const std::string& symbol) noexcept {
    for (const auto& entry : kEntries) {
        if (symbol == entry.symbol) {
            return true;
        }
    }
    return false;
}

bool Grade::meetsMinimum(const Grade& minimum) const noexcept {

    // Letter grades: compare points
    if (kind_ == Kind::Letter &&
        minimum.kind_ == Kind::Letter) {
        return points_ >= minimum.points_;
    }

    // Non-letter grades: only S satisfies S
    return kind_ == Kind::Satisfactory &&
           minimum.kind_ == Kind::Satisfactory;
}