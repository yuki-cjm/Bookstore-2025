#include "utils/Error.hpp"
#include "BookManager.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <iomanip>
#include <map>

BookManager::BookManager()
{
    bookfile_name = "BookFile";
    bookFile.open(bookfile_name, std::ios::in | std::ios::out | std::ios::binary);
    if(!bookFile)
    {
        bookFile.open(bookfile_name, std::ios::out | std::ios::binary);
        bookFile.close();
        bookFile.open(bookfile_name, std::ios::in | std::ios::out | std::ios::binary);
    }
    total_book = 0;
    sorted_index.clear();
}

BookManager::~BookManager()
{
    bookFile.close();
}

int BookManager::setBookManager(std::fstream &totalFile, int point)
{
    totalFile.clear();
    totalFile.seekg(point);
    totalFile.read(reinterpret_cast<char*>(&total_book), 4);
    int count_sorted_index, index;
    char temp[21];
    std::string isbn;
    totalFile.read(reinterpret_cast<char*>(&count_sorted_index), 4);
    for(int i = 0 ;i < count_sorted_index; i++)
    {
        totalFile.read(temp, 21);
        isbn = std::string(temp);
        totalFile.read(reinterpret_cast<char*>(&index), 4);
        sorted_index.insert({isbn, index});
    }
    return 8 + 25 * count_sorted_index;
}

int BookManager::recordBookManager(std::fstream &totalFile, int point)
{
    totalFile.clear();
    totalFile.seekp(point);
    totalFile.write(reinterpret_cast<char*>(&total_book), 4);
    int count_sorted_index = sorted_index.size();
    char temp[21];
    totalFile.write(reinterpret_cast<char*>(&count_sorted_index), 4);
    for(auto it = sorted_index.begin(); it != sorted_index.end(); it++)
    {
        memset(temp, 0, 21);
        strncpy(temp, (it->first).c_str(), 20);
        temp[20] = '\0';
        totalFile.write(temp, 21);
        totalFile.write(reinterpret_cast<char*>(&(it->second)), 4);
    }
    return 8 + 25 * count_sorted_index;
}

std::vector<int> BookManager::findBookName(const std::string &bookname)
{
    return BookName_bookList.Find(bookname);
}

std::vector<int> BookManager::findAuthor(const std::string &author)
{
    return Author_bookList.Find(author);
}

std::vector<int> BookManager::findKeyword(const std::string &keyword)
{
    return Keyword_bookList.Find(keyword);
}

int BookManager::findBook(const std::string &isbn)
{
    std::vector<int> index = ISBN_bookList.Find(isbn);
    if(index.size() > 1)
        throw BookstoreError("Invalid");
    if(index.size() == 0)
        return -1;
    else
        return index[0];
}

std::shared_ptr<Book> BookManager::getBook(int index)
{
    bookFile.clear();
    bookFile.seekg(Booksize * index);
    char tmp[21], temp[61];
    std::string isbn;
    std::string bookname;
    std::string author;
    std::string keyword;
    int quantity;
    double price;
    bookFile.read(tmp, 21);
    isbn = std::string(tmp);
    bookFile.read(temp, 61);
    bookname = std::string(temp);
    bookFile.read(temp, 61);
    author = std::string(temp);
    bookFile.read(temp, 61);
    keyword = std::string(temp);
    bookFile.read(reinterpret_cast<char*>(&quantity), 4);
    bookFile.read(reinterpret_cast<char*>(&price), 8);
    return std::make_shared<Book>(Book{index, isbn, bookname, author, keyword, quantity, price});
}

std::shared_ptr<Book> BookManager::getBook(const std::string &isbn)
{
    int index = findBook(isbn);
    return getBook(index);
}

std::string BookManager::getISBN(int index)
{
    bookFile.clear();
    bookFile.seekg(Booksize * index);
    char temp[21];
    bookFile.read(temp, 21);
    return std::string(temp);
}

double BookManager::getPrice(int index)
{
    double price;
    bookFile.clear();
    bookFile.seekg(Booksize * index + 208);
    bookFile.read(reinterpret_cast<char*>(&price), 8);
    return price;
}

void BookManager::changeQuantity(int index, int delta_quantity)
{
    bookFile.clear();
    bookFile.seekg(Booksize * index + 204);
    int quantity;
    bookFile.read(reinterpret_cast<char*>(&quantity), 4);
    quantity += delta_quantity;
    if(quantity < 0)
        throw BookstoreError("Invalid");
    bookFile.seekp(Booksize * index + 204);
    bookFile.write(reinterpret_cast<char*>(&quantity), 4);
}

