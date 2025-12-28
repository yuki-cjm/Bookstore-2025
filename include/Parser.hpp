#pragma once

#include <string>

class Program;

class Parser
{
  public:
    void parseLine(const std::string &line, Program *program);

  private:
    // 语句解析分发
    void parseQuit    (int &pointer, const std::string &line, Program *program);
    void parseExit    (int &pointer, const std::string &line, Program *program);
    void parseSu      (int &pointer, const std::string &line, Program *program);
    void parseLogout  (int &pointer, const std::string &line, Program *program);
    void parseRegister(int &pointer, const std::string &line, Program *program);
    void parsePasswd  (int &pointer, const std::string &line, Program *program);
    void parseUseradd (int &pointer, const std::string &line, Program *program);
    void parseDelete  (int &pointer, const std::string &line, Program *program);
    void parseShow    (int &pointer, const std::string &line, Program *program);
    void parseBuy     (int &pointer, const std::string &line, Program *program);
    void parseSelect  (int &pointer, const std::string &line, Program *program);
    void parseModify  (int &pointer, const std::string &line, Program *program);
    void parseImport  (int &pointer, const std::string &line, Program *program);
    void parseLog     (int &pointer, const std::string &line, Program *program);
    void parseReport  (int &pointer, const std::string &line, Program *program);
};