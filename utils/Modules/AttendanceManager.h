class AttendanceManager : public HRM {
public:
    const char* AttendanceDetailsTable = R"(
        CREATE TABLE IF NOT EXISTS AttendanceDetails (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            attendance_date DATE,
            check_in_time TIME,
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    const char* AttendanceStatusTable = R"(
        CREATE TABLE IF NOT EXISTS AttendanceStatus (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            attendance_date DATE,
            earliest_check_in_time TIME,
            latest_check_in_time TIME,
            first_status VARCHAR(20),
            second_status VARCHAR(20),
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    AttendanceManager() {
        sql::Statement* stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(AttendanceDetailsTable));
        stmt->execute(sql::SQLString(AttendanceStatusTable));
        delete stmt;
    }

    // 判断签到时间的状态：迟到/正常/早退
    std::string checkStatus(const std::string& check_in_time, bool isFirstCheck) {
        if (isFirstCheck) {
            // 判断早签到时间，10:00:00之前为早到，否则为迟到
            if (check_in_time < "10:00:00") {
                return "早到";
            } else {
                return "迟到";
            }
        } else {
            // 判断晚签到时间，19:00:00之后为正常，之前为早退
            if (check_in_time > "19:00:00") {
                return "正常";
            } else {
                return "早退";
            }
        }
    }

    // 插入签到记录
    void insertCheckIn(int employee_id, const std::string& attendance_date, const std::string& check_in_time) {
        if (!employeeExists(employee_id)) {
            LOGE("员工ID %d 不存在", employee_id);
            return; // 员工不存在，退出
        }

        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO AttendanceDetails (employee_id, attendance_date, check_in_time) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, attendance_date);
            pstmt->setString(3, check_in_time);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in insertCheckIn: %s", e.what());
        }
    }

    // 更新考勤状态
    void updateAttendanceStatus(int employee_id, const std::string& attendance_date) {
        if (!employeeExists(employee_id)) {
            LOGE("员工ID %d 不存在", employee_id);
            return; // 员工不存在，退出
        }

        try {
            // 获取当天的所有签到时间
            std::string query = "SELECT check_in_time FROM AttendanceDetails WHERE employee_id = ? AND attendance_date = ?";
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(query);
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, attendance_date);
            sql::ResultSet* res = pstmt->executeQuery();

            std::string earliest_time = "23:59:59";
            std::string latest_time = "00:00:00";
            while (res->next()) {
                std::string check_in_time = res->getString("check_in_time");

                // 判断最早和最晚签到时间
                if (check_in_time < earliest_time) {
                    earliest_time = check_in_time;
                }
                if (check_in_time > latest_time) {
                    latest_time = check_in_time;
                }
            }
            delete pstmt;

            // 判断早签到和晚签到的状态
            std::string first_status = checkStatus(earliest_time, true);
            std::string second_status = checkStatus(latest_time, false);

            // 更新考勤状态表
            pstmt = tsql.con->prepareStatement(
                "INSERT INTO AttendanceStatus (employee_id, attendance_date, earliest_check_in_time, latest_check_in_time, first_status, second_status) VALUES (?, ?, ?, ?, ?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, attendance_date);
            pstmt->setString(3, earliest_time);
            pstmt->setString(4, latest_time);
            pstmt->setString(5, first_status);
            pstmt->setString(6, second_status);
            pstmt->execute();
            delete pstmt;

        } catch (sql::SQLException& e) {
            LOGE("Error in updateAttendanceStatus: %s", e.what());
        }
    }
};
