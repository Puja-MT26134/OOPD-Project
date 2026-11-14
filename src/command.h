#ifndef ACADERP_COMMAND_H
#define ACADERP_COMMAND_H

#include <optional>
#include <string>

#include "course.h"
#include "program_rules.h"
#include "term.h"

class Registrar;

// One parsed script line. execute() returns the single output line of a
// query, or nothing for a mutating command.
class Command {
public:
    virtual ~Command() = default;
    virtual std::optional<std::string> execute(Registrar& registrar) const = 0;
};

class ProgramCommand : public Command {
public:
    explicit ProgramCommand(ProgramRules rules);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    ProgramRules rules_;
};

class CourseCommand : public Command {
public:
    explicit CourseCommand(CourseSpec spec);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    CourseSpec spec_;
};

class StudentCommand : public Command {
public:
    StudentCommand(std::string roll, std::string name);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    std::string roll_;
    std::string name_;
};

class RegisterCommand : public Command {
public:
    RegisterCommand(std::string roll, std::string courseCode, Term term);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    std::string roll_;
    std::string courseCode_;
    Term term_;
};

class GradeCommand : public Command {
public:
    GradeCommand(std::string roll, std::string courseCode, Term term,
                 std::string symbol);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    std::string roll_;
    std::string courseCode_;
    Term term_;
    std::string symbol_;
};

class EligibleCommand : public Command {
public:
    EligibleCommand(std::string roll, std::string courseCode, Term term);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    std::string roll_;
    std::string courseCode_;
    Term term_;
};

   class ConvertCommand : public Command {
   public:
       ConvertCommand(std::string roll, std::string courseCode, std::string symbol);
       std::optional<std::string> execute(Registrar& registrar) const override;

   private:
       std::string roll_;
       std::string courseCode_;
       std::string symbol_;
   };

class GpaCommand : public Command {
public:
    explicit GpaCommand(std::string roll);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    std::string roll_;
};

class AvgCommand : public Command {
public:
    explicit AvgCommand(std::string roll);
    std::optional<std::string> execute(Registrar& registrar) const override;

private:
    std::string roll_;
};

#endif  // ACADERP_COMMAND_H