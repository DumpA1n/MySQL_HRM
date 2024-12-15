// 员工信息管理模块
class EmployeeManager : public HRM {
public:
    const char* Employees = R"(
        CREATE TABLE IF NOT EXISTS Employees (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(255),
            id_card VARCHAR(20),
            contact VARCHAR(50),
            address VARCHAR(255),
            position VARCHAR(100),
            department VARCHAR(100),
            hire_date DATE,
            education_background TEXT,
            work_experience TEXT,
            emergency_contact VARCHAR(255)
        );
    )";
    EmployeeManager() {
        sql::Statement *stmt;
        stmt = tsql.con->createStatement();
        stmt->execute(sql::SQLString(Employees));
        delete stmt;
    }
    void addEmployee(const string& name = "", 
                     const string& id_card = "", 
                     const string& contact = "", 
                     const string& address = "",
                     const string& position = "", 
                     const string& department = "", 
                     const string& hire_date = "1970-01-01",
                     const string& education_background = "", 
                     const string& work_experience = "", 
                     const string& emergency_contact = "") {
        try {
            if (name.empty()) {
                LOGE("名称不能为空");
                return;
            }
            sql::PreparedStatement *pstmt;
            pstmt = tsql.con->prepareStatement("INSERT INTO Employees (name, id_card, contact, address, position, department, hire_date, education_background, work_experience, emergency_contact) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            pstmt->setString(1, name);
            pstmt->setString(2, id_card);
            pstmt->setString(3, contact);
            pstmt->setString(4, address);
            pstmt->setString(5, position);
            pstmt->setString(6, department);
            pstmt->setString(7, hire_date);
            pstmt->setString(8, education_background);
            pstmt->setString(9, work_experience);
            pstmt->setString(10, emergency_contact);
            pstmt->execute();
            delete pstmt;
        } catch (sql::SQLException &e) {
            std::cerr << "Error in addEmployee: " << e.what() << std::endl;
            return;
        }
        LOGI("添加成功");
    }
    void deleteEmployee(const string& id = "") {
        try {
            sql::PreparedStatement* pstmt;
            if (id.empty()) {
                LOGE("移除失败: id不能为空");
                return;
            }
            pstmt = tsql.con->prepareStatement("DELETE FROM Employees WHERE id = ?");
            pstmt->setString(1, id);
            pstmt->executeUpdate();
            delete pstmt;
        } catch (sql::SQLException &e) {
            std::cerr << "Error in deleteEmployee: " << e.what() << std::endl;
            return;
        }
        LOGI("移除成功: id = %s", id.c_str());
    }
    unique_ptr<sql::ResultSet> executeQuery(const sql::SQLString query) {
        try {
            unique_ptr<sql::Statement> stmt(tsql.con->createStatement());
            unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
            stmt.release();
            return std::move(res);
        } catch (sql::SQLException &e) {
            std::cerr << "Error in executeQuery: " << e.what() << std::endl;
            return nullptr;
        }
    }
};