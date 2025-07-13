#ifndef DEBUGGER_hpp 
#define DEBUGGER_hpp

#include <global.hh>

#include <runner/runner.hh>

class Debugger
{
private:
    Core* core;

    std::vector<std::pair<uw,std::string>> breakPoints;
public:
    Debugger(Core *_core);
    void addBreakPoint(uw pc, std::string message = "");
};

#endif

// #ifndef DEBUGGER_hpp 
// #define DEBUGGER_hpp

// #include <global.hh>

// #include <runner/runner.hh>

// class Debugger : public Runner
// {
// private:
//     std::vector<std::pair<uw,std::string>> breakPoints;

//     bool isBreak = false;

//     void checkBreakPoints();
//     void userControl();
// public:
//     Debugger(Core *_core) : Runner(_core) {}
//     void step();
//     void emu_init();

//     void addBreakPoint(uw pc, std::string message = "");
// };

// #endif