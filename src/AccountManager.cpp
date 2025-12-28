#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>

#include "utils/Error.hpp"
#include "BlockLinkedList.h"
#include "AccountManager.hpp"

AccountManager::AccountManager()
{
    accountfile_name = "../file/AccountFile";
    accountFile.open(accountfile_name, std::ios::in | std::ios::out | std::ios::binary);
    if(!accountFile)
    {
        accountFile.open(accountfile_name, std::ios::out | std::ios::binary);
        accountFile.close();
        accountFile.open(accountfile_name, std::ios::in | std::ios::out | std::ios::binary);
    }
    total_account = 0;
    count_deleted_account = 0;
    deleted_account.clear();
}

AccountManager::~AccountManager()
{
    accountFile.close();
}

void AccountManager::setAccountManager(std::fstream &totalFile)
{
    totalFile.clear();
    totalFile.seekg(4);
    totalFile.read(reinterpret_cast<char*>(&total_account), 4);
    totalFile.read(reinterpret_cast<char*>(&count_deleted_account), 4);
    int index;
    deleted_account.clear();
    for(int i = 0; i < count_deleted_account; i++)
    {
        totalFile.read(reinterpret_cast<char*>(&index), 4);
        deleted_account.push_back(index);
    }
}

void AccountManager::recordAccountManager(std::fstream &totalFile)
{
    totalFile.clear();
    totalFile.seekp(4);
    totalFile.write(reinterpret_cast<char*>(&total_account), 4);
    totalFile.write(reinterpret_cast<char*>(&count_deleted_account), 4);
    for(int i = 0; i < count_deleted_account; i++)
        totalFile.write(reinterpret_cast<char*>(&deleted_account[i]), 4);
}

int AccountManager::findAccount(const std::string &userid)
{
    std::vector<int> list = std::move(accountList.Find(userid));
    if(list.size() != 1)
        throw BookstoreError("Invalid");
    for(int index : deleted_account)
        if(index == list[0])
            throw BookstoreError("Invalid");
    return list[0];
}

std::shared_ptr<Account> AccountManager::getAccount(int index)
{
    accountFile.clear();
    accountFile.seekg(index * Accountsize);
    char temp[31];
    std::string username;
    std::string userid;
    std::string password;
    int privilege;
    accountFile.read(temp, 31);
    username = std::string(temp);
    accountFile.read(temp, 31);
    userid = std::string(temp);
    accountFile.read(temp, 31);
    password = std::string(temp);
    accountFile.read(reinterpret_cast<char*>(&privilege), 4);
    return std::make_shared<Account>(Account{index, username, userid, password, privilege});
}

std::shared_ptr<Account> AccountManager::getAccount(const std::string &userid)
{
    int index = findAccount(userid);
    return getAccount(index);
}

void AccountManager::addAccount(const std::string &userid, const std::string &password, int privilege, const std::string &username)
{
    int index;
    if(count_deleted_account != 0)
    {
        index = deleted_account.back();
        deleted_account.pop_back();
        count_deleted_account--;
    }
    else
    {
        index = total_account;
        total_account++;
    }
    accountList.Insert(userid, index);
    accountFile.clear();
    accountFile.seekp(index * Accountsize);
    char temp[31];
    temp[30] = '\0';
    memset(temp, 0, 30);
    strncpy(temp, username.c_str(), 30);
    accountFile.write(temp, 31);
    memset(temp, 0, 30);
    strncpy(temp, userid.c_str(), 30);
    accountFile.write(temp, 31);
    memset(temp, 0, 30);
    strncpy(temp, password.c_str(), 30);
    accountFile.write(temp, 31);
    accountFile.write(reinterpret_cast<char*>(&privilege), 4);
}

void AccountManager::changePassword(int index, const std::string &password)
{
    accountFile.clear();
    accountFile.seekp(index * Accountsize + 62);
    char temp[31];
    memset(temp, 0, 31);
    strncpy(temp, password.c_str(), 30);
    temp[30] = '\0';
    accountFile.write(temp, 31);
}

void AccountManager::deleteAccount(const std::string &userid, int index)
{
    count_deleted_account++;
    deleted_account.push_back(index);
    accountList.Delete(userid, index);
}
