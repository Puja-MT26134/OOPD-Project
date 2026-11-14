#include "gpa.h"

#include "exceptions.h"

int GpaAccumulator::hundredths() const {
     if (denominator_ == 0) {
        throw NoGpaDataException("No GPA data available");
    }

    return static_cast<int>(
        (200 * numerator_ + denominator_) /
        (2 * denominator_)
    );
}

std::string GpaAccumulator::format(int hundredths) {
     int whole = hundredths / 100;
    int fraction = hundredths % 100;

    if (fraction < 10) {
        return std::to_string(whole) + ".0" + std::to_string(fraction);
    }

    return std::to_string(whole) + "." + std::to_string(fraction);
}

std::string GpaAccumulator::averageLetter(int hundredths) {
      struct Band {
        int lowerEdge;
        const char* letter;
    };

    static const Band bands[] = {
        {950, "A"},
        {850, "A-"},
        {750, "B"},
        {650, "B-"},
        {550, "C"},
        {450, "C-"},
        {350, "D"},
        {100, "E"}
    };

    for (const auto& band : bands) {
        if (hundredths >= band.lowerEdge) {
            return band.letter;
        }
    }

    return "F";
}