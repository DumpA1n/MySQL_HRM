#ifndef MENU_H
#define MENU_H

extern bool debugMode;
extern bool screenResized;
extern int screenW;
extern int screenH;

class Manager
{
public:
    bool init = false;
    EmployeeManager* em = nullptr;
    RecruitmentManager* rm = nullptr;
    void initialize() {
        em = new EmployeeManager();
        rm = new RecruitmentManager();
        init = true;
    }
    ~Manager() {
        delete em;
        delete rm;
    }
} mgr;

vector<unique_ptr<sql::ResultSet>> resList;
vector<string> queryList;
unordered_map<string, string> translationMap = {
    {"id", "ID"},
    {"name", "名称"},
    {"id_card", "身份证"},
    {"contact", "联系方式"},
    {"address", "地址"},
    {"position", "职位"},
    {"department", "部门"},
    {"hire_date", "入职日期"},
    {"education_background", "教育背景"},
    {"work_experience", "工作经验"},
    {"emergency_contact", "紧急联系人"},

    {"job_id", "职位编号"},
    {"candidate_name", "姓名"},
    {"candidate_email", "邮箱"},
    {"resume", "简历"},
    {"status", "状态"},

    {"application_id", "简历编号"},
    {"interview_date", "面试日期"},
    {"reminder_sent", "是否发送提醒"},


    {"", ""}
};


void GenerateTable(sql::ResultSet* res, int index) {
    string windowName = "SQL Query Results (" + to_string(index) + ")";
    if (res == nullptr) {
        ImGui::Begin(windowName.c_str());
        ImGui::Text("Error: No results to display.");
        ImGui::End();
        return;
    }

    try {
        // 获取列数
        sql::ResultSetMetaData* metaData = res->getMetaData();
        int colCount = metaData->getColumnCount();
        int rowCount = res->getRow();

        ImGui::Begin(windowName.c_str());

        // 动态创建表格，根据列数设置表格的列数
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
        static int freeze_cols = 1;
        static int freeze_rows = 1;
        ImGui::CheckboxFlags("手动调整列宽度", &flags, ImGuiTableFlags_Resizable);

        ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * rowCount);
        if (ImGui::BeginTable("SQLTable", colCount, flags, outer_size)) {
            // 生成表头，获取每一列的名称并创建表头
            ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
            for (int col = 1; col <= colCount; ++col) {
                ImGui::TableSetupColumn(translationMap[metaData->getColumnLabel(col)].c_str());
            }
            ImGui::TableHeadersRow();

            // 遍历 ResultSet 数据，逐行填充表格
            res->beforeFirst();
            while (res->next()) {
                ImGui::TableNextRow(); // 创建一行

                // 遍历每一列
                for (int col = 1; col <= colCount; ++col) {
                    ImGui::TableSetColumnIndex(col - 1); // 设置列索引

                    // 获取列数据并显示（根据数据类型自动处理）
                    int colType = metaData->getColumnType(col);
                    if (colType == sql::DataType::VARCHAR || colType == sql::DataType::CHAR) {
                        ImGui::Text("%s", res->getString(col).c_str());
                    } else if (colType == sql::DataType::INTEGER || colType == sql::DataType::SMALLINT) {
                        ImGui::Text("%d", res->getInt(col));
                    } else if (colType == sql::DataType::DOUBLE) {
                        ImGui::Text("%.2f", res->getDouble(col));
                    } else {
                        ImGui::Text("%s", res->getString(col).c_str());
                    }
                }
            }
            ImGui::EndTable();
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error generating table: " << e.what() << std::endl;
    }

    if (ImGui::Button("关闭")) {
        resList.erase(resList.begin() + index);
    }
    ImGui::End();
}

