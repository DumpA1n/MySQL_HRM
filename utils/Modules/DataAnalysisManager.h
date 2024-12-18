class DataAnalysisManager {
public:
    // 获取员工的出勤情况
    std::vector<std::tuple<std::string, int, int>> getAttendanceAnalysis() {
        std::vector<std::tuple<std::string, int, int>> attendanceData;
        try {
            sql::Statement* stmt = tsql.con->createStatement();
            sql::ResultSet* res = stmt->executeQuery(
                "SELECT e.name, COUNT(a.id) AS total, SUM(CASE WHEN a.status = 'Late' THEN 1 ELSE 0 END) AS late FROM attendance a "
                "JOIN employees e ON a.employee_id = e.id GROUP BY e.name"
            );
            while (res->next()) {
                std::string name = res->getString("name");
                int total = res->getInt("total");
                int late = res->getInt("late");
                attendanceData.push_back({ name, total, late });
            }
            delete res;
            delete stmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in getAttendanceAnalysis: %s", e.what());
        }
        return attendanceData;
    }

    // 获取报销情况分析
    std::vector<std::tuple<std::string, double>> getReimbursementAnalysis() {
        std::vector<std::tuple<std::string, double>> reimbursementData;
        try {
            sql::Statement* stmt = tsql.con->createStatement();
            sql::ResultSet* res = stmt->executeQuery(
                "SELECT e.name, SUM(r.amount) AS total_amount FROM reimbursements r "
                "JOIN employees e ON r.employee_id = e.id GROUP BY e.name"
            );
            while (res->next()) {
                std::string name = res->getString("name");
                double totalAmount = res->getDouble("total_amount");
                reimbursementData.push_back({ name, totalAmount });
            }
            delete res;
            delete stmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in getReimbursementAnalysis: %s", e.what());
        }
        return reimbursementData;
    }

    // 获取绩效分析
    std::vector<std::tuple<std::string, int>> getPerformanceAnalysis() {
        std::vector<std::tuple<std::string, int>> performanceData;
        try {
            sql::Statement* stmt = tsql.con->createStatement();
            sql::ResultSet* res = stmt->executeQuery(
                "SELECT e.name, AVG(p.score) AS avg_score FROM performance p "
                "JOIN employees e ON p.employee_id = e.id GROUP BY e.name"
            );
            while (res->next()) {
                std::string name = res->getString("name");
                int avgScore = res->getInt("avg_score");
                performanceData.push_back({ name, avgScore });
            }
            delete res;
            delete stmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in getPerformanceAnalysis: %s", e.what());
        }
        return performanceData;
    }

    // 获取招聘申请状态分析
    std::vector<std::tuple<std::string, int>> getJobApplicationsStatus() {
        std::vector<std::tuple<std::string, int>> applicationStatusData;
        try {
            sql::Statement* stmt = tsql.con->createStatement();
            sql::ResultSet* res = stmt->executeQuery(
                "SELECT status, COUNT(*) AS count FROM applications GROUP BY status"
            );
            while (res->next()) {
                std::string status = res->getString("status");
                int count = res->getInt("count");
                applicationStatusData.push_back({ status, count });
            }
            delete res;
            delete stmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in getJobApplicationsStatus: %s", e.what());
        }
        return applicationStatusData;
    }
};
