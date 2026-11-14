#ifndef ACADERP_TERM_H
#define ACADERP_TERM_H

#include <string>

class Term {
public:
    // Parses "YYYY-S" (S = 1, 2 or 3). Throws ParseException otherwise.
    static Term parse(const std::string& text);

    int year() const noexcept { return year_; }
    int semester() const noexcept { return semester_; }
    std::string toString() const;

    bool operator==(const Term& o) const noexcept { return key() == o.key(); }
    bool operator!=(const Term& o) const noexcept { return key() != o.key(); }
    bool operator<(const Term& o) const noexcept { return key() < o.key(); }
    bool operator<=(const Term& o) const noexcept { return key() <= o.key(); }
    bool operator>(const Term& o) const noexcept { return key() > o.key(); }
    bool operator>=(const Term& o) const noexcept { return key() >= o.key(); }

private:
    Term(int year, int semester) noexcept : year_(year), semester_(semester) {}
    int key() const noexcept { return year_ * 10 + semester_; }

    int year_;
    int semester_;
};

#endif  // ACADERP_TERM_H