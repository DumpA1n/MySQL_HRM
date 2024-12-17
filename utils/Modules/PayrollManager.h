// 薪酬管理模块
class PayrollManager : public HRM {
public:
    const char* PayrollTable = R"(
        CREATE TABLE IF NOT EXISTS Payroll (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            month VARCHAR(20),
            amount DECIMAL(10, 2),
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    PayrollManager() {
        sql::Statement* stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(PayrollTable));
        delete stmt;
    }

    void generatePayroll(int employee_id, const string& month, double amount) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO Payroll (employee_id, month, amount) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, month);
            pstmt->setDouble(3, amount);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in generatePayroll: %s", e.what());
        }
    }

    void updatePayroll(int id, double amount) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "UPDATE Payroll SET amount = ? WHERE id = ?"
            );
            pstmt->setDouble(1, amount);
            pstmt->setInt(2, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in updatePayroll: %s", e.what());
        }
    }

    void deletePayroll(int id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "DELETE FROM Payroll WHERE id = ?"
            );
            pstmt->setInt(1, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in deletePayroll: %s", e.what());
        }
    }

    void getPayroll(int employee_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "SELECT * FROM Payroll WHERE employee_id = ?"
            );
            pstmt->setInt(1, employee_id);
            sql::ResultSet* res = pstmt->executeQuery();
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                          << ", Month: " << res->getString("month")
                          << ", Amount: " << res->getDouble("amount") << std::endl;
            }
            delete res;
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in getPayroll: %s", e.what());
        }
    }
};