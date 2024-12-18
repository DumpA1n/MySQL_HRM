// 培训发展管理模块
class TrainingManager : public HRM {
public:
    const char* TrainingTable = R"(
        CREATE TABLE IF NOT EXISTS TrainingCourses (
            id INT AUTO_INCREMENT PRIMARY KEY,
            course_name VARCHAR(255),
            description TEXT,
            duration_days INT,
            level VARCHAR(50),
            start_date DATE
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

    void generateRandomTrainingCourses(int numCourses = 5) {

        // 课程名称模板
        vector<string> courseNames = {
            "高级C++编程与性能优化", 
            "OpenGL与Vulkan基础",
            "实时渲染技术", 
            "计算机图形学入门",
            "跨平台移动开发",
            "游戏引擎开发",
            "AR/VR图形技术",
            "GPU计算与并行编程",
            "深度学习与计算机视觉",
            "云渲染架构设计"
        };

        // 课程描述模板
        vector<string> descriptions = {
            "本课程覆盖{0}相关基础知识和实战技巧，适合初学者和进阶者。",
            "通过实际案例学习{0}技术的应用，掌握核心技能。",
            "深入探讨{0}的关键技术点，并提供高效解决方案。",
            "涵盖{0}从基础到高阶的内容，帮助学员全面提升。",
            "通过大量实战演练和项目开发，掌握{0}的核心应用。",
            "讲解最新{0}技术趋势，提供创新性解决方案。",
            "本课程针对{0}的常见难点进行详细剖析，提升实践能力。",
            "通过{0}技术的深入学习，培养独立解决问题的能力。",
            "专注于{0}的高性能优化，打造业内领先技术能力。",
            "面向实际项目开发，全面提升学员对{0}的理解与应用。"
        };

        // 难度级别
        vector<string> levels = {"初级", "中级", "高级"};

        // 持续时间范围（天）
        int minDuration = 3;
        int maxDuration = 30;

        // 随机日期生成基准
        time_t currentTime = time(nullptr);
        tm* currentDate = localtime(&currentTime);

        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO TrainingCourses (course_name, description, duration_days, level, start_date) "
                "VALUES (?, ?, ?, ?, ?)"
            );

            // 设置随机数种子
            srand(time(nullptr));

            for (int i = 0; i < numCourses; ++i) {
                // 随机选择课程名称
                string courseName = courseNames[rand() % courseNames.size()];

                // 格式化课程描述
                string description = fmt::format(
                    descriptions[rand() % descriptions.size()], 
                    courseName
                );

                // 随机生成课程持续时间
                int duration = minDuration + rand() % (maxDuration - minDuration + 1);

                // 随机选择课程难度
                string level = levels[rand() % levels.size()];

                // 随机生成开始日期
                int daysOffset = rand() % 90; // 假设90天内随机开始
                currentDate->tm_mday += daysOffset;
                mktime(currentDate); // 确保日期合法
                char startDate[11];
                strftime(startDate, sizeof(startDate), "%Y-%m-%d", currentDate);

                // 执行插入
                pstmt->setString(1, courseName);
                pstmt->setString(2, description);
                pstmt->setInt(3, duration);
                pstmt->setString(4, level);
                pstmt->setString(5, startDate);
                pstmt->execute();
            }

            delete pstmt;
            LOGI("成功生成 %d 条随机培训课程", numCourses);
        } catch (sql::SQLException& e) {
            LOGE("生成随机培训课程失败: %s", e.what());
        }
    }
};