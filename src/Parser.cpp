#include <iostream>
#include <string>
#include <vector>

#include "utils/Error.hpp"
#include "Program.hpp"
#include "Parser.hpp"

class Program;

bool isvisibleletter(char c)
{
    return !(c <= 32 || c == 127);
}

std::vector<std::string> getkeywords(const std::string &keywords)
{
    std::vector<std::string> answer;
    std::string str{};
    for(int i = 0; i < keywords.length(); i++)
        if(keywords[i] == '|')
        {
            if(str.empty())
                throw BookstoreError("Invalid");
            answer.push_back(str);
            str = "";
        }
        else
        {
            str = str + keywords[i];
        }
    answer.push_back(str);
    return answer;
}

int stringtoint(const std::string &quantity)
{
    int number{};
    for(int i = 0; i < quantity.length(); i++)
        number = number * 10 + (quantity[i] - '0');
    if(number != 0 && quantity[0] == '0')
        throw BookstoreError("Invalid");
    return number;
}

double stringtodouble(const std::string &price)
{
    if(price.length() >= 2 && price[0] == '0' && price[1] != '.')
        throw BookstoreError("Invalid");
    double number{};
    int point = 0, count = 1;
    for(int i = 0; i < price.length(); i++)
        if(price[i] == '.')
        {
            if(point != 0)
                throw BookstoreError("Invalid");
            else
                if(i == 0 || i == price.length() - 1)
                    throw BookstoreError("Invalid");
                else
                    point = i;
        }
        else
            if(point != 0)
            {
                count *= 10;
                number = number + 1.0 * (price[i] - '0') / count;
            }
            else
                number = number * 10 + (price[i] - '0');
    return number;
}

//type = 0表示默认
//type = 1表示获取的是UserID, Password, CurrentPassword, NewPassword
//type = 2表示获取的是Username
//type = 3表示获取的是Privilege
//type = 4表示获取的是ISBN
//type = 5表示获取的是BookName, Author
//type = 6表示获取的是Keyword
//type = 7表示获取的是Quantity, Count
//type = 8表示获取的是Price, TotalCost
std::string getword(int &pointer, const std::string &line, int type = 0)
{
    std::string temp{""};
    // 去除前导空格
    while(std::isspace(static_cast<int>(line[pointer])) && pointer < line.length())
        pointer++;

    // 获得第一个单词
    while(line[pointer] != ' ' && pointer < line.length())
    {
        char c = line[pointer];
        if(type == 1 && !(c >= 'a' && c <='z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_')
        || type == 2 && !isvisibleletter(c)
        || type == 3 && !(c >= '0' && c <= '9')
        || type == 4 && !isvisibleletter(c)
        || type == 5 && (!isvisibleletter(c) || c == '"')
        || type == 6 && (!isvisibleletter(c) || c == '"')
        || type == 7 && !(c >= '0' && c <= '9')
        || type == 8 && !(c >= '0' && c <= '9' || c == '.'))
            throw BookstoreError("Invalid");
        temp.push_back(line[pointer]);
        pointer++;
    }
    if(type == 1 && temp.length() > 30
    || type == 2 && temp.length() > 30
    || type == 3 && temp.length() > 1
    || type == 4 && temp.length() > 20
    || type == 5 && temp.length() > 60
    || type == 6 && temp.length() > 60
    || type == 7 && temp.length() > 10
    || type == 8 && temp.length() > 13)
        throw BookstoreError("Invalid");
    if(type == 7 && temp.length() == 10 && temp > "2147483647")
        throw BookstoreError("Invalid");
    return temp;
}


void Parser::parseLine(const std::string &line, Program *program)
{
    int pointer{0};
    std::string order{getword(pointer, line)};
    if(order == "quit")          parseQuit    (pointer, line, program);
    else if(order == "exit")     parseExit    (pointer, line, program);
    else if(order == "su")       parseSu      (pointer, line, program);
    else if(order == "logout")   parseLogout  (pointer, line, program);
    else if(order == "register") parseRegister(pointer, line, program);
    else if(order == "passwd")   parsePasswd  (pointer, line, program);
    else if(order == "useradd")  parseUseradd (pointer, line, program);
    else if(order == "delete")   parseDelete  (pointer, line, program);
    else if(order == "show")     parseShow    (pointer, line, program);
    else if(order == "buy")      parseBuy     (pointer, line, program);
    else if(order == "select")   parseSelect  (pointer, line, program);
    else if(order == "modify")   parseModify  (pointer, line, program);
    else if(order == "import")   parseImport  (pointer, line, program);
    else if(order == "log")      parseLog     (pointer, line, program);
    else if(order == "report")   parseReport  (pointer, line, program);
    else if(!order.empty())      throw BookstoreError("Invalid");
}

