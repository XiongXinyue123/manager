#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <limits>
#include <cstdlib>
#include "UserManager.hpp"
#include "TaskManager.hpp"
#include "ReminderThread.hpp"

using namespace std;

// 函数声明
void printHelp();
time_t parseDateTime(const string& input);
time_t getTodayStart();
Priority parsePriority(const string& input);
Category parseCategory(const string& input);
bool interactiveAddTask(TaskManager& taskManager);
void interactiveShell(const string& username, const string& password);

// 主函数
int main(int argc, char* argv[]) {
    // 创建数据目录
    system("mkdir -p data");

    if (argc < 2) {
        printHelp();
        return 0;
    }

    string command = argv[1];

    if (command == "help" || command == "-h" || command == "--help") {
        printHelp();
        return 0;
    }

    // run 命令：交互模式
    if (command == "run") {
        string username, password;
        cout << "用户名: ";
        cin >> username;
        cout << "密码: ";
        cin >> password;
        cin.ignore();

        interactiveShell(username, password);
        return 0;
    }

    // 其他命令需要认证
    if (argc < 4) {
        cerr << "用法: " << argv[0] << " <用户名> <密码> <命令> [参数]" << endl;
        printHelp();
        return 1;
    }

    string username = argv[1];
    string password = argv[2];
    string cmd = argv[3];

    UserManager userManager;
    if (!userManager.login(username, password)) {
        cerr << "❌ 登录失败! 用户名或密码错误" << endl;
        return 1;
    }

    TaskManager taskManager(username);

    if (cmd == "addtask") {
        cin.ignore();
        if (interactiveAddTask(taskManager)) {
            cout << "✅ 任务添加成功!" << endl;
        } else {
            cerr << "❌ 任务添加失败!" << endl;
        }
    } else if (cmd == "showtask") {
        auto tasks = taskManager.getAllTasks();
        taskManager.displayTasks(tasks);
    } else if (cmd == "deltask") {
        if (argc < 5) {
            cerr << "用法: " << argv[0] << " " << username << " " << password << " deltask <id>" << endl;
            return 1;
        }
        int id = stoi(argv[4]);
        if (taskManager.deleteTask(id)) {
            cout << "✅ 任务删除成功!" << endl;
        } else {
            cerr << "❌ 任务删除失败! 未找到ID " << id << endl;
        }
    } else {
        cerr << "❌ 未知命令: " << cmd << endl;
        printHelp();
    }

    return 0;
}

// ==================== 辅助函数实现 ====================

void printHelp() {
    cout << "\n========================================" << endl;
    cout << "   📅 日程管理软件 - 使用说明" << endl;
    cout << "========================================" << endl;
    cout << "命令格式:" << endl;
    cout << "  myschedule run                           - 交互模式运行" << endl;
    cout << "  myschedule <用户名> <密码> addtask       - 添加任务（交互式）" << endl;
    cout << "  myschedule <用户名> <密码> showtask      - 显示所有任务" << endl;
    cout << "  myschedule <用户名> <密码> deltask <id>  - 删除指定ID任务" << endl;
    cout << "  myschedule help                          - 显示帮助" << endl;
    cout << "\n交互模式命令:" << endl;
    cout << "  add       - 添加任务" << endl;
    cout << "  show      - 显示所有任务" << endl;
    cout << "  showday   - 显示今天的任务" << endl;
    cout << "  showmonth - 显示本月任务" << endl;
    cout << "  del <id>  - 删除任务" << endl;
    cout << "  help      - 显示帮助" << endl;
    cout << "  exit      - 退出程序" << endl;
    cout << "========================================\n" << endl;
}

time_t parseDateTime(const string& input) {
    struct tm tm = {};
    stringstream ss(input);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        return -1;
    }
    return mktime(&tm);
}

time_t getTodayStart() {
    time_t now = time(nullptr);
    struct tm* tm = localtime(&now);
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    return mktime(tm);
}

