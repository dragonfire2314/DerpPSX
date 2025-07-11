/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */
#include "mem.h"
#include <R3000A.h>
#include <core.h>
#include <stdio.h>
#include <string.h>

void CstrMem::reset() {
    // Leave ROM intact, it contains BIOS
    memset(ram.ptr, 0, ram.size);
    memset(hwr.ptr, 0, hwr.size);
    
    //bus.interruptSet(CstrBus::INT_SPU);
}

void CstrMem::init()
{

}

template <class T>
void CstrMem::write(uw addr, T data) {
    if ((addr & MEM_MASK) < MEM_BOUNDS_RAM) { // RAM
        // A shorter method to allow mem write
        if (!(core->getCPU()->copr[12] & 0x10000)) {
            accessMem(ram, T) = data;
        }
        return;
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_SCR) { // Scratchpad
        accessMem(hwr, T) = data;
        return;
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_HWR) { // Hardware
		//std::cout << "[Type] " << typeid(data).name();
		core->getIO()->write(data, addr);
        return;
    }
    
    if ((addr) != 0xfffe0130) { // Known: 0x804, 0x800, 0x1e988
        printx("/// PSeudo Mem Write(%u): 0x%x <- 0x%x | PC: %x", (uw)sizeof(T), addr, data, core->getCPU()->pc);
    }
}

template void CstrMem::write<uw>(uw, uw);
template void CstrMem::write<uh>(uw, uh);
template void CstrMem::write<ub>(uw, ub);

template <class T>
T CstrMem::read(uw addr) {
    if ((addr & MEM_MASK) < MEM_BOUNDS_RAM) { // RAM
		//if ((addr & MEM_MASK) < 0x0c) std::cout << "Bios Call?" << std::endl;
        return accessMem(ram, T);
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_SCR) { // Scratchpad
        return accessMem(hwr, T);
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_HWR) { // Hardware
		return core->getIO()->read(addr);
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_ROM) { // ROM
        return accessMem(rom, T);
    }
    
    printx("/// PSeudo Mem Read(%u): 0x%x | PC: %x", (uw)sizeof(T), addr, core->getCPU()->pc);
    return 0;
}

template uw CstrMem::read<uw>(uw);
template uh CstrMem::read<uh>(uw);
template ub CstrMem::read<ub>(uw);

// OTC
//void CstrMem::executeDMA(CstrBus::castDMA *dma) {
//    uw *p = (uw *)&ram.ptr[dma->madr & (ram.size - 1)];
//    
//    if (dma->chcr == 0x11000002) {
//        while(dma->bcr--) {
//            *p-- = (dma->madr - 4) & 0xffffff;
//            dma->madr -= 4;
//        }
//        p++; *p = 0xffffff;
//    }
//}