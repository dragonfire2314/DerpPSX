#include <cpu/cpu.hh>
#include <core.hh>

#define opcode \
    ((code >> 26) & 63)

CPU::CPU(Core* _core) : core(_core)
{}

void CPU::init()
{

}

void CPU::reset()
{
    memset(&base, 0, sizeof(base));
    memset(&copr, 0, sizeof(copr));
    cop2c = { 0 };
    cop2d = { 0 };
    
    copr[12] = 0x10900000;
    copr[15] = 0x2; // Co-processor Revision
    
    setpc(0xbfc00000);
    res.s64 = opcodeCount = 0;

	//Temp TODO - What does this do???
	base[29] = 0x500;
}

void CPU::setpc(uw addr)
{
    pc = addr;
    
    if ((pc >> 20) == 0xbfc) {
        instCache = (uw *)&core->getMem()->rom.ptr[addr & 0x000fffff];
    }
    else {
        instCache = (uw *)&core->getMem()->ram.ptr[addr & 0x00ffffff];
    }
}

void CPU::printTTY()
{
    if ((0xA0 == (pc & 0x1FFFFFFF) && base[9] == 0x3C) || (0xB0 == (pc & 0x1FFFFFFF) && base[9] == 0x3D))
    {
        // printf("\033[1;34m%c\033[0m", (char)base[4]);
        printf("%c", (char)base[4]);
    }
}

void CPU::bootstrap()
{
    if (pc == 0x80030000) 
    {
        std::ifstream exeFile("tests/psxtest_cpu.exe", std::ios::binary | std::ios::in);
        uint32_t size;
        if (!exeFile) 
        {
            std::cerr << "Error: " << strerror(errno);
            fprintf(stderr, "Cant open EXE???\n");
            exit(1);
        }

        // Get length of file
        exeFile.seekg(0, std::ios::end);
        size = exeFile.tellg();
        exeFile.seekg(0, std::ios::beg);

        // core->getMem()->ram.ptr;
        uint8_t* exe_data = new uint8_t[size];
        exeFile.read((char*)exe_data, size);

        struct PSX_EXE_HEADER
        {
            uint8_t id[8];
            uint8_t zeros[8];
            uint32_t initalPC;
            uint32_t initalR28;
            uint32_t exeAddress;
            uint32_t exeSize;
            uint32_t dataStartAddress;
            uint32_t dataSectionSize;
            uint32_t bssStartAddress;
            uint32_t bssSectionSize;
            uint32_t initalR29_30;
            // TODO - Finish header
        };

        PSX_EXE_HEADER* exeHeader;

        exeHeader = (PSX_EXE_HEADER*)exe_data;

        fprintf(stderr, "PC: %x\n", exeHeader->initalPC);
        fprintf(stderr, "28: %x\n", exeHeader->initalR28);
        fprintf(stderr, "address: %x\n", exeHeader->exeAddress);
        fprintf(stderr, "size: %x\n", exeHeader->exeSize);
        fprintf(stderr, "29_30: %x\n", exeHeader->initalR29_30);
        fprintf(stderr, "size: %x\n", size);

        int exeSizeInBytes = exeHeader->exeSize;

        // core->getMem()->ram.ptr
        memcpy(core->getMem()->ram.ptr + (exeHeader->exeAddress & 0x1FFFFF), exe_data + 0x800, exeSizeInBytes - 2048);
        
        setpc(exeHeader->initalPC);
        base[28] = exeHeader->initalR28; 
        base[29] = exeHeader->initalR29_30; 
        base[30] = exeHeader->initalR29_30; 
    }
}

