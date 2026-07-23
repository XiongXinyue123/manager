#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "CryptoUtils.hpp"

class UserManager {
private:
    std::unordered_map<std::string, std::string> users;  // 用户名 -> 密码HASH
    std::string currentUser;
    const std::string USER_FILE = "data/users.dat";

    void loadUsers() {
        FILE* fp = fopen(USER_FILE.c_str(), "r");
        if (fp == NULL) {
            return;  // 文件不存在
        }
        
        users.clear();
        char line[512];
        while (fgets(line, sizeof(line), fp) != NULL) {
            // 去掉换行符
            size_t len = strlen(line);
            if (len > 0 && line[len-1] == '\n') {
                line[len-1] = '\0';
            }
            
            // 解析：用户名|密码HASH
            std::string str(line);
            size_t pos = str.find('|');
            if (pos != std::string::npos) {
                std::string username = str.substr(0, pos);
                std::string hash = str.substr(pos + 1);
                users[username] = hash;
            }
        }
        fclose(fp);
    }

    void saveUsers() {
        FILE* fp = fopen(USER_FILE.c_str(), "w");
        if (fp == NULL) {
            std::cerr << "无法保存用户数据!" << std::endl;
            return;
        }
        
        for (const auto& pair : users) {
            fprintf(fp, "%s|%s\n", pair.first.c_str(), pair.second.c_str());
        }
        fclose(fp);
    }

public:
    UserManager() {
        loadUsers();
    }

    // 注册新用户
    bool registerUser(const std::string& username, const std::string& password) {
        // 检查用户名是否已存在
        if (users.find(username) != users.end()) {
            return false;
        }
        // 加密密码后保存
        std::string hash = CryptoUtils::hash(password);
        users[username] = hash;
        saveUsers();
        return true;
    }

    // 用户登录
    bool login(const std::string& username, const std::string& password) {
        auto it = users.find(username);
        if (it == users.end()) {
            return false;  // 用户不存在
        }
        std::string hash = CryptoUtils::hash(password);
        if (it->second == hash) {
            currentUser = username;
            return true;
        }
        return false;  // 密码错误
    }

    // 登出
    void logout() {
        currentUser.clear();
    }

    // 获取当前登录用户
    std::string getCurrentUser() const {
        return currentUser;
    }

    // 是否已登录
    bool isLoggedIn() const {
        return !currentUser.empty();
    }
};

#endif
