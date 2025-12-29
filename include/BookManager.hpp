#pragma once

#include "BlockLinkedList.h"

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <map>

struct Book
{
    int index;
    std::string ISBN_{""};
    std::string bookname_{""};
    std::string author_{""};
    std::string keyword_{""};
    int quantity_{0};
    double price_{0};
};

class BookManager
{
  private:
    std::fstream bookFile; // index->Book
    std::string bookfile_name; //文件路径
    int total_book;
    static const int Booksize = 216;
    BlockLinkedList<20> ISBN_bookList{"ISBN_book_blocklinkedlist", "ISBN_book_blocklinkedlist_basicinformation"}; // ISBN->index
    BlockLinkedList<60> BookName_bookList{"BookName_book_blocklinkedlist", "BookName_book_blocklinkedlist_basicinformation"}; // BookName->index
    BlockLinkedList<60> Author_bookList{"Author_book_blocklinkedlist", "Author_book_blocklinedlist_basicinformation"}; // Author->index
    BlockLinkedList<60> Keyword_bookList{"Keyword_book_blocklinkedlist", "Keyword_book_blocklinkedlist_basicinformation"}; // Keyword->index
    std::map<std::string, int> sorted_index; // isbn->index, sorted by isbn
    
  public:
    BookManager();
    ~BookManager();

    int setBookManager(std::fstream &totalFile, int point);
    int recordBookManager(std::fstream &totalFile, int point);

    std::vector<int> findBookName(const std::string &bookname);
    std::vector<int> findAuthor(const std::string &author);
    std::vector<int> findKeyword(const std::string &keyword);

    int findBook(const std::string &isbn); // 返回index
    std::shared_ptr<Book> getBook(int index);
    std::shared_ptr<Book> getBook(const std::string &isbn);
    std::string getISBN(int index);
    double getPrice(int index);

    void changeQuantity(int index, int delta_quantity);
    void modify(int index, const std::string &isbn, const std::string &bookname, const std::string &author, const std::string &keyword, std::vector<std::string> &keywords, double &price, bool has_price);
    int addBook(const std::string &isbn);

    void printBook(int index);
    void printall();
};