

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
            LOGE("TSQL::ERROR: Failed to get instance");
            return;
        }
        LOGI("TSQL::SUCCESS: get instance");

        con = driver->connect(hostName, userName, password);
        if (con == nullptr) {
            LOGE("TSQL::ERROR: Failed to connect");
            return;
        }
        LOGI("TSQL::SUCCESS: connect");

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


class HRM {
public:
    unique_ptr<sql::ResultSet> executeQuery(const sql::SQLString query) {
        try {
            unique_ptr<sql::Statement> stmt(tsql.con->createStatement());
            unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
            stmt.release();
            return std::move(res);
        } catch (sql::SQLException &e) {
            LOGE("Error in executeQuery: %s", e.what());
            return nullptr;
        }
    }
};

void deleteColumn(const std::string& tableName, const std::string& columnName, int id) {
    try {
        // 查询所有依赖于指定表的外键
        std::unique_ptr<sql::PreparedStatement> pstmt(
            tsql.con->prepareStatement(
                "SELECT TABLE_NAME, COLUMN_NAME "
                "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE "
                "WHERE REFERENCED_TABLE_NAME = ? "
                "AND REFERENCED_COLUMN_NAME = ?"
            )
        );
        pstmt->setString(1, tableName);  // 动态传递表名
        pstmt->setString(2, columnName); // 动态传递列名
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 遍历所有依赖的表并删除对应记录
        while (res->next()) {
            std::string dependentTableName = res->getString("TABLE_NAME");
            std::string dependentColumnName = res->getString("COLUMN_NAME");

            // 动态生成删除语句
            std::string deleteQuery = "DELETE FROM " + dependentTableName + " WHERE " + dependentColumnName + " = ?";
            std::unique_ptr<sql::PreparedStatement> deleteStmt(
                tsql.con->prepareStatement(deleteQuery)
            );
            deleteStmt->setInt(1, id);
            deleteStmt->executeUpdate();

            LOGI("成功删除依赖记录: 表 %s, 列 %s", dependentTableName.c_str(), dependentColumnName.c_str());
        }

        // 删除指定表中的记录
        std::string deleteMainQuery = "DELETE FROM " + tableName + " WHERE " + columnName + " = ?";
        std::unique_ptr<sql::PreparedStatement> deleteMainStmt(
            tsql.con->prepareStatement(deleteMainQuery)
        );
        deleteMainStmt->setInt(1, id);
        deleteMainStmt->executeUpdate();

        LOGI("移除成功: 表 %s, id = %d", tableName.c_str(), id);
    } catch (sql::SQLException& e) {
        LOGE("Error in deleteColumn: %s", e.what());
    }
}

void updateColumnValue(const std::string& tableName, const std::string& columnName, 
                       const std::string& keyColumn, int keyValue, const std::string& newValue) {
    try {
        // 检查参数是否有效
        if (tableName.empty() || columnName.empty() || keyColumn.empty()) {
            LOGE("表名、列名或键名不能为空");
            return;
        }
        LOGI("新值 = %s", newValue.c_str());

        // 动态生成更新语句
        std::string updateQuery = "UPDATE " + tableName + " SET " + columnName + " = ? WHERE " + keyColumn + " = ?";
        std::unique_ptr<sql::PreparedStatement> pstmt(tsql.con->prepareStatement(updateQuery));

        // 设置参数
        pstmt->setString(1, newValue);  // 新值
        pstmt->setInt(2, keyValue);     // 主键或标识符值

        // 执行更新
        int affectedRows = pstmt->executeUpdate();
        if (affectedRows > 0) {
            LOGI("更新成功: 表 %s, 列 %s, 键列 %s = %d, 新值 = %s", 
                 tableName.c_str(), columnName.c_str(), keyColumn.c_str(), keyValue, newValue.c_str());
        } else {
            LOGE("未找到匹配的记录: 表 %s, 键列 %s = %d", 
                 tableName.c_str(), keyColumn.c_str(), keyValue);
        }
    } catch (sql::SQLException& e) {
        LOGE("Error in updateColumnValue: %s", e.what());
    }
}

// 检查员工是否存在于 Employees 表
bool employeeExists(int employee_id) {
    try {
        std::string query = "SELECT COUNT(*) FROM Employees WHERE id = ?";
        sql::PreparedStatement* pstmt = tsql.con->prepareStatement(query);
        pstmt->setInt(1, employee_id);
        sql::ResultSet* res = pstmt->executeQuery();
        res->next();
        int count = res->getInt(1);
        delete pstmt;
        return count > 0;
    } catch (sql::SQLException& e) {
        LOGE("Error in employeeExists: %s", e.what());
        return false;
    }
}

string GetCurEmployeeName(int employee_id) {
    try {
        std::string query = "SELECT name FROM Employees WHERE id = ?";
        sql::PreparedStatement* pstmt = tsql.con->prepareStatement(query);
        pstmt->setInt(1, employee_id);
        sql::ResultSet* res = pstmt->executeQuery();
        res->next();
        string name = res->getString(1);
        delete pstmt;
        return name;
    } catch (sql::SQLException& e) {
        LOGE("Error in employeeExists: %s", e.what());
        return false;
    }
}

string buildQuery(const string& field, const string& value, bool& hasCondition, bool like = false) {
    if (!value.empty()) {
        std::ostringstream condition;
        if (hasCondition) {
            if (like)
                condition << " AND " << field << " like '%" << value << "%'";
            else condition << " AND " << field << " = '" << value << "'";
        } else {
            if (like)
                condition << " WHERE " << field << " like '%" << value << "%'";
            else condition << " WHERE " << field << " = '" << value << "'";
            hasCondition = true;
        }
        return condition.str();
    }
    return "";
}

// 获取当前日期
std::string getCurrentDate() {
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    
    // 格式化当前日期为 "YYYY-MM-DD"
    std::ostringstream oss;
    oss << std::put_time(now, "%Y-%m-%d");
    return oss.str();
}

// 获取当前时间
std::string getCurrentTime() {
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    
    // 格式化当前时间为 "HH:MM:SS"
    std::ostringstream oss;
    oss << std::put_time(now, "%H:%M:%S");
    return oss.str();
}