#include <iostream>
#include <string>
#include <filesystem>

#include "utils/Error.hpp"
#include "Parser.hpp"
#include "Program.hpp"


int main()
{
    // freopen("../testcases/BasicTestCase-4/4.in", "r", stdin);
    // freopen("../BookStore.out", "w", stdout);

    std::filesystem::create_directory("../file");
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