#ifndef TIMER_HPP
#define TIMER_HPP

#include <global.h>

class Core;

class Timer
{
private:
	uw counter_value[3];
	uw counter_mode[3];
	uw counter_target[3];

    Core* core;
public:
    Timer(Core* _core);

	uw read(uw address);
	void write(uw address, uw data);

	void hblank();
	void dotclock();
	void systemclock();
};


#endif