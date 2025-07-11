#ifndef R3000A_hpp 
#define R3000A_hpp

#include <global.h>

#include <utility>
#include <cpu/gte/gte.hh>

class Core;

/*  5-bit */
#define sa \
    ((code >>  6) & 31)

#define rd \
    ((code >> 11) & 31)

#define rt \
    ((code >> 16) & 31)

#define rs \
    ((code >> 21) & 31)

#define data32 \
    *(uw *)&core->getMem()->hwr.ptr[0x1070]

#define mask32 \
    *(uw *)&core->getMem()->hwr.ptr[0x1074]

#define data16 \
    *(uh *)&core->getMem()->hwr.ptr[0x1070]

#define mask16 \
    *(uh *)&core->getMem()->hwr.ptr[0x1074]



class CstrMips {
    // Co-processor specifics
    enum {
        MFC = 0x00,
        CFC = 0x02,
        MTC = 0x04,
        CTC = 0x06,
        RFE = 0x10,
    };
    
    const uw mask[4][4] = {
        { 0x00ffffff, 0x0000ffff, 0x000000ff, 0x00000000 },
        { 0x00000000, 0xff000000, 0xffff0000, 0xffffff00 },
        { 0xffffff00, 0xffff0000, 0xff000000, 0x00000000 },
        { 0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff },
    };
    
    const ub shift[4][4] = {
        { 0x18, 0x10, 0x08, 0x00 },
        { 0x00, 0x08, 0x10, 0x18 },
        { 0x18, 0x10, 0x08, 0x00 },
        { 0x00, 0x08, 0x10, 0x18 },
    };
    
    union { // Product & quotient (lo, hi)
        sd s64; uw u32[2];
    } res;
    
public:
    uw *instCache, opcodeCount;
    
    void branch(uw);
    
    
    // Co-processor 2 (GTE)
    union pair {
        uw iuw[32]; uh iuh[32*2]; ub iub[32*4];
        sw isw[32]; sh ish[32*2]; sb isb[32*4];
    } cop2c, cop2d;
    
	//Bios name map
	std::map<int, std::string> bios_call_names;

    void executeCop2(uw);
    void   writeCop2(uw);
    void    readCop2(uw);
    
    void opcodeRTPS(ub);
    void opcodeNCCS(ub);
    void opcodeNCS (ub);

    Core* core;

    GTE gte;
    
public:
    uw base[32], copr[16], pc;
    
	CstrMips(Core* _core);

    void init();

	void exception(uw, bool);
    void reset();
    void setpc(uw);
    void bootstrap();
    void run();
	void step(bool);
};

#endif