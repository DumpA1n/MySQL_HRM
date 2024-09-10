#pragma once
#ifndef MOUDULE_H
#define MOUDULE_H

#include <iostream>
#include <mysql/jdbc.h>

using namespace std;
using namespace sql;

class TSQL {
public:
    mysql::MySQL_Driver* driver;
    Connection* con;
    sql::SQLString DBName;
    TSQL() {
        driver = nullptr;
        con = nullptr;
        DBName = "test";
    }
    void initialize(const sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password) {
        driver = mysql::get_mysql_driver_instance();
        if (driver == nullptr) {
            cerr << "TSQL::ERROR: Failed to get instance" << endl;
            return;
        }
        cout << "TSQL::SUCCESS: get instance" << endl;

        con = driver->connect(hostName, userName, password);
        if (con == nullptr) {
            cerr << "TSQL::ERROR: Failed to connect" << endl;
            return;
        }
        cout << "TSQL::SUCCESS: connect" << endl;

        Statement *stmt;
        stmt = con->createStatement();
        stmt->execute("CREATE DATABASE IF NOT EXISTS " + DBName);
        stmt->execute("USE " + DBName);
        delete stmt;
    }
    ~TSQL() {
        delete con;
    }
} tsql;


// 员工信息管理模块
class EmployeeManager {
public:
    struct Employee {
        int id;                          // 员工ID
        std::string name;                // 姓名
        std::string id_card;             // 身份证
        std::string contact;             // 联系方式
        std::string address;             // 地址
        std::string position;            // 职位
        std::string department;          // 部门
        std::string hire_date;           // 入职日期
        std::string education_background;// 教育背景
        std::string work_experience;     // 工作经验
        std::string emergency_contact;   // 紧急联系人
    };
    const char* Employees = R"(
        CREATE TABLE IF NOT EXISTS Employees (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(255),
            id_card VARCHAR(20),
            contact VARCHAR(50),
            address VARCHAR(255),
            position VARCHAR(100),
            department VARCHAR(100),
            hire_date DATE,
            education_background TEXT,
            work_experience TEXT,
            emergency_contact VARCHAR(255)
        );
    )";
    EmployeeManager() {
        sql::Statement *stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(Employees));
        delete stmt;
    }
    void addEmployee(const string& name = "", 
                     const string& id_card = "", 
                     const string& contact = "", 
                     const string& address = "",
                     const string& position = "", 
                     const string& department = "", 
                     const string& hire_date = "1900-01-01",
                     const string& education_background = "", 
                     const string& work_experience = "", 
                     const string& emergency_contact = "") {
        try {
            sql::PreparedStatement *pstmt;
            pstmt = tsql.con->prepareStatement("INSERT INTO Employees (name, id_card, contact, address, position, department, hire_date, education_background, work_experience, emergency_contact) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            pstmt->setString(1, name);
            pstmt->setString(2, id_card);
            pstmt->setString(3, contact);
            pstmt->setString(4, address);
            pstmt->setString(5, position);
            pstmt->setString(6, department);
            pstmt->setString(7, hire_date);
            pstmt->setString(8, education_background);
            pstmt->setString(9, work_experience);
            pstmt->setString(10, emergency_contact);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException &e) {
            std::cerr << "Error in addEmployee: " << e.what() << std::endl;
        }
    }
    void displayEmployees() {
        try {
            sql::Statement *stmt;
            stmt = tsql.con->createStatement();
            sql::ResultSet *res = stmt->executeQuery("SELECT * FROM Employees");
            while (res->next()) {
                cout << "ID: " << res->getInt("id") << ", Name: " << res->getString("name") << endl;
            }
            delete res;
            delete stmt;
        } catch (sql::SQLException &e) {
            std::cerr << "Error in displayEmployees: " << e.what() << std::endl;
        }
    }
    unique_ptr<sql::ResultSet> getEmployees() {
        try {
            unique_ptr<sql::Statement> stmt(tsql.con->createStatement());
            unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM Employees"));
            stmt.release();
            return std::move(res);
        } catch (sql::SQLException &e) {
            std::cerr << "Error in getEmployees: " << e.what() << std::endl;
            return nullptr;
        }
    }
    void deleteEmployee(const string& id = "") {
        try {
            sql::PreparedStatement* pstmt;
            if (!id.empty()) {
                pstmt = tsql.con->prepareStatement("DELETE FROM Employees WHERE id = ?");
                pstmt->setString(1, id);
            } else {
                return;
            }
            pstmt->executeUpdate();
            delete pstmt;
        } catch (sql::SQLException &e) {
            std::cerr << "Error in deleteEmployee: " << e.what() << std::endl;
        }
    }
    unique_ptr<sql::ResultSet> executeQuery(const sql::SQLString cmd) {
        try {
            unique_ptr<sql::Statement> stmt(tsql.con->createStatement());
            unique_ptr<sql::ResultSet> res(stmt->executeQuery(cmd));
            stmt.release();
            return std::move(res);
        } catch (sql::SQLException &e) {
            std::cerr << "Error in executeQuery: " << e.what() << std::endl;
            return nullptr;
        }
    }
};

// 招聘入职管理模块
class RecruitmentManager {

};

// 考勤管理模块
class AttendanceManager {

};

// 薪酬管理模块
class PayrollManager {

};

// 绩效评估管理模块
class PerformanceManager {

};

// 培训发展管理模块
class TrainingManager {

};

// 员工自助服务模块
class EmployeeServiceManager {

};

#endif // MOUDULE_H