#ifndef ACADERP_GPA_H
#define ACADERP_GPA_H

#include <string>

class GpaAccumulator {
public:
    void add(int credits, int points) noexcept {
        numerator_ += static_cast<long long>(credits) * points;
        denominator_ += credits;
    }

    bool empty() const noexcept { return denominator_ == 0; }

    // GPA in hundredths, rounded half-up. Throws NoGpaDataException if empty.
    int hundredths() const;

    // 813 -> "8.13",  500 -> "5.00",  5 -> "0.05"
    static std::string format(int hundredths);

    // Letter whose band contains the already-rounded GPA.
    static std::string averageLetter(int hundredths);

private:
    long long numerator_ = 0;
    long long denominator_ = 0;
};

#endif  // ACADERP_GPA_H