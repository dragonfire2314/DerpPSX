#include <timer.h>

#include <core.h>

Timer::Timer(Core* _core)
{
    core = _core;
}

uw Timer::read(uw address)
{
	ub timer_number = (address & (0xF0)) / 16;

	switch (address & (0x0F))
	{
	case 0x0:
		//printf("\033[1;33mRead to timer%x Counter Value: %x\033[0m\n", timer_number, counter_value[timer_number]);
		return counter_value[timer_number];
		break;
	case 0x4:
		//printf("\033[1;33mRead to timer%x Counter Mode: %x\033[0m\n", timer_number, counter_mode[timer_number]);
		return counter_mode[timer_number];
		break;
	case 0x8:
		//printf("\033[1;33mRead to timer%x Counter Target: %x\033[0m\n", timer_number, counter_target[timer_number]);
		return counter_target[timer_number];
		break;
	default:
		printf("\033[1;31m%s\033[0m\n", "Read to bad timer address.");
		break;
	}

	return uw();
}

void Timer::write(uw address, uw data)
{
	ub timer_number = (address & (0xF0)) / 16;

	switch (address & (0x0F)) 
	{
	case 0x0:
		//printf("\033[1;33mWrite to timer%x Counter Value: %x\033[0m\n", timer_number, data);

		counter_value[timer_number] = data;
		break;
	case 0x4:
		//printf("\033[1;33mWrite to timer%x Counter Mode: %x\033[0m\n", timer_number, data);

		counter_mode[timer_number] = data;
		break;
	case 0x8:
		//printf("\033[1;33mWrite to timer%x Counter Target: %x\033[0m\n", timer_number, data);

		counter_target[timer_number] = data;
		break;
	default:
		printf("\033[1;31m%s\033[0m\n", "Write to bad timer address.");
		break;
	}
}

void Timer::hblank()
{
	//Check if timer 1 needs to incrament on h-blank
	if ((counter_mode[1] & 0x300) == 0x300 || (counter_mode[1] & 0x300) == 0x100) { //XOR to speed up? maybe?
		counter_value[1]++;
		//Check if it triggers IRQ
		if ((counter_mode[1] & 0x10) == 0x10) //Triggers on target
			if (counter_value[1] == counter_target[1]) { //Trigger the IRQ
				core->getIO()->IRQ(IO_Controller::TMR1);
				counter_value[1] = 0;
			}
		if ((counter_mode[1] & 0x10) == 0x20) //Triggers on max value
			if (counter_value[1] == 0xFFFF) { //Trigger the IRQ
				core->getIO()->IRQ(IO_Controller::TMR1);
				counter_value[1] = 0;
			}
		if (counter_value[1] == 0xFFFF) {
			counter_value[1] = 0;
		}
	}

	//Stuff needs to be done with sync mode later
}

void Timer::dotclock()
{
}

void Timer::systemclock()
{
}