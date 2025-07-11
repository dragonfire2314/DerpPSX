#include <disk/disk.hh>

bool Disk::openDisk(std::string diskLocation)
{
    fileStream.open(diskLocation, std::ifstream::in | std::ifstream::binary);

    if (fileStream.is_open())
        return true;

    printf("Failed to open disk!!\n");
    system("PASUE");

    return false;
}

void Disk::seek(ub amm, ub ass, ub asect)
{
    uw location_to_seek = (asect * sectorOffest) + ((ass-2) * secondOffest);

    // printf("Seek to: %x\n", location_to_seek);
    // printf("amm: %x\n", amm);
    // printf("ass: %x\n", ass);
    // printf("asect: %x\n", asect);
    // system("PAUSE");

    fileStream.seekg(location_to_seek);
}

std::vector<ub> Disk::readSector()
{
    char sector[0x930];
    fileStream.read(sector, 0x930);
    
    std::vector<ub> vec;
    vec.reserve(0x930);
    vec.insert(vec.begin(), sector, sector+0x930);

    return vec;
}

ub Disk::readByte()
{
    char ret;
    fileStream.get(ret);
    return static_cast<ub>(ret);
}

uw Disk::readWord()
{
    uw ret = 0;

    ret |= readByte() << 24;
    ret |= readByte() << 16;
    ret |= readByte() << 8;
    ret |= readByte();

    return ret;
}
