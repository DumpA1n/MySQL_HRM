// 员工自助服务模块
class EmployeeServiceManager : public HRM {
public:
    // 表结构保持不变
    const char* LeaveRequestsTable = R"(
        CREATE TABLE IF NOT EXISTS LeaveRequests (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            start_date DATE,
            end_date DATE,
            status ENUM('待处理', '通过', '不通过') DEFAULT '待处理',
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    const char* ReimbursementsTable = R"(
        CREATE TABLE IF NOT EXISTS Reimbursements (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            amount DECIMAL(10, 2),
            status ENUM('待处理', '通过', '不通过') DEFAULT '待处理',
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    EmployeeServiceManager() {
        sql::Statement* stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(LeaveRequestsTable));
        stmt->execute(sql::SQLString(ReimbursementsTable));
        delete stmt;
    }

    void submitLeaveRequest(int employee_id, const string& start_date, const string& end_date) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO LeaveRequests (employee_id, start_date, end_date) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, start_date);
            pstmt->setString(3, end_date);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in submitLeaveRequest: %s", e.what());
        }
    }

    void submitReimbursement(int employee_id, double amount) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO Reimbursements (employee_id, amount) VALUES (?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setDouble(2, amount);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in submitReimbursement: %s", e.what());
        }
    }

    void updateLeaveRequestStatus(int request_id, const string& status) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "UPDATE LeaveRequests SET status = ? WHERE id = ?"
            );
            pstmt->setString(1, status);
            pstmt->setInt(2, request_id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in updateLeaveRequestStatus: %s", e.what());
        }
    }

    void updateReimbursementStatus(int request_id, const string& status) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "UPDATE Reimbursements SET status = ? WHERE id = ?"
            );
            pstmt->setString(1, status);
            pstmt->setInt(2, request_id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in updateReimbursementStatus: %s", e.what());
        }
    }
};

