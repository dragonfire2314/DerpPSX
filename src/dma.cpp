#include <dma.hh>

#include <core.hh>
#include <disk/disk.hh>

DMA::DMA(Core *_core)
{
    core = _core;
}

DMA::~DMA()
{
}

void DMA::writeControl(uw data)
{
	//Set enable flags for channels
	if ((data & 0x8) == 0x8) channels[0].enabled = true;
	else channels[0].enabled = false;
	if ((data & 0x80) == 0x80) channels[1].enabled = true;
	else channels[1].enabled = false;
	if ((data & 0x800) == 0x800) channels[2].enabled = true;
	else channels[2].enabled = false;
	if ((data & 0x8000) == 0x8000) channels[3].enabled = true;
	else channels[3].enabled = false;
	if ((data & 0x80000) == 0x80000) channels[4].enabled = true;
	else channels[4].enabled = false;
	if ((data & 0x800000) == 0x800000) channels[5].enabled = true;
	else channels[5].enabled = false;
	if ((data & 0x8000000) == 0x8000000) channels[6].enabled = true;
	else channels[6].enabled = false;
}

uw DMA::readControl()
{
	return dma_control;
}

void DMA::dmaWrite(uw address, uw data)
{
	//Get channel
	uw channel = ((address & 0xF0) >> 4) - 8;
	switch ((address & 0xF)) {
	case 0x0: //Base address
		dmaBaseAddress(channel, data);
		break;
	case 0x4: //Block control
		dmaBlockControl(channel, data);
		break;
	case 0x8: //Control
		dmaChannelControl(channel, data);
		break;
	}
}

void DMA::dmaChannelControl(ub channel, uw data)
{
	channels[channel].transferDirection = data & 0x01;
	channels[channel].step = (data & 0x02) >> 1;
	channels[channel].mode = (data & 0x600) >> 9;
	channels[channel].start = (data & 0x1000000) >> 24;
	channels[channel].trigger = (data & 0x10000000) >> 28;

	//Check if DMA should start
	if (channels[channel].enabled && channels[channel].start) {
		//std::cout << std::hex << (int)channel << "\n";
		//int c = getchar();;

		//Do DMA
		switch (channel) 
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			dmaGPU();
			break;
		case 3:
			dmaCDROM();
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			dmaOTC();
			break;
		}
	}
}

void DMA::dmaBaseAddress(ub channel, uw data)
{
	channels[channel].baseAddress = data & 0x00FFFFFF;
}

void DMA::dmaBlockControl(ub channel, uw data)
{
	channels[channel].blockAmount = (data & 0xFFFF0000) >> 16;
	channels[channel].blockSize = (data & 0xFFFF);
	channels[channel].numWords = (data & 0xFFFF);
}

uw DMA::dmaRead(uw address)
{
	//Get channel
	uw channel = ((address & 0xF0) >> 4) - 8;
	switch ((address & 0xF)) {
	case 0x0: //Base address
		return channels[channel].baseAddress;
		break;
	case 0x4: //Block control
		switch (channels[channel].mode)
		{
		case 0x00:
			return channels[channel].numWords;
			break;
		case 0x01:
			return ((channels[channel].blockAmount << 16) | channels[channel].blockSize);
			break;
		case 0x02:
			return 0;
			break;
		}
		break;
	case 0x8: //Control
		uw ret = channels[channel].transferDirection |
			(channels[channel].step << 1) |
			(channels[channel].mode << 9) |
			(channels[channel].start << 24) |
			(channels[channel].trigger << 28);
		return ret;
		break;
	}
	return 0;
}


