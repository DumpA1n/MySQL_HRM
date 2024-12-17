// 培训发展管理模块
class TrainingManager : public HRM {
public:
    const char* TrainingTable = R"(
        CREATE TABLE IF NOT EXISTS TrainingCourses (
            id INT AUTO_INCREMENT PRIMARY KEY,
            course_name VARCHAR(255),
            description TEXT
        );
    )";

    const char* EnrollmentTable = R"(
        CREATE TABLE IF NOT EXISTS Enrollments (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            course_id INT,
            FOREIGN KEY (employee_id) REFERENCES Employees(id),
            FOREIGN KEY (course_id) REFERENCES TrainingCourses(id)
        );
    )";

    TrainingManager() {
        sql::Statement* stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(TrainingTable));
        stmt->execute(sql::SQLString(EnrollmentTable));
        delete stmt;
    }

    void addTrainingCourse(const string& course_name, const string& description) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO TrainingCourses (course_name, description) VALUES (?, ?)"
            );
            pstmt->setString(1, course_name);
            pstmt->setString(2, description);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in addTrainingCourse: %s", e.what());
        }
    }

    void enrollInCourse(int employee_id, int course_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO Enrollments (employee_id, course_id) VALUES (?, ?)"
            );
            pstmt->setInt(1, employee_id);
            pstmt->setInt(2, course_id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in enrollInCourse: %s", e.what());
        }
    }
};