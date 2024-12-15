// 考勤管理模块
class AttendanceManager : public HRM {
public:
    const char* AttendanceTable = R"(
        CREATE TABLE IF NOT EXISTS Attendance (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            attendance_date DATE,
            status VARCHAR(20),
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    AttendanceManager() {
        sql::Statement* stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(AttendanceTable));
        delete stmt;
    }

    void recordAttendance(int employee_id, const string& attendance_date, const string& status) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO Attendance (employee_id, attendance_date, status) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, attendance_date);
            pstmt->setString(3, status);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in recordAttendance: " << e.what() << std::endl;
        }
    }

    void getAttendance(int employee_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "SELECT * FROM Attendance WHERE employee_id = ?"
            );
            pstmt->setInt(1, employee_id);
            sql::ResultSet* res = pstmt->executeQuery();
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                          << ", Date: " << res->getString("attendance_date")
                          << ", Status: " << res->getString("status") << std::endl;
            }
            delete res;
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in getAttendance: " << e.what() << std::endl;
        }
    }

    void deleteAttendance(int id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "DELETE FROM Attendance WHERE id = ?"
            );
            pstmt->setInt(1, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in deleteAttendance: " << e.what() << std::endl;
        }
    }
};