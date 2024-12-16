// 员工自助服务模块
class EmployeeServiceManager : public HRM {
public:
    const char* LeaveRequestsTable = R"(
        CREATE TABLE IF NOT EXISTS LeaveRequests (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            start_date DATE,
            end_date DATE,
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    const char* ReimbursementsTable = R"(
        CREATE TABLE IF NOT EXISTS Reimbursements (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            amount DECIMAL(10, 2),
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
            std::cerr << "Error in submitLeaveRequest: " << e.what() << std::endl;
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
            std::cerr << "Error in submitReimbursement: " << e.what() << std::endl;
        }
    }
};
