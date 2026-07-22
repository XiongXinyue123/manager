#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Task.hpp"

class TaskManager {
private:
    std::vector<Task> tasks;
    int nextId = 1;
    std::string dataFile;
    mutable std::mutex taskMutex;  // mutable 允许在const函数中加锁

    std::string getTaskFileName(const std::string& username) {
        return "data/tasks_" + username + ".dat";
    }

public:
    TaskManager(const std::string& username) {
        dataFile = getTaskFileName(username);
        loadTasks();
    }

    // 添加任务
    bool addTask(const std::string& name, std::time_t startTime,
                 Priority priority = Priority::MEDIUM,
                 Category category = Category::LIFE,
                 std::time_t remindTime = 0) {
        std::lock_guard<std::mutex> lock(taskMutex);

        // 检查开始时间是否唯一
        for (const auto& task : tasks) {
            if (task.getStartTime() == startTime) {
                std::cerr << "错误: 开始时间已存在!" << std::endl;
                return false;
            }
        }

        // 检查 任务名称+开始时间 是否唯一
        for (const auto& task : tasks) {
            if (task.getName() == name && task.getStartTime() == startTime) {
                std::cerr << "错误: 任务名称和开始时间组合已存在!" << std::endl;
                return false;
            }
        }

        Task task(nextId++, name, startTime, priority, category, remindTime);
        tasks.push_back(task);
        saveTasks();
        return true;
    }

    // 删除任务
    bool deleteTask(int id) {
        std::lock_guard<std::mutex> lock(taskMutex);
        auto it = std::find_if(tasks.begin(), tasks.end(),
            [id](const Task& t) { return t.getId() == id; });

        if (it == tasks.end()) {
            return false;
        }

        tasks.erase(it);
        saveTasks();
        return true;
    }

    // 获取某天的任务
    std::vector<Task> getTasksByDay(std::time_t day) const {
        std::lock_guard<std::mutex> lock(taskMutex);
        std::vector<Task> result;
        std::tm* dayTm = std::localtime(&day);

        for (const auto& task : tasks) {
            std::tm* taskTm = std::localtime(&task.getStartTime());
            if (taskTm->tm_year == dayTm->tm_year &&
                taskTm->tm_mon == dayTm->tm_mon &&
                taskTm->tm_mday == dayTm->tm_mday) {
                result.push_back(task);
            }
        }

        std::sort(result.begin(), result.end(),
            [](const Task& a, const Task& b) {
                return a.getStartTime() < b.getStartTime();
            });

        return result;
    }

    // 获取某月的任务
    std::vector<Task> getTasksByMonth(int year, int month) const {
        std::lock_guard<std::mutex> lock(taskMutex);
        std::vector<Task> result;

        for (const auto& task : tasks) {
            std::tm* tm = std::localtime(&task.getStartTime());
            if (tm->tm_year == year - 1900 && tm->tm_mon == month - 1) {
                result.push_back(task);
            }
        }

        std::sort(result.begin(), result.end(),
            [](const Task& a, const Task& b) {
                return a.getStartTime() < b.getStartTime();
            });

        return result;
    }

    // 获取所有需要提醒的任务（提醒时间到了且还没提醒过）
    std::vector<Task> getTasksNeedingReminder() {
        std::lock_guard<std::mutex> lock(taskMutex);
        std::vector<Task> result;
        std::time_t now = std::time(nullptr);

        for (auto& task : tasks) {
            if (!task.isReminded() && task.getRemindTime() > 0 &&
                task.getRemindTime() <= now) {
                result.push_back(task);
                task.setReminded(true);
            }
        }

        if (!result.empty()) {
            saveTasks();
        }
        return result;
    }

    // 显示任务列表（格式化输出）
    void displayTasks(const std::vector<Task>& tasks) const {
        if (tasks.empty()) {
            std::cout << "没有找到任务" << std::endl;
            return;
        }

        std::cout << "+-----+---------------------------+---------------------+----------+----------+---------------------+" << std::endl;
        std::cout << "| ID  | 任务名称                  | 开始时间            | 优先级   | 分类     | 提醒时间            |" << std::endl;
        std::cout << "+-----+---------------------------+---------------------+----------+----------+---------------------+" << std::endl;

        for (const auto& task : tasks) {
            std::cout << "| " << std::setw(3) << task.getId() << " | "
                      << std::setw(25) << task.getName() << " | "
                      << std::setw(19) << task.getStartTimeStr() << " | "
                      << std::setw(8) << task.getPriorityStr() << " | "
                      << std::setw(8) << task.getCategoryStr() << " | "
                      << std::setw(19) << task.getRemindTimeStr() << " |"
                      << std::endl;
        }
        std::cout << "+-----+---------------------------+---------------------+----------+----------+---------------------+" << std::endl;
    }

    // 从文件加载任务
    void loadTasks() {
        std::ifstream file(dataFile);
        if (!file.is_open()) return;

        tasks.clear();
        std::string line;
        while (std::getline(file, line)) {
            Task task;
            task.deserialize(line);
            tasks.push_back(task);
            if (task.getId() >= nextId) {
                nextId = task.getId() + 1;
            }
        }
        file.close();
    }

    // 保存任务到文件
    void saveTasks() {
        std::ofstream file(dataFile);
        if (!file.is_open()) {
            std::cerr << "无法保存任务数据!" << std::endl;
            return;
        }

        for (const auto& task : tasks) {
            file << task.serialize() << std::endl;
        }
        file.close();
    }

    // 获取所有任务
    std::vector<Task> getAllTasks() const {
        std::lock_guard<std::mutex> lock(taskMutex);
        return tasks;
    }
};

#endif
