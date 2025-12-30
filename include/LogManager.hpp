#pragma once

#include "BlockLinkedList.h"

#include <string>
#include <fstream>
#include <vector>

class LogManager
{
  private:
    std::fstream transactionLogFile;      // 交易日志文件路径
    std::fstream financeReportFile;       // 财务报表文件路径
    std::fstream employeeReportFile;      // 员工报告文件路径
    std::fstream operationLogFile;        // 操作日志文件路径

    int total_transactionlog;
    int total_financelog;
    int total_employeelog;
    int total_operationlog;

    std::vector<std::string> employee_userid;
    BlockLinkedList<30> employeeReportList{"employeereport_blocklinkedlist", "employeereport_blocklinkedlist_basicinformation"};

    static const int transactionlogsize = 16;
    static const int financelogsize = 128;
    static const int employeelogsize = 335;
    static const int operationlogsize = 360;
  public:
    LogManager();
    ~LogManager();

    int setLogManager(std::fstream &totalFile, int point);
    int recordLogManager(std::fstream &totalFile, int point);

    void addEmployee(const std::string &userid);
    
    void addTransactionLog(double finance);
    void showTransaction(int count);

    void addFinanceLog(const std::string &userid, const std::string &isbn, int quantity, double finance);
    void showFinanceLog();

    void addEmployeeLog(int privilege, const std::string &userid, const std::string &old_isbn, const std::string &isbn, const std::string &bookname, const std::string &author, const std::string &keyword, double price, bool has_price);
    void addEmployeeLog(int privilege, const std::string &userid, const std::string &isbn, int quantity, double totalcost);
    void showEmployeeLog();

    void addOperationLog(const std::string &log);
    void showOperationLog();
};