#ifndef REMINDER_THREAD_HPP
#define REMINDER_THREAD_HPP

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <cstdlib>
#include "TaskManager.hpp"

class ReminderThread {
private:
    std::thread thread;
    std::atomic<bool> running;
    TaskManager* taskManager;

    // ========== 语音播报任务信息 ==========
    void speakReminder(const Task& task) {
        // 构建播报内容
        std::string msg = "任务 " + task.getName() + 
                          "时间到了，优先级 " + task.getPriorityStr() +
                          "，分类 " + task.getCategoryStr();
        
        // 用 espeak 播报（中文）
        std::string cmd = "espeak -v zh -s 150 '" + msg + "' &";
        system(cmd.c_str());
        
        // 终端响铃（额外的提示音）
        std::cout << '\a' << std::flush;
    }

    // ========== 打印提醒信息 ==========
    void printReminder(const Task& task) {
        std::cout << "\n========================================================" << std::endl;
        std::cout << "                  任务提醒 " << std::endl;
        std::cout << "========================================================" << std::endl;
        std::cout << "   任务名称: " << task.getName() << std::endl;
        std::cout << "   开始时间: " << task.getStartTimeStr() << std::endl;
        std::cout << "   优先级: " << task.getPriorityStr() << std::endl;
        std::cout << "   分类: " << task.getCategoryStr() << std::endl;
        std::cout << "   提醒时间: " << task.getRemindTimeStr() << std::endl;
        std::cout << "========================================================" << std::endl;
        std::cout << std::flush;
    }

    // ========== 后台线程主循环 ==========
    void run() {
        while (running) {
            // 获取需要提醒的任务
            auto tasks = taskManager->getTasksNeedingReminder();
            
            for (const auto& task : tasks) {
                // 1. 语音播报任务信息
                speakReminder(task);
                
                // 2. 打印提醒信息
                printReminder(task);
                
                // 3. 等待一下再继续
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            
            // 每 30 秒检查一次
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }

public:
    // ========== 构造函数 ==========
    ReminderThread(TaskManager* tm) : taskManager(tm), running(true) {
        thread = std::thread(&ReminderThread::run, this);
    }

    // ========== 析构函数 ==========
    ~ReminderThread() {
        stop();
    }

    // ========== 停止线程 ==========
    void stop() {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
    }
};

#endif
