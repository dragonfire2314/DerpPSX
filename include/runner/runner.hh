#ifndef RUNNER_hpp 
#define RUNNER_hpp

#include <global.h>

class Core;

class Runner
{
protected:
    Core* core;
public:
    Runner(Core* _core);
    virtual void step();
    virtual void emu_init();
};

#endif