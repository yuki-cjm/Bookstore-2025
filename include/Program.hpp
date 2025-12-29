#pragma once

#include <vector>
#include <string>
#include <memory>

#include "AccountManager.hpp"
#include "BookManager.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"

struct Account;
struct Book;
class Statement;

class Parser;

enum Token
{
    ISBN,
    NAME,
    AUTHOR,
    KEYWORD,
    PRICE,
    ALL
};

class Program
{
private:
    std::fstream totalFile;
    AccountManager accountmanager_;
    BookManager bookmanager_;
    LogManager logmanager_;
    Parser parser;
    std::vector<std::shared_ptr<Account>> loginStack;
    int book_index_now;
    
    bool programEnd_;

    void sortBook(std::vector<int> &index);
    void printloginStack();
public:
    Program();
    ~Program();
    
    void execute(const std::string &line);
    bool programRun();

    void programEnd();

    void Su(const std::string &userid, const std::string &password);
    void Logout();
    void Register(const std::string &userid, const std::string &password, const std::string &username);
    void Passwd(const std::string &userid, const std::string &password1, const std::string &password2);
    void Useradd(const std::string &userid, const std::string &password, int privilege, const std::string &username);
    void Delete(const std::string &userid);

    void Show(Token type, const std::string &content);
    void Buy(const std::string &isbn, int quantity);
    void Select(const std::string &isbn);
    void Modify(const std::string &isbn, const std::string &bookname, const std::string &author, const std::string &keyword, std::vector<std::string> &keywords, double price, bool has_price);
    void Import(int quantity, double totalcost);

    void ShowFinance(int count);
};