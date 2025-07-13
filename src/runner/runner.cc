#include <runner/runner.hh>

#include <core.hh>
#include <logging.hh>

Runner::Runner(Core* _core)
{
    core = _core;
}

void Runner::step()
{
    //CPU
	//Get resoluition to adjust timings
	int horizontalRes = core->getGPU()->get_horizontal_timings();
	// int horizontalRes = 368;
	//CPU
	for (int t = 0; t < horizontalRes; t++) {
		for (int i = 0; i < 564480 / horizontalRes; i++) {
			core->getCPU()->step(false);
		}
		core->getTimer()->hblank();
		core->getCDROM()->step(564480);
	}
	core->getIO()->IRQ(IO_Controller::VBLANK);
	core->getGPU()->status.evenOrOdd ^= 1;
}

void Runner::emu_init()
{
    //Load bios
	// std::ifstream Openfile("J:/Downloads/ps1 roms/psxtest_gte", std::ios::binary | std::ios::in);
	std::ifstream Openfile(core->getConfiguration()->getAppConfiguration().biosLocation, std::ios::binary | std::ios::in);
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
}