Priority parsePriority(const string& input) {
    if (input == "高" || input == "high") return Priority::HIGH;
    if (input == "中" || input == "medium") return Priority::MEDIUM;
    if (input == "低" || input == "low") return Priority::LOW;
    return Priority::MEDIUM;
}

Category parseCategory(const string& input) {
    if (input == "学习" || input == "study") return Category::STUDY;
    if (input == "娱乐" || input == "entertainment") return Category::ENTERTAINMENT;
    if (input == "生活" || input == "life") return Category::LIFE;
    return Category::LIFE;
}

bool interactiveAddTask(TaskManager& taskManager) {
    string name, startTimeStr, priorityStr, categoryStr, remindTimeStr;

    cout << "任务名称: ";
    getline(cin, name);
    if (name.empty()) return false;

    cout << "开始时间 (格式: YYYY-MM-DD HH:MM): ";
    getline(cin, startTimeStr);
    time_t startTime = parseDateTime(startTimeStr);
    if (startTime == -1) {
        cerr << "❌ 无效的时间格式!" << endl;
        return false;
    }

    cout << "优先级 (高/中/低, 默认: 中): ";
    getline(cin, priorityStr);
    Priority priority = parsePriority(priorityStr);

    cout << "分类 (学习/娱乐/生活, 默认: 生活): ";
    getline(cin, categoryStr);
    Category category = parseCategory(categoryStr);

    cout << "提醒时间 (格式: YYYY-MM-DD HH:MM, 留空则无提醒): ";
    getline(cin, remindTimeStr);
    time_t remindTime = 0;
    if (!remindTimeStr.empty()) {
        remindTime = parseDateTime(remindTimeStr);
        if (remindTime == -1) {
            cerr << "❌ 无效的时间格式!" << endl;
            return false;
        }
    }

    return taskManager.addTask(name, startTime, priority, category, remindTime);
}

void interactiveShell(const string& username, const string& password) {
    UserManager userManager;
    if (!userManager.login(username, password)) {
        cerr << "❌ 登录失败!" << endl;
        return;
    }

    TaskManager taskManager(username);
    ReminderThread reminder(&taskManager);

    cout << "\n✅ 欢迎 " << username << "!" << endl;
    cout << "输入 help 查看可用命令\n" << endl;

    string command;
    while (true) {
        cout << "schedule> ";
        getline(cin, command);

        if (command == "exit" || command == "quit") {
            break;
        } else if (command == "help") {
            printHelp();
        } else if (command == "add") {
            if (interactiveAddTask(taskManager)) {
                cout << "✅ 任务添加成功!" << endl;
            } else {
                cerr << "❌ 任务添加失败!" << endl;
            }
        } else if (command == "show") {
            auto tasks = taskManager.getAllTasks();
            taskManager.displayTasks(tasks);
        } else if (command == "showday") {
            auto tasks = taskManager.getTasksByDay(getTodayStart());
            taskManager.displayTasks(tasks);
        } else if (command == "showmonth") {
            time_t now = time(nullptr);
            struct tm* tm = localtime(&now);
            auto tasks = taskManager.getTasksByMonth(tm->tm_year + 1900, tm->tm_mon + 1);
            taskManager.displayTasks(tasks);
        } else if (command.substr(0, 3) == "del") {
            if (command.length() > 4) {
                int id = stoi(command.substr(4));
                if (taskManager.deleteTask(id)) {
                    cout << "✅ 任务删除成功!" << endl;
                } else {
                    cerr << "❌ 任务删除失败! 未找到ID " << id << endl;
                }
            } else {
                cerr << "用法: del <id>" << endl;
            }
        } else if (command.empty()) {
            // 忽略空输入
        } else {
            cerr << "❌ 未知命令: " << command << " (输入 help 查看帮助)" << endl;
        }
    }

    cout << "👋 再见!" << endl;
}

