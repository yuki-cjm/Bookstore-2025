#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "BlockLinkedList.h"
#include "BookManager.hpp"

struct Account {
    int index;
    std::string username_;
    std::string userID_;
    std::string password_;
    int privilege_;
    int book_index = -1;

    void printAccount();
};

class AccountManager {
  private:
    static const int Accountsize = 97;
    std::fstream accountFile; // index->Account
    std::string accountfile_name; //文件路径
    int total_account;
    int count_deleted_account;
    std::vector<int> deleted_account; //记录被delete的账户的index
    BlockLinkedList<30> accountList{"../file/account_blocklinkedlist", "../file/account_blocklinkedlist_basicinformation"}; // userid->index

  public:
    AccountManager();
    ~AccountManager();

    void setAccountManager(std::fstream &totalFile);
    void recordAccountManager(std::fstream &totalFile);

    int findAccount(const std::string &userid);//返回index
    std::shared_ptr<Account> getAccount(int index);
    std::shared_ptr<Account> getAccount(const std::string &userid);
    void addAccount(const std::string &userid, const std::string &password, int privilege, const std::string &username);
    void changePassword(int index, const std::string &password);
    void deleteAccount(const std::string &userid, int index);
};