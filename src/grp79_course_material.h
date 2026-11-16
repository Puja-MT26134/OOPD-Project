// Group ID: 79
#ifndef ACADERP_COURSE_MATERIAL_H
#define ACADERP_COURSE_MATERIAL_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

// Teaching material of a course. clone() makes a deep copy through the
// concrete type, so a new instructor can edit their copy without touching
// the original (the Prototype pattern).
class CourseMaterial {
public:
    CourseMaterial(std::string courseCode, std::string owner, std::string title);
    virtual ~CourseMaterial() = default;
    CourseMaterial& operator=(const CourseMaterial&) = delete;

    const std::string& courseCode() const noexcept { return courseCode_; }
    const std::string& owner() const noexcept { return owner_; }
    const std::string& title() const noexcept { return title_; }
    void setOwner(std::string owner) { owner_ = std::move(owner); }
    void setTitle(std::string title) { title_ = std::move(title); }

    virtual std::string kind() const = 0;
    virtual std::unique_ptr<CourseMaterial> clone() const = 0;

protected:
    // Only the concrete classes may copy, and only from inside clone().
    CourseMaterial(const CourseMaterial&) = default;

private:
    std::string courseCode_;
    std::string owner_;
    std::string title_;
};

class Syllabus : public CourseMaterial {
public:
    Syllabus(std::string courseCode, std::string owner, std::string title);
    void addTopic(std::string topic) { topics_.push_back(std::move(topic)); }
    void setTopic(std::size_t index, std::string topic) {
        topics_.at(index) = std::move(topic);
    }
    const std::vector<std::string>& topics() const noexcept { return topics_; }
    std::string kind() const override { return "Syllabus"; }
    std::unique_ptr<CourseMaterial> clone() const override;

private:
    std::vector<std::string> topics_;
};

class LectureNotes : public CourseMaterial {
public:
    LectureNotes(std::string courseCode, std::string owner, std::string title);
    void addLecture(std::string text) { lectures_.push_back(std::move(text)); }
    void setLecture(std::size_t index, std::string text) {
        lectures_.at(index) = std::move(text);
    }
    const std::vector<std::string>& lectures() const noexcept { return lectures_; }
    std::string kind() const override { return "LectureNotes"; }
    std::unique_ptr<CourseMaterial> clone() const override;

private:
    std::vector<std::string> lectures_;
};

class Assignment : public CourseMaterial {
public:
    Assignment(std::string courseCode, std::string owner, std::string title);
    void addProblem(std::string problem) { problems_.push_back(std::move(problem)); }
    void setProblem(std::size_t index, std::string problem) {
        problems_.at(index) = std::move(problem);
    }
    const std::vector<std::string>& problems() const noexcept { return problems_; }
    std::string kind() const override { return "Assignment"; }
    std::unique_ptr<CourseMaterial> clone() const override;

private:
    std::vector<std::string> problems_;
};

// All the material of one course. copyFor() deep-copies every item and hands
// the copies to another faculty member.
class MaterialBundle {
public:
    void add(std::unique_ptr<CourseMaterial> item) { items_.push_back(std::move(item)); }
    const std::vector<std::unique_ptr<CourseMaterial>>& items() const noexcept {
        return items_;
    }
    MaterialBundle copyFor(const std::string& newOwner) const;

private:
    std::vector<std::unique_ptr<CourseMaterial>> items_;
};

#endif  // ACADERP_COURSE_MATERIAL_H