void CPU::step(bool branched)
{
    printTTY();

    //Handle IRQs
    if (branched == false) {
       if ((core->getIO()->i_stat & core->getIO()->i_mask) > 0) {
           if (((copr[12] & 0x400) & (copr[13] & 0x400)) && ((copr[12] & 0x1) == 0x1)) 
           {
            //    printf("\033[1;36m%s: %x\033[0m\n", "IRQ WAS FIRED", (core->getIO()->i_stat & core->getIO()->i_mask));
               exception(0x400, false);
           }
       }
    }

    // TODO - make this optional based on if the user loads a PSX-EXE
    // bootstrap();

    uw code = *instCache++; pc += 4;
    base[0] = 0;
    opcodeCount++;

    int32_t a;
    int32_t b;

switch(opcode) {
        case 0: // SPECIAL
            switch(code & 63) {
                case 0: // SLL
                    if (code) { // No operation?
                        base[rd] = base[rt] << sa;
                    }
                    break;
                    
                case 2: // SRL
                    base[rd] = base[rt] >> sa;
                    break;
                    
                case 3: // SRA
                    base[rd] = (sw)base[rt] >> sa;
                    break;
                    
                case 4: // SLLV
                    base[rd] = base[rt] << (base[rs] & 31);
                    break;
                    
                case 6: // SRLV
                    base[rd] = base[rt] >> (base[rs] & 31);
                    break;
                    
                case 7: // SRAV
                    base[rd] = (sw)base[rt] >> (base[rs] & 31);
                    break;
                    
                case 8: // JR
                    if (base[rs] % 4 != 0)
                    {
                        exception(0x10, branched);
                        break;
                    }
                    branch(base[rs]);
                    break;
                    
                case 9: // JALR
                    if (base[rs] % 4 != 0)
                    {
                        exception(0x10, branched);
                        break;
                    }
                    base[rd] = pc + 4;
                    branch(base[rs]);
                    break;
                    
                case 12: // SYSCALL
                    pc -= 4;
					//std::cout << "Syscall" << std::endl;
                    exception(0x20, branched);
                    break;
                    
                case 13: // BREAK
                    break;
                    
                case 16: // MFHI
                    base[rd] = res.u32[1];
                    break;
                    
                case 17: // MTHI
                    res.u32[1] = base[rs];
                    break;
                    
                case 18: // MFLO
                    base[rd] = res.u32[0];
                    break;
                    
                case 19: // MTLO
                    res.u32[0] = base[rs];
                    break;
                    
                case 24: // MULT
                    res.s64 = (sd)(sw)base[rs] * (sw)base[rt];
                    break;
                    
                case 25: // MULTU
                    res.s64 = (sd)base[rs] * base[rt];
                    break;
                    
                case 26: // DIV
                    if (0 == base[rt]) {
                        res.u32[0] = ((sw)base[rs] < 0) ? 0x00000001 : 0xffffffff;
                        res.u32[1] = rs;
                    }
                    else if((uw)base[rs] == 0x80000000 && (uw)base[rt] == 0xffffffff) {
                        res.u32[0] = 0x80000000;
                        res.u32[1] = 0x00000000;
                    }
                    else {
                        res.u32[0] = (sw)base[rs] / (sw)base[rt];
                        res.u32[1] = (sw)base[rs] % (sw)base[rt];
                    }
                    break;
                    
                case 27: // DIVU
                    if (base[rt]) {
                        res.u32[0] = base[rs] / base[rt];
                        res.u32[1] = base[rs] % base[rt];
                    }
                    break;
                    
                case 32: // ADD
                    a = base[rs];
                    b = base[rt];
                    if ((!((a ^ b) & 0x80000000) && (((a+b)) ^ a) & 0x80000000)) { [[unlikely]]	
                        exception(0x30, branched);
                        break;
                    }
                    if (((int64_t)a + (int64_t)b) > INT32_MAX) 
                    {
                         exception(0x60, branched);
                         break;
                    }
                    base[rd] = base[rs] + base[rt];
                    break;
                    
                case 33: // ADDU
                    base[rd] = base[rs] + base[rt];
                    break;
                    
                case 34: // SUB
                    a = base[rs];
                    b = base[rt];
                    if (((a ^ b) & 0x80000000) && ((a - b) ^ a) & 0x80000000) { [[unlikely]]	
                        exception(0x30, branched);
                        break;
                    }
                    base[rd] = base[rs] - base[rt];
                    break;
                    
                case 35: // SUBU
                    base[rd] = base[rs] - base[rt];
                    break;
                    
                case 36: // AND
                    base[rd] = base[rs] & base[rt];
                    break;
                    
                case 37: // OR
                    base[rd] = base[rs] | base[rt];
                    break;
                    
                case 38: // XOR
                    base[rd] = base[rs] ^ base[rt];
                    break;
                    
                case 39: // NOR
                    base[rd] = ~(base[rs] | base[rt]);
                    break;
                    
                case 42: // SLT
                    base[rd] = (sw)base[rs] < (sw)base[rt];
                    break;
                    
                case 43: // SLTU
                    base[rd] = base[rs] < base[rt];
                    break;
                    
                default:
                    printx("/// PSeudo 0x%08x | Unknown special opcode 0x%08x | %d", pc, code, (code & 63));
                    break;
            }
            break;
            
        case 1: // REGIMM
            if (0x11 == rt)
            {
                base[31] = pc + 4;
                    
                if ((sw)base[rs] >= 0) {
                    branch(baddr);
                }
                break;
            }
            if (0x10 == rt)
            {
                base[31] = pc + 4;

                if ((sw)base[rs] < 0) {
                    branch(baddr);
                }
                break;
            }
            if ((rt & 0x1) == 0x1) 
            {
                if ((sw)base[rs] >= 0) {
                        branch(baddr);
                }
                break;
            }
            else 
            {
                if ((sw)base[rs] < 0) {
                    branch(baddr);
                }
                break;
            }

            
        case 2: // J
            branch(saddr);
            break;
            
        case 3: // JAL
            base[31] = pc + 4;
            branch(saddr);
            break;
            
        case 4: // BEQ
            if (base[rs] == base[rt]) {
                branch(baddr);
            }
            break;
            
        case 5: // BNE
            if (base[rs] != base[rt]) {
                branch(baddr);
            }
            break;
            
        case 6: // BLEZ
            if ((sw)base[rs] <= 0) {
                branch(baddr);
            }
            break;
            
        case 7: // BGTZ
            if ((sw)base[rs] > 0) {
                branch(baddr);
            }
            break;
            
        case 8: // ADDI
            a = base[rs];
            b = imm;
            if ((!((a ^ b) & 0x80000000) && (((a+b)) ^ a) & 0x80000000)) { [[unlikely]]	
                exception(0x30, branched);
                break;
            }
            base[rt] = base[rs] + imm;
            break;
            
        case 9: // ADDIU
            base[rt] = base[rs] + imm;
            break;
            
        case 10: // SLTI
            base[rt] = (sw)base[rs] < imm;
            break;
            
        case 11: // SLTIU
            base[rt] = base[rs] < immu;
            break;
            
        case 12: // ANDI
            base[rt] = base[rs] & immu;
            break;
            
        case 13: // ORI
            base[rt] = base[rs] | immu;
            break;
            
        case 14: // XORI
            base[rt] = base[rs] ^ immu;
            break;
            
        case 15: // LUI
            base[rt] = code << 16;
            break;
            
        case 16: // COP0
			//std::cout << "cop0 something" << std::endl;
            switch(rs) {
                case MFC:
                    base[rt] = copr[rd];
					break;

                case MTC:
                    copr[rd] = base[rt];
                    break;
                    
                case RFE: // Return from exception
                    copr[12] = (copr[12] & ~(0xf)) | ((copr[12] >> 2) & 0xf);
                    break;
                    
                default:
                    printx("/// PSeudo 0x%08x | Unknown cop0 opcode 0x%08x | %d", pc, code, rs);
                    break;
            }
            break;
            
        case 18: // COP2
            switch(rs) {
                case MFC:
                    //base[rt] = cop2d.iuw[rd];
                    base[rt] = gte.read(rd);
                    break;
                    
                case CFC:
                    // base[rt] = cop2c.iuw[rd];
                    base[rt] = gte.read(rd+32);
                    break;
                    
                case MTC:
                    // cop2d.iuw[rd] = base[rt];
                    // writeCop2(rd);
                    gte.write(rd, base[rt]);
                    break;
                    
                case CTC:
                    //cop2c.iuw[rd] = base[rt];
                    gte.write(rd+32, base[rt]);
                    break;
                    
                default: // Execute GTE opcode
                    gte.execCommand(code);//Is this right?
                    break;
            }
            break;
            
        case 32: // LB
            base[rt] = (sb)core->getMem()->read<ub>(ob);
            break;
            
        case 33: // LH
            base[rt] = (sh)core->getMem()->read<uh>(ob);
            break;
            
        case 34: // LWL
            opcodeLWx(<<, 0);
            break;
            
        case 35: // LW
            base[rt] = core->getMem()->read<uw>(ob);
            break;
            
        case 36: // LBU
            base[rt] = core->getMem()->read<ub>(ob);
            break;
            
        case 37: // LHU
            base[rt] = core->getMem()->read<uh>(ob);
            break;
            
        case 38: // LWR
            opcodeLWx(>>, 1);
            break;
            
        case 40: // SB
            core->getMem()->write<ub>(ob, base[rt]);
            break;
            
        case 41: // SH
            core->getMem()->write<uh>(ob, base[rt]);
            break;
            
        case 42: // SWL
            opcodeSWx(>>, 2);
            break;
            
        case 43: // SW
            core->getMem()->write<uw>(ob, base[rt]);
            break;
            
        case 46: // SWR
            opcodeSWx(<<, 3);
            break;
            
        case 50: // LWC2
            //cop2d.iuw[rt] = core->getMem()->read<uw>(ob);
            //writeCop2(rt);
            gte.write(rt, core->getMem()->read<uw>(ob));
			//std::cout << "cop something" << std::endl;
            break;
            
        case 58: // SWC2
            //readCop2(rt);
            // core->getMem()->write<uw>(ob, cop2d.iuw[rt]);
            core->getMem()->write<uw>(ob, gte.read(rt));
			//std::cout << "cop something" << std::endl;
            break;
            
        default:
            printx("/// PSeudo 0x%08x | Unknown basic opcode 0x%08x | %d", pc, code, opcode);
            break;
    }

    // switch (opcode)
    // {
    // case 0x00: // Special
    //     switch (code & 63) 
    //     {
    //     case 0x00: OP_SLL(); break;    
    //     case 0x02: OP_SRL(); break;    
    //     case 0x03: OP_SRA(); break;    
    //     case 0x04: OP_SLLV(); break;   
    //     case 0x06: OP_SRLV(); break;   
    //     case 0x07: OP_SRAV(); break;   
    //     case 0x08: OP_JR(); break;     
    //     case 0x09: OP_JALR(); break;    
    //     case 0x0C: OP_SYSCALL(branched); break;
    //     case 0x0D: OP_BREAK(); break;  
    //     case 0x10: OP_MFHI(); break; 
    //     case 0x11: OP_MTHI(); break; 
    //     case 0x12: OP_MFLO(); break; 
    //     case 0x13: OP_MTLO(); break; 
    //     case 0x18: OP_MULT(); break;  
    //     case 0x19: OP_MULTU(); break; 
    //     case 0x1A: OP_DIV(); break;   
    //     case 0x1B: OP_DIVU(); break;  
    //     case 0x20: OP_ADD(); break;  
    //     case 0x21: OP_ADDU(); break; 
    //     case 0x22: OP_SUB(); break;  
    //     case 0x23: OP_SUBU(); break; 
    //     case 0x24: OP_AND(); break;  
    //     case 0x25: OP_OR(); break;   
    //     case 0x26: OP_XOR(); break;  
    //     case 0x27: OP_NOR(); break;  
    //     case 0x2A: OP_SLT(); break;  
    //     case 0x2B: OP_SLTU(); break; 
    //     default:
    //         fprintf(stderr, "Unknown special opcode: 0x%x\n", code);
    //         exit(1);
    //     break;
    //     }
    //     break;
    // case 0x01: OP_BcondZ(); break;  
    // case 0x02: OP_J(); break;       
    // case 0x03: OP_JAL(); break;     
    // case 0x04: OP_BEQ(); break;     
    // case 0x05: OP_BNE(); break;     
    // case 0x06: OP_BLEZ(); break;    
    // case 0x07: OP_BGTZ(); break;    
    // case 0x08: OP_ADDI(); break;  
    // case 0x09: OP_ADDIU(); break; 
    // case 0x0A: OP_SLTI(); break;  
    // case 0x0B: OP_SLTIU(); break; 
    // case 0x0C: OP_ANDI(); break;  
    // case 0x0D: OP_ORI(); break;   
    // case 0x0E: OP_XORI(); break;  
    // case 0x0F: OP_LUI(); break;   
    // case 0x10: OP_COP0(); break; 
    // case 0x11: OP_COP1(); break; 
    // case 0x12: OP_COP2(); break; 
    // case 0x13: OP_COP3(); break;  
    // case 0x20: OP_LB(); break;  
    // case 0x21: OP_LH(); break;  
    // case 0x22: OP_LWL(); break; 
    // case 0x23: OP_LW(); break;  
    // case 0x24: OP_LBU(); break; 
    // case 0x25: OP_LHU(); break; 
    // case 0x26: OP_LWR(); break; 
    // case 0x28: OP_SB(); break; 
    // case 0x29: OP_SH(); break; 
    // case 0x2A: OP_SWL(); break;
    // case 0x2B: OP_SW(); break; 
    // case 0x2E: OP_SWR(); break;
    // case 0x30: OP_LWC0(); break; 
    // case 0x31: OP_LWC1(); break; 
    // case 0x32: OP_LWC2(); break; 
    // case 0x33: OP_LWC3(); break; 
    // case 0x38: OP_SWC0(); break; 
    // case 0x39: OP_SWC1(); break; 
    // case 0x3A: OP_SWC2(); break; 
    // case 0x3B: OP_SWC3(); break; 
    // default:
    //     fprintf(stderr, "Unknown special opcode: 0x%x\n", code);
    //     exit(1);
    // break;
    // }
}

