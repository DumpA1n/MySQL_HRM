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


    void generateRandomJobPosts(int numPosts = 5) {

        vector<string> titleTemplates = {
            "C++软件工程师", 
            "图形渲染开发工程师",
            "游戏引擎开发者", 
            "AR/VR研发工程师",
            "移动图形开发专家",
            "计算机图形学研究员", 
            "跨平台开发工程师",
            "系统性能优化工程师",
            "图形库架构师",
            "嵌入式图形开发工程师",
            "实时渲染技术专家", 
            "GPU计算优化工程师",
            "计算机视觉算法工程师",
            "互动图形应用开发者",
            "多媒体技术研发工程师",
            "3D建模与动画开发工程师",
            "图形算法研究员",
            "云渲染技术开发者",
            "智能图形渲染工程师",
            "跨终端图形解决方案架构师"
        };

        vector<string> descriptionTemplates = {
            "负责{0}相关技术研究和产品开发，参与核心技术攻关。\n主要职责包括：\n1、设计和实现创新技术方案\n2、优化系统性能\n3、撰写技术文档和研发报告",
            
            "我们正在寻找passionate的{0}工程师，加入我们的技术团队。\n岗位职责：\n1、参与前沿技术探索\n2、解决复杂技术难题\n3、推动产品技术革新",
            
            "作为{0}核心研发成员，你将：\n1、负责关键技术模块设计\n2、进行性能调优\n3、推进技术创新\n4、参与技术方案评审",
            
            "深入研究图形渲染和计算机图形学前沿技术，打造行业领先的{0}解决方案。\n职责：\n1、算法创新\n2、性能优化\n3、技术预研",
            
            "面向未来科技，我们招募顶尖{0}专家，共同构建革命性图形技术。\n关键任务：\n1、开发高性能渲染引擎\n2、攻克关键技术难点\n3、引领技术发展方向",
            
            "为游戏、影视、仿真等领域提供卓越的图形技术支持。{0}岗位诚招精英。\n主要工作：\n1、设计跨平台渲染方案\n2、优化图形渲染流程\n3、研究先进渲染技术",
            
            "我们的{0}团队致力于突破图形计算技术边界，打造极致用户体验。\n核心职责：\n1、架构设计\n2、关键技术攻关\n3、性能极限探索",
            
            "面向全球市场，打造超越业界的图形技术。{0}岗位提供极具竞争力的发展平台。\n职责范围：\n1、前沿技术研究\n2、架构设计\n3、技术创新",
            
            "为推动图形技术革命，诚聘{0}技术专家。\n岗位职责：\n1、算法研究\n2、系统优化\n3、技术预研与创新",
            
            "加入我们的{0}团队，共同定义图形技术的未来。\n主要工作：\n1、开发高性能渲染引擎\n2、解决复杂技术挑战\n3、推动技术创新",
            
            "致力于人工智能与图形技术融合的{0}岗位，开拓技术新边界。\n职责：\n1、算法创新\n2、系统架构\n3、性能优化",
            
            "构建跨平台、高性能的图形解决方案。{0}岗位诚招技术精英。\n关键任务：\n1、架构设计\n2、技术攻关\n3、性能调优",
            
            "我们的{0}团队致力于推动图形技术的极限。\n主要职责：\n1、渲染技术研究\n2、性能优化\n3、创新算法设计",
            
            "面向全球市场的{0}技术创新团队，提供技术领先的解决方案。\n岗位职责：\n1、技术预研\n2、系统优化\n3、算法创新",
            
            "为游戏、影视、AR/VR等领域提供顶级{0}解决方案。\n核心工作：\n1、渲染引擎开发\n2、性能优化\n3、技术创新",
            
            "打造世界级图形技术，{0}岗位诚邀技术精英。\n主要职责：\n1、架构设计\n2、关键技术攻关\n3、性能极限探索",
            
            "推动计算机图形学技术革命的{0}岗位，开拓技术新视野。\n职责范围：\n1、算法研究\n2、系统优化\n3、创新技术开发",
            
            "为下一代图形技术而生的{0}团队，重新定义图形计算边界。\n核心任务：\n1、前沿技术研究\n2、创新算法设计\n3、性能极致优化",
            
            "高度创新的{0}岗位，挑战图形技术的极限。\n主要工作：\n1、渲染技术攻关\n2、跨平台方案设计\n3、技术创新",
            
            "我们的{0}团队致力于突破图形计算的技术瓶颈。\n职责：\n1、算法创新\n2、系统架构\n3、性能调优"
        };

        vector<string> requirementsTemplates = {
            "岗位要求：\n1、计算机相关专业本科及以上学历\n2、{0}相关工作经验3年以上\n3、扎实的编程和算法基础\n4、优秀的问题分析和解决能力",
            
            "理想候选人：\n1、精通{0}开发技术\n2、熟悉主流开发框架和工具\n3、有创新精神和团队协作能力\n4、良好的技术前瞻性思维",
            
            "职位需求：\n1、深入理解{0}技术原理\n2、具备独立解决复杂技术问题能力\n3、热爱技术钻研，学习能力强\n4、有开源项目贡献者优先",
            
            "技术要求：\n1、精通C++，熟悉{0}开发\n2、具有扎实的计算机图形学基础\n3、熟悉OpenGL/Vulkan/DirectX等图形API\n4、有性能优化和渲染技术经验",
            
            "岗位期望：\n1、{0}相关专业背景\n2、3-5年图形开发经验\n3、深入理解渲染管线\n4、具备技术创新能力",
            
            "核心能力要求：\n1、精通{0}技术\n2、熟悉跨平台开发\n3、具有图形算法和性能优化经验\n4、良好的技术沟通能力",
            
            "理想kandidat具备：\n1、{0}相关工作经验\n2、扎实的图形学和计算机图形技术基础\n3、优秀的问题分析和解决能力\n4、持续学习的技术热情",
            
            "技术精英要求：\n1、精通{0}开发技术\n2、具有GPU编程和渲染优化经验\n3、熟悉主流图形库和渲染框架\n4、有技术创新追求",
            
            "岗位要求：\n1、计算机图形学背景\n2、{0}相关技术深入研究\n3、卓越的编程和算法能力\n4、开放和创新的技术思维",
            
            "候选人应具备：\n1、深入理解{0}技术原理\n2、熟练掌握图形渲染相关技术\n3、有架构设计经验\n4、优秀的团队协作能力",
            
            "技术精英招募：\n1、{0}开发3年以上经验\n2、精通C++和图形编程\n3、熟悉主流渲染技术\n4、有技术创新追求",
            
            "高级岗位要求：\n1、计算机图形学背景\n2、{0}技术专家级别\n3、深入理解渲染管线\n4、具备系统架构设计能力",
            
            "岗位期望：\n1、精通{0}相关技术\n2、具有跨平台开发经验\n3、优秀的性能调优能力\n4、技术视野开阔",
            
            "技术要求：\n1、{0}领域专家\n2、精通GPU编程\n3、熟悉图形算法和渲染技术\n4、持续技术创新能力",
            
            "候选人应具备：\n1、深入的{0}技术背景\n2、优秀的编程和架构设计能力\n3、技术前瞻性思维\n4、卓越的学习能力",
            
            "核心能力要求：\n1、{0}相关工作经验\n2、精通渲染技术和性能优化\n3、具有技术创新能力\n4、优秀的团队协作精神",
            
            "理想kandidat：\n1、{0}技术深度研究\n2、熟悉复杂图形系统架构\n3、具有算法创新能力\n4、持续学习的技术热情",
            
            "岗位要求：\n1、计算机图形学专业背景\n2、{0}领域深入研究\n3、卓越的技术洞察力\n4、开源项目贡献经验",
            
            "技术精英招募：\n1、精通{0}开发技术\n2、GPU渲染优化专家\n3、具有系统架构设计经验\n4、技术创新追求",
            
            "高级岗位期望：\n1、{0}技术专家\n2、深入理解计算机图形学\n3、优秀的技术解决方案设计能力\n4、技术前沿洞察力",
            
            "核心技术要求：\n1、{0}领域资深专家\n2、熟悉多种渲染技术\n3、具备架构设计能力\n4、持续技术创新精神"
        };

        vector<string> salaryRanges = {
            "15k ~ 25k", 
            "20k ~ 30k", 
            "25k ~ 35k", 
            "30k ~ 40k", 
            "18k ~ 28k"
        };

        try {
            sql::PreparedStatement* pstmt = tsql.con->prepareStatement(
                "INSERT INTO JobPosts (title, description, requirements, salary_range) VALUES (?, ?, ?, ?)"
            );

            // 设置随机数种子
            srand(time(nullptr));

            for (int i = 0; i < numPosts; ++i) {
                // 随机选择模板
                string title = titleTemplates[rand() % titleTemplates.size()];
                
                // 格式化职位描述
                string description = fmt::format(
                    descriptionTemplates[rand() % descriptionTemplates.size()], 
                    title
                );

                // 格式化职位要求
                string requirements = fmt::format(
                    requirementsTemplates[rand() % requirementsTemplates.size()], 
                    title
                );

                string salaryRange = salaryRanges[rand() % salaryRanges.size()];

                // 执行插入
                pstmt->setString(1, title);
                pstmt->setString(2, description);
                pstmt->setString(3, requirements);
                pstmt->setString(4, salaryRange);
                pstmt->execute();
            }

            delete pstmt;
            LOGI("成功生成 %d 条随机职位", numPosts);
        } catch (sql::SQLException& e) {
            LOGE("生成随机职位失败: %s", e.what());
        }
    }
};