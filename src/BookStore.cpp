#include <iostream>
#include <string>

#include "utils/Error.hpp"
#include "Parser.hpp"
#include "Program.hpp"


int main()
{
    // freopen("../testcases/ComplexTestCase-1/160.in", "r", stdin);
    // freopen("../BookStore.out", "w", stdout);

    Program program;
    std::string line;
    while(program.programRun() && std::getline(std::cin, line))
    {
        if(line.empty()) continue;
        try { program.execute(line); }
        catch (const BookstoreError& e) { std::cout << e.message() << '\n'; }
    }
    return 0;
}