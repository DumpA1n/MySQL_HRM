#pragma once
#ifndef MENU_H
#define MENU_H

void ShowMenu(User user) {
    if (!mgr.init)
        mgr.initialize();

    ImGui::SetNextWindowSize(ImVec2(screenW * 0.2f, screenH * 0.6f));
    ImGui::SetNextWindowPos(ImVec2(screenW * 0.0f, screenH * 0.2f));

    ImGui::Begin("HRM");

    if (ImGui::Button("Console Clear")) { system("cls"); }

// #################################################### 员工管理 ####################################################
    if (ImGui::CollapsingHeader("员工管理"))
    {
        ImGui::Indent(30.0f);

        if (user.role == "admin" && ImGui::CollapsingHeader("添加员工")) {
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

            ImGui::InputText("名称##name2", name, sizeof(name));
            ImGui::InputText("身份证##id_card2", id_card, sizeof(id_card));
            ImGui::InputText("联系方式", contact, sizeof(contact));
            ImGui::InputText("地址", address, sizeof(address));
            ImGui::InputText("职位", position, sizeof(position));
            ImGui::InputText("部门", department, sizeof(department));
            ImGui::InputText("入职日期", hire_date, sizeof(hire_date));
            ImGui::InputText("教育背景", education_background, sizeof(education_background));
            ImGui::InputText("工作经验", work_experience, sizeof(work_experience));
            ImGui::InputText("紧急联系人", emergency_contact, sizeof(emergency_contact));
            if (ImGui::Button("添加##addEmployee")) {
                mgr.emplmgr->addEmployee(name, id_card, contact, address, position, department, hire_date, education_background, work_experience, emergency_contact);
            }
            if (ImGui::Button("随机添加")) {
                mgr.emplmgr->GenerateRandomEmployeeData();
            }
        }

        if (user.role == "admin" && ImGui::CollapsingHeader("创键员工账号")) {
            static int id;
            static char username[128];
            static char password[128];

            ImGui::InputInt("ID##IDaddtouser", &id);
            ImGui::InputText("用户名##UNaddtouser", username, sizeof(username));
            ImGui::InputText("密码##PWaddtouser", password, sizeof(password));
            if (ImGui::Button("添加##addEmployeetoUser")) {
                mgr.emplmgr->addEmployeeToUsers(id, username, password);
            }
        }

        if (ImGui::CollapsingHeader("查询员工")) {
            string query = "select * from Employees";
            bool hasCondition = false;

            static bool useCustomQuery = false;
            static char customQuery[1024] = "select * from Employees";
            ImGui::Checkbox("使用自定义SQL", &useCustomQuery);
            if (useCustomQuery) {
                ImGui::SetTooltip("TABLE Employees \nid INT AUTO_INCREMENT PRIMARY KEY \nname VARCHAR(255) \nid_card VARCHAR(20) \ncontact VARCHAR(50) \naddress VARCHAR(255) \nposition VARCHAR(100) \ndepartment VARCHAR(100) \nhire_date DATE \neducation_background TEXT \nwork_experience TEXT \nemergency_contact VARCHAR(255)");
                ImGui::InputTextMultiline("##customQuery1", customQuery, sizeof(customQuery), ImVec2(300.0f, 100.0f));
            }
            if (ImGui::Button("查询##Employees1")) {
                if (useCustomQuery) {
                    queryResults.push_back(QueryResult(customQuery, "Employees", std::move(mgr.emplmgr->executeQuery(customQuery))));
                } else {
                    queryResults.push_back(QueryResult(query, "Employees", std::move(mgr.emplmgr->executeQuery(query.c_str()))));
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("清除所有查询结果")) {
                queryResults.clear();
            }
        }

        if (user.role == "admin" && ImGui::CollapsingHeader("移除员工")) {
            static int idRange[2];
            ImGui::InputInt2("id", idRange);
            if (ImGui::Button("闭区间移除")) {
                for (int i = idRange[0]; i <= idRange[1]; i++) {
                    mgr.emplmgr->deleteColumn(i);
                }
            }
        }
        ImGui::Unindent(30.0f);
    }
















// #################################################### 招聘与入职管理 ####################################################
    if (ImGui::CollapsingHeader("招聘入职管理")) {
        ImGui::Indent(30.0f);

        if (user.role == "user") {
            if (ImGui::Button("查看入职任务")) {
                static string query = "SELECT * FROM OnboardingTasks WHERE id = " + to_string(user.employeeId);
                queryResults.push_back(QueryResult(query, "OnboardingTasks", std::move(mgr.recumgr->executeQuery(query.c_str()))));
            }
        } else {
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
                mgr.recumgr->addJobPost(title, description, requirements, salary_range);
            }
            if (ImGui::Button("查看职位")) {
                static string query = "SELECT * FROM JobPosts";
                queryResults.push_back(QueryResult(query, "JobPosts", std::move(mgr.recumgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("简历查询")) {
            static int job_id = 1;  // 默认为1号职位
            ImGui::InputInt("职位编号", &job_id);
            if (ImGui::Button("随机生成简历")) {
                mgr.recumgr->addRandomApplication(job_id);
            }

            if (ImGui::Button("查询##2")) {
                static string query = "select * from Applications";
                queryResults.push_back(QueryResult(query, "Applications", std::move(mgr.recumgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("面试安排")) {
            static int application_id;
            static char interview_date[64];
            ImGui::InputInt("简历编号", &application_id);
            ImGui::InputText("面试日期", interview_date, sizeof(interview_date));

            if (ImGui::Button("安排面试")) {
                mgr.recumgr->scheduleInterview(application_id, interview_date);
            }

            if (ImGui::Button("查看面试")) {
                static string query = "SELECT * FROM Interviews";
                queryResults.push_back(QueryResult(query, "Interviews", std::move(mgr.recumgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("入职任务管理")) {
            static int employee_id;

            ImGui::InputInt("员工编号", &employee_id);

            if (ImGui::Button("生成入职任务")) {
                mgr.recumgr->createOnboardingTasks(employee_id);
            }

            if (ImGui::Button("查看入职任务")) {
                static string query = "SELECT * FROM OnboardingTasks";
                queryResults.push_back(QueryResult(query, "OnboardingTasks", std::move(mgr.recumgr->executeQuery(query.c_str()))));
            }
        }
        }
        ImGui::Unindent(30.0f);
    }

















// #################################################### 考勤管理 ####################################################
    if (ImGui::CollapsingHeader("考勤管理"))
    {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("考勤记录")) {
            static char employee_id[32];
            static char attendance_date[32];
            static char status[16];

            ImGui::InputText("员工ID##AttendanceManager1", employee_id, sizeof(employee_id));
            ImGui::InputText("考勤日期", attendance_date, sizeof(attendance_date));
            ImGui::InputText("状态 (Present/Absent)", status, sizeof(status));

            if (ImGui::Button("记录考勤")) {
                mgr.attemgr->recordAttendance(stoi(employee_id), attendance_date, status);
            }

            if (ImGui::Button("查询考勤")) {
                static string query = "SELECT * FROM Attendance";
                queryResults.push_back(QueryResult(query, "Attendance", std::move(mgr.attemgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }

















// #################################################### 薪资管理 ####################################################
    if (ImGui::CollapsingHeader("薪资管理"))
    {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("生成工资单")) {
            static char employee_id[32];
            static char month[16];
            static char amount[16];
            ImGui::InputText("员工ID##PayrollManager1", employee_id, sizeof(employee_id));
            ImGui::InputText("月份 (YYYY-MM)", month, sizeof(month));
            ImGui::InputText("工资", amount, sizeof(amount));

            if (ImGui::Button("生成")) {
                mgr.payrmgr->generatePayroll(stoi(employee_id), month, stod(amount));
            }

            if (ImGui::Button("查询##3")) {
                static string query = "SELECT * FROM Payroll";
                queryResults.push_back(QueryResult(query, "Payroll", std::move(mgr.payrmgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }




















// #################################################### 绩效评估 ####################################################
    if (ImGui::CollapsingHeader("绩效评估"))
    {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("绩效评估")) {
            static char employee_id[32];
            static char review_period[32];
            static char score[16];

            ImGui::InputText("员工ID##PerformanceManager1", employee_id, sizeof(employee_id));
            ImGui::InputText("评估周期", review_period, sizeof(review_period));
            ImGui::InputText("评分 (0-100)", score, sizeof(score));

            if (ImGui::Button("提交评估")) {
                mgr.perfmgr->submitPerformanceReview(stoi(employee_id), review_period, stoi(score));
            }

            if (ImGui::Button("查询评估")) {
                static string query = "SELECT * FROM Performance";
                queryResults.push_back(QueryResult(query, "Performance", std::move(mgr.perfmgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }


















// #################################################### 员工培训 ####################################################
    if (ImGui::CollapsingHeader("员工培训"))
    {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("培训课程管理")) {
            static char course_name[128];
            static char description[256];
            ImGui::InputText("课程名称", course_name, sizeof(course_name));
            ImGui::InputTextMultiline("描述", description, sizeof(description), ImVec2(0.0f, 100.0f));

            if (ImGui::Button("发布")) {
                mgr.traimgr->addTrainingCourse(course_name, description);
            }

            if (ImGui::Button("查询##4")) {
                static string query = "SELECT * FROM TrainingCourses";
                queryResults.push_back(QueryResult(query, "TrainingCourses", std::move(mgr.traimgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("报名培训课程")) {
            static char employee_id[32];
            static char course_id[32];

            ImGui::InputText("员工ID##TrainingManager1", employee_id, sizeof(employee_id));
            ImGui::InputText("课程ID", course_id, sizeof(course_id));

            if (ImGui::Button("报名")) {
                mgr.traimgr->enrollInCourse(stoi(employee_id), stoi(course_id));
            }

            if (ImGui::Button("查询##5")) {
                static string query = "SELECT * FROM Enrollments";
                queryResults.push_back(QueryResult(query, "Enrollments", std::move(mgr.traimgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }
















// #################################################### 员工服务 ####################################################
    if (ImGui::CollapsingHeader("员工服务"))
    {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("休假管理")) {
            static char employee_id[32];
            static char start_date[32];
            static char end_date[32];

            ImGui::InputText("员工ID##EmployeeServiceManager1", employee_id, sizeof(employee_id));
            ImGui::InputText("开始日期", start_date, sizeof(start_date));
            ImGui::InputText("结束日期", end_date, sizeof(end_date));

            if (ImGui::Button("提交申请")) {
                mgr.emplsvcmgr->submitLeaveRequest(stoi(employee_id), start_date, end_date);
            }

            if (ImGui::Button("查询申请")) {
                static string query = "SELECT * FROM LeaveRequests";
                queryResults.push_back(QueryResult(query, "LeaveRequests", std::move(mgr.emplsvcmgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("报销管理")) {
            static char employee_id[32];
            static char amount[32];
            ImGui::InputText("员工ID##EmployeeServiceManager2", employee_id, sizeof(employee_id));
            ImGui::InputText("报销金额", amount, sizeof(amount));

            if (ImGui::Button("提交报销")) {
                mgr.emplsvcmgr->submitReimbursement(stoi(employee_id), stod(amount));
            }

            if (ImGui::Button("查询报销")) {
                static string query = "SELECT * FROM Reimbursements";
                queryResults.push_back(QueryResult(query, "Reimbursements", std::move(mgr.emplsvcmgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }



    GenerateTable(queryResults);
    ShowDebugWindow();
    ImGui::End();
}

#endif // MENU_H