#pragma once
#ifndef LOGIN_MENU_H
#define LOGIN_MENU_H

struct User {
    std::string username;
    std::string role;
};

bool binitializeUsersTable = false;
void initializeUsersTable() {
    try {
        if (tsql.con == nullptr) {
            std::cerr << "Database connection not initialized." << std::endl;
            return;
        }

        std::unique_ptr<sql::Statement> stmt(tsql.con->createStatement());
        stmt->execute("CREATE TABLE IF NOT EXISTS users ("
                     "id INT AUTO_INCREMENT PRIMARY KEY,"
                     "username VARCHAR(255) UNIQUE NOT NULL,"
                     "password VARCHAR(255) NOT NULL,"
                     "role ENUM('admin', 'user') NOT NULL"
                     ") ENGINE=InnoDB;");

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT COUNT(*) AS user_count FROM users WHERE username = 'admin'"));
        res->next();

        if (res->getInt("user_count") == 0) {
            std::unique_ptr<sql::PreparedStatement> pstmt(
                tsql.con->prepareStatement(
                    "INSERT INTO users (username, password, role) VALUES (?, ?, ?)"));

            pstmt->setString(1, "admin");
            pstmt->setString(2, "admin");
            pstmt->setString(3, "admin");
            pstmt->execute();

            LOGI("Default root user created.");
        } else {
            LOGI("Root user already exists.");
        }
        binitializeUsersTable = true;
    } catch (sql::SQLException& e) {
        std::cerr << "MySQL error during initialization: " << e.what() << std::endl;
    }
}

bool authenticateUser(const std::string& username, const std::string& password, User& user) {
    try {
        if (tsql.con == nullptr) {
            std::cerr << "Database connection not initialized." << std::endl;
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(
            tsql.con->prepareStatement("SELECT username, role FROM users WHERE username = ? AND password = ?"));

        pstmt->setString(1, username);
        pstmt->setString(2, password);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            user.username = res->getString("username");
            user.role = res->getString("role");
            return true;
        }
    } catch (sql::SQLException& e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }

    return false;
}

void RenderLoginWindow() {
    if (!binitializeUsersTable) initializeUsersTable();

    static char username[128] = "admin";
    static char password[128] = "admin";
    static bool isLoggedIn = false;
    static std::string loginMessage = "";
    static User loggedInUser;

    if (isLoggedIn) {
        ImGui::SetNextWindowSize(ImVec2(screenW * 0.2f, screenH * 0.2f));
        ImGui::SetNextWindowPos(ImVec2(screenW * 0.0f, screenH * 0.0f));
    } else {
        ImGui::SetNextWindowSize(ImVec2(screenW * 0.2f, screenH * 0.2f));
        ImGui::SetNextWindowPos(ImVec2(screenW * 0.4f, screenH * 0.35f));
    }

    ImGui::Begin("Login");

    if (!isLoggedIn) {
        ImGui::InputText("用户名", username, sizeof(username));
        ImGui::InputText("密码", password, sizeof(password), ImGuiInputTextFlags_Password);

        if (ImGui::Button("登录")) {
            User user;
            if (authenticateUser(username, password, user)) {
                isLoggedIn = true;
                loggedInUser = user;
                loginMessage = "欢迎, " + user.username + " (" + user.role + ").";
            } else {
                loginMessage = "登陆失败! 错误的用户名或密码.";
            }
        }

        if (!loginMessage.empty()) {
            ImGui::Spacing();
            ImGui::Text("%s", loginMessage.c_str());
        }
    } else {
        ImGui::Text("欢迎, %s (%s)!", loggedInUser.username.c_str(), loggedInUser.role.c_str());

        if (loggedInUser.role == "admin") {
            ShowMenu();
            ImGui::Text("您是管理员.");
        } else {
            ImGui::Text("您是用户.");
        }

        if (ImGui::Button("登出")) {
            isLoggedIn = false;
            loginMessage = "登出成功.";
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
        }
    }

    ImGui::End();
}

#endif // LOGIN_MENU_H
