

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
            std::cerr << "Error in executeQuery: " << e.what() << std::endl;
            return nullptr;
        }
    }
};

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