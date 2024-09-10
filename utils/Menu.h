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
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
        static int freeze_cols = 1;
        static int freeze_rows = 1;
        // ImGui::CheckboxFlags("手动调整宽度", &flags, ImGuiTableFlags_Resizable);

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

void ShowMenu() {
    if (!mgr.init)
        mgr.initialize();

    static string debuginfo;

    ImGui::Begin("HRM");

    if (ImGui::Button("Console Clear")) { system("cls"); }

    if (ImGui::CollapsingHeader("员工管理"))
    {
        ImGui::Indent(30.0f);
        if (ImGui::CollapsingHeader("添加员工")) {
            static char name[128];
            static char id_card[128];
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("名称", name, sizeof(name));
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::InputText("身份", id_card, sizeof(id_card));
            if (ImGui::Button("添加")) {
                mgr.em->addEmployee(name, id_card);
                debuginfo += "Add Success \n";
            }
        }
        if (ImGui::CollapsingHeader("查询员工")) {
            static char cmd[512] = "select * from Employees";
            ImGui::InputTextMultiline("", cmd, sizeof(cmd), ImVec2(300.0f, 100.0f));
            if (ImGui::Button("查询")) {
                // mgr.em->displayEmployees();
                resList.push_back(std::move(mgr.em->executeQuery(cmd)));
                // resList.push_back(std::move(mgr.em->getEmployees()));
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