// TODO - Redo branch code, this count2 scares me, idk what it does
int count2 = 0; // Temp hack ????
void CPU::branch(uw addr)
{
    // Execute instruction in slot
    step (true);
    setpc(addr);
    
    if ((++count2 % 4) == 0) { count2 = 0;
        // Exceptions
        if (data32 & mask32) {
            if ((copr[12] & 0x401) == 0x401) {
                exception(0x400, false);
            }
        }
    }
}

//Probably works
void CPU::exception(uw code, bool branched)
{
    if (branched) {
        printx("  Exception %s", "branched");
    }

    copr[12] = (copr[12] & ~(0x3f)) | ((copr[12] << 2) & 0x3f);
    copr[13] = code;
    copr[14] = pc;
    
    if ((copr[12] & 0x40000) == 0x40000)
        setpc(0xbfc00180);
    else 
        setpc(0x80000080);
}


inline void CPU::OP_UNUSED() { fprintf(stderr, "ILLEGAL INSTRUCTION\n"); }

inline void CPU::OP_BcondZ() { 
    switch(rt) {
    case 0: // BLTZ
        if ((sw)base[rs] < 0) {
            branch(baddr);
        }
        break;
        
    case 1: // BGEZ
        if ((sw)base[rs] >= 0) {
            branch(baddr);
        }
        break;
        
    case 17: // BGEZAL
        base[31] = pc + 4;
        
        if ((sw)base[rs] >= 0) {
            branch(baddr);
        }
        break;
        
    default:
        printf("  0x%08x | Unknown bcond opcode 0x%08x | %d", pc, code, rt);
        break;
    }
}
inline void CPU::OP_J() { branch(saddr); }
inline void CPU::OP_JAL() { base[31] = pc + 4; branch(saddr); }
inline void CPU::OP_BEQ() { 
    if (base[rs] == base[rt]) {
        branch(baddr);
    }
}
inline void CPU::OP_BNE() { 
    if (base[rs] != base[rt]) {
        branch(baddr);
    }
}
inline void CPU::OP_BLEZ() { 
    if ((sw)base[rs] <= 0) {
        branch(baddr);
    }
}
inline void CPU::OP_BGTZ() { 
    if ((sw)base[rs] > 0) {
        branch(baddr);
    } 
}
inline void CPU::OP_ADDI() { base[rt] = base[rs] + imm; }
inline void CPU::OP_ADDIU() { base[rt] = base[rs] + imm; }
inline void CPU::OP_SLTI() { base[rt] = (sw)base[rs] < imm; }
inline void CPU::OP_SLTIU() { base[rt] = base[rs] < immu; }
inline void CPU::OP_ANDI() { base[rt] = base[rs] & immu; }
inline void CPU::OP_ORI() { base[rt] = base[rs] | immu; }
inline void CPU::OP_XORI() { base[rt] = base[rs] ^ immu; }
inline void CPU::OP_LUI() { base[rt] = code << 16; }
inline void CPU::OP_COP0() { 
    switch(rs) {
    case MFC:
        base[rt] = copr[rd];
		break;

    case MTC:
        copr[rd] = base[rt];
        break;
        
    case RFE: // Return from exception
        copr[12] = (copr[12] & ~(0xf)) | ((copr[12] >> 2) & 0xf);
        break;
        
    default:
        printx("  0x%08x | Unknown cop0 opcode 0x%08x | %d", pc, code, rs);
        break;
    }
}
inline void CPU::OP_COP1() { fprintf(stderr, "COP1 doesn't exist\n"); }
inline void CPU::OP_COP2() { 
    switch(rs) {
    case MFC:
        //base[rt] = cop2d.iuw[rd];
        base[rt] = gte.read(rd);
        break;
        
    case CFC:
        // base[rt] = cop2c.iuw[rd];
        base[rt] = gte.read(rd+32);
        break;
        
    case MTC:
        // cop2d.iuw[rd] = base[rt];
        // writeCop2(rd);
        gte.write(rd, base[rt]);
        break;
        
    case CTC:
        //cop2c.iuw[rd] = base[rt];
        gte.write(rd+32, base[rt]);
        break;
        
    default: // Execute GTE opcode
        gte.execCommand(code);//Is this right?
        break;
    }
}
inline void CPU::OP_COP3() { fprintf(stderr, "COP3 doesn't exist\n"); exit(0); }

