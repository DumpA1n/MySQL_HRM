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
    DataAnalysisManager* analmgr = nullptr;

    void initialize() {
        hrm = new HRM();
        emplmgr = new EmployeeManager();
        recumgr = new RecruitmentManager();
        attemgr = new AttendanceManager();
        payrmgr = new PayrollManager();
        perfmgr = new PerformanceManager();
        traimgr = new TrainingManager();
        emplsvcmgr = new EmployeeServiceManager();
        analmgr = new DataAnalysisManager();
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
        delete analmgr;
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

    {"employee_id", "员工ID"},

    {"job_id", "职位编号"},
    {"candidate_name", "姓名"},
    {"candidate_email", "邮箱"},
    {"candidate_phone", "手机号"},
    {"resume", "简历内容"},
    {"status", "状态"},
    {"application_date", "投递时间"},
    {"score", "评分"},

    {"title", "标题"},
    {"description", "描述"},
    {"requirements", "需求"},
    {"salary_range", "薪资范围"},

    {"application_id", "简历编号"},
    {"reminder_sent", "是否发送提醒"},

    {"attendance_date", "签到日期"},
    {"earliest_check_in_time", "早签到时间"},
    {"latest_check_in_time", "晚签到时间"},
    {"first_status", "早签到状态"},
    {"second_status", "晚签到状态"},

    {"month", "月份"},
    {"amount", "￥"},

    {"review_period", "评估时间"},

    {"course_id", "课程ID"},
    {"course_name", "课程名称"},

    {"start_date", "开始时间"},
    {"end_date", "结束时间"},
    {"leave_type", "请假类型"},
    {"leave_reason", "请假原因"},

    {"reimbursement_type", "报销类型"},
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
    // 如果需要更新，执行查询并刷新结果
    for (auto& it : queryResults) {
        it.resultSet = mgr.hrm->executeQuery(it.query.c_str());
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
                            static std::map<int, std::map<int, std::string>> editValues; // 存储待编辑值

                            while (res->next()) {
                                bool showRow = false;
                                // 关键字过滤
                                for (int col = 1; col <= colCount; ++col) {
                                    if (res->getString(col).find(filterKeyword) != std::string::npos) {
                                        showRow = true;
                                        break;
                                    }
                                }

                                if (showRow) {
                                    ImGui::TableNextRow(0, 30.0f);
                                    int rowId = res->getInt(1);

                                    for (int col = 1; col <= colCount; ++col) {
                                        ImGui::TableSetColumnIndex(col - 1);
                                        std::pair<int, int> cellId = {rowId, col};
                                        bool selected = selectedCells.count(cellId) > 0;

                                        if (ImGui::Selectable((res->getString(col) + "##" + std::to_string(rowId) + std::to_string(col)).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns)) {
                                            if (selected) {
                                                selectedCells.erase(cellId);
                                            } else {
                                                selectedCells.insert(cellId);
                                            }
                                        }

                                        // 右键菜单
                                        if (!selectedCells.empty() && ImGui::BeginPopupContextItem(("CellContextMenu##" + std::to_string(rowId) + std::to_string(col)).c_str())) {
                                            if (selectedCells.size() == 1) {
                                                if (ImGui::Button("编辑此行")) {
                                                    // 初始化待编辑值
                                                    editValues[rowId].clear();
                                                    for (int editCol = 1; editCol <= colCount; ++editCol) {
                                                        editValues[rowId][editCol] = res->getString(editCol);
                                                    }
                                                    ImGui::OpenPopup(("EditRowPopup##" + std::to_string(rowId)).c_str());
                                                }

                                                // 编辑弹窗逻辑
                                                if (ImGui::BeginPopup(("EditRowPopup##" + std::to_string(rowId)).c_str())) {
                                                    for (int editCol = 1; editCol <= colCount; ++editCol) {
                                                        std::string columnName = metaData->getColumnLabel(editCol);
                                                        std::string& cellValue = editValues[rowId][editCol];
                                                        ImGui::InputText(("##Edit" + columnName).c_str(), &cellValue[0], cellValue.size() + 128);
                                                        // cellValue[cellValue.length()] = '\0';
                                                        ImGui::SameLine();
                                                        ImGui::Text("%s", columnName.c_str());
                                                    }

                                                    if (ImGui::Button("保存修改")) {
                                                        for (int editCol = 1; editCol <= colCount; ++editCol) {
                                                            std::string columnName = metaData->getColumnLabel(editCol);
                                                            char buf[1024];
                                                            std::string value = editValues[rowId][editCol];
                                                            strncpy(buf, value.c_str(), sizeof(buf) - 1);
                                                            buf[sizeof(buf) - 1] = '\0';
                                                            // LOGI("tableName: %s, columnName: %s, rowId: %d, newValue: %s, size: %u", result.tableName.c_str(), columnName.c_str(), rowId, value.c_str(), value.length());
                                                            updateColumnValue(result.tableName, columnName, "id", rowId, buf);
                                                        }
                                                        result.bUpdated = true;
                                                        editValues.erase(rowId);
                                                        ImGui::CloseCurrentPopup();
                                                    }

                                                    ImGui::SameLine();
                                                    if (ImGui::Button("取消")) {
                                                        editValues.erase(rowId);
                                                        ImGui::CloseCurrentPopup();
                                                    }

                                                    ImGui::EndPopup();
                                                }

                                                if (ImGui::Button("删除此行")) {
                                                    deleteColumn(result.tableName, "id", rowId);
                                                    result.bUpdated = true;
                                                    selectedCells.clear();
                                                    ImGui::CloseCurrentPopup();
                                                }
                                            } else if (selectedCells.size() > 1) {
                                                if (ImGui::Button("批量删除")) {
                                                    for (const auto& cell : selectedCells) {
                                                        deleteColumn(result.tableName, "id", cell.first);
                                                    }
                                                    result.bUpdated = true;
                                                    selectedCells.clear();
                                                    ImGui::CloseCurrentPopup();
                                                }
                                            }
                                            if (ImGui::Button("取消选择")) {
                                                    selectedCells.clear();
                                                ImGui::CloseCurrentPopup();
                                            }
                                            ImGui::EndPopup();
                                        }
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








void RenderDataAnalysisPanel(DataAnalysisManager* analysisManager) {
    ImGui::SetNextWindowSize(ImVec2(screenW * 0.2f, screenH * 0.8f));
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.8f, screenH * 0.0f));

    ImGui::Begin("数据分析");

    // 员工出勤情况分析
    if (ImGui::CollapsingHeader("员工出勤情况分析")) {
        auto attendanceData = analysisManager->getAttendanceAnalysis();
        std::vector<const char*> names;
        std::vector<int> totalCounts;
        std::vector<int> lateCounts;

        for (auto& data : attendanceData) {
            names.push_back(std::get<0>(data).c_str());
            totalCounts.push_back(std::get<1>(data));
            lateCounts.push_back(std::get<2>(data));
        }

        // 绘制条形图显示员工出勤情况
        ImGui::Text("员工出勤情况");
        if (ImPlot::BeginPlot("出勤情况", "员工", "出勤天数")) {
            ImPlot::PlotBars("总出勤", totalCounts.data(), totalCounts.size(), 0.5f);
            ImPlot::PlotBars("迟到", lateCounts.data(), lateCounts.size(), 0.5f);
            ImPlot::EndPlot();
        }
    }

    // 报销情况分析
    if (ImGui::CollapsingHeader("报销情况分析")) {
        auto reimbursementData = analysisManager->getReimbursementAnalysis();
        std::vector<const char*> names;
        std::vector<double> amounts;

        for (auto& data : reimbursementData) {
            names.push_back(std::get<0>(data).c_str());
            amounts.push_back(std::get<1>(data));
        }

        // 绘制条形图显示员工报销情况
        ImGui::Text("报销金额");
        if (ImPlot::BeginPlot("报销情况", "员工", "报销金额")) {
            ImPlot::PlotBars("报销金额", amounts.data(), amounts.size(), 0.5f);
            ImPlot::EndPlot();
        }
    }

    // 绩效评估分析
    if (ImGui::CollapsingHeader("员工绩效分析")) {
        auto performanceData = analysisManager->getPerformanceAnalysis();
        std::vector<const char*> names;
        std::vector<int> scores;

        for (auto& data : performanceData) {
            names.push_back(std::get<0>(data).c_str());
            scores.push_back(std::get<1>(data));
        }

        // 绘制绩效分数条形图
        ImGui::Text("员工绩效分数");
        if (ImPlot::BeginPlot("绩效分析", "员工", "绩效分数")) {
            ImPlot::PlotBars("绩效得分", scores.data(), scores.size(), 0.5f);
            ImPlot::EndPlot();
        }
    }

    // 招聘申请状态分析
    if (ImGui::CollapsingHeader("招聘申请状态分析")) {
        auto applicationStatusData = analysisManager->getJobApplicationsStatus();
        std::vector<const char*> statuses;
        std::vector<double> counts;

        // 准备数据
        for (auto& data : applicationStatusData) {
            statuses.push_back(std::get<0>(data).c_str()); // 状态名称
            counts.push_back(static_cast<double>(std::get<1>(data))); // 数量
        }

        // 计算数据总和（手动累加）
        double total = 0.0;
        for (size_t i = 0; i < counts.size(); ++i) {
            total += counts[i];
        }

        // 将数据值归一化为百分比
        std::vector<double> normalized_counts;
        for (size_t i = 0; i < counts.size(); ++i) {
            normalized_counts.push_back((counts[i] / total) * 100.0);
        }

        // 绘制标题
        ImGui::Text("招聘申请状态分布");

        // 设置饼图中心位置和半径
        double x_center = 0.5;
        double y_center = 0.5;
        double radius = 0.4;

        // 使用 ImPlot 绘制饼图
        if (ImPlot::BeginPlot("招聘申请状态分布", ImVec2(300, 300))) {
            ImPlot::PlotPieChart(
                statuses.data(),         // 状态标签数组
                normalized_counts.data(),           // 数据值数组
                normalized_counts.size(),           // 数据数量
                x_center,                // 饼图中心 X 坐标
                y_center,                // 饼图中心 Y 坐标
                radius,                  // 饼图半径
                "%.1f%%",                // 数据标签格式
                90                       // 起始角度
            );
            ImPlot::EndPlot();
        }
    }

    ImGui::End();
}




#endif