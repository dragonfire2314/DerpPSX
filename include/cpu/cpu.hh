#ifndef CPU_hpp 
#define CPU_hpp

#include <global.hh>

#include <utility>
#include <cpu/gte/gte.hh>

class Core;

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

/* 16-bit */
#define imm \
    ((sh)code)

#define immu \
    (code & 0xffff)

/* 32-bit */
#define ob \
    (base[rs] + imm)

#define baddr \
    (pc + (imm << 2))

#define saddr \
    ((code & 0x3ffffff) << 2) | (pc & 0xf0000000)

#define opcodeSWx(o, d) \
    core->getMem()->write<uw>(ob & ~3, (base[rt] o shift[d][ob & 3]) | (core->getMem()->read<uw>(ob & ~3) & mask[d][ob & 3]))

#define opcodeLWx(o, d) \
    base[rt] = (base[rt] & mask[d][ob & 3]) | (core->getMem()->read<uw>(ob & ~3) o shift[d][ob & 3])

class CPU {

    union {
        sd s64; 
        uw u32[2];
    } res;
    
private:
    void printTTY();

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

public:
    uw *instCache, opcodeCount;
    
    void branch(uw addr);
    
    // Co-processor 2 (GTE)
    union pair {
        uw iuw[32]; uh iuh[32*2]; ub iub[32*4];
        sw isw[32]; sh ish[32*2]; sb isb[32*4];
    } cop2c, cop2d;

    enum {
        MFC = 0x00,
        CFC = 0x02,
        MTC = 0x04,
        CTC = 0x06,
        RFE = 0x10,
    };

    enum EXECPTION {
        INT = 0x00,
        MOD = 0x01,
        TLBL = 0x02,
        TLBS = 0x03,
        ADRESS_ERROR_LOAD = 0x04,
        ADDRES_ERROR_STORE = 0x05,
        BUS_ERROR_FETCH = 0x06,
        BUS_ERROR_LOAD_STORE = 0x07,
        SYSCALL = 0x08,
        BREAKPOINT = 0x09,
        RI = 0x0A,
        COP_UNUSABLE = 0x0B,
        ARITHMETIC_OVERFLOW = 0x0C,
    };

    union EXECPTIONS_CAUSE
    {
        struct
        {
            ub zero : 2;
            ub excode : 5;
            ub not_used : 1;
            ub interrupt : 8;
            uh not_used_2 : 12;
            ub cop_numer : 2;
            ub not_used_3 : 1;
            ub branch_delay : 1;
        };
        uw reg;
    };

    union EXECPTION_STATUS {
        enum class Mode : uint32_t { kernel = 0, user = 1 };
        enum class BootExceptionVectors { ram = 0, rom = 1 };
        struct {
            uint32_t interruptEnable : 1;
            Mode mode : 1;

            uint32_t previousInterruptEnable : 1;
            Mode previousMode : 1;

            uint32_t oldInterruptEnable : 1;
            Mode oldMode : 1;

            uint32_t : 2;

            uint32_t interruptMask : 8;
            uint32_t isolateCache : 1;
            uint32_t swappedCache : 1;
            uint32_t writeZeroAsParityBits : 1;
            uint32_t : 1;  // CM
            uint32_t cacheParityError : 1;
            uint32_t tlbShutdown : 1;  // TS

            BootExceptionVectors bootExceptionVectors : 1;
            uint32_t : 2;
            uint32_t reverseEndianness : 1;
            uint32_t : 2;

            uint32_t cop0Enable : 1;
            uint32_t cop1Enable : 1;
            uint32_t cop2Enable : 1;
            uint32_t cop3Enable : 1;
        };

        void enterException() {
            oldInterruptEnable = previousInterruptEnable;
            oldMode = previousMode;

            previousInterruptEnable = interruptEnable;
            previousMode = mode;

            interruptEnable = false;
            mode = EXECPTION_STATUS::Mode::kernel;
        };

        uint32_t reg;
        EXECPTION_STATUS() : reg(0) {}
    };

    EXECPTIONS_CAUSE cause;
    EXECPTION_STATUS status;

    struct DELAY_SLOT {
    enum TYPE {
            VALID,
            EMPTY
        };
        uw data;
        ub reg;
        TYPE type;
    };

    #define EMPTY_SLOT {0,0,DELAY_SLOT::TYPE::EMPTY}

    DELAY_SLOT slots[2];

    void moveDelaySlots();
    void pushDelaySlot(uw _data, ub reg);

    Core* core;

    GTE gte;
    
public:
    uw base[32], copr[16], pc;

    uw execption_pc;

    void setReg(ub reg, uw data);
    
    bool isDelayInstruction = false;
    ub delayedInstruction;
    ub delayRegister;
    uw delayAddress;
    
    
	CPU(Core* _core);

    void init();

	void exception(EXECPTION e, bool branched);
    void reset();
    void setpc(uw addr);
    void bootstrap();
	void step(bool branched);

private:
    uw code;

    void OP_UNUSED();

    inline void SPECIAL();
    inline void OP_BcondZ();
    inline void OP_J();
    inline void OP_JAL();
    inline void OP_BEQ();
    inline void OP_BNE();
    inline void OP_BLEZ();
    inline void OP_BGTZ();
    inline void OP_ADDI();
    inline void OP_ADDIU();
    inline void OP_SLTI();
    inline void OP_SLTIU();
    inline void OP_ANDI();
    inline void OP_ORI();
    inline void OP_XORI();
    inline void OP_LUI();
    inline void OP_COP0();
    inline void OP_COP1();
    inline void OP_COP2();
    inline void OP_COP3();
    inline void OP_LB();
    inline void OP_LH();
    inline void OP_LWL();
    inline void OP_LW();
    inline void OP_LBU();
    inline void OP_LHU();
    inline void OP_LWR();
    inline void OP_SB();
    inline void OP_SH();
    inline void OP_SWL();
    inline void OP_SW();
    inline void OP_SWR();
    inline void OP_LWC0();
    inline void OP_LWC1();
    inline void OP_LWC2();
    inline void OP_LWC3();
    inline void OP_SWC0();
    inline void OP_SWC1();
    inline void OP_SWC2();
    inline void OP_SWC3();


    inline void OP_SLL();
    inline void OP_SRL();
    inline void OP_SRA();
    inline void OP_SLLV();
    inline void OP_SRLV();
    inline void OP_SRAV();
    inline void OP_JR();
    inline void OP_JALR();
    inline void OP_SYSCALL(bool branched);
    inline void OP_BREAK();
    inline void OP_MFHI();
    inline void OP_MTHI();
    inline void OP_MFLO();
    inline void OP_MTLO();
    inline void OP_MULT();
    inline void OP_MULTU();
    inline void OP_DIV();
    inline void OP_DIVU();
    inline void OP_ADD();
    inline void OP_ADDU();
    inline void OP_SUB();
    inline void OP_SUBU();
    inline void OP_AND();
    inline void OP_OR();
    inline void OP_XOR();
    inline void OP_NOR();
    inline void OP_SLT();
    inline void OP_SLTU();
};

#endif