inline void CPU::OP_LB() { base[rt] = (sb)core->getMem()->read<ub>(ob); }
inline void CPU::OP_LH() { base[rt] = (sh)core->getMem()->read<uh>(ob); }
inline void CPU::OP_LWL() { opcodeLWx(<<, 0); }
inline void CPU::OP_LW() { base[rt] = core->getMem()->read<uw>(ob); }
inline void CPU::OP_LBU() { base[rt] = core->getMem()->read<ub>(ob); }
inline void CPU::OP_LHU() { base[rt] = core->getMem()->read<uh>(ob); }
inline void CPU::OP_LWR() { opcodeLWx(>>, 1); }
inline void CPU::OP_SB() { core->getMem()->write<ub>(ob, base[rt]); }
inline void CPU::OP_SH() { core->getMem()->write<uh>(ob, base[rt]); }
inline void CPU::OP_SWL() { opcodeSWx(>>, 2); }
inline void CPU::OP_SW() { core->getMem()->write<uw>(ob, base[rt]); }
inline void CPU::OP_SWR() { opcodeSWx(<<, 3); }

inline void CPU::OP_LWC0() { fprintf(stderr, "OP_LWC0 doesn't exist\n"); exit(0); }
inline void CPU::OP_LWC1() { fprintf(stderr, "OP_LWC1 doesn't exist\n"); exit(0); }
inline void CPU::OP_LWC2() { gte.write(rt, core->getMem()->read<uw>(ob)); }
inline void CPU::OP_LWC3() { fprintf(stderr, "OP_LWC3 doesn't exist\n"); exit(0); }
inline void CPU::OP_SWC0() { fprintf(stderr, "OP_LWC0 doesn't exist\n"); exit(0); }
inline void CPU::OP_SWC1() { fprintf(stderr, "OP_LWC1 doesn't exist\n"); exit(0); }
inline void CPU::OP_SWC2() { core->getMem()->write<uw>(ob, gte.read(rt)); }
inline void CPU::OP_SWC3() { fprintf(stderr, "OP_LWC3 doesn't exist\n"); exit(0); }

