#ifndef CDROM_hpp 
#define CDROM_hpp

#include <global.h>
#include <deque>
#include <disk/disk.hh>

class Core;

class CDROM
{
private:
	Core *core;

    template<typename T, size_t size>
    class FIFO 
    {
    public:
        std::deque<T> data_q;
    
        bool push(T value)
        {
            //Limit size
            if (data_q.size() < size) 
            {
                data_q.push_front(value);
                return true;
            }
            else 
            {
                printf("ERROR FIFO overfilled");
                system("PAUSE");
                return false;
            }
        }
        T pop()
        {   
            T ret = data_q.back();
            data_q.pop_back();
            return ret;
        }
        T peek()
        {
            return data_q.back();
        }
        T& ref()
        {
            return data_q.back();
        }
        T& front_ref()
        {
            return data_q.front();
        }
        bool isEmpty()
        {
            return data_q.empty();
        }
        bool isFull()
        {
            return data_q.size() == size;
        }
        void clear()
        {
            while (!data_q.empty())
            {
                data_q.clear();
            }
        }
    };
 
    struct Interrupt
    {   
        FIFO<ub, 16> responseFIFO;
        ub type = 0;
        int delay = 0;
        bool awk = false;

        Interrupt() = default;
        Interrupt(ub _type) : type(_type) {}
        Interrupt(ub _type, int _delay) : type(_type), delay(_delay) {}
    };

    union CD_Status
    {
        struct 
        {
            ub index   : 2;
            ub xaFIFO_empty : 1;
            ub parameterFIFO_empty : 1;
            ub parameterFIFO_not_full : 1;
            ub responseFIFO_empty : 1;
            ub dataFIFO_empty  : 1;
            ub transmissionBusy : 1;
        };
        ub reg;

        CD_Status() : reg(0x18) {}
    };

    union DISK_Status
    {
        enum class Status {PLAY, READ, SEEK, PAUSE};
        struct 
        {
            ub error : 1;
            ub motor_on : 1;
            ub seek_error : 1;
            ub id_error : 1;
            ub shell_open : 1;
            ub read : 1;
            ub seek : 1;
            ub play : 1;
        };
        ub reg;

        DISK_Status() : reg(0x0) {}
        void setStatus(Status status)
        {
            read = 0;
            play = 0;
            seek = 0;
            if (status == Status::PLAY)
                play = 1;
            if (status == Status::SEEK)
                seek = 1;
            if (status == Status::READ)
                read = 1;
        }
    };

    union Mode
    {
        struct 
        {
            ub Speed       : 1;  // (0=Normal speed, 1=Double speed)
            ub XA_ADPCM    : 1;  // (0=Off, 1=Send XA-ADPCM sectors to SPU Audio Input)
            ub Sector_Size : 1;  // (0=800h=DataOnly, 1=924h=WholeSectorExceptSyncBytes)
            ub Ignore_Bit  : 1;  // (0=Normal, 1=Ignore Sector Size and Setloc position)
            ub XA_Filter   : 1;  // (0=Off, 1=Process only XA-ADPCM sectors that match Setfilter)
            ub Report      : 1;  // (0=Off, 1=Enable Report-Interrupts for Audio Play)
            ub AutoPause   : 1;  // (0=Off, 1=Auto Pause upon End of Track) ;for Audio Play
            ub CDDA        : 1;  // (0=Off, 1=Allow to Read CD-DA Sectors; ignore missing EDC)
        };

        ub _reg;
    };

    Mode mode;

    Disk disk;
    
    DISK_Status disk_status;

    CD_Status status;

    FIFO<ub, 16> parameterFIFO;

    FIFO<Interrupt, 16> interruptFIFO;

    std::vector<ub> dataBuffer;
    uw dataPointer = 0;

    bool is_command = false;
    ub command;

    //1F801802h.Index0 - Parameter Fifo (W)
    ub paramterByte;

    //1F801803h.Index0 - Request Register (W)
    ub SMEN = 0;
    ub BFWR = 0;
    ub want_data = 0;

    //1F801803h.Index1|3 - Interrupt Flag Register (R/W)
    ub InterruptFlagRegister = 0;
    ub InterruptEnableRegister = 0;

    int readCycleDelay = 50000;

    void doCommand();
    void Command_test();
    void Command_GetStat();
    void Command_ReadTOC();
    void Command_GetID();
    void Command_SetLoc();
    void Command_SeekL();
    void Command_SetMode();
    void Command_ReadN();
    void Command_Pause();
    void Command_Init();

    void addInterrupt(ub type, int delay = 50000) 
    {
        interruptFIFO.push(Interrupt(type, delay));
    }

    void setIntRespose(ub data)
    {
        interruptFIFO.front_ref().responseFIFO.push(data);
    }



public:
    CDROM(Core *_core)
    {
        core = _core;
    }

    Disk* getDisk() { return &disk; }

    void init();

    void write(uw address, uw data);
	uw read(uw address);

    ub readByte();

    void step(uw cycles);
};

#endif


// template<typename T, size_t size>
//     class FIFO 
//     {
//         std::array<T, size> data;
        
//         std::_Array_iterator<T,16> front;
//         std::_Array_iterator<T,16> back;

//         size_t current_size;
        
//     public:
//         FIFO()
//         {
//             front = data.begin();
//             back = data.begin();
//         }
//         bool push(T value)
//         { 
                    // NO LIMITS RIGHT NOW
//             if (back != data.begin())
//             {
//                 //Move data to back before adding more data
//                 std::copy(data.begin(), front - data.begin(), back);
//             }

//             *fount = value;
//             front++;
//         }
//         T pop()
//         {   
//             T ret = *back;
//             back++;
//             return ret;
//         }
//         const T& peek()
//         {
//             return *back;
//         }
//         T& ref()
//         {
//             return *back;
//         }
//         bool isEmpty()
//         {
//             if (current_size == 0) { return true; }
//             else { return false; }
//         }
//         bool isFull()
//         {
//             if (current_size == size) { return true; }
//             else { return false; }
//         }
//         void clear()
//         {
//             data.fill(0);
//         }
//     };