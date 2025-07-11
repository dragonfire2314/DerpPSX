#include <runner/debug/debugger.h>

#include <core.h>
#include <logging.h>

void Debugger::emu_init()
{
    //Load bios
	std::ifstream Openfile("/home/tanner/DerpPSX/ps1Roms/SCPH1001.BIN", std::ios::binary | std::ios::in);
	// std::ifstream Openfile("J:/Downloads/ps1 roms/psxtest_gte.exe", std::ios::binary | std::ios::in);

	uint32_t size;
	if (!Openfile) 
	{
        std::cerr << "Error: " << strerror(errno);
		LOG::error(LOG::SystemLocation::APP, "Bios failed to load");
		fprintf(stderr, "Cant open BIOS???\n");
		exit(1);
	}
	size = 524288;

	core->getMem()->rom.ptr = new uint8_t[size];
	Openfile.read((char*)core->getMem()->rom.ptr, size);

	LOG::debug(LOG::SystemLocation::APP, "Bios loaded %i", 6);

	core->getRenderer()->init();

	LOG::info(LOG::SystemLocation::APP, "Core init complete");

	core->getCPU()->reset();
	core->getMem()->reset();




    // addBreakPoint(0x800363dc, "MONSTER FUNCTION");
    // addBreakPoint(0x800582e0, "missing calls");

    // for(unsigned int i = 0x800363d0; i < 0x800376a0; i+=0x100) 
    // {
    //     addBreakPoint(i, std::to_string(i));
    // }

    // addBreakPoint(0xbfc00184, "Wrong Execption");
}

void Debugger::step()
{
    //CPU
	//Get resoluition to adjust timings
	int horizontalRes = core->getGPU()->get_horizontal_timings();
	// int horizontalRes = 368;
	//CPU
	for (int t = 0; t < horizontalRes; t++) 
    {
		for (int i = 0; i < 564480 / horizontalRes; i++) 
        {
            checkBreakPoints();

            if (isBreak)
            {
                userControl();
            }
            else
            {
			    core->getCPU()->step(false);
                core->getCDROM()->step(1);
            }
		}
		core->getTimer()->hblank();
	}
	core->getIO()->IRQ(IO_Controller::VBLANK);
	core->getGPU()->status.evenOrOdd ^= 1;
}

void Debugger::userControl()
{
    std::string response;
    std::cin >> response;

    if ("c" == response)
    {
        isBreak = false;
        core->getCPU()->step(false);
        return;
    }
    if ("s" == response)
    {
        core->getCPU()->step(false);
        printf("PC:%x\n", core->getCPU()->pc);
    }
}

void Debugger::checkBreakPoints()
{
    //Trigger Break Points
    for (const auto& x : breakPoints) 
    {
        if (x.first == core->getCPU()->pc) 
        {   
            printf("Break Point hit: %x message: %s\n", x.first, x.second.c_str());
            isBreak = true;
        }
    }
}

void Debugger::addBreakPoint(uw pc, std::string message)
{
    breakPoints.push_back(std::make_pair(pc, message));
}