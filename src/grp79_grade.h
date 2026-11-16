// Group ID: 79
#ifndef ACADERP_GRADE_H
#define ACADERP_GRADE_H

#include <string>

class Grade {
public:
    enum class Kind { Letter, Incomplete, Satisfactory, Unsatisfactory };

    // Throws InvalidGradeSymbolException for anything not on a scale.
    static Grade fromSymbol(const std::string& symbol);


    // True when the symbol is on a scale. Never throws.
    static bool isKnownSymbol(const std::string& symbol) noexcept;
    
    const std::string& symbol() const noexcept { return symbol_; }
    int points() const noexcept { return points_; }

    bool isLetter() const noexcept { return kind_ == Kind::Letter; }
    bool isIncomplete() const noexcept { return kind_ == Kind::Incomplete; }
    bool isSatisfactory() const noexcept { return kind_ == Kind::Satisfactory; }
    bool isUnsatisfactory() const noexcept { return kind_ == Kind::Unsatisfactory; }
    bool isFinal() const noexcept { return kind_ != Kind::Incomplete; }
    bool isPass() const noexcept { return pass_; }

    // True when this grade is at or above `minimum`.
    // Two letters compare by points; S satisfies only a minimum of S.
    bool meetsMinimum(const Grade& minimum) const noexcept;

private:
    Grade(std::string symbol, int points, Kind kind, bool pass);

    std::string symbol_;
    int points_;
    Kind kind_;
    bool pass_;
};

#endif  // ACADERP_GRADE_H