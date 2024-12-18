// 员工自助服务模块
class EmployeeServiceManager : public HRM {
public:
    const char* LeaveRequestsTable = R"(
        CREATE TABLE IF NOT EXISTS LeaveRequests (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            start_date DATE,
            end_date DATE,
            leave_type ENUM('病假', '事假', '其他') DEFAULT '病假',
            leave_reason TEXT,
            status ENUM('待处理', '通过', '不通过') DEFAULT '待处理',
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    const char* ReimbursementsTable = R"(
        CREATE TABLE IF NOT EXISTS Reimbursements (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            amount DECIMAL(10, 2),
            reimbursement_type ENUM('交通费', '住宿费', '餐饮费', '其他') DEFAULT '交通费',
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

    void submitLeaveRequest(int employee_id, const string& start_date, const string& end_date, int leave_type, const string& leave_reason) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO LeaveRequests (employee_id, start_date, end_date, leave_type, leave_reason) VALUES (?, ?, ?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, start_date);
            pstmt->setString(3, end_date);
            pstmt->setInt(4, leave_type);  // 请假类型
            pstmt->setString(5, leave_reason);  // 请假原因
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in submitLeaveRequest: %s", e.what());
        }
    }

    void submitReimbursement(int employee_id, double amount, int reimbursement_type) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO Reimbursements (employee_id, amount, reimbursement_type) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setDouble(2, amount);
            pstmt->setInt(3, reimbursement_type);  // 传递报销类型
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in submitReimbursement: %s", e.what());
        }
    }
};
