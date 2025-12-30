#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>


#include "utils/Error.hpp"
#include "AccountManager.hpp"
#include "BookManager.hpp"
#include "LogManager.hpp"
#include "Program.hpp"


Program::Program()
{
    programEnd_ = false;
    totalFile.open("totalFile", std::ios::in | std::ios::out | std::ios::binary);
    if(!totalFile)
    {
        totalFile.open("totalFile", std::ios::out | std::ios::binary);
        totalFile.close();
        totalFile.open("totalFile", std::ios::in | std::ios::out | std::ios::binary);
        std::string userid = "root";
        std::string password = "sjtu";
        std::string username = "caojiaming";
        accountmanager_.addAccount(userid, password, 7, username);
    }
    else
    {
        int point = 0;
        point += accountmanager_.setAccountManager(totalFile, point);
        point += bookmanager_.setBookManager(totalFile, point);
        point += logmanager_.setLogManager(totalFile, point);
    }
    book_index_now = -1;
}

Program::~Program()
{
    int point = 0;
    point += accountmanager_.recordAccountManager(totalFile, point);
    point += bookmanager_.recordBookManager(totalFile, point);
    point += logmanager_.recordLogManager(totalFile, point);
    totalFile.close();
}

void Program::sortBook(std::vector<int> &index)
{
    // can be optimized
    std::vector<std::string> isbn;
    isbn.clear();
    for(auto it = index.begin(); it != index.end(); it++)
        isbn.push_back(bookmanager_.getISBN(*it));
    for(int i = 0; i < index.size(); i++)
        for(int j = i + 1; j < index.size(); j++)
            if(isbn[i] > isbn[j])
            {
                std::swap(isbn[i], isbn[j]);
                std::swap(index[i], index[j]);
            }
}

void Program::printloginStack()
{
    std::cout << "loginstack:" << std::endl;
    for(int i = 0; i < loginStack.size(); i++)
        loginStack[i]->printAccount();
    if(loginStack.size() == 0)
        std::cout << "empty" << std::endl;
}

void Program::execute(const std::string &line)
{
    parser.parseLine(line, this);
}

void Program::programEnd()
{
    programEnd_ = true;
}

bool Program::programRun()
{
    return !(programEnd_);
}

void Program::Su(const std::string &userid, const std::string &password)
{
    std::shared_ptr<Account> account = accountmanager_.getAccount(userid);
    if(!account)
        throw BookstoreError("Invalid");
    if(!password.empty())
        if(password == account->password_)
            loginStack.push_back(account);
        else
            throw BookstoreError("Invalid");
    else
        if(loginStack.size() != 0 && loginStack.back()->privilege_ > account->privilege_)
            loginStack.push_back(account);
        else
            throw BookstoreError("Invalid");
    book_index_now = -1;
    // printloginStack();

    std::string log = userid + " login.";
    logmanager_.addOperationLog(log);
}

void Program::Logout()
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 1)
        throw BookstoreError("Invalid");

    std::string log = loginStack.back()->userID_ + " logout.";
    logmanager_.addOperationLog(log);

    loginStack.pop_back();
    if(loginStack.empty())
        book_index_now = -1;
    else
        book_index_now = loginStack.back()->book_index;
    // printloginStack();
}

void Program::Register(const std::string &userid, const std::string &password, const std::string &username)
{
    if(accountmanager_.findAccount(userid) != -1)
        throw BookstoreError("Invalid");
    accountmanager_.addAccount(userid, password, 1, username);

    std::string log = userid + " register : password = "  + password + " , username = " + username + '.';
    logmanager_.addOperationLog(log);
}

void Program::Passwd(const std::string &userid, const std::string &password1, const std::string &password2)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 1)
        throw BookstoreError("Invalid");
    std::shared_ptr<Account> account = accountmanager_.getAccount(userid);
    if(!account)
        throw BookstoreError("Invalid");
    if(password2.empty())
        if(loginStack.back()->privilege_ == 7)
        {
            std::string log = loginStack.back()->userID_ + " change password from old password: " + account->password_ + " to new password: " + password1 + '.';
            logmanager_.addOperationLog(log);

            accountmanager_.changePassword(account->index, password1);
        }
        else
            throw BookstoreError("Invalid");
    else
        if(account->password_ == password1)
        {
            std::string log = loginStack.back()->userID_ + " change password from old password: " + account->password_ + " to new password: " + password2 + '.';
            logmanager_.addOperationLog(log);

            accountmanager_.changePassword(account->index, password2);
        }
        else
            throw BookstoreError("Invalid");
}

void Program::Useradd(const std::string &userid, const std::string &password, int privilege, const std::string &username)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 3)
        throw BookstoreError("Invalid");
    if(privilege >= loginStack.back()->privilege_)
        throw BookstoreError("Invalid");
    if(privilege != 1 && privilege != 3 && privilege != 7)
        throw BookstoreError("Invalid");
    if(accountmanager_.findAccount(userid) != -1)
        throw BookstoreError("Invalid");
    accountmanager_.addAccount(userid, password, privilege, username);
    if(privilege == 3)
        logmanager_.addEmployee(userid);

    std::string log = loginStack.back()->userID_ + " adduser " + userid + " : password = " + password + " , privilege = " + std::to_string(privilege) + " , username = " + username + '.';
    logmanager_.addOperationLog(log);
}

