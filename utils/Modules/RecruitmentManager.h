// 招聘入职管理模块
class RecruitmentManager : public HRM {
public:
    const char* JobPosts = R"(
        CREATE TABLE IF NOT EXISTS JobPosts (
            id INT AUTO_INCREMENT PRIMARY KEY,
            title VARCHAR(255),
            description TEXT,
            requirements TEXT,
            salary_range VARCHAR(50),
            status VARCHAR(20) DEFAULT 'Open'
        );
    )";

    const char* Applications = R"(
        CREATE TABLE IF NOT EXISTS Applications (
            id INT AUTO_INCREMENT PRIMARY KEY,
            job_id INT,
            candidate_name VARCHAR(255),
            candidate_email VARCHAR(255),
            resume TEXT,
            status VARCHAR(20) DEFAULT 'Pending',
            FOREIGN KEY (job_id) REFERENCES JobPosts(id)
        );
    )";

    const char* Interviews = R"(
        CREATE TABLE IF NOT EXISTS Interviews (
            id INT AUTO_INCREMENT PRIMARY KEY,
            application_id INT,
            interview_date DATETIME,
            reminder_sent BOOLEAN DEFAULT FALSE,
            status VARCHAR(20) DEFAULT 'Scheduled',
            FOREIGN KEY (application_id) REFERENCES Applications(id)
        );
    )";

    const char* OnboardingTasks = R"(
        CREATE TABLE IF NOT EXISTS OnboardingTasks (
            id INT AUTO_INCREMENT PRIMARY KEY,
            employee_id INT,
            task_description TEXT,
            due_date DATE,
            completed BOOLEAN DEFAULT FALSE,
            FOREIGN KEY (employee_id) REFERENCES Employees(id)
        );
    )";

    RecruitmentManager() {
        try {
            sql::Statement* stmt;
            stmt = tsql.con->createStatement();
            stmt->execute(sql::SQLString(JobPosts));
            stmt->execute(sql::SQLString(Applications));
            stmt->execute(sql::SQLString(Interviews));
            stmt->execute(sql::SQLString(OnboardingTasks));
            delete stmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in RecruitmentManager create table: %s", e.what());
        }
    }

    void addRandomApplication(int job_id) {
        auto RandomGenResume = [](int sig, string name = "") {
            if (sig == 1) {
                const vector<string> firstNames = {"John", "Jane", "Alex", "Chris", "Katie", "Michael", "Laura", "David", "Emily"};
                const vector<string> lastNames = {"Smith", "Johnson", "Brown", "Davis", "Miller", "Wilson", "Moore", "Taylor", "Anderson"};
                return firstNames[rand() % firstNames.size()] + " " + lastNames[rand() % lastNames.size()];
            } else if (sig == 2) {
                const vector<string> emailDomains = {"example.com", "testmail.com", "randomemail.org"};
                string domain = emailDomains[rand() % emailDomains.size()];
                string email = name;
                std::replace(email.begin(), email.end(), ' ', '.');
                return email + "@" + domain;
            } else if (sig == 3) {
                const vector<string> skills = {
                    "C++ Programming", "Java Development", "Database Management", "MySQL Programming", "Project Management", 
                    "Software Testing", "Web Development", "Data Analysis", "Machine Learning"
                };
                string resume = "Skills:\n";
                for (int i = 0; i < 3; ++i) {  // 随机选择3个技能
                    resume += "- " + skills[rand() % skills.size()] + "\n";
                }
                return resume;
            }
        };
        srand((unsigned)time(0));
        string name = RandomGenResume(1);
        string email = RandomGenResume(2, name);
        string resume = RandomGenResume(3);

        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement("INSERT INTO Applications (job_id, candidate_name, candidate_email, resume) VALUES (?, ?, ?, ?)");
            pstmt->setInt(1, job_id);
            pstmt->setString(2, name);
            pstmt->setString(3, email);
            pstmt->setString(4, resume);
            pstmt->execute();
            delete pstmt;
            LOGI("随机简历生成成功");
        } catch (sql::SQLException& e) {
            LOGE("Error in addRandomApplication: %s", e.what());
        }
    }

    void addJobPost(const string& title, const string& description, const string& requirements, const string& salary_range) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement("INSERT INTO JobPosts (title, description, requirements, salary_range) VALUES (?, ?, ?, ?)");
            pstmt->setString(1, title);
            pstmt->setString(2, description);
            pstmt->setString(3, requirements);
            pstmt->setString(4, salary_range);
            pstmt->execute();
            delete pstmt;
            LOGI("职位发布成功");
        } catch (sql::SQLException& e) {
            LOGE("Error in addJobPost: %s", e.what());
        }
    }

    unique_ptr<sql::ResultSet> autoFilterResumes(const string& keyword) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement("SELECT * FROM Applications WHERE resume LIKE ?");
            pstmt->setString(1, "%" + keyword + "%");
            unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            delete pstmt;
            return res;
        } catch (sql::SQLException& e) {
            LOGE("Error in autoFilterResumes: %s", e.what());
        }
        return nullptr;
    }

    void scheduleInterview(int application_id, const string& interview_date) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement("INSERT INTO Interviews (application_id, interview_date) VALUES (?, ?)");
            pstmt->setInt(1, application_id);
            pstmt->setString(2, interview_date);
            pstmt->execute();
            delete pstmt;
            LOGI("面试安排成功");
        } catch (sql::SQLException& e) {
            LOGE("Error in scheduleInterview: %s", e.what());
        }
    }

    void createOnboardingTasks(int employee_id) {
        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement("INSERT INTO OnboardingTasks (employee_id, task_description, due_date) VALUES (?, 'Complete onboarding training', ?)");
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, "2024-09-30");
            pstmt->execute();

            pstmt = tsql.con->prepareStatement("INSERT INTO OnboardingTasks (employee_id, task_description, due_date) VALUES (?, 'Receive office equipment', ?)");
            pstmt->setInt(1, employee_id);
            pstmt->setString(2, "2024-09-30");
            pstmt->execute();

            delete pstmt;
        } catch (sql::SQLException& e) {
            LOGE("Error in createOnboardingTasks: %s", e.what());
        }
    }
};