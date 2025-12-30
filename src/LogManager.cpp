#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <chrono>
#include <ctime>

#include "utils/Error.hpp"
#include "LogManager.hpp"

LogManager::LogManager()
{
    transactionLogFile.open("transactionLogFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!transactionLogFile)
    {
        transactionLogFile.open("transactionLogFile", std::ios::out | std::ios::binary);
        transactionLogFile.close();
        transactionLogFile.open("transactionLogFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    operationLogFile.open("operationLogFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!operationLogFile)
    {
        operationLogFile.open("operationLogFile", std::ios::out | std::ios::binary);
        operationLogFile.close();
        operationLogFile.open("operationLogFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    financeReportFile.open("financeReportFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!financeReportFile)
    {
        financeReportFile.open("financeReportFile", std::ios::out | std::ios::binary);
        financeReportFile.close();
        financeReportFile.open("financeReportFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    employeeReportFile.open("employeeReportFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!employeeReportFile)
    {
        employeeReportFile.open("employeeReportFile", std::ios::out | std::ios::binary);
        employeeReportFile.close();
        employeeReportFile.open("employeeReportFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    total_transactionlog = 0;
    total_operationlog = 0;
    total_financelog = 0;
    total_employeelog = 0;
    employee_userid.clear();
}

LogManager::~LogManager()
{
    transactionLogFile.close();
    operationLogFile.close();
    employeeReportFile.close();
}

int LogManager::setLogManager(std::fstream &totalFile, int point)
{
    totalFile.clear();
    totalFile.seekg(point);
    totalFile.read(reinterpret_cast<char*>(&total_transactionlog), 4);
    totalFile.read(reinterpret_cast<char*>(&total_operationlog), 4);
    totalFile.read(reinterpret_cast<char*>(&total_financelog), 4);
    totalFile.read(reinterpret_cast<char*>(&total_employeelog), 4);
    int employee_count;
    totalFile.read(reinterpret_cast<char*>(&employee_count), 4);
    char temp[31];
    for(int i = 0; i < employee_count; i++)
    {
        totalFile.read(temp, 31);
        employee_userid.push_back(std::string(temp));
    }
    return 20 + 31 * employee_count;
}

int LogManager::recordLogManager(std::fstream &totalFile, int point)
{
    totalFile.clear();
    totalFile.seekp(point);
    totalFile.write(reinterpret_cast<char*>(&total_transactionlog), 4);
    totalFile.write(reinterpret_cast<char*>(&total_operationlog), 4);
    totalFile.write(reinterpret_cast<char*>(&total_financelog), 4);
    totalFile.write(reinterpret_cast<char*>(&total_employeelog), 4);
    int employee_count = employee_userid.size();
    totalFile.write(reinterpret_cast<char*>(&employee_count), 4);
    char temp[31];
    for(int i = 0; i < employee_count; i++)
    {
        memset(temp, 0, 31);
        strncpy(temp, employee_userid[i].c_str(), 30);
        temp[30] = '\0';
        totalFile.write(temp, 31);
    }
    return 20 + 31 * employee_count;
}

void LogManager::addEmployee(const std::string &userid)
{
    employee_userid.push_back(userid);
}

void LogManager::addTransactionLog(double finance)
{
    double income{0}, expenditure{0};
    if(total_transactionlog != 0)
    {
        transactionLogFile.clear();
        transactionLogFile.seekg((total_transactionlog - 1) * transactionlogsize);
        transactionLogFile.read(reinterpret_cast<char*>(&income), 8);
        transactionLogFile.read(reinterpret_cast<char*>(&expenditure), 8);
    }
    if(finance >= 0)
        income += finance;
    else
        expenditure += (-finance);
    transactionLogFile.clear();
    transactionLogFile.seekp(total_transactionlog * transactionlogsize);
    transactionLogFile.write(reinterpret_cast<char*>(&income), 8);
    transactionLogFile.write(reinterpret_cast<char*>(&expenditure), 8);
    total_transactionlog++;
}

void LogManager::showTransaction(int count)
{
    if(count == 0)
    {
        std::cout << '\n';
        return;
    }
    if(count > total_transactionlog)
        throw BookstoreError("Invalid");
    double income = 0, expenditure = 0;
    if(count == -1)
    {
        if(total_transactionlog != 0)
        {
            transactionLogFile.clear();
            transactionLogFile.seekg((total_transactionlog - 1) * transactionlogsize);
            transactionLogFile.read(reinterpret_cast<char*>(&income), 8);
            transactionLogFile.read(reinterpret_cast<char*>(&expenditure), 8);
        }
        std::cout << std::fixed << std::setprecision(2) << "+ " << income << " - " << expenditure << '\n';
    }
    else
    {
        transactionLogFile.clear();
        transactionLogFile.seekg((total_transactionlog - 1) * transactionlogsize);
        transactionLogFile.read(reinterpret_cast<char*>(&income), 8);
        transactionLogFile.read(reinterpret_cast<char*>(&expenditure), 8);

        double temp_income, temp_expenditure;
        if(total_transactionlog == count)
            temp_income = 0, temp_expenditure = 0;
        else
        {
            transactionLogFile.clear();
            transactionLogFile.seekg((total_transactionlog - count - 1) * transactionlogsize);
            transactionLogFile.read(reinterpret_cast<char*>(&temp_income), 8);
            transactionLogFile.read(reinterpret_cast<char*>(&temp_expenditure), 8);
        }
        std::cout << std::fixed << std::setprecision(2) << "+ " << income - temp_income << " - " << expenditure - temp_expenditure << '\n';
    }
}

void LogManager::addFinanceLog(const std::string &userid, const std::string &isbn, int quantity, double finance)
{
    financeReportFile.clear();
    financeReportFile.seekp(total_financelog * financelogsize);
    char temp[31];
    memset(temp, 0, 31);
    strncpy(temp, userid.c_str(), 30);
    temp[30] = '\0';
    financeReportFile.write(temp, 31);
    char tmp[61];
    memset(tmp, 0, 61);
    strncpy(tmp, isbn.c_str(), 60);
    tmp[60] = '\0';
    financeReportFile.write(tmp, 61);
    financeReportFile.write(reinterpret_cast<char*>(&quantity), 4);
    financeReportFile.write(reinterpret_cast<char*>(&finance), 8);
    total_financelog++;

    //with the help AI
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;
    #if defined(__linux__) || defined(__APPLE__)
        localtime_r(&t, &local_time);  // Linux/macOS线程安全函数
    #endif
    int year   = local_time.tm_year + 1900;  // 实际年份
    int month  = local_time.tm_mon + 1;      // 实际月份 (1-12)
    int day    = local_time.tm_mday;         // 日 (1-31)
    int hour   = local_time.tm_hour;         // 时 (0-23)
    int minute = local_time.tm_min;          // 分 (0-59)
    int second = local_time.tm_sec;          // 秒 (0-59)
    financeReportFile.write(reinterpret_cast<char*>(&year), 4);
    financeReportFile.write(reinterpret_cast<char*>(&month), 4);
    financeReportFile.write(reinterpret_cast<char*>(&day), 4);
    financeReportFile.write(reinterpret_cast<char*>(&hour), 4);
    financeReportFile.write(reinterpret_cast<char*>(&minute), 4);
    financeReportFile.write(reinterpret_cast<char*>(&second), 4);
    // std::cout << "当前时间：" << std::endl;
    // std::cout << "年份: " << year   << std::endl;
    // std::cout << "月份: " << month  << std::endl;
    // std::cout << "日期: " << day    << std::endl;
    // std::cout << "小时: " << hour   << std::endl;
    // std::cout << "分钟: " << minute << std::endl;
    // std::cout << "秒钟: " << second << std::endl;
}

void printCentered(char* str, int width)
{
    int empty = width - strlen(str);
    for(int i = 0; i < empty / 2; i++)
        std::cout << ' ';
    std::cout << str;
    for(int i = 0; i < empty - empty / 2; i++)
        std::cout << ' ';
}

void printCentered(const std::string &str, int width)
{
    int empty = width - str.length();
    for(int i = 0; i < empty / 2; i++)
        std::cout << ' ';
    std::cout << str;
    for(int i = 0; i < empty - empty / 2; i++)
        std::cout << ' ';
}

void LogManager::showFinanceLog()
{
    if(total_financelog == 0)
    {
        std::cout << "There is no transaction!" << std::endl;
        return ;
    }
    std::cout << '\n';
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    std::cout << "|        time         |             userID             | operation |                             ISBN                             |  quantity  |   totalcost   |\n";
    std::cout << "|---------------------|--------------------------------|-----------|--------------------------------------------------------------|------------|---------------|\n";
    financeReportFile.clear();
    financeReportFile.seekg(0);
    for(int i = 0; i < total_financelog; i++)
    {
        int quantity, year, month, day, hour, minute, second;
        double finance;
        char userid[31];
        financeReportFile.read(userid, 31);
        char isbn[61];
        financeReportFile.read(isbn, 61);
        financeReportFile.read(reinterpret_cast<char*>(&quantity), 4);
        financeReportFile.read(reinterpret_cast<char*>(&finance),8);
        financeReportFile.read(reinterpret_cast<char*>(&year), 4);
        financeReportFile.read(reinterpret_cast<char*>(&month), 4);
        financeReportFile.read(reinterpret_cast<char*>(&day), 4);
        financeReportFile.read(reinterpret_cast<char*>(&hour), 4);
        financeReportFile.read(reinterpret_cast<char*>(&minute), 4);
        financeReportFile.read(reinterpret_cast<char*>(&second), 4);
        std::cout << "| " << year << '-' << std::setw(2) << std::setfill('0') << month << '-' << std::setw(2) << day << ' ' << std::setw(2) << hour << ':' << std::setw(2) << minute << ':' << std::setw(2) << second << " | ";
        printCentered(userid, 30);
        std::cout << " | ";

        if(quantity < 0)
            std::cout << "   buy    | ";
        else
            std::cout << "  import  | ";

        printCentered(isbn, 60);
        std::cout << " | ";
        
        if(quantity < 0)
            quantity = -quantity;

        std::string str;
        str = std::to_string(quantity);
        printCentered(str, 10);
        std::cout << " | ";
        str = std::to_string(finance);
        str.erase(str.end() - 4, str.end());
        printCentered(str, 13);
        std::cout << " |\n";
    }
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";
}

void LogManager::addEmployeeLog(int privilege, const std::string &userid, const std::string &old_isbn, const std::string &isbn, const std::string &bookname, const std::string &author, const std::string &keyword, double price, bool has_price)
{
    bool getchange = false;
    std::string log = "change " + old_isbn + "'s";
    if(!isbn.empty())
    {
        log = log +  " ISBN to " + isbn;
        getchange = true;
    }
    if(!bookname.empty())
    {
        if(getchange)
            log = log + " , ";
        log = log + " bookname to " + bookname;
        getchange = true;
    }
    if(!author.empty())
    {
        if(getchange)
            log = log + " , ";
        log = log + " author to " + author;
        getchange = true;
    }
    if(!keyword.empty())
    {
        if(getchange)
            log = log + " , ";
        log = log + " keyword to " + keyword;
        getchange = true;
    }
    if(has_price)
    {
        if(getchange)
            log = log + " , ";
        std::string str_price = std::to_string(price);
        str_price.erase(str_price.end() - 4, str_price.end());
        log = log + " price to " + str_price;
        getchange = true;
    }
    log = log + '.';

    std::string operationlog = userid + " " + log;
    addOperationLog(operationlog);

    if(privilege > 3)
        return ;

    employeeReportFile.clear();
    employeeReportList.Insert(userid, total_employeelog);
    employeeReportFile.seekp(total_employeelog * employeelogsize);
    char temp[311];
    memset(temp, 0, 311);
    strncpy(temp, log.c_str(), 310);
    temp[310] = '\0';
    employeeReportFile.write(temp, 311);

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;
    #if defined(__linux__) || defined(__APPLE__)
        localtime_r(&t, &local_time);  // Linux/macOS线程安全函数
    #endif
    int year   = local_time.tm_year + 1900;  // 实际年份
    int month  = local_time.tm_mon + 1;      // 实际月份 (1-12)
    int day    = local_time.tm_mday;         // 日 (1-31)
    int hour   = local_time.tm_hour;         // 时 (0-23)
    int minute = local_time.tm_min;          // 分 (0-59)
    int second = local_time.tm_sec;          // 秒 (0-59)
    employeeReportFile.write(reinterpret_cast<char*>(&year), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&month), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&day), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&hour), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&minute), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&second), 4);
    total_employeelog++;
}

void LogManager::addEmployeeLog(int privilege, const std::string &userid, const std::string &isbn, int quantity, double totalcost)
{
    std::string cost = std::to_string(totalcost);
    cost.erase(cost.end() - 4, cost.end());
    std::string log = "cost " + cost + " to import " + std::to_string(quantity) + " of " + isbn;
    
    std::string operationlog = userid + " " + log;
    addOperationLog(operationlog);

    if(privilege > 3)
        return ;

    employeeReportList.Insert(userid, total_employeelog);
    employeeReportFile.clear();
    employeeReportFile.seekp(total_employeelog * employeelogsize);

    char temp[311];
    memset(temp, 0, 311);
    strncpy(temp, log.c_str(), 310);
    temp[310] = '\0';
    employeeReportFile.write(temp, 311);

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;
    #if defined(__linux__) || defined(__APPLE__)
        localtime_r(&t, &local_time);  // Linux/macOS线程安全函数
    #endif
    int year   = local_time.tm_year + 1900;  // 实际年份
    int month  = local_time.tm_mon + 1;      // 实际月份 (1-12)
    int day    = local_time.tm_mday;         // 日 (1-31)
    int hour   = local_time.tm_hour;         // 时 (0-23)
    int minute = local_time.tm_min;          // 分 (0-59)
    int second = local_time.tm_sec;          // 秒 (0-59)
    employeeReportFile.write(reinterpret_cast<char*>(&year), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&month), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&day), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&hour), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&minute), 4);
    employeeReportFile.write(reinterpret_cast<char*>(&second), 4);
    total_employeelog++;
}

