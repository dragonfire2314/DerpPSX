#ifndef CORE_HPP
#define CORE_HPP

#include <string.h>

#include <gpu.hh>
#include <cpu/cpu.hh>
#include <mem.hh>
#include <io_controller.hh>
#include <dma.hh>
#include <renderer/renderAPI.hh>
#include <renderer/OpenGL/opengl.hh>
#include <timer.hh>
#include <runner/debug/debugger.hh>
#include <cdrom.hh>
#include <configuration.hh>

class Core 
{
private:
    GPU* gpu;
    CstrMem* mem;
    CPU* cpu;
    DMA* dma;
    IO_Controller* io;
    RenderAPI* renderer;
    Timer* timer;
    CDROM* cdrom;

    Debugger* debugger = nullptr;

    Configuration* configuration;
public:
    Core(){}
    ~Core(){}

    void init(Configuration* _configuration);

    void addDebugger(Debugger* debugger);
    Debugger* getDebugger();

    GPU* getGPU();
    CstrMem* getMem();
    CPU* getCPU();
    DMA* getDMA();
    IO_Controller* getIO();
    RenderAPI* getRenderer();
    Timer* getTimer();
    CDROM* getCDROM();
    Configuration* getConfiguration();
};

#endif