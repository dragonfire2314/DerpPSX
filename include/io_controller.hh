#ifndef IO_CONTROLLER_HPP
#define IO_CONTROLLER_HPP

#include <global.hh>

class Core;

class IO_Controller
{
private:
    Core* core;
public:
	enum IRQ_TYPE {
		VBLANK = 0x01,
		GPU = 0x02,
		CDROM = 0x04,
		DMA = 0x08,
		TMR0 = 0x10,
		TMR1 = 0x20,
		TMR2 = 0x40,
		CONTROLLER = 0x80,
		SIO = 0x100,
		SPU = 0x200,
		LIGHT_PEN = 0x400
	};

	IO_Controller(Core* _core) 
    {
       core = _core;
    }
	~IO_Controller() {}

    void init();


	uw* dataPointerForGPU_Hack = 0;
	uw dataForGPU_Hack = 0;

	uw i_stat = 0;
	uw i_mask = 0;

	uw read(uw address);
	void write(uw data, uw address);

	void IRQ(IRQ_TYPE type);
};


#endif