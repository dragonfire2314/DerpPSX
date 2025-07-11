#ifndef CORE_HPP
#define CORE_HPP

#include <string.h>

#include <gpu.h>
#include <R3000A.h>
#include <mem.h>
#include <io_controller.h>
#include <dma.h>
#include <renderer/renderAPI.h>
#include <renderer/OpenGL/opengl.h>
#include <timer.h>
#include <runner/debug/debugger.h>
#include <cdrom.h>

class Core 
{
private:
    GPU* gpu;
    CstrMem* mem;
    CstrMips* cpu;
    DMA* dma;
    IO_Controller* io;
    RenderAPI* renderer;
    Timer* timer;
    CDROM* cdrom;

    Debugger* debugger = nullptr;
public:
    Core(){}
    ~Core(){}

    void init();

    void addDebugger(Debugger* debugger);
    Debugger* getDebugger();

    GPU* getGPU();
    CstrMem* getMem();
    CstrMips* getCPU();
    DMA* getDMA();
    IO_Controller* getIO();
    RenderAPI* getRenderer();
    Timer* getTimer();
    CDROM* getCDROM();
};

#endif