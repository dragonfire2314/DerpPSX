#include <core.h>

void Core::init() 
{
    gpu = new GPU(this);
    mem = new CstrMem(this);
    cpu = new CstrMips(this);
    dma = new DMA(this);
    io = new IO_Controller(this);
    renderer = new OpenGL(this);
    timer = new Timer(this);
    cdrom = new CDROM(this);

    mem->init();
    gpu->gpu_init();
    cpu->init();
    io->init();
    cdrom->init();
}

void Core::addDebugger(Debugger* _debugger)
{
    debugger = _debugger;
}
Debugger* Core::getDebugger()
{
    return debugger;
}


GPU* Core::getGPU()
{
    return gpu;
}
CstrMem* Core::getMem()
{
    return mem;
}
CstrMips* Core::getCPU()
{
    return cpu;
}
DMA* Core::getDMA()
{
    return dma;
}
IO_Controller* Core::getIO()
{
    return io;
}
RenderAPI* Core::getRenderer()
{
    return renderer;
}
Timer* Core::getTimer()
{
    return timer;
}
CDROM* Core::getCDROM()
{
    return cdrom;
}