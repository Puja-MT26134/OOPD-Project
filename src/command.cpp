#include "command.h"

#include <utility>

#include "gpa.h"
#include "registrar.h"

ProgramCommand::ProgramCommand(ProgramRules rules) : rules_(std::move(rules)) {}

std::optional<std::string> ProgramCommand::execute(Registrar& registrar) const {
    registrar.setProgram(rules_);
    return std::nullopt;
}

CourseCommand::CourseCommand(CourseSpec spec) : spec_(std::move(spec)) {}

std::optional<std::string> CourseCommand::execute(Registrar& registrar) const {
    registrar.addCourse(spec_);
    return std::nullopt;
}

StudentCommand::StudentCommand(std::string roll, std::string name)
    : roll_(std::move(roll)), name_(std::move(name)) {}

std::optional<std::string> StudentCommand::execute(Registrar& registrar) const {
    registrar.addStudent(roll_, name_);
    return std::nullopt;
}

RegisterCommand::RegisterCommand(std::string roll, std::string courseCode,
                                 Term term)
    : roll_(std::move(roll)), courseCode_(std::move(courseCode)), term_(term) {}

std::optional<std::string> RegisterCommand::execute(Registrar& registrar) const {
    registrar.registerCourse(roll_, courseCode_, term_);
    return std::nullopt;
}

GradeCommand::GradeCommand(std::string roll, std::string courseCode, Term term,
                           std::string symbol)
    : roll_(std::move(roll)),
      courseCode_(std::move(courseCode)),
      term_(term),
      symbol_(std::move(symbol)) {}

std::optional<std::string> GradeCommand::execute(Registrar& registrar) const {
    registrar.recordGrade(roll_, courseCode_, term_, symbol_);
    return std::nullopt;
}

TranscriptCommand::TranscriptCommand(std::string roll) : roll_(std::move(roll)) {}

std::optional<std::string> TranscriptCommand::execute(Registrar& registrar) const {
    return "TRANSCRIPT " + roll_ + " " + registrar.transcriptLine(roll_);
}


GradCommand::GradCommand(std::string roll) : roll_(std::move(roll)) {}

std::optional<std::string> GradCommand::execute(Registrar& registrar) const {
    return "GRAD " + roll_ + " " + registrar.graduationVerdict(roll_);
}

EligibleCommand::EligibleCommand(std::string roll, std::string courseCode, Term term)
    : roll_(std::move(roll)), courseCode_(std::move(courseCode)), term_(term) {}

std::optional<std::string> EligibleCommand::execute(Registrar& registrar) const {
    return "ELIGIBLE " + roll_ + " " + courseCode_ + " " +
           registrar.eligibilityVerdict(roll_, courseCode_, term_);
}

   ConvertCommand::ConvertCommand(std::string roll, std::string courseCode,
                                  std::string symbol)
       : roll_(std::move(roll)),
         courseCode_(std::move(courseCode)),
         symbol_(std::move(symbol)) {}

   std::optional<std::string> ConvertCommand::execute(Registrar& registrar) const {
       registrar.convertThesis(roll_, courseCode_, symbol_);
       return std::nullopt;
   }

GpaCommand::GpaCommand(std::string roll) : roll_(std::move(roll)) {}

std::optional<std::string> GpaCommand::execute(Registrar& registrar) const {
    return "GPA " + roll_ + " " +
           GpaAccumulator::format(registrar.gpaHundredths(roll_));
}

AvgCommand::AvgCommand(std::string roll) : roll_(std::move(roll)) {}

std::optional<std::string> AvgCommand::execute(Registrar& registrar) const {
    return "AVG " + roll_ + " " + registrar.averageLetter(roll_);
}