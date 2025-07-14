#include <io_controller.hh>

#include <gpu.hh>
#include <core.hh>
#include <logging.hh>

void IO_Controller::init()
{

}

uw IO_Controller::read(uw address)
{
	//Catch SPU
	if (address >= 0x1f801c00 && address <= 0x1f801e80) { 
		// std::cout << "[SPU] read: " << std::hex << address << std::endl;
		return 0;
	}

	//Catch DMA
	if (address >= 0x1f801080 && address <= 0x1f8010ef)
	{
		// std::cout << "[DMA] : " << std::hex << address << std::endl;

		// LOG::debug(LOG::SystemLocation::DMA, "read: %x", address);
		return core->getDMA()->dmaRead(address);
	}

	//Catch CDROM
	if (address >= 0x1f801800 && address <= 0x1f801803)
	{
		// std::cout << "[CDROM] read: " << std::hex << address << std::endl;
		return core->getCDROM()->read(address);
		// return cd.read(address);
		//cd.load_PXS_EXE();
		//int c = getchar();;

	}

	//Catch Timer
	if (address >= 0x1f801100 && address <= 0x1f801130)
	{
		return core->getTimer()->read(address);
	}

	switch (address) 
	{
	case 0x1F8010F0: //Read DMA
		// std::cout << "[DMA] control read: " << std::hex << address << std::endl;

		// LOG::debug(LOG::SystemLocation::DMA, "control read: %x", address);
		return core->getDMA()->readControl();
		break;
	case 0x1F801070: //Interupt status
		// std::cout << "\033[31m" << "[INT] stat read: " << std::hex << address << "\033[0m" << std::endl;
		// printf("[INT] stat read:%x, value:%x\n", address, i_stat);
		return i_stat;
	case 0x1F801074: //Interupt mask
		// std::cout << "\033[31m" << "[INT] mask read: " << std::hex << address << "\033[0m" << std::endl;
		// printf("[INT] mask read:%x, value:%x\n", address, i_mask);
		return i_mask;
	case 0x1F801DAE: //SPU Status Register
		// std::cout << "[SPU] stat read: " << std::hex << address << std::endl;
		return 0;
		break;
	case 0x1F801814: //GPU Status Register
		// std::cout << "[GPU] stat read: " << std::hex << address << std::endl;
		// std::cout << "GPU Status read: " << std::hex << *(cpu.instCache) << std::endl;
		// LOG::debug(LOG::SystemLocation::GPU, "stat read: %x", address);
		return core->getGPU()->gpuReadStat();
		return 0;
		break;
	case 0x1F801810: //GPU READ
		// std::cout << "[GP0] read: " << std::hex << address << std::endl;
		return core->getGPU()->gpuRead();
		return 0;
		break;
	default:
		//std::cout << "[HARDWARE] read: " << std::hex << address << std::endl;
		return 0;
	}

	return uw();
}

void IO_Controller::write(uw data, uw address)
{
	//Catch SPU
	if (address >= 0x1f801c00 && address <= 0x1f801e80) 
	{
		// std::cout << "[SPU] write: " << std::hex << address << " data: " << data << std::endl;
	}
	
	// std::cout << "IO write: " << std::hex << address << " data: " << data << std::endl;

	//Catch DMA
	if (address >= 0x1f801080 && address <= 0x1f8010ef) 
	{
		// std::cout << "[DMA] write: " << std::hex << address << " data: " << data << std::endl;
		LOG::debug(LOG::SystemLocation::DMA, "write: %x, data: %x", address, data);
		core->getDMA()->dmaWrite(address, data);
		return;
	}

	//Catch CDROM
	if (address >= 0x1f801800 && address <= 0x1f801803)
	{
		// std::cout << "[CDROM] write: " << std::hex << address << " data: " << data << std::endl;
		core->getCDROM()->write(address, data);
		//cpu.pc = 0xbfc069a0;
		// cd.write(address, data);
		//int c = getchar();;
	}

	//Catch Timer
	if (address >= 0x1f801100 && address <= 0x1f801130)
	{
		core->getTimer()->write(address, data);
	}

	switch (address) 
	{
	case 0x1F801070: //Interupt status
		// std::cout << "\033[31m" << "[INT] stat write: " << std::hex << address << " data: " << data << "\033[0m" << std::endl;

		//Writing a 1 does nothing, 0 is awk. So a 1 is ignoroed. Otherwise wirtting a 1 would cause an intrupt
		// printf("i_stat pre:%x\n", i_stat);
		i_stat = (i_stat & data);
		// printf("i_stat post:%x\n", i_stat);
		break;
	case 0x1F801074: //Interupt mask
		// std::cout << "\033[31m" << "[INT] mask write: " << std::hex << address << " data: " << data << "\033[0m" << std::endl;
		i_mask = data;
		break;
	case 0x1F8010F0: //Write DMA
		// std::cout << "[DMA] write control: " << std::hex << address << " data: " << data << std::endl;
		// LOG::debug(LOG::SystemLocation::DMA, "write control: %x, data: %x", address, data);
		core->getDMA()->writeControl(data);
		break;
	case 0x1f801810: // GPU Send GP0 command
		// std::cout << "[GPO] send commande: " << std::hex  << " data: " << data << std::endl;
		// LOG::debug(LOG::SystemLocation::GPU, "GP0: send command: data: %x", data);
		core->getGPU()->gpu0Command(data);
		break;
	case 0x1f801814: // GPU Send GP1 command
		// std::cout << "[GP1] send commande: " << std::hex << " data: " << data << std::endl;
		// LOG::debug(LOG::SystemLocation::GPU, "GP1: send command: data: %x", data);
		core->getGPU()->gpu1Command(data);
		break;
	case 0x1F801020: //COM_DELAY
		// std::cout << "[COM DELAY] set: " << data << std::endl;
		break;
	default:
		//std::cout << "[Hardware] write: " << std::hex << address << " data: " << data << std::endl;
		break;
	}

	//Catch Trace
	if (address == 0x1F802041) {
		printf("[Trace] %x\n", data);
		// printf("[Trace] PC: %x\n", core->getCPU()->pc);
		LOG::debug(LOG::SystemLocation::APP, "[Trace] %x", data);
		// int c = getchar();;
	}
}

void IO_Controller::IRQ(IRQ_TYPE type)
{
	if ((i_mask & type) == type) {
		// printf("\033[1;36m%s%x\033[0m\n", "IRQ WAS CALLED: ", type);
		i_stat = i_stat | type;
		core->getCPU()->cause.reg = core->getCPU()->cause.reg | 0x400;
	}
}
