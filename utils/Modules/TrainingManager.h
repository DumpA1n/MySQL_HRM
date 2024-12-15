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
            std::cerr << "Error in addTrainingCourse: " << e.what() << std::endl;
        }
    }

    void updateTrainingCourse(int id, const string& course_name, const string& description) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "UPDATE TrainingCourses SET course_name = ?, description = ? WHERE id = ?"
            );
            pstmt->setString(1, course_name);
            pstmt->setString(2, description);
            pstmt->setInt(3, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in updateTrainingCourse: " << e.what() << std::endl;
        }
    }

    void deleteTrainingCourse(int id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "DELETE FROM TrainingCourses WHERE id = ?"
            );
            pstmt->setInt(1, id);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in deleteTrainingCourse: " << e.what() << std::endl;
        }
    }

    void getTrainingCourses() {
        try {
            sql::Statement* stmt = tsql.con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM TrainingCourses");
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                          << ", Name: " << res->getString("course_name")
                          << ", Description: " << res->getString("description") << std::endl;
            }
            delete res;
            delete stmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in getTrainingCourses: " << e.what() << std::endl;
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
            std::cerr << "Error in enrollInCourse: " << e.what() << std::endl;
        }
    }

    void getEnrollments(int employee_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "SELECT * FROM Enrollments WHERE employee_id = ?"
            );
            pstmt->setInt(1, employee_id);
            sql::ResultSet* res = pstmt->executeQuery();
            while (res->next()) {
                std::cout << "Enrollment ID: " << res->getInt("id")
                          << ", Course ID: " << res->getInt("course_id") << std::endl;
            }
            delete res;
            delete pstmt;
        } catch (sql::SQLException& e) {
            std::cerr << "Error in getEnrollments: " << e.what() << std::endl;
        }
    }
};