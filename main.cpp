#include <iostream>
#include <mysql/jdbc.h>

using namespace std;
using namespace sql;

int main() {
    try {
        // 获取 MySQL 驱动
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        
        // 连接到 MySQL 服务器 (!!!修改为你的数据库用户名和密码!!!)
        std::unique_ptr<Connection> con(driver->connect("tcp://127.0.0.1:3306", "root", "7222"));

        // 创建一个 Statement 对象来执行 SQL 语句
        std::unique_ptr<Statement> stmt(con->createStatement());

        // 创建数据库
        stmt->execute("CREATE DATABASE IF NOT EXISTS test_db");
        stmt->execute("USE test_db");

        // 创建表
        const char* _sql_ct_students = R"(
            CREATE TABLE IF NOT EXISTS students (
                id INT AUTO_INCREMENT PRIMARY KEY, 
                student_id VARCHAR(10), 
                name VARCHAR(10), 
                age INT, 
                gender VARCHAR(10), 
                class VARCHAR(10)
            )
        )";
        stmt->execute(sql::SQLString(_sql_ct_students));

        // 插入数据
        std::unique_ptr<PreparedStatement> pstmt(con->prepareStatement(
            "INSERT INTO students (student_id, name, age, gender, class) VALUES (?, ?, ?, ?, ?)"));
        
        pstmt->setString(1, "20230001");
        pstmt->setString(2, "shabi");
        pstmt->setInt(3, 20);
        pstmt->setString(4, "Male");
        pstmt->setString(5, "CS101");
        pstmt->executeUpdate();

        pstmt->setString(1, "20230002");
        pstmt->setString(2, "shiwo");
        pstmt->setInt(3, 21);
        pstmt->setString(4, "Female");
        pstmt->setString(5, "CS102");
        pstmt->executeUpdate();

        // 查询数据
        std::unique_ptr<ResultSet> res(stmt->executeQuery("SELECT * FROM students"));

        // 输出查询结果
        while (res->next()) {
            cout << "ID: " << res->getInt("id")
                 << ", ID: " << res->getString("student_id")
                 << ", Name: " << res->getString("name")
                 << ", Age: " << res->getInt("age")
                 << ", Gender: " << res->getString("gender")
                 << ", Class: " << res->getString("class") << endl;
        }

    } catch (SQLException& e) {
        cerr << "SQL error: " << e.what() << endl;
        return 1;
    }
    getchar();
    return 0;
}
