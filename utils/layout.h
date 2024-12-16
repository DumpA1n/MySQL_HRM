#pragma once
#ifndef LAYOUT_H
#define LAYOUT_H

class Manager
{
public:
    bool init = false;
    EmployeeManager* emplmgr = nullptr;
    RecruitmentManager* recumgr = nullptr;
    AttendanceManager* attemgr = nullptr;
    PayrollManager* payrmgr = nullptr;
    PerformanceManager* perfmgr = nullptr;
    TrainingManager* traimgr = nullptr;
    EmployeeServiceManager* emplsvcmgr = nullptr;

    void initialize() {
        emplmgr = new EmployeeManager();
        recumgr = new RecruitmentManager();
        attemgr = new AttendanceManager();
        payrmgr = new PayrollManager();
        perfmgr = new PerformanceManager();
        traimgr = new TrainingManager();
        emplsvcmgr = new EmployeeServiceManager();
        init = true;
    }
    ~Manager() {
        delete emplmgr;
        delete recumgr;
        delete attemgr;
        delete payrmgr;
        delete perfmgr;
        delete traimgr;
        delete emplsvcmgr;
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
};

void ShowDebugWindow() {
    ImGui::SetNextWindowSize(ImVec2(screenW * 1.0f, screenH * 0.2f));
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.0f, screenH * 0.8f));

    ImGui::Begin("Debug Info");

    if (ImGui::Button("Clear")) {
        mlog.clear();
    }
    ImGui::SameLine();
    static bool autoScroll = true;
    ImGui::Checkbox("Auto Scroll", &autoScroll);

    ImGui::BeginChild("LogRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& log : mlog.get()) {
        ImGui::Text(log.c_str());
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::End();
}


void GenerateTable(sql::ResultSet* res, int index) {
    ImGui::SetNextWindowSize(ImVec2(screenW * 0.8f, screenH * 0.8f));
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.2f, screenH * 0.0f));

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

void GenerateTable(std::vector<std::unique_ptr<sql::ResultSet>>& resList) {
    if (resList.empty()) return;

    static int currentTableIndex = 0;
    static char filterKeyword[128] = "";

    ImGui::SetNextWindowSize(ImVec2(screenW * 0.8f, screenH * 0.8f));
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.2f, screenH * 0.0f));
    ImGui::Begin("SQL Query Results");

    // 表格选择下拉菜单
    if (!resList.empty()) {
        std::vector<std::string> tableNames;
        for (size_t i = 0; i < resList.size(); ++i) {
            tableNames.push_back("Table " + std::to_string(i + 1));
        }

        std::vector<const char*> cTableNames;
        for (const auto& name : tableNames) {
            cTableNames.push_back(name.c_str());
        }

        ImGui::Text("Select a table to display:");
        ImGui::Combo("##TableSelector", &currentTableIndex, cTableNames.data(), cTableNames.size());
    }

    // 显示过滤输入框
    ImGui::Spacing();
    ImGui::Text("Filter rows by keyword:");
    ImGui::InputText("##FilterKeyword", filterKeyword, sizeof(filterKeyword));

    // 显示当前表格
    if (currentTableIndex >= 0 && currentTableIndex < static_cast<int>(resList.size())) {
        sql::ResultSet* res = resList[currentTableIndex].get();

        if (!res) {
            ImGui::Text("Error: No results to display for the selected table.");
        } else {
            try {
                // 获取列数
                sql::ResultSetMetaData* metaData = res->getMetaData();
                int colCount = metaData->getColumnCount();
                int rowCount = res->getRow();

                // 显示表格标题
                static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
                static int freeze_cols = 1;
                static int freeze_rows = 1;

                ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * min(rowCount, 30));
                if (ImGui::BeginTable(("SQLTable##" + std::to_string(currentTableIndex)).c_str(), colCount, flags, outer_size)) {
                    // 表头
                    ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
                    for (int col = 1; col <= colCount; ++col) {
                        ImGui::TableSetupColumn(translationMap[metaData->getColumnLabel(col)].c_str());
                    }
                    ImGui::TableHeadersRow();

                    // 遍历行数据，显示包含关键字的行
                    res->beforeFirst();
                    while (res->next()) {
                        bool showRow = false;

                        // 检查行中的所有列是否包含关键字
                        for (int col = 1; col <= colCount; ++col) {
                            std::string cellValue = res->getString(col);
                            if (cellValue.find(filterKeyword) != string::npos) {
                                showRow = true;
                                break;
                            }
                        }

                        // 如果该行包含关键字，则显示
                        if (showRow) {
                            ImGui::TableNextRow();
                            for (int col = 1; col <= colCount; ++col) {
                                ImGui::TableSetColumnIndex(col - 1);
                                ImGui::Text("%s", res->getString(col).c_str());
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            } catch (sql::SQLException& e) {
                std::cerr << "Error generating table: " << e.what() << std::endl;
            }
        }
    } else {
        ImGui::Text("No tables available.");
    }

    if (ImGui::Button("关闭")) {
        resList.erase(resList.begin() + currentTableIndex);
    }

    ImGui::End();
}

#endif