void Parser::parseQuit(int &pointer, const std::string &line, Program *program)
{
    std::string str = getword(pointer, line);
    if(str.empty())
        program->programEnd();
    else
        throw BookstoreError("Invalid");
}

void Parser::parseExit(int &pointer, const std::string &line, Program *program)
{
    std::string str = getword(pointer, line);
    if(str.empty())
        program->programEnd();
    else
        throw BookstoreError("Invalid");
}

void Parser::parseSu(int &pointer, const std::string &line, Program *program)
{
    std::string UserID = getword(pointer, line, 1);
    std::string Password = getword(pointer, line, 1);
    std::string str = getword(pointer, line);
    if(!str.empty())
        throw BookstoreError("Invalid");
    program->Su(UserID, Password);
}

void Parser::parseLogout(int &pointer, const std::string &line, Program *program)
{
    std::string str = getword(pointer, line);
    if(str.empty())
        program->Logout();
    else
        throw BookstoreError("Invalid");
}

void Parser::parseRegister(int &pointer, const std::string &line, Program *program)
{
    std::string UserID = getword(pointer, line, 1);
    std::string Password = getword(pointer, line, 1);
    std::string Username = getword(pointer, line, 2);
    if(Username.empty())
        throw BookstoreError("Invalid");
    std::string str = getword(pointer, line);
    if(str.empty())
        program->Register(UserID, Password, Username);
    else
        throw BookstoreError("Invalid");
}

void Parser::parsePasswd(int &pointer, const std::string &line, Program *program)
{
    std::string UserID = getword(pointer, line, 1);
    std::string Password1 = getword(pointer, line, 1);
    std::string Password2 = getword(pointer, line, 1);
    std::string str = getword(pointer, line);
    if(str.empty())
        program->Passwd(UserID, Password1, Password2);
    else 
        throw BookstoreError("Invalid");
}

void Parser::parseUseradd(int &pointer, const std::string &line, Program *program)
{
    std::string UserID = getword(pointer, line, 1);
    std::string Password = getword(pointer, line, 1);
    std::string Privilege = getword(pointer, line, 3);
    std::string Username = getword(pointer, line, 2);
    std::string str = getword(pointer, line);
    if(str.empty())
        program->Useradd(UserID, Password, Privilege[0] - '0', Username);
    else
        throw BookstoreError("Invalid");
}

void Parser::parseDelete(int &pointer, const std::string &line, Program *program)
{
    std::string UserID = getword(pointer, line, 1);
    std::string str = getword(pointer, line);
    if(str.empty())
        program->Delete(UserID);
    else
        throw BookstoreError("Invalid");
}

void Parser::parseShow(int &pointer, const std::string &line, Program *program)
{
    std::string str = getword(pointer, line);
    if(str == "finance")
    {
        str = getword(pointer, line, 7);
        if(str.empty())
            program->ShowFinance(-1);
        else
            program->ShowFinance(stringtoint(str));
    }
    else
    {
        if(str.empty())
        {
            program->Show(Token::ALL, str);
            return;
        }
        std::string getagain = getword(pointer, line);
        if(!getagain.empty())
            throw BookstoreError("Invalid");
        std::string type{};
        int p = 0;
        if(str[p] == '-') p++;
        else throw BookstoreError("Invalid");

        for(; p < str.length(); p++)
            if(str[p] == '=')
                break;
            else
                type += str[p];
        if(p == str.length() - 1) throw BookstoreError("Invalid");
        p++;
        if(type == "ISBN")
        {
            type = getword(p, str, 4);
            if(type.empty())
                throw BookstoreError("Invalid");
            program->Show(Token::ISBN, type);
        }
        else if(type == "name")
        {
            if(!(str[p] == '"' && str[str.length() - 1] == '"' && p != str.length() - 1 && p != str.length() - 2))
                throw BookstoreError("Invalid");
            type = str.substr(p + 1, str.length() - 2 - p);
            p = 0;
            type= getword(p, type, 5);
            if(type.empty())
                throw BookstoreError("Invalid");
            program->Show(Token::NAME, type);
        }
        else if(type == "author")
        {
            if(!(str[p] == '"' && str[str.length() - 1] == '"' && p != str.length() - 1 && p != str.length() - 2))
                throw BookstoreError("Invalid");
            type = str.substr(p + 1, str.length() - 2 - p);
            p = 0;
            type= getword(p, type, 5);
            if(type.empty())
                throw BookstoreError("Invalid");
            program->Show(Token::AUTHOR, type);
        }
        else if(type == "keyword")
        {
            if(!(str[p] == '"' && str[str.length() - 1] == '"' && p != str.length() - 1 && p != str.length() - 2))
                throw BookstoreError("Invalid");
            type = str.substr(p + 1, str.length() - 2 - p);
            if(type.length() > 60 || type.empty())
                throw BookstoreError("Invalid");
            for(int i = 0; i < type.length(); i++)
                if(!isvisibleletter(type[i]) || type[i] == '"' || type[i] == '|')
                    throw BookstoreError("Invalid");
            program->Show(Token::KEYWORD, type);
        }
        else
            throw BookstoreError("Invalid");
    }
}

