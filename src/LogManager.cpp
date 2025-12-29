#include <iostream>
#include <fstream>
#include <iomanip>

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
    return 16;
}

int LogManager::recordLogManager(std::fstream &totalFile, int point)
{
    totalFile.clear();
    totalFile.seekp(point);
    totalFile.write(reinterpret_cast<char*>(&total_transactionlog), 4);
    totalFile.write(reinterpret_cast<char*>(&total_operationlog), 4);
    totalFile.write(reinterpret_cast<char*>(&total_financelog), 4);
    totalFile.write(reinterpret_cast<char*>(&total_employeelog), 4);
    return 16;
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