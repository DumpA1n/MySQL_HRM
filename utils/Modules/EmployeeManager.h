// 随机生成姓名
std::string generateRandomName() {
    const std::vector<std::string> firstNames = {"李", "张", "王", "赵", "刘"};
    const std::vector<std::string> lastNames = {"伟", "芳", "娜", "敏", "军", "磊"};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_first(0, firstNames.size() - 1);
    std::uniform_int_distribution<> dis_last(0, lastNames.size() - 1);

    return firstNames[dis_first(gen)] + lastNames[dis_last(gen)];
}

// 随机生成身份证
std::string generateRandomIdCard() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);

    std::string idCard;
    for (int i = 0; i < 18; ++i) {
        idCard += std::to_string(dis(gen));
    }
    return idCard;
}

// 随机生成电话
std::string generateRandomPhone() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);

    std::string phone = "1";
    std::uniform_int_distribution<> dis2(3, 4);  // 第二位是 3 或 4
    phone += std::to_string(dis2(gen));

    for (int i = 0; i < 9; ++i) {
        phone += std::to_string(dis(gen));
    }
    return phone;
}

// 随机生成地址
std::string generateRandomAddress() {
    const std::vector<std::string> streets = {"大道", "街道", "路", "小区"};
    const std::vector<std::string> cities = {"北京", "上海", "广州", "深圳", "杭州"};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_street(0, streets.size() - 1);
    std::uniform_int_distribution<> dis_city(0, cities.size() - 1);
    
    return cities[dis_city(gen)] + streets[dis_street(gen)] + std::to_string(dis_city(gen) + 1) + "号";
}

// 随机生成入职日期
std::string generateRandomDate() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_year(2010, 2023);
    std::uniform_int_distribution<> dis_month(1, 12);
    std::uniform_int_distribution<> dis_day(1, 28);

    std::stringstream date;
    date << dis_year(gen) << "-" << dis_month(gen) << "-" << dis_day(gen);
    return date.str();
}

// 随机生成教育背景
std::string generateRandomEducationBackground() {
    const std::vector<std::string> education = {"本科", "硕士", "博士", "大专"};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, education.size() - 1);

    return education[dis(gen)];
}

// 随机生成工作经验
std::string generateRandomWorkExperience() {
    const std::vector<std::string> experiences = {
        "无工作经验",
        "2年软件开发经验",
        "5年项目管理经验",
        "1年市场营销经验",
        "3年销售经验"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, experiences.size() - 1);

    return experiences[dis(gen)];
}

// 随机生成紧急联系人
std::string generateRandomEmergencyContact() {
    return "张三  13012345678";
}

// 随机生成职位
std::string generateRandomPosition() {
    const std::vector<std::string> positions = {
        "软件工程师", "产品经理", "UI设计师", "前端工程师", "后端工程师", "技术经理", "数据分析师"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, positions.size() - 1);

    return positions[dis(gen)];
}

// 随机生成部门
std::string generateRandomDepartment() {
    const std::vector<std::string> departments = {
        "技术部", "人事部", "市场部", "销售部", "财务部", "产品部", "客服部"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, departments.size() - 1);

    return departments[dis(gen)];
}








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
            LOGE("Error in addEmployee: %s", e.what());
            return;
        }
        LOGI("添加成功");
    }

    void addEmployeeToUsers(int id, const string& username = "", const string& password = "") {
        try {
            sql::PreparedStatement *pstmt;
            pstmt = tsql.con->prepareStatement(
                "INSERT INTO Users (username, password, role, employee_id) VALUES (?, ?, ?, ?)");
            pstmt->setString(1, username);
            pstmt->setString(2, password);
            pstmt->setString(3, "user");
            pstmt->setInt(4, id);
            pstmt->executeUpdate();
            delete pstmt;
            LOGI("员工添加成功, ID: %d, 账号: %s, 密码: %s", id, username.c_str(), password.c_str());
        } catch (sql::SQLException &e) {
            LOGE("Error in addEmployee: %s", e.what());
            return;
        }
    }

    void deleteColumn(int id) {
        try {
            // 查询所有依赖于 Employees 表的外键
            std::unique_ptr<sql::PreparedStatement> pstmt(
                tsql.con->prepareStatement(
                    "SELECT TABLE_NAME, COLUMN_NAME "
                    "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE "
                    "WHERE REFERENCED_TABLE_NAME = 'Employees' "
                    "AND REFERENCED_COLUMN_NAME = 'id'"
                )
            );
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            // 遍历所有依赖的表并删除对应记录
            while (res->next()) {
                std::string tableName = res->getString("TABLE_NAME");
                std::string columnName = res->getString("COLUMN_NAME");

                // 动态生成删除语句
                std::string deleteQuery = "DELETE FROM " + tableName + " WHERE " + columnName + " = ?";
                std::unique_ptr<sql::PreparedStatement> deleteStmt(
                    tsql.con->prepareStatement(deleteQuery)
                );
                deleteStmt->setInt(1, id);
                deleteStmt->executeUpdate();

                LOGI("成功删除依赖记录: 表 %s, 列 %s", tableName.c_str(), columnName.c_str());
            }

            // 删除 Employees 表中的记录
            pstmt.reset(tsql.con->prepareStatement("DELETE FROM Employees WHERE id = ?"));
            pstmt->setInt(1, id);
            pstmt->executeUpdate();

            LOGI("移除成功: id = %d", id);
        } catch (sql::SQLException& e) {
            LOGE("Error in deleteEmployeeWithDependencies: %s", e.what());
        }
    }

    // 随机生成员工数据并添加到数据库
    void GenerateRandomEmployeeData() {
        static char name[32];
        static char id_card[32];
        static char contact[32];
        static char address[128];
        static char position[128];
        static char department[64];
        static char hire_date[32];
        static char education_background[256];
        static char work_experience[256];
        static char emergency_contact[64];

        // 随机生成数据
        strcpy(name, generateRandomName().c_str());
        strcpy(id_card, generateRandomIdCard().c_str());
        strcpy(contact, generateRandomPhone().c_str());
        strcpy(address, generateRandomAddress().c_str());
        strcpy(position, generateRandomPosition().c_str());
        strcpy(department, generateRandomDepartment().c_str());
        strcpy(hire_date, generateRandomDate().c_str());
        strcpy(education_background, generateRandomEducationBackground().c_str());
        strcpy(work_experience, generateRandomWorkExperience().c_str());
        strcpy(emergency_contact, generateRandomEmergencyContact().c_str());

        // 直接将生成的数据添加到数据库
        addEmployee(name, id_card, contact, address, position, department, hire_date, education_background, work_experience, emergency_contact);
    }
};
