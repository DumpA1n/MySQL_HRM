#pragma once
#ifndef MENU_H
#define MENU_H
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

void ShowMenu() {
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

        if (curLoginUser.role == "admin" && ImGui::CollapsingHeader("添加员工")) {
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

        if (ImGui::CollapsingHeader("查询员工")) {
            string query = "select * from Employees";
            if (curLoginUser.role == "user")
                query = "SELECT * FROM Employees WHERE id = " + to_string(curLoginUser.employeeId);
            bool hasCondition = false;

            static bool useCustomQuery = false;
            static char customQuery[1024] = "select * from Employees";
            ImGui::Checkbox("使用自定义SQL", &useCustomQuery);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("TABLE Employees \nid INT AUTO_INCREMENT PRIMARY KEY \nname VARCHAR(255) \nid_card VARCHAR(20) \ncontact VARCHAR(50) \naddress VARCHAR(255) \nposition VARCHAR(100) \ndepartment VARCHAR(100) \nhire_date DATE \neducation_background TEXT \nwork_experience TEXT \nemergency_contact VARCHAR(255)");

            if (useCustomQuery) {
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

        if (curLoginUser.role == "admin" && ImGui::CollapsingHeader("创键员工账号")) {
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

        if (curLoginUser.role == "admin" && ImGui::CollapsingHeader("移除员工")) {
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

        if (curLoginUser.role == "user") {
            if (ImGui::Button("查看入职任务")) {
                static string query = "SELECT * FROM OnboardingTasks WHERE id = " + to_string(curLoginUser.employeeId);
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
            ImGui::SameLine();
            if (ImGui::Button("随机生成")) {
                mgr.recumgr->generateRandomJobPosts(10);
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
                mgr.recumgr->generateRandomApplications(job_id);
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
            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;

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
            static int employee_id;
            static char attendance_date[32];
            static char check_in_time[16];

            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;
            strncpy(attendance_date, getCurrentDate().c_str(), sizeof(attendance_date) - 1);
            strncpy(check_in_time, getCurrentTime().c_str(), sizeof(check_in_time) - 1);

            ImGui::InputInt("员工ID##AttendanceManager1", &employee_id);
            ImGui::InputText("考勤日期", attendance_date, sizeof(attendance_date));
            ImGui::InputText("签到时间", check_in_time, sizeof(check_in_time));

            if (ImGui::Button("签到")) {
                mgr.attemgr->insertCheckIn(employee_id, attendance_date, check_in_time);
                mgr.attemgr->updateAttendanceStatus(employee_id, attendance_date);
            }

            if (ImGui::Button("查询考勤")) {
                static std::string query = "SELECT * FROM AttendanceStatus";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM AttendanceStatus WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "AttendanceStatus", std::move(mgr.attemgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }

















// #################################################### 薪资管理 ####################################################
    if (ImGui::CollapsingHeader("薪资管理"))
    {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("生成工资单")) {
            static int employee_id;
            static char month[16];
            static double amount;

            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;
            ImGui::InputInt("员工ID##PayrollManager1", &employee_id);
            ImGui::InputText("月份 (YYYY-MM)", month, sizeof(month));
            ImGui::InputDouble("工资", &amount);

            if (ImGui::Button("生成")) {
                mgr.payrmgr->generatePayroll(employee_id, month, amount);
            }

            if (ImGui::Button("查询##3")) {
                static string query = "SELECT * FROM Payroll";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM Payroll WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "Payroll", std::move(mgr.payrmgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }




















// #################################################### 绩效评估 ####################################################
    if (ImGui::CollapsingHeader("绩效评估##1"))
    {
        ImGui::Indent(30.0f);

        // if (ImGui::CollapsingHeader("绩效评估##2")) {
            static int employee_id;
            static char review_period[32];
            static int score;

            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;
            ImGui::InputInt("员工ID##PerformanceManager1", &employee_id);
            ImGui::InputText("评估周期", review_period, sizeof(review_period));
            ImGui::InputInt("评分 (0-100)", &score);

            if (ImGui::Button("提交评估")) {
                mgr.perfmgr->submitPerformanceReview(employee_id, review_period, score);
            }

            if (ImGui::Button("查询评估")) {
                static string query = "SELECT * FROM Performance";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM Performance WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "Performance", std::move(mgr.perfmgr->executeQuery(query.c_str()))));
            }
        // }

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
            ImGui::SameLine();
            if (ImGui::Button("随机生成")) {
                mgr.traimgr->generateRandomTrainingCourses(10);
            }

            if (ImGui::Button("查询##4")) {
                static string query = "SELECT * FROM TrainingCourses";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM TrainingCourses WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "TrainingCourses", std::move(mgr.traimgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("报名培训课程")) {
            static int employee_id;
            static int course_id;

            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;
            ImGui::InputInt("员工ID##TrainingManager1", &employee_id);
            ImGui::InputInt("课程ID", &course_id);

            if (ImGui::Button("报名")) {
                mgr.traimgr->enrollInCourse(employee_id, course_id);
            }

            if (ImGui::Button("查询##5")) {
                static string query = "SELECT * FROM Enrollments";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM Enrollments WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "Enrollments", std::move(mgr.traimgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }
















// #################################################### 员工服务 ####################################################
// 员工自助服务模块
    if (ImGui::CollapsingHeader("员工服务")) {
        ImGui::Indent(30.0f);

        if (ImGui::CollapsingHeader("请假管理")) {
            static int employee_id;
            static char start_date[32];
            static char end_date[32];

            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;
            ImGui::InputInt("员工ID##EmployeeServiceManager1", &employee_id);
            ImGui::InputText("开始日期", start_date, sizeof(start_date));
            ImGui::InputText("结束日期", end_date, sizeof(end_date));

            if (ImGui::Button("提交请假申请")) {
                mgr.emplsvcmgr->submitLeaveRequest(employee_id, start_date, end_date);
            }

            if (ImGui::Button("查询请假申请")) {
                static std::string query = "SELECT * FROM LeaveRequests";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM LeaveRequests WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "LeaveRequests", std::move(mgr.emplsvcmgr->executeQuery(query.c_str()))));
            }
        }

        if (ImGui::CollapsingHeader("报销管理")) {
            static int employee_id;
            static char amount[32];

            if (curLoginUser.role == "user")
                *reinterpret_cast<int*>(&employee_id) = curLoginUser.employeeId;
            ImGui::InputInt("员工ID##EmployeeServiceManager2", &employee_id);
            ImGui::InputText("报销金额", amount, sizeof(amount));

            if (ImGui::Button("提交报销申请")) {
                mgr.emplsvcmgr->submitReimbursement(employee_id, stod(amount));
            }

            if (ImGui::Button("查询报销申请")) {
                static std::string query = "SELECT * FROM Reimbursements";
                if (curLoginUser.role == "user")
                    query = "SELECT * FROM Reimbursements WHERE employee_id = " + to_string(curLoginUser.employeeId);
                queryResults.push_back(QueryResult(query, "Reimbursements", std::move(mgr.emplsvcmgr->executeQuery(query.c_str()))));
            }
        }

        ImGui::Unindent(30.0f);
    }


    RenderDataAnalysisPanel(mgr.analmgr);

    GenerateTable(queryResults);
    ShowDebugWindow();
    ImGui::End();
}

#endif // MENU_H