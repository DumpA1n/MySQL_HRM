#ifndef MENU_H
#define MENU_H

class Manager
{
public:
    bool init = false;
    EmployeeManager* em = nullptr;
    void initialize() {
        em = new EmployeeManager();
        init = true;
    }
    ~Manager() {
        delete em;
    }
} mgr;

vector<unique_ptr<sql::ResultSet>> resList;
vector<string> queryList;

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
                ImGui::TableSetupColumn(metaData->getColumnLabel(col).c_str());
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

void ShowDebugWindow(static string& debuginfo) {
    static string alldebuginfo;
    alldebuginfo += debuginfo;

    ImGui::Begin("Debug Info");

    if (ImGui::Button("Clear")) {
        debuginfo.clear();
        alldebuginfo.clear();
    }
    ImGui::Text(debuginfo.c_str());

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
            condition << " WHERE " << field << " = '" << value << "'";
            hasCondition = true;
        }
        return condition.str();
    }
    return "";
}

void ShowMenu() {
    if (!mgr.init)
        mgr.initialize();

    static string debuginfo;

    ImGui::Begin("HRM");

    if (ImGui::Button("Console Clear")) { system("cls"); }

    if (ImGui::CollapsingHeader("员工管理"))
    {
        ImGui::Indent(30.0f);
        // #############################################################################
        // #############################################################################
        if (ImGui::CollapsingHeader("添加员工")) {
            static char name[128];
            static char id_card[128];
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("名称", name, sizeof(name));
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("身份证", id_card, sizeof(id_card));
            if (ImGui::Button("添加")) {
                mgr.em->addEmployee(name, id_card);
                debuginfo += to_string(glfwGetTime()) + " Add Success \n";
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
            ImGui::InputText("名称", name, sizeof(name));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("身份证", id_card, sizeof(id_card));

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
                debuginfo += "Search Success \n";
            }
            ImGui::SameLine();
            if (ImGui::Button("清除")) {
                resList.clear();
            }
        }
        if (ImGui::CollapsingHeader("移除员工")) {
            static char id[32];
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("id", id, sizeof(id));
            if (ImGui::Button("移除")) {
                mgr.em->deleteEmployee(id);
                debuginfo += "Delete Success: id = " + string(id) + "\n";
            }
        }
        ImGui::Unindent(30.0f);
    }

    if (ImGui::CollapsingHeader("RecruitmentManager"))
    {
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
    ShowDebugWindow(debuginfo);
    ImGui::End();
}

#endif // MENU_H