void Program::Delete(const std::string &userid)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 7)
        throw BookstoreError("Invalid");
    int index = accountmanager_.findAccount(userid);
    if(index == -1)
        throw BookstoreError("Invalid");
    //This can be optimized.
    for(std::shared_ptr<Account> account : loginStack)
        if(account->userID_ == userid)
            throw BookstoreError("Invalid");
    accountmanager_.deleteAccount(userid, index);

    std::string log = loginStack.back()->userID_ + " delete user: " + userid + '.';
    logmanager_.addOperationLog(log);
}

void Program::Show(Token type, const std::string &content)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 1)
        throw BookstoreError("Invalid");
    switch(type)
    {
        case Token::ISBN: {
            int index = bookmanager_.findBook(content);
            if(index == -1)
                std::cout << '\n';
            else
                bookmanager_.printBook(index);}
            break;
        case Token::NAME: {
            std::vector<int> index = bookmanager_.findBookName(content);
            sortBook(index);
            for(int it : index)
                bookmanager_.printBook(it);
            if(index.size() == 0)
                std::cout << '\n';}
            break;
        case Token::AUTHOR: {
            std::vector<int> index = bookmanager_.findAuthor(content);
            sortBook(index);
            for(int it : index)
                bookmanager_.printBook(it);
            if(index.size() == 0)
                std::cout << '\n';}
            break;
        case Token::KEYWORD: {
            std::vector<int> index = bookmanager_.findKeyword(content);
            sortBook(index);
            for(int it : index)
                bookmanager_.printBook(it);
            if(index.size() == 0)
                std::cout << '\n';}
            break;
        case Token::ALL: {
            bookmanager_.printall();}
            break;
        default:
            throw BookstoreError("Invalid");
    }
}

void Program::Buy(const std::string &isbn, int quantity)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 1)
        throw BookstoreError("Invalid");
    int index = bookmanager_.findBook(isbn);
    if(index == -1)
        throw BookstoreError("Invalid");
    std::shared_ptr<Book> book = bookmanager_.getBook(index);
    if(book->quantity_ < quantity)
        throw BookstoreError("Invalid");
    bookmanager_.changeQuantity(index, -quantity);
    std::cout << std::fixed << std::setprecision(2) << book->price_ * quantity << '\n';

    logmanager_.addTransactionLog(book->price_ * quantity);
    logmanager_.addFinanceLog(loginStack.back()->userID_, book->ISBN_, -quantity, book->price_ * quantity);
    double totalcost = book->price_ * quantity;
    std::string cost = std::to_string(totalcost);
    cost.erase(cost.end() - 4, cost.end());
    std::string log = loginStack.back()->userID_ + " spend " + cost + " to buy " + std::to_string(quantity) + " of " + book->ISBN_ + '.';
    logmanager_.addOperationLog(log);
}

void Program::Select(const std::string &isbn)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 3)
        throw BookstoreError("Invalid");
    int index = bookmanager_.findBook(isbn);
    if(index == -1)
    {
        index = bookmanager_.addBook(isbn);
        book_index_now = index;
        loginStack.back()->book_index = index;

        std::string log = loginStack.back()->userID_ + "add new book: " + isbn + '.';
        logmanager_.addOperationLog(log);
    }
    else
    {
        book_index_now = index;
        loginStack.back()->book_index = index;
    }
}

void Program::Modify(const std::string &isbn, const std::string &bookname, const std::string &author, const std::string &keyword, std::vector<std::string> &keywords, double price, bool has_price)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 3)
        throw BookstoreError("Invalid");
    if(book_index_now == -1)
        throw BookstoreError("Invalid");
    bookmanager_.modify(book_index_now, isbn, bookname, author, keyword, keywords, price, has_price);

    std::string old_isbn = bookmanager_.getISBN(book_index_now);
    logmanager_.addEmployeeLog(loginStack.back()->privilege_, loginStack.back()->userID_, old_isbn, isbn, bookname, author, keyword, price, has_price);
}

void Program::Import(int quantity, double totalcost)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 3)
        throw BookstoreError("Invalid");
    if(book_index_now == -1)
        throw BookstoreError("Invalid");
    bookmanager_.changeQuantity(book_index_now, quantity);

    logmanager_.addTransactionLog(-totalcost);
    std::shared_ptr<Book> book = bookmanager_.getBook(book_index_now);
    logmanager_.addFinanceLog(loginStack.back()->userID_, book->ISBN_, quantity, totalcost);
    logmanager_.addEmployeeLog(loginStack.back()->privilege_, loginStack.back()->userID_, book->ISBN_, quantity, totalcost);
}

void Program::ShowFinance(int count)
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 7)
        throw BookstoreError("Invalid");
    logmanager_.showTransaction(count);
}

void Program::ReportFinance()
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 7)
        throw BookstoreError("Invalid");
    logmanager_.showFinanceLog();
}

void Program::ReportEmployee()
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 7)
        throw BookstoreError("Invalid");
    logmanager_.showEmployeeLog();
}

void Program::ReportOperation()
{
    if(loginStack.empty() || loginStack.back()->privilege_ < 7)
        throw BookstoreError("Invalid");
    logmanager_.showOperationLog();
}