void DMA::dmaGPU() 
{
	//Check tranfer mode
	//Linked list mode
	if (channels[2].mode == 2) 
	{
		uw* p = (uw*)& core->getMem()->ram.ptr[channels[2].baseAddress & (core->getMem()->ram.size - 1)];
		do {
			uw hdr = *(uw*)& core->getMem()->ram.ptr[channels[2].baseAddress & (core->getMem()->ram.size - 1)];
			p = (uw*)& core->getMem()->ram.ptr[(channels[2].baseAddress + 4) & 0x1ffffc];
			if ((hdr >> 24) != 0) {
				core->getGPU()->gpu0Command(p, hdr >> 24);
			}
			channels[2].baseAddress = hdr & 0xffffff;
		} while (channels[2].baseAddress != 0xffffff);
	}

	//Data copy mode
	if (channels[2].mode == 1) 
	{
		if (channels[2].transferDirection == 0)
		{
			int inc = 0;
			if (channels[2].step == 1) inc = (-4);
			else inc = 4;

			uw* p = (uw*)& core->getMem()->ram.ptr[channels[2].baseAddress & 0x1ffffc];

			for (int i = 0; i < channels[2].blockAmount * channels[2].blockSize; i++) 
			{
				p[i] = core->getGPU()->gpuRead();
			}
		}
		else 
		{
			int inc = 0;
			if (channels[2].step == 1) inc = (-4);
			else inc = 4;

			uw* p = (uw*)& core->getMem()->ram.ptr[channels[2].baseAddress & 0x1ffffc];

			for (int i = 0; i < channels[2].blockAmount * channels[2].blockSize; i++) 
			{
				core->getGPU()->gpu0Command(*(p + i));
			}
		}
	}

	//end
	channels[2].start = 0;
	channels[2].trigger = 0;
	//Trigger interupt
	core->getIO()->IRQ(IO_Controller::DMA);

	//int c = getchar();;
}

void DMA::dmaOTC()
{
	//Do a OTC dma

	//For now just say that we did it and flip the start bits to zero
	channels[6].start = 0;
	channels[6].trigger = 0;

	//Trigger interupt
	core->getIO()->IRQ(IO_Controller::DMA);

	//Actuall dmaOTC code
	//Backwards linked list thing?
	//uw address = channels[6].baseAddress;
	//for (uw i = channels[6].numWords; i > 0; i--) 
	//{
	//	core->getMem()->write<uw>(address, (address - 4));

	//	address -= 4;
	//}
	//address += 4;
	//core->getMem()->write<uw>(address, 0xFFFFFF);
	//channels[6].baseAddress = address;


	uw* p = (uw*)& core->getMem()->ram.ptr[channels[6].baseAddress & (core->getMem()->ram.size - 1)];

	while (channels[6].numWords--) {
		*p-- = (channels[6].baseAddress - 4) & 0xffffff;
		channels[6].baseAddress -= 4;
	}
	p++; *p = 0xffffff;
}

void DMA::dmaCDROM()
{
	// printf("CDROM DMA\n");
	// int c = getchar();;

	//Data copy mode
	int inc = 0;
	if (channels[3].step == 1) inc = (-4);
	else inc = 4;

	uw* p = (uw*)&core->getMem()->ram.ptr[channels[3].baseAddress & 0x1ffffc];

	// printf("DMA CDROM: %i\n", channels[3].numWords);
	// int c = getchar();;

	for (uw i = 0; i < channels[3].numWords; i++)
	{
		uw res = 0;

		res |= core->getCDROM()->readByte() << 0;
		res |= core->getCDROM()->readByte() << 8;
		res |= core->getCDROM()->readByte() << 16;
		res |= core->getCDROM()->readByte() << 24;

		// printf("%02x ", (res & 0x000000FF) >> 0);
		// printf("%02x ", (res & 0x0000FF00) >> 8);
		// printf("%02x ", (res & 0x00FF0000) >> 16);
		// printf("%02x ", (res & 0xFF000000) >> 24);

		p[i] = res;
	}

	// printf("\n");

	// int c = getchar();;

	//end
	channels[3].start = 0;
	channels[3].trigger = 0;
	//Trigger interupt
	core->getIO()->IRQ(IO_Controller::DMA);
}
