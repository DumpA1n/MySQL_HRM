// 绩效评估管理模块
class PerformanceManager : public HRM {
public:
    const char* PerformanceTable = R"(
        CREATE TABLE IF NOT EXISTS Performance (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            review_period VARCHAR(50),
            score INT,
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    PerformanceManager() {
        sql::Statement* stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(PerformanceTable));
        delete stmt;
    }

    void submitPerformanceReview(int employee_id, const string& review_period, int score) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO Performance (employee_id, review_period, score) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, review_period);
            pstmt->setInt(3, score);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in submitPerformanceReview: " << e.what() << std::endl;
        }
    }

    void updatePerformanceReview(int id, int score) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "UPDATE Performance SET score = ? WHERE id = ?"
            );
            pstmt->setInt(1, score);
            pstmt->setInt(2, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in updatePerformanceReview: " << e.what() << std::endl;
        }
    }

    void deletePerformanceReview(int id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "DELETE FROM Performance WHERE id = ?"
            );
            pstmt->setInt(1, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in deletePerformanceReview: " << e.what() << std::endl;
        }
    }

    void getPerformanceReviews(int employee_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "SELECT * FROM Performance WHERE employee_id = ?"
            );
            pstmt->setInt(1, employee_id);
            sql::ResultSet* res = pstmt->executeQuery();
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                          << ", Period: " << res->getString("review_period")
                          << ", Score: " << res->getInt("score") << std::endl;
            }
            delete res;
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in getPerformanceReviews: " << e.what() << std::endl;
        }
    }
};