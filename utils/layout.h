#pragma once
#ifndef LAYOUT_H
#define LAYOUT_H

class Manager
{
public:
    bool init = false;
    HRM* hrm = nullptr;
    EmployeeManager* emplmgr = nullptr;
    RecruitmentManager* recumgr = nullptr;
    AttendanceManager* attemgr = nullptr;
    PayrollManager* payrmgr = nullptr;
    PerformanceManager* perfmgr = nullptr;
    TrainingManager* traimgr = nullptr;
    EmployeeServiceManager* emplsvcmgr = nullptr;

    void initialize() {
        hrm = new HRM();
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
        delete hrm;
        delete emplmgr;
        delete recumgr;
        delete attemgr;
        delete payrmgr;
        delete perfmgr;
        delete traimgr;
        delete emplsvcmgr;
    }
} mgr;

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

vector<unique_ptr<sql::ResultSet>> resList;
vector<string> queryList;

struct QueryResult {
    std::string query;               // SQL 查询语句
    std::string tableName;           // 表名
    std::unique_ptr<sql::ResultSet> resultSet; // 查询结果
    bool bTabOpen;                   // 默认 Tab 打开
    bool bUpdated;                   // 是否需要更新的标志

    QueryResult(const std::string& q, const std::string& t, std::unique_ptr<sql::ResultSet> res)
        : query(q), tableName(t), resultSet(std::move(res)), bTabOpen(true), bUpdated(false) {}
};


std::vector<QueryResult> queryResults;  // 用来存储多个查询结果



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







void GenerateTable(std::vector<QueryResult>& queryResults) {
    // if (queryResults.empty()) return;

    // 如果需要更新，执行查询并刷新结果
    for (auto& it : queryResults) {
        // if (it.bUpdated) {
            it.resultSet = mgr.hrm->executeQuery(it.query.c_str());
        //     it.bUpdated = false;
        // }
    }

    static char filterKeyword[128] = "";

    ImGui::SetNextWindowSize(ImVec2(screenW * 0.6f, screenH * 0.8f));
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.2f, screenH * 0.0f));
    ImGui::Begin("SQL Query Results");

    if (ImGui::BeginTabBar("QueryResultsTabBar")) {
        for (size_t i = 0; i < queryResults.size(); ++i) {
            QueryResult& result = queryResults[i];

            if (ImGui::BeginTabItem(("Tab " + std::to_string(i + 1) + " (" + result.tableName + ")").c_str(), &result.bTabOpen)) {

                ImGui::Spacing();
                ImGui::Text("Filter:");
                ImGui::SameLine();
                ImGui::InputText("##FilterKeyword", filterKeyword, sizeof(filterKeyword));

                sql::ResultSet* res = result.resultSet.get();

                if (!res) {
                    ImGui::Text("Error: No results to display for the selected table.");
                } else {
                    try {
                        sql::ResultSetMetaData* metaData = res->getMetaData();
                        int colCount = metaData->getColumnCount();

                        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
                        static int freeze_cols = 1;
                        static int freeze_rows = 1;

                        ImVec2 outer_size = ImVec2(0.0f, 0.0f);
                        if (ImGui::BeginTable(("SQLTable##" + std::to_string(i)).c_str(), colCount, flags, outer_size)) {
                            ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
                            for (int col = 1; col <= colCount; ++col) {
                                if (translationMap.find(metaData->getColumnLabel(col)) != translationMap.end())
                                    ImGui::TableSetupColumn(translationMap[metaData->getColumnLabel(col)].c_str());
                                else ImGui::TableSetupColumn(metaData->getColumnLabel(col).c_str());
                            }
                            ImGui::TableHeadersRow();

                            res->beforeFirst();
                            static std::set<std::pair<int, int>> selectedCells; // 存储选中的行和列

                            while (res->next()) {
                                ImGui::TableNextRow(0, 30.0f);
                                int rowId = res->getInt(1);

                                for (int col = 1; col <= colCount; ++col) {
                                    ImGui::TableSetColumnIndex(col - 1);
                                    std::pair<int, int> cellId = {rowId, col};
                                    bool selected = selectedCells.count(cellId) > 0;

                                    if (ImGui::Selectable(res->getString(col).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns)) {
                                        if (selected) {
                                            selectedCells.erase(cellId);
                                        } else {
                                            selectedCells.insert(cellId);
                                        }
                                    }

                                    if (ImGui::BeginPopupContextItem(("CellContextMenu##" + std::to_string(rowId) + std::to_string(col)).c_str())) {
                                        if (selectedCells.empty()) {
                                            static char newValue[128] = "";
                                            ImGui::Text("编辑:");
                                            ImGui::InputText("##NewValue", newValue, sizeof(newValue));
                                            if (ImGui::Button("修改")) {
                                                std::string columnName = metaData->getColumnLabel(col);
                                                updateColumnValue(result.tableName, columnName, "id", rowId, newValue);
                                                result.bUpdated = true;
                                                selectedCells.clear();
                                                ImGui::CloseCurrentPopup();
                                            }
                                            ImGui::SameLine();
                                            if (ImGui::Button("取消")) {
                                                ImGui::CloseCurrentPopup();
                                            }
                                            if (ImGui::MenuItem("删除此行")) {
                                                deleteColumn(result.tableName, "id", rowId);
                                                result.bUpdated = true;
                                                selectedCells.clear();
                                                ImGui::CloseCurrentPopup();
                                            }
                                        } else {
                                            if (ImGui::MenuItem("批量删除")) {
                                                for (const auto& cell : selectedCells) {
                                                    deleteColumn(result.tableName, "id", cell.first);
                                                }
                                                result.bUpdated = true;
                                                selectedCells.clear();
                                                ImGui::CloseCurrentPopup();
                                            }
                                            if (ImGui::MenuItem("取消所有选择")) {
                                                selectedCells.clear();
                                                ImGui::CloseCurrentPopup();
                                            }
                                        }

                                        ImGui::EndPopup();
                                    }
                                }
                            }

                            ImGui::EndTable();
                        }
                    } catch (sql::SQLException& e) {
                        std::cerr << "Error generating table: " << e.what() << std::endl;
                    }
                }

                if (!result.bTabOpen) {
                    queryResults.erase(queryResults.begin() + i);
                    --i;
                }

                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}












#endif