void LogManager::showEmployeeLog()
{
    if(employee_userid.empty())
    {
        std::cout << "There is no employee!!!\n";
        return ;
    }
    std::cout << '\n';
    for(std::string userid : employee_userid)
    {
        std::cout << userid << '\n';
        std::cout << "--------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
        std::vector<int> logs_index = std::move(employeeReportList.Find(userid));
        if(logs_index.empty())
        {
            std::cout << "No Log !!!\n\n";
            break;
        }
        for(int log_index : logs_index)
        {
            employeeReportFile.seekg(log_index * employeelogsize);
            char log[311];
            int year, month, day, hour, minute, second;
            employeeReportFile.read(log, 311);
            employeeReportFile.read(reinterpret_cast<char*>(&year), 4);
            employeeReportFile.read(reinterpret_cast<char*>(&month), 4);
            employeeReportFile.read(reinterpret_cast<char*>(&day), 4);
            employeeReportFile.read(reinterpret_cast<char*>(&hour), 4);
            employeeReportFile.read(reinterpret_cast<char*>(&minute), 4);
            employeeReportFile.read(reinterpret_cast<char*>(&second), 4);
            std::cout << year << '-' << std::setw(2) << std::setfill('0') << month << '-' << std::setw(2) << day << ' ' << std::setw(2) << hour << ':' << std::setw(2) << minute << ':' << std::setw(2) << second << "   " << log << "\n";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

void LogManager::addOperationLog(const std::string &log)
{
    operationLogFile.clear();
    operationLogFile.seekp(total_operationlog * operationlogsize);
    char temp[336];
    memset(temp, 0, 336);
    strncpy(temp, log.c_str(), 335);
    temp[335] = '\0';
    operationLogFile.write(temp, 336);

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;
    #if defined(__linux__) || defined(__APPLE__)
        localtime_r(&t, &local_time);  // Linux/macOS线程安全函数
    #endif
    int year   = local_time.tm_year + 1900;  // 实际年份
    int month  = local_time.tm_mon + 1;      // 实际月份 (1-12)
    int day    = local_time.tm_mday;         // 日 (1-31)
    int hour   = local_time.tm_hour;         // 时 (0-23)
    int minute = local_time.tm_min;          // 分 (0-59)
    int second = local_time.tm_sec;          // 秒 (0-59)
    operationLogFile.write(reinterpret_cast<char*>(&year), 4);
    operationLogFile.write(reinterpret_cast<char*>(&month), 4);
    operationLogFile.write(reinterpret_cast<char*>(&day), 4);
    operationLogFile.write(reinterpret_cast<char*>(&hour), 4);
    operationLogFile.write(reinterpret_cast<char*>(&minute), 4);
    operationLogFile.write(reinterpret_cast<char*>(&second), 4);
    total_operationlog++;
}

void LogManager::showOperationLog()
{
    if(total_operationlog == 0)
    {
        std::cout << "There is no operationlog!!!\n\n";
        return ;
    }
    operationLogFile.clear();
    operationLogFile.seekg(0);
    for(int i = 0; i < total_operationlog; i++)
    {
        char log[336];
        int year, month, day, hour, minute, second;
        operationLogFile.read(log, 336);
        operationLogFile.read(reinterpret_cast<char*>(&year), 4);
        operationLogFile.read(reinterpret_cast<char*>(&month), 4);
        operationLogFile.read(reinterpret_cast<char*>(&day), 4);
        operationLogFile.read(reinterpret_cast<char*>(&hour), 4);
        operationLogFile.read(reinterpret_cast<char*>(&minute), 4);
        operationLogFile.read(reinterpret_cast<char*>(&second), 4);
        std::cout << year << '-' << std::setw(2) << std::setfill('0') << month << '-' << std::setw(2) << day << ' ' << std::setw(2) << hour << ':' << std::setw(2) << minute << ':' << std::setw(2) << second << "   " << log << "\n";
    }
    std::cout << '\n';
}