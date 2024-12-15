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

    void updateLeaveRequest(int id, const string& start_date, const string& end_date) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "UPDATE LeaveRequests SET start_date = ?, end_date = ? WHERE id = ?"
            );
            pstmt->setString(1, start_date);
            pstmt->setString(2, end_date);
            pstmt->setInt(3, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in updateLeaveRequest: " << e.what() << std::endl;
        }
    }

    void deleteLeaveRequest(int id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "DELETE FROM LeaveRequests WHERE id = ?"
            );
            pstmt->setInt(1, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in deleteLeaveRequest: " << e.what() << std::endl;
        }
    }

    void getLeaveRequests(int employee_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "SELECT * FROM LeaveRequests WHERE employee_id = ?"
            );
            pstmt->setInt(1, employee_id);
            sql::ResultSet* res = pstmt->executeQuery();
            while (res->next()) {
                std::cout << "Leave ID: " << res->getInt("id")
                          << ", Start Date: " << res->getString("start_date")
                          << ", End Date: " << res->getString("end_date") << std::endl;
            }
        } catch (sql::SQLException& e) {
            std::cerr << "Error in getLeaveRequests: " << e.what() << std::endl;
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

    void deleteReimbursement(int id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "DELETE FROM Reimbursements WHERE id = ?"
            );
            pstmt->setInt(1, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in deleteLeaveRequest: " << e.what() << std::endl;
        }
    }
};
