#include "qt_gui.h"

// ========== MainWindow 实现 ==========
MainWindow::MainWindow(TaskManager* tm, const QString& username) 
    : taskManager(tm), currentUser(username) {
    setWindowTitle(" 日程管理 - " + username);
    resize(900, 600);
    
    taskList = new QListWidget();
    taskList->setFixedWidth(350);
    
    QPushButton* refreshBtn = new QPushButton("🔄 刷新");
    QPushButton* deleteBtn = new QPushButton("🗑️ 删除选中");
    QPushButton* logoutBtn = new QPushButton(" 退出");
    
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel(" 我的任务"));
    leftLayout->addWidget(taskList);
    leftLayout->addWidget(refreshBtn);
    leftLayout->addWidget(deleteBtn);
    leftLayout->addWidget(logoutBtn);
    
    QLabel* detailLabel = new QLabel(" 任务详情");
    detailEdit = new QTextEdit();
    detailEdit->setReadOnly(true);
    
    QGroupBox* addGroup = new QGroupBox("➕ 添加任务");
    QLabel* nameLabel = new QLabel("名称:");
    nameEdit = new QLineEdit();
    
    QLabel* startLabel = new QLabel("开始时间:");
    startTimeEdit = new QDateTimeEdit();
    startTimeEdit->setCalendarPopup(true);
    startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    
    QLabel* priorityLabel = new QLabel("优先级:");
    priorityCombo = new QComboBox();
    priorityCombo->addItems({"低", "中", "高"});
    
    QLabel* categoryLabel = new QLabel("分类:");
    categoryCombo = new QComboBox();
    categoryCombo->addItems({"学习", "娱乐", "生活"});
    
    QLabel* remindLabel = new QLabel("提醒时间:");
    remindTimeEdit = new QDateTimeEdit();
    remindTimeEdit->setCalendarPopup(true);
    remindTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    remindTimeEdit->setDateTime(QDateTime::currentDateTime());
    
    QPushButton* addBtn = new QPushButton("✅ 添加任务");
    
    QGridLayout* addLayout = new QGridLayout();
    addLayout->addWidget(nameLabel, 0, 0);
    addLayout->addWidget(nameEdit, 0, 1);
    addLayout->addWidget(startLabel, 1, 0);
    addLayout->addWidget(startTimeEdit, 1, 1);
    addLayout->addWidget(priorityLabel, 2, 0);
    addLayout->addWidget(priorityCombo, 2, 1);
    addLayout->addWidget(categoryLabel, 3, 0);
    addLayout->addWidget(categoryCombo, 3, 1);
    addLayout->addWidget(remindLabel, 4, 0);
    addLayout->addWidget(remindTimeEdit, 4, 1);
    addLayout->addWidget(addBtn, 5, 0, 1, 2);
    addGroup->setLayout(addLayout);
    
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(detailLabel);
    rightLayout->addWidget(detailEdit);
    rightLayout->addWidget(addGroup);
    
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
    
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshTasks);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteTask);
    connect(taskList, &QListWidget::itemClicked, this, &MainWindow::showTaskDetail);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::logout);
    
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refreshTasks);
    refreshTimer->start(30000);
    
    refreshTasks();
}

void MainWindow::refreshTasks() {
    taskList->clear();
    auto tasks = taskManager->getAllTasks();
    for (const auto& task : tasks) {
        QString text = QString("[%1] %2 - %3")
            .arg(task.getId())
            .arg(QString::fromStdString(task.getName()))
            .arg(QString::fromStdString(task.getStartTimeStr()));
        taskList->addItem(text);
    }
}