void BookManager::modify(int index, const std::string &isbn, const std::string &bookname, const std::string &author, const std::string &keyword, std::vector<std::string> &keywords, double &price)
{
    bookFile.clear();
    if(!isbn.empty())
    {
        if(sorted_index.find(isbn) != sorted_index.end())
            throw BookstoreError("Invalid");
        char temp[21];
        bookFile.seekg(Booksize * index);
        bookFile.read(temp, 21);
        std::string isbn_ = std::string(temp);
        if(!isbn_.empty())
        {
            sorted_index.erase(isbn_);
            ISBN_bookList.Delete(isbn_, index);
        }
        bookFile.clear();
        bookFile.seekp(Booksize * index);
        memset(temp, 0, 21);
        strncpy(temp, isbn.c_str(), 20);
        temp[20] = '\0';
        bookFile.write(temp, 21);
        sorted_index.insert({isbn, index});
        ISBN_bookList.Insert(isbn, index);
    }
    if(!bookname.empty())
    {
        char temp[61];
        bookFile.seekg(Booksize * index + 21);
        bookFile.read(temp, 61);
        std::string bookname_ = std::string(temp);
        if(!bookname_.empty())
            BookName_bookList.Delete(bookname_, index);
        bookFile.clear();
        bookFile.seekp(Booksize * index + 21);
        memset(temp, 0, 61);
        strncpy(temp, bookname.c_str(), 60);
        temp[60] = '\0';
        bookFile.write(temp, 61);
        BookName_bookList.Insert(bookname, index);
    }
    if(!author.empty())
    {
        char temp[61];
        bookFile.seekg(Booksize * index + 82);
        bookFile.read(temp, 61);
        std::string author_ = std::string(temp);
        if(!author.empty())
            Author_bookList.Delete(author_, index);
        bookFile.clear();
        bookFile.seekp(Booksize * index + 82);
        memset(temp, 0, 61);
        strncpy(temp, author.c_str(), 60);
        temp[60] = '\0';
        bookFile.write(temp, 61);
        Author_bookList.Insert(author, index);
    }
    if(!keywords.empty())
    {
        char temp[61];
        bookFile.seekg(Booksize * index + 143);
        bookFile.read(temp, 61);
        std::string keyword_ = std::string(temp);
        std::vector<std::string> keywords_;
        keywords_.clear();
        std::string tmp{};
        for(int i = 0; i < keyword_.length(); i++)
            if(keyword_[i] == '|')
            {
                if(tmp.empty())
                    throw BookstoreError("Invalid");
                else
                {
                    keywords_.push_back(tmp);
                    tmp = "";
                }
            }
            else
                tmp.push_back(keyword_[i]);
        keywords_.push_back(tmp);
        for(auto &&element : keywords_)
            Keyword_bookList.Delete(element, index);
        bookFile.clear();
        bookFile.seekp(Booksize * index + 143);
        memset(temp, 0, 61);
        strncpy(temp, keyword.c_str(), 60);
        temp[60] = '\0';
        bookFile.write(temp, 61);
        for(auto &&element : keywords)
            Keyword_bookList.Insert(element,  index);
    }
    if(price > 1e-6)
    {
        bookFile.seekp(Booksize * index + 208);
        bookFile.write(reinterpret_cast<char*>(&price), 8);
    }
}

int BookManager::addBook(const std::string &isbn)
{
    bookFile.clear();
    int index = total_book;
    bookFile.seekp(Booksize * index);
    char temp[21];
    memset(temp, 0, 21);
    strncpy(temp, isbn.c_str(), 20);
    temp[20] = '\0';
    bookFile.write(temp, 21);
    std::string str = "";
    char tmp[61];
    memset(tmp, 0, 61);
    strncpy(tmp, str.c_str(), 60);
    temp[60] = '\0';
    bookFile.write(tmp, 61);
    bookFile.write(tmp, 61);
    bookFile.write(tmp, 61);
    int quantity = 0;
    bookFile.write(reinterpret_cast<char*>(&quantity), 4);
    double price = 0;
    bookFile.write(reinterpret_cast<char*>(&price), 8);
    sorted_index.insert({isbn, total_book});
    ISBN_bookList.Insert(isbn, total_book);
    total_book++;
    return total_book - 1;
}

void BookManager::printBook(int index)
{
    std::shared_ptr<Book> book = getBook(index);
    std::cout << book->ISBN_ << '\t' << book->bookname_ << '\t' << book->author_ << '\t' << book->keyword_ << '\t' << std::fixed << std::setprecision(2) << book->price_ << '\t' << book->quantity_ << '\n';
}

void BookManager::printall()
{
    if(sorted_index.size() == 0)
        std::cout << '\n';
    for(auto it = sorted_index.begin(); it != sorted_index.end(); it++)
        printBook(it->second);
}