void ShowDebugWindow() {
    static ImVec2 initial_size = ImVec2(screenW * 1.0f, screenH * 0.2f);
    static ImVec2 initial_pos = ImVec2(screenW * 0.0f, screenH * 0.8f);

    // if (screenResized) {
        ImGui::SetNextWindowSize(initial_size);
        ImGui::SetNextWindowPos(initial_pos);
    //     screenResized = false;
    // }

    ImGui::Begin("Debug Info");

    if (ImGui::Button("Clear")) {
        mlog.clear();
    }
    ImGui::Text(mlog.infoS.c_str());

    ImGui::End();
}

string buildQuery(const string& field, const string& value, bool& hasCondition, bool like = false) {
    if (!value.empty()) {
        std::ostringstream condition;
        if (hasCondition) {
            if (like)
                condition << " AND " << field << " like '%" << value << "%'";
            else condition << " AND " << field << " = '" << value << "'";
        } else {
            if (like)
                condition << " WHERE " << field << " like '%" << value << "%'";
            else condition << " WHERE " << field << " = '" << value << "'";
            hasCondition = true;
        }
        return condition.str();
    }
    return "";
}

void ShowMenu() {
    if (!mgr.init)
        mgr.initialize();

    ImGui::Begin("HRM");

    if (ImGui::Button("Console Clear")) { system("cls"); }

// #################################################### 员工管理 ####################################################
    if (ImGui::CollapsingHeader("员工管理"))
    {
        ImGui::Indent(30.0f);
        // #############################################################################
        // #############################################################################
        if (ImGui::CollapsingHeader("添加员工")) {
            static char name[128];
            static char id_card[128];
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("名称##name1", name, sizeof(name));
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("身份证##id_card1", id_card, sizeof(id_card));
            if (ImGui::Button("添加")) {
                mgr.em->addEmployee(name, id_card);
            }
        }
        // #############################################################################
        // #############################################################################
        if (ImGui::CollapsingHeader("查询员工")) {
            string query = "select * from Employees";
            bool hasCondition = false;
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

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("名称##name2", name, sizeof(name));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("身份证##id_card2", id_card, sizeof(id_card));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("联系方式", contact, sizeof(contact));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("地址", address, sizeof(address));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("职位", position, sizeof(position));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("部门", department, sizeof(department));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("入职日期", hire_date, sizeof(hire_date));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("教育背景", education_background, sizeof(education_background));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("工作经验", work_experience, sizeof(work_experience));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("紧急联系人", emergency_contact, sizeof(emergency_contact));

            static bool useCustomQuery = false;
            static char customQuery[1024] = "select * from Employees";
            ImGui::Checkbox("使用自定义SQL", &useCustomQuery);
            if (useCustomQuery) {
                ImGui::SetTooltip("TABLE Employees \nid INT AUTO_INCREMENT PRIMARY KEY \nname VARCHAR(255) \nid_card VARCHAR(20) \ncontact VARCHAR(50) \naddress VARCHAR(255) \nposition VARCHAR(100) \ndepartment VARCHAR(100) \nhire_date DATE \neducation_background TEXT \nwork_experience TEXT \nemergency_contact VARCHAR(255)");
                ImGui::InputTextMultiline("", customQuery, sizeof(customQuery), ImVec2(300.0f, 100.0f));
            }
            if (ImGui::Button("查询")) {
                if (useCustomQuery) {
                    resList.push_back(std::move(mgr.em->executeQuery(customQuery)));
                } else {
                    query += buildQuery("name", string(name), hasCondition, true);
                    query += buildQuery("id_card", string(id_card), hasCondition);
                    query += buildQuery("contact", string(contact), hasCondition);
                    query += buildQuery("address", string(address), hasCondition);
                    query += buildQuery("position", string(position), hasCondition);
                    query += buildQuery("department", string(department), hasCondition);
                    query += buildQuery("hire_date", string(hire_date), hasCondition);
                    query += buildQuery("education_background", string(education_background), hasCondition);
                    query += buildQuery("work_experience", string(work_experience), hasCondition);
                    query += buildQuery("emergency_contact", string(emergency_contact), hasCondition);
                    resList.push_back(std::move(mgr.em->executeQuery(query.c_str())));
                }
                LOGI("Search Success");
            }
            ImGui::SameLine();
            if (ImGui::Button("清除所有查询结果")) {
                resList.clear();
            }
        }
        if (ImGui::CollapsingHeader("移除员工")) {
            static char id[32];
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("id", id, sizeof(id));
            if (ImGui::Button("移除")) {
                mgr.em->deleteEmployee(id);
            }
        }
        ImGui::Unindent(30.0f);
    }

// #################################################### 招聘与入职管理 ####################################################
    if (ImGui::CollapsingHeader("招聘与入职管理")) {
        ImGui::Indent(30.0f);
        if (debugMode) {
            if (ImGui::CollapsingHeader("调试功能##招聘与入职管理")) {
                static int job_id = 1;  // 默认为1号职位
                ImGui::InputInt("职位编号", &job_id);
                if (ImGui::Button("随机生成简历")) {
                    mgr.rm->addRandomApplication(job_id);
                }
            }
        }

        if (ImGui::CollapsingHeader("发布职位")) {
            static char title[128] = "C++(图形研发)";
            static char description[1024] = "1、负责移动端3D渲染引擎的功能研究及开发；\n2、参与项目的AR特效开发，特效SDK维护；\n3、对渲染相关技术进行前瞻性研究；";
            static char requirements[1024] = "1、熟悉OpenGL/OpenGL ES/DirectX/Metal/Vulkan其中一种图形库开发，理解渲染优化策略，熟悉Shader和GLSL；\n2、扎实的C++语言基础，熟悉移动端开发平台环境；\n3、有良好的分析和解决问题能力，逻辑思维清晰严谨。";
            static char salary_range[64] = "15k ~ 20k";
            ImGui::InputText("职位标题", title, sizeof(title));
            ImGui::InputTextMultiline("职位描述", description, sizeof(description), ImVec2(0.0f, 100.0f));
            ImGui::InputTextMultiline("职位要求", requirements, sizeof(requirements), ImVec2(0.0f, 100.0f));
            ImGui::InputText("薪资范围", salary_range, sizeof(salary_range));
            if (ImGui::Button("发布")) {
                mgr.rm->addJobPost(title, description, requirements, salary_range);
            }
            if (ImGui::Button("查看已发布职位")) {
                mgr.rm->addJobPost(title, description, requirements, salary_range);
            }
        }

        if (ImGui::CollapsingHeader("简历筛选")) {
            static char keyword[128];
            ImGui::InputText("关键词", keyword, sizeof(keyword));
            if (ImGui::Button("筛选简历")) {
                resList.push_back(mgr.rm->autoFilterResumes(keyword));
            }
        }

        if (ImGui::CollapsingHeader("面试安排")) {
            static int application_id;
            static char interview_date[64];
            ImGui::InputInt("简历编号", &application_id);
            ImGui::InputText("面试日期", interview_date, sizeof(interview_date));
            if (ImGui::Button("安排面试")) {
                mgr.rm->scheduleInterview(application_id, interview_date);
            }
            if (ImGui::Button("查看面试排期")) {
                static string query = "SELECT * FROM Interviews";
                resList.push_back(mgr.rm->executeQuery(query));
            }
        }

        ImGui::Unindent(30.0f);
    }

    if (ImGui::CollapsingHeader("AttendanceManager"))
    {
    }

    if (ImGui::CollapsingHeader("PayrollManager"))
    {
    }

    if (ImGui::CollapsingHeader("PerformanceManager"))
    {
    }

    if (ImGui::CollapsingHeader("TrainingManager"))
    {
    }

    if (ImGui::CollapsingHeader("EmployeeServiceManager"))
    {
    }

    for (int i = 0; i < resList.size(); i++) {
        GenerateTable(resList[i].get(), i);
    }
    ShowDebugWindow();
    ImGui::End();
}

#endif // MENU_H