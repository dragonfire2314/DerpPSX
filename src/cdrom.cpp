#include <cdrom.h>

#include <core.h>



void CDROM::init()
{
    disk.openDisk("/home/tanner/DerpPSX/ps1Roms/DDR/DDR.bin");
}

void CDROM::step(uw cycles)
{
    if (!interruptFIFO.isEmpty()) {
        interruptFIFO.ref().delay -= cycles;

        if (interruptFIFO.ref().delay <= 0) {
            status.transmissionBusy = 0;
            // printf("Stuffs:%x\n", (InterruptFlagRegister & 7) & (interruptFIFO.ref().type & 7));
            if ((InterruptFlagRegister & 7) & (interruptFIFO.ref().type & 7) & InterruptEnableRegister) {
                // core->getDebugger()->addBreakPoint(0x80000084, "Interupt Handler");
                core->getIO()->IRQ(IO_Controller::IRQ_TYPE::CDROM);
            }
        }
    }

    //While read is going, read sector every number of cycles
    readCycleDelay -= cycles;
    if (readCycleDelay <= 0) {
        if (disk_status.read == 1) {
        // printf("INT 1 fired\n");
            addInterrupt(1);
            setIntRespose(disk_status.reg);
            readCycleDelay = 0x36cd2;
        }
    }
    
}

void CDROM::write(uw address, uw data)
{
    ub reg = (address & 0xF);

    if (reg == 0x0) 
    {
        // printf("W Status: %x\n", data);

        status.index = (data & 0x3);
        return;
    }

    if (reg == 0x1) 
    {
        if (status.index == 0) 
        {   
            // printf("W Command: %x\n", data);
            command = static_cast<ub>(data);
            doCommand();
            return;
        }
    }

    if (reg == 0x2) 
    {
        if (status.index == 0) 
        {
            // printf("W Parameter: %x\n", data);
            parameterFIFO.push(static_cast<ub>(data));
            return;
        }
        if (status.index == 1)
        {
            InterruptEnableRegister = data;
            return;
        }
    }

    if (reg == 0x3) 
    {   
        if (status.index == 0) 
        {
            if ((data & 0x80) == 0x80)
            {
                if (dataBuffer.empty()) 
                {
                    //Load dataBuffer
                    dataBuffer = disk.readSector();
                    status.dataFIFO_empty = 1;
                    dataPointer = 0;
                }
            }
            else 
            {
                dataBuffer.clear();
                status.dataFIFO_empty = 0;
            }
            return;
        }

        if (status.index == 1) 
        {
            // printf("W Interrupt Flag: %x\n", data);
            if ((data & 0x40) == 0x40) 
            {
                parameterFIFO.clear();
                return;
            }

            if (!interruptFIFO.isEmpty()) 
            {
                // interruptFIFO.ref().awk = ((data & 0x1F) & interruptFIFO.ref().type);
                interruptFIFO.ref().awk = true;
                if (interruptFIFO.ref().responseFIFO.isEmpty()) 
                {
                    interruptFIFO.pop();
                }
            }

            InterruptFlagRegister = (data & 0x1F);

            // printf("PC:%x\n", core->getCPU()->pc);
            return;
        }
    }
        

    printf("CDROM write address: %x, data: %x not fount, index: %i\n", address, data, status.index);
}

uw CDROM::read(uw address)
{
    ub reg = (address & 0xF);

    if (reg == 0x0) 
    {
        // printf("BUSYSTS:%x, DRQSTS:%x, RSLRRDY:%x, PRMWRDY:%x, PRMEMPT:%x, ADPBUSY:%x\n", BUSYSTS, DRQSTS, RSLRRDY, PRMWRDY, PRMEMPT, ADPBUSY);
        // printf("returning Status:%x\n", status.reg);
        status.parameterFIFO_empty = parameterFIFO.isEmpty();
        status.parameterFIFO_not_full = parameterFIFO.isEmpty();
        if (!interruptFIFO.isEmpty()) {
            status.responseFIFO_empty = !interruptFIFO.ref().responseFIFO.isEmpty();
        }
        else {
            status.responseFIFO_empty = 0;
        }

        // printf("R Status: %x\n", status.reg);

        return (
            status.reg
        );
    }

    if (reg == 0x1) 
    {
        ub ret = 0;
        if (!interruptFIFO.ref().responseFIFO.isEmpty()){
            ret = interruptFIFO.ref().responseFIFO.pop();

            if (interruptFIFO.ref().responseFIFO.isEmpty()) 
            {
                interruptFIFO.pop();
            }
        }
        // printf("R Response FIFO: %x\n", ret);
        return ret;
    }

    if (reg == 0x2) 
    {

    }

    if (reg == 0x3) 
    {
        if (status.index == 0)
        {
            return InterruptEnableRegister;
        }

        if (status.index == 1) 
        {
            ub ret = 0b11100000;
            if (!interruptFIFO.isEmpty())
                ret |= interruptFIFO.ref().type;

            // printf("R Interrupt Flag: %x\n", ret);
            return ret;
        }
    }

    printf("CDROM read address: %x not fount \n", address);
    return 0;
}

ub CDROM::readByte()
{
    if (dataBuffer.empty()){ return 0; }

    ub dataStart = 12; //Skip Sync
    if (!mode.Sector_Size) { dataStart += 12; } //Check if we want 0x800 or 0x924

    ub ret = dataBuffer[dataStart + dataPointer];
    dataPointer++;
    //Check if data fifo is empty
    if (!mode.Sector_Size && dataPointer >= 0x800) { status.dataFIFO_empty = 0; }
    if (mode.Sector_Size && dataPointer >= 0x924) { status.dataFIFO_empty = 0; }

    return ret;
}

void CDROM::doCommand()
{
    printf("\x1b[33m cmd: %x\x1b[0m\n", command);

    switch(command)
    {
    case 0x01:
        Command_GetStat();
        break;
    case 0x02:
        Command_SetLoc();
        break;
    case 0x06:
        Command_ReadN();
        break;
    case 0x09:
        Command_Pause();
        break;
    case 0x0a:
        Command_Init();
        break;
    case 0x0e:
        Command_SetMode();
        break;
    case 0x15:
        Command_SeekL();
        break;    
    case 0x1a:
        Command_GetID();
        break;
    case 0x1e:
        Command_ReadTOC();
        break;
    case 0x19: // Test
        Command_test();
        break;
    default:
        printf("CDROM command not found: %x\n", command);
        system("PAUSE");
        break;
    }

    // system("PAUSE");
}

ub fromBCD(ub bcd) {
    ub hi = (bcd & 0xF0) >> 4;
    ub lo = bcd & 0xF;

    return hi * 10 + lo;
}

void CDROM::Command_test()
{
    ub subCommand = parameterFIFO.pop();

    printf("CDROM test subcommand: %x\n", subCommand);
    switch(subCommand)
    {
    case 0x20: // Test
        addInterrupt(3);
        setIntRespose(0x97);
        setIntRespose(0x01);
        setIntRespose(0x10);
        setIntRespose(0xC2);
        break;
    default:
        printf("CDROM test subcommand not found: %x\n", subCommand);
        break;
    }
}

void CDROM::Command_Init()
{
    addInterrupt(3);
    setIntRespose(disk_status.reg);

    disk_status.setStatus(DISK_Status::Status::READ);

    addInterrupt(2);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_ReadTOC()
{
    addInterrupt(3);
    setIntRespose(disk_status.reg);

    addInterrupt(2);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_GetStat()
{
    disk_status.shell_open = 0;

    addInterrupt(3);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_SetMode()
{
    ub _mode = parameterFIFO.pop();
    mode._reg = _mode;

    addInterrupt(3);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_Pause()
{
    addInterrupt(3);
    setIntRespose(disk_status.reg);

    disk_status.setStatus(DISK_Status::Status::PAUSE);

    addInterrupt(2);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_ReadN()
{
    addInterrupt(3);
    setIntRespose(disk_status.reg);

    disk_status.setStatus(DISK_Status::Status::READ);

    addInterrupt(1);
    setIntRespose(disk_status.reg);

    readCycleDelay = 0x36cd2;
}

void CDROM::Command_SetLoc()
{
    ub amm = parameterFIFO.pop();
    ub ass = parameterFIFO.pop();
    ub asect = parameterFIFO.pop();

    amm = fromBCD(amm);
    ass = fromBCD(ass);
    asect = fromBCD(asect);

    disk.seek(amm, ass, asect);

    addInterrupt(3);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_SeekL()
{
    disk_status.seek = 1;

    addInterrupt(3);
    setIntRespose(disk_status.reg);

    disk_status.seek = 0;

    addInterrupt(2);
    setIntRespose(disk_status.reg);
}

void CDROM::Command_GetID()
{
    disk_status.motor_on = 1;

    addInterrupt(3);
    setIntRespose(disk_status.reg);

    //Check if disk
    //For no disk
    // addInterrupt(5);
    // setIntRespose(0x08);
    // setIntRespose(0x40);

    // setIntRespose(0x00);
    // setIntRespose(0x00);
    
    // setIntRespose(0x00);
    // setIntRespose(0x00);
    // setIntRespose(0x00);
    // setIntRespose(0x00);

    //Disk
    addInterrupt(2);
    setIntRespose(0x02);
    setIntRespose(0x00);

    setIntRespose(0x20);
    setIntRespose(0x00);
    
    setIntRespose(0x53);
    setIntRespose(0x43);
    setIntRespose(0x45);
    setIntRespose(0x41);
}