int main()
{
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
                // if (base[rs] % 4 != 0)
                // {
                //     exception(0x10, branched);
                //     break;
                // }
                branch(base[rs]);
                break;
                
            case 9: // JALR
                // if (base[rs] % 4 != 0)
                // {
                //     exception(0x10, branched);
                //     break;
                // }
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
                // if (0 == base[rt]) {
                //     res.u32[0] = ((sw)base[rs] < 0) ? 0x00000001 : 0xffffffff;
                //     res.u32[1] = rs;
                // }
                // else if((uw)base[rs] == 0x80000000 && (uw)base[rt] == 0xffffffff) {
                //     res.u32[0] = 0x80000000;
                //     res.u32[1] = 0x00000000;
                // }
                // else {
                    res.u32[0] = (sw)base[rs] / (sw)base[rt];
                    res.u32[1] = (sw)base[rs] % (sw)base[rt];
                // }
                break;
                
            case 27: // DIVU
                if (0 == base[rt]) {
                    res.u32[0] = 0xffffffff;
                    res.u32[1] = base[rs];
                }
                res.u32[0] = base[rs] / base[rt];
                res.u32[1] = base[rs] % base[rt];
                break;
                
            case 32: // ADD
                // a = base[rs];
                // b = base[rt];
                // if ((!((a ^ b) & 0x80000000) && (((a+b)) ^ a) & 0x80000000)) { [[unlikely]]	
                //     exception(0x30, branched);
                //     break;
                // }
                // if (((int64_t)a + (int64_t)b) > INT32_MAX) 
                // {
                //      exception(0x60, branched);
                //      break;
                // }
                base[rd] = base[rs] + base[rt];
                break;
                
            case 33: // ADDU
                base[rd] = base[rs] + base[rt];


                break;
                
            case 34: // SUB
                // a = base[rs];
                // b = base[rt];
                // if (((a ^ b) & 0x80000000) && ((a - b) ^ a) & 0x80000000) { [[unlikely]]	
                //     exception(0x30, branched);
                //     break;
                // }
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
                printf("  0x%08x | Unknown special opcode 0x%08x | %d", pc, code, (code & 63));
                break;
        }
        break;
        
    case 1: // REGIMM
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
            printx("/// PSeudo 0x%08x | Unknown bcond opcode 0x%08x | %d", pc, code, rt);
            break;
        }
        break;
        
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
        // a = base[rs];
        // b = imm;
        // if ((!((a ^ b) & 0x80000000) && (((a+b)) ^ a) & 0x80000000)) { [[unlikely]]	
        //     exception(0x30, branched);
        //     break;
        // }
        base[rd] = base[rs] + imm;
        break;
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
                printx("  0x%08x | Unknown cop0 opcode 0x%08x | %d\n", pc, code, rs);
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
        fprintf(stderr, "PC: %x Unknown basic opcode 0x%08x | %x | isBranched: %d\n", pc,  code, opcode, branched);
        break;
    }
}