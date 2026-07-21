#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

enum class Priority { LOW, MEDIUM, HIGH };
enum class Category { STUDY, ENTERTAINMENT, LIFE };

class Task {
private:
    int id;
    std::string name;
    std::time_t startTime;
    Priority priority;
    Category category;
    std::time_t remindTime;
    bool reminded;

public:
    Task() : id(0), priority(Priority::MEDIUM), category(Category::LIFE), reminded(false) {}
    
    Task(int id, const std::string& name, std::time_t startTime, 
         Priority priority = Priority::MEDIUM, 
         Category category = Category::LIFE,
         std::time_t remindTime = 0)
        : id(id), name(name), startTime(startTime), priority(priority), 
          category(category), remindTime(remindTime), reminded(false) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::time_t getStartTime() const { return startTime; }
    Priority getPriority() const { return priority; }
    Category getCategory() const { return category; }
    std::time_t getRemindTime() const { return remindTime; }
    bool isReminded() const { return reminded; }

    void setId(int id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setStartTime(std::time_t time) { startTime = time; }
    void setPriority(Priority p) { priority = p; }
    void setCategory(Category c) { category = c; }
    void setRemindTime(std::time_t time) { remindTime = time; }
    void setReminded(bool r) { reminded = r; }

    std::string serialize() const {
        std::stringstream ss;
        ss << id << "|" << name << "|" << startTime << "|" 
           << static_cast<int>(priority) << "|" << static_cast<int>(category) << "|"
           << remindTime << "|" << (reminded ? "1" : "0");
        return ss.str();
    }

    void deserialize(const std::string& data) {
        std::stringstream ss(data);
        std::string token;
        int priorityInt, categoryInt;
        
        std::getline(ss, token, '|'); id = std::stoi(token);
        std::getline(ss, name, '|');
        std::getline(ss, token, '|'); startTime = std::stol(token);
        std::getline(ss, token, '|'); priorityInt = std::stoi(token);
        std::getline(ss, token, '|'); categoryInt = std::stoi(token);
        std::getline(ss, token, '|'); remindTime = std::stol(token);
        std::getline(ss, token, '|'); reminded = (token == "1");
        
        priority = static_cast<Priority>(priorityInt);
        category = static_cast<Category>(categoryInt);
    }

    std::string getPriorityStr() const {
        switch(priority) {
            case Priority::LOW: return "低";
            case Priority::MEDIUM: return "中";
            case Priority::HIGH: return "高";
            default: return "未知";
        }
    }

    std::string getCategoryStr() const {
        switch(category) {
            case Category::STUDY: return "学习";
            case Category::ENTERTAINMENT: return "娱乐";
            case Category::LIFE: return "生活";
            default: return "未知";
        }
    }

    std::string getTimeStr(std::time_t time) const {
        std::tm* tm = std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M");
        return ss.str();
    }

    std::string getStartTimeStr() const {
        return getTimeStr(startTime);
    }

    std::string getRemindTimeStr() const {
        if (remindTime > 0) {
            return getTimeStr(remindTime);
        }
        return "无提醒";
    }
};

#endif