inline void CPU::OP_SLL() {if (code) { base[rd] = base[rt] << sa; }}
inline void CPU::OP_SRL() { base[rd] = base[rt] >> sa; }
inline void CPU::OP_SRA() { base[rd] = (sw)base[rt] >> sa; }
inline void CPU::OP_SLLV() { base[rd] = base[rt] << (base[rs] & 31); }
inline void CPU::OP_SRLV() { base[rd] = base[rt] >> (base[rs] & 31); }
inline void CPU::OP_SRAV() { base[rd] = (sw)base[rt] >> (base[rs] & 31); }
inline void CPU::OP_JR() { branch(base[rs]); }
inline void CPU::OP_JALR() { base[rd] = pc + 4; branch(base[rs]); }
inline void CPU::OP_SYSCALL(bool branched) { pc -= 4; exception(0x20, branched); }
inline void CPU::OP_BREAK() {  }
inline void CPU::OP_MFHI() { base[rd] = res.u32[1]; }
inline void CPU::OP_MTHI() { res.u32[1] = base[rs]; }
inline void CPU::OP_MFLO() { base[rd] = res.u32[0]; }
inline void CPU::OP_MTLO() { res.u32[0] = base[rs]; }
inline void CPU::OP_MULT() { res.s64 = (sd)(sw)base[rs] * (sw)base[rt]; }
inline void CPU::OP_MULTU() { res.s64 = (sd)base[rs] * base[rt]; }
inline void CPU::OP_DIV() { 
    if (0 == base[rt]) {
        res.u32[0] = ((sw)base[rs] < 0) ? 0x00000001 : 0xffffffff;
        res.u32[1] = rs;
    }
    else if((uw)base[rs] == 0x80000000 && (uw)base[rt] == 0xffffffff) {
        res.u32[0] = 0x80000000;
        res.u32[1] = 0x00000000;
    }
    else {
        res.u32[0] = (sw)base[rs] / (sw)base[rt];
        res.u32[1] = (sw)base[rs] % (sw)base[rt];
    }
}
inline void CPU::OP_DIVU() { 
    if (base[rt]) {
        res.u32[0] = base[rs] / base[rt];
        res.u32[1] = base[rs] % base[rt];
    }
}
inline void CPU::OP_ADD() { base[rd] = base[rs] + base[rt]; } // Sign these?
inline void CPU::OP_ADDU() { base[rd] = base[rs] + base[rt]; }
inline void CPU::OP_SUB() { base[rd] = base[rs] - base[rt]; } 
inline void CPU::OP_SUBU() { base[rd] = base[rs] - base[rt]; } // Sign?
inline void CPU::OP_AND() { base[rd] = base[rs] & base[rt]; }
inline void CPU::OP_OR() { base[rd] = base[rs] | base[rt]; }
inline void CPU::OP_XOR() { base[rd] = base[rs] ^ base[rt]; }
inline void CPU::OP_NOR() { base[rd] = ~(base[rs] | base[rt]); }
inline void CPU::OP_SLT() { base[rd] = (sw)base[rs] < (sw)base[rt]; }
inline void CPU::OP_SLTU() { base[rd] = base[rs] < base[rt]; }