#ifndef REMINDER_THREAD_HPP
#define REMINDER_THREAD_HPP

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include "TaskManager.hpp"

class ReminderThread {
private:
    std::thread thread;
    std::atomic<bool> running;
    TaskManager* taskManager;

    void run() {
        while (running) {
            auto tasks = taskManager->getTasksNeedingReminder();
            for (const auto& task : tasks) {
                std::cout << "\n==================== 任务提醒 ====================" << std::endl;
                std::cout << "📢 任务 \"" << task.getName() << "\" 时间到了！" << std::endl;
                std::cout << "   开始时间: " << task.getStartTimeStr() << std::endl;
                std::cout << "   优先级: " << task.getPriorityStr() << std::endl;
                std::cout << "   分类: " << task.getCategoryStr() << std::endl;
                std::cout << "====================================================" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }

public:
    ReminderThread(TaskManager* tm) : taskManager(tm), running(true) {
        thread = std::thread(&ReminderThread::run, this);
    }

    ~ReminderThread() {
        stop();
    }

    void stop() {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
    }
};

#endif
