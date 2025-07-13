#ifndef DISK_HPP
#define DISK_HPP

#include <global.hh>
#include <fstream>

class Disk 
{
private:
    const uw minuteOffset = 0;
    const uw secondOffest = 0x2B110;
    const uw sectorOffest = 0x930;

    std::fstream fileStream;
public:
    Disk() = default;

    bool openDisk(std::string diskLocation);

    void seek(ub amm, ub ass, ub asect);
    std::vector<ub> readSector();
    ub readByte();
    uw readWord();
};

#endif