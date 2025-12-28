#pragma once

#include <string>
#include <fstream>

class LogManager
{
  private:
    std::fstream transactionLogFile;      // 交易日志文件路径
    std::fstream operationLogFile;        // 操作日志文件路径
    std::fstream financeReportFile;       // 财务报表文件路径
    std::fstream employeeReportFile;      // 员工报告文件路径

    int total_transactionlog;
    int total_operationlog;
    int total_financelog;
    int total_employeelog;

    static const int transactionlogsize = 16;
  public:
    LogManager();
    ~LogManager();

    void addTransactionLog(double finance);
    void showTransaction(int count);
};