#include <iostream>
#include <fstream>
#include <iomanip>

#include "utils/Error.hpp"
#include "LogManager.hpp"

LogManager::LogManager()
{
    transactionLogFile.open("../file/transactionLogFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!transactionLogFile)
    {
        transactionLogFile.open("../file/transactionLogFile", std::ios::out | std::ios::binary);
        transactionLogFile.close();
        transactionLogFile.open("../file/transactionLogFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    operationLogFile.open("../file/operationLogFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!transactionLogFile)
    {
        operationLogFile.open("../file/operationLogFile", std::ios::out | std::ios::binary);
        operationLogFile.close();
        operationLogFile.open("../file/operationLogFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    employeeReportFile.open("../file/transactionLogFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!transactionLogFile)
    {
        employeeReportFile.open("../file/employeeReportFile", std::ios::out | std::ios::binary);
        employeeReportFile.close();
        employeeReportFile.open("../file/employeeReportFile", std::ios::in | std::ios::out | std::ios::binary);
    }
    total_transactionlog = 0;
    total_operationlog = 0;
    total_employeelog = 0;
}

LogManager::~LogManager()
{
    transactionLogFile.close();
    operationLogFile.close();
    employeeReportFile.close();
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
        std::cout << total_transactionlog << std::endl;
        std::cout << income << ' ' << expenditure << std::endl;
    }
    if(finance >= 0)
        income += finance;
    else
        expenditure += (-finance);
    std::cout << income << ' ' << expenditure << std::endl;
    transactionLogFile.clear();
    transactionLogFile.seekp(total_transactionlog * transactionlogsize);
    transactionLogFile.write(reinterpret_cast<char*>(&income), 8);
    transactionLogFile.write(reinterpret_cast<char*>(&expenditure), 8);
    total_transactionlog++;
}

void LogManager::showTransaction(int count)
{
    std::cout <<"total_transactionlog:  " << total_transactionlog << std::endl;
    double temp_income, temp_expenditure;
    for(int i = 0; i < total_transactionlog; i++)
    {
        transactionLogFile.clear();
        transactionLogFile.seekg(i * transactionlogsize);
        transactionLogFile.read(reinterpret_cast<char*>(&temp_income), 8);
        transactionLogFile.read(reinterpret_cast<char*>(&temp_expenditure), 8);
        std::cout << temp_income << ' ' << temp_expenditure << std::endl;
    }
    if(count > total_transactionlog)
        throw BookstoreError("invalid");
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
        std::cout << std::fixed << std::setprecision(2) << "+ " << income << "- " << expenditure << '\n';
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
        std::cout << std::fixed << std::setprecision(2) << "+ " << income - temp_income << "- " << expenditure - temp_expenditure << '\n';
    }
}