#ifndef TIMER_HPP
#define TIMER_HPP

#include <global.hh>

class Core;

class Timer
{
private:
	union MODE
    {
        struct
        {
            ub sync_enbled : 1;
			ub sync_mode : 2;
			ub reset_counter : 1;
			ub IRQ_when_target : 1;
			ub IRQ_when_FFFF : 1;
			ub IRQ_repeat : 1;
			ub IRQ_toggle : 1;
			ub clock_source : 2;
			ub interrupt_request : 1;
			ub reached_target : 1;
			ub reached_FFFF : 1;
			ub unknown : 2;
			uw garbage : 17;
        };
        uw reg;
    };

	union HIGH_LOW
    {
        struct
        {
            uh low;
            uh high;
        };
        uw reg;
    };

	HIGH_LOW counter_value[3];
	MODE counter_mode[3];
	HIGH_LOW counter_target[3];

    Core* core;
public:
    Timer(Core* _core);

	uw read(uw address);
	void write(uw address, uw data);

	void hblank();
	void dotclock();
	void systemclock();

	void tick();
};


#endif