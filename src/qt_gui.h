#ifndef QT_GUI_H
#define QT_GUI_H

#include <QtWidgets>
#include "UserManager.hpp"
#include "TaskManager.hpp"
#include "ReminderThread.hpp"

class MainWindow : public QWidget {
    Q_OBJECT
private:
    TaskManager* taskManager;
    QString currentUser;
    QListWidget* taskList;
    QTextEdit* detailEdit;
    QLineEdit* nameEdit;
    QDateTimeEdit* startTimeEdit;
    QComboBox* priorityCombo;
    QComboBox* categoryCombo;
    QDateTimeEdit* remindTimeEdit;
    QTimer* refreshTimer;

public:
    MainWindow(TaskManager* tm, const QString& username);

private slots:
    void refreshTasks();
    void showTaskDetail(QListWidgetItem* item);
    void deleteTask();
    void addTask();
    void logout();
};

class LoginWindow : public QWidget {
    Q_OBJECT
private:
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginBtn;
    QPushButton* registerBtn;
    QLabel* statusLabel;
    UserManager userManager;

public:
    LoginWindow(QWidget* parent = nullptr);

private slots:
    void onLogin();
    void onRegister();
};

#endif