void MainWindow::showTaskDetail(QListWidgetItem* item) {
    int id = item->text().split("]")[0].replace("[", "").toInt();
    auto tasks = taskManager->getAllTasks();
    for (const auto& task : tasks) {
        if (task.getId() == id) {
            QString detail = QString(
                "📌 任务ID: %1\n"
                "📝 名称: %2\n"
                "🕐 开始时间: %3\n"
                "⭐ 优先级: %4\n"
                "📂 分类: %5\n"
                "🔔 提醒时间: %6\n"
                "✅ 已提醒: %7"
            )
            .arg(task.getId())
            .arg(QString::fromStdString(task.getName()))
            .arg(QString::fromStdString(task.getStartTimeStr()))
            .arg(QString::fromStdString(task.getPriorityStr()))
            .arg(QString::fromStdString(task.getCategoryStr()))
            .arg(QString::fromStdString(task.getRemindTimeStr()))
            .arg(task.isReminded() ? "是" : "否");
            detailEdit->setText(detail);
            break;
        }
    }
}

void MainWindow::deleteTask() {
    QListWidgetItem* item = taskList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "提示", "请先选择一个任务");
        return;
    }
    
    int id = item->text().split("]")[0].replace("[", "").toInt();
    if (QMessageBox::question(this, "确认删除", "确定要删除任务吗？") == QMessageBox::Yes) {
        taskManager->deleteTask(id);
        refreshTasks();
        detailEdit->clear();
    }
}

void MainWindow::addTask() {
    QString name = nameEdit->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入任务名称");
        return;
    }
    
    time_t startTime = startTimeEdit->dateTime().toTime_t();
    Priority priority = static_cast<Priority>(priorityCombo->currentIndex());
    Category category = static_cast<Category>(categoryCombo->currentIndex());
    time_t remindTime = remindTimeEdit->dateTime().toTime_t();
    
    if (taskManager->addTask(name.toStdString(), startTime, priority, category, remindTime)) {
        QMessageBox::information(this, "成功", "✅ 任务添加成功!");
        nameEdit->clear();
        refreshTasks();
    } else {
        QMessageBox::warning(this, "失败", "❌ 任务添加失败! 时间可能冲突");
    }
}

void MainWindow::logout() {
    QMessageBox::information(this, "提示", "👋 再见!");
    this->close();
}

// ========== LoginWindow 实现 ==========
LoginWindow::LoginWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("日程管理系统 - 登录");
    resize(350, 200);
    
    QLabel* titleLabel = new QLabel("📅 日程管理");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    QLabel* userLabel = new QLabel("用户名:");
    usernameEdit = new QLineEdit();
    
    QLabel* passLabel = new QLabel("密码:");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    loginBtn = new QPushButton("登录");
    registerBtn = new QPushButton("注册");
    statusLabel = new QLabel();
    statusLabel->setStyleSheet("color: red;");
    
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(titleLabel, 0, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(userLabel, 1, 0);
    layout->addWidget(usernameEdit, 1, 1);
    layout->addWidget(passLabel, 2, 0);
    layout->addWidget(passwordEdit, 2, 1);
    layout->addWidget(loginBtn, 3, 0);
    layout->addWidget(registerBtn, 3, 1);
    layout->addWidget(statusLabel, 4, 0, 1, 2);
    
    setLayout(layout);
    
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::onLogin);
    connect(registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegister);
}

void LoginWindow::onLogin() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("请输入用户名和密码");
        return;
    }
    
    if (userManager.login(username.toStdString(), password.toStdString())) {
        statusLabel->setStyleSheet("color: green;");
        statusLabel->setText("✅ 登录成功!");
        
        TaskManager* taskManager = new TaskManager(username.toStdString());
        ReminderThread* reminder = new ReminderThread(taskManager);
        MainWindow* mainWin = new MainWindow(taskManager, username);
        mainWin->show();
        
        this->close();
    } else {
        statusLabel->setStyleSheet("color: red;");
        statusLabel->setText("❌ 登录失败! 用户名或密码错误");
    }
}

void LoginWindow::onRegister() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("请输入用户名和密码");
        return;
    }
    
    if (userManager.registerUser(username.toStdString(), password.toStdString())) {
        statusLabel->setStyleSheet("color: green;");
        statusLabel->setText("✅ 注册成功! 请点击登录");
    } else {
        statusLabel->setStyleSheet("color: red;");
        statusLabel->setText("❌ 用户名已存在!");
    }
}

// ========== 主函数 ==========
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    system("mkdir -p data");
    
    LoginWindow login;
    login.show();
    
    return app.exec();
}