void Parser::parseBuy(int &pointer, const std::string &line, Program *program)
{
    std::string isbn = getword(pointer, line, 4);
    std::string quantity = getword(pointer, line, 7);
    std::string str = getword(pointer, line);
    if(!str.empty())
        throw BookstoreError("Invalid");
    int number = stringtoint(quantity);
    if(!(number > 0))
        throw BookstoreError("Invalid");
    program->Buy(isbn, number);
}

void Parser::parseSelect(int &pointer, const std::string &line, Program *program)
{
    std::string isbn = getword(pointer, line, 4);
    std::string str = getword(pointer, line);
    if(!str.empty())
        throw BookstoreError("Invalid");
    program->Select(isbn);
}

void Parser::parseModify(int &pointer, const std::string &line, Program *program)
{
    std::string isbn{};
    std::string bookname{};
    std::string keyword{};
    std::vector<std::string> keywords{};
    std::string author;
    double price{0};
    std::string str;
    bool getprice = false;
    while(1)
    {
        str = getword(pointer, line);
        if(str.empty())
            break;
        std::string type{};
        int p = 0;
        if(str[p] == '-') p++;
        else throw BookstoreError("Invalid");

        for(; p < str.length(); p++)
            if(str[p] == '=')
                break;
            else
                type += str[p];
        if(p == str.length() - 1)
            throw BookstoreError("Invalid");
        p++;
        if(type == "ISBN")
        {
            if(!isbn.empty())
                throw BookstoreError("Invalid");
            type = getword(p, str, 4);
            if(type.empty())
                throw BookstoreError("Invalid");
            isbn = type;
        }
        else if(type == "name")
        {
            if(!bookname.empty())
                throw BookstoreError("Invalid");
            if(!(str[p] == '"' && str[str.length() - 1] == '"' && p != str.length() - 1 && p != str.length() - 2))
                throw BookstoreError("Invalid");
            type = str.substr(p + 1, str.length() - p - 2);
            p = 0;
            type = getword(p, type, 5);
            if(type.empty())
                throw BookstoreError("Invalid");
            bookname = type;
        }
        else if(type == "author")
        {
            if(!author.empty())
                throw BookstoreError("Invalid");
            if(!(str[p] == '"' && str[str.length() - 1] == '"' && p != str.length() - 1 && p != str.length() - 2))
                throw BookstoreError("Invalid");
            type = str.substr(p + 1, str.length() - p - 2);
            p = 0;
            type = getword(p, type, 5);
            if(type.empty())
                throw BookstoreError("Invalid");
            author = type;
        }
        else if(type == "keyword")
        {
            if(!keywords.empty())
                throw BookstoreError("Invalid");
            if(!(str[p] == '"' && str[str.length() - 1] == '"' && p != str.length() - 1 && p != str.length() - 2))
                throw BookstoreError("Invalid");
            type = str.substr(p + 1, str.length() - p - 2);
            p = 0;
            keyword = getword(p, type, 6);
            keywords = std::move(getkeywords(keyword));
            if(keywords.empty())
                throw BookstoreError("Invalid");
            for(auto it1 = keywords.begin(); it1 != keywords.end(); it1++)
                for(auto it2 = it1 + 1; it2 != keywords.end(); it2++)
                    if(*it1 == *it2)
                        throw BookstoreError("Invalid");
        }
        else if(type == "price")
        {
            if(getprice)
                throw BookstoreError("Invalid");
            type = getword(p, str, 8);
            price = stringtodouble(type);
            getprice = true;
        }
        else
            throw BookstoreError("Invalid");
    }
    program->Modify(isbn, bookname, author, keyword, keywords, price);
}

void Parser::parseImport(int &pointer, const std::string &line, Program *program)
{
    std::string str;
    int quantity;
    double totalcost;
    str = getword(pointer, line, 7);
    quantity = stringtoint(str);
    if(!(quantity > 0))
        throw BookstoreError("Invalid");
    str = getword(pointer, line, 8);
    totalcost = stringtodouble(str);
    if(!(totalcost > 0))
        throw BookstoreError("Invalid");
    str= getword(pointer, line);
    if(!str.empty())
        throw BookstoreError("Invalid");
    program->Import(quantity, totalcost);
}

void Parser::parseLog(int &pointer, const std::string &line, Program *program)
{

}

void Parser::parseReport(int &pointer, const std::string &line, Program *program)
{
    
}