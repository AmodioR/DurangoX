
#include "BinaryInspector.h"

#include <fstream>
#include <iomanip>
#include <iostream>

bool BinaryInspector::inspect(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << filePath << '\n';
        return false;
    }

    unsigned char bytes[4] = {};

    file.read(reinterpret_cast<char*>(bytes), sizeof(bytes));

    if (file.gcount() != sizeof(bytes))
    {
        std::cerr << "File is too small.\n";
        return false;
    }

    std::cout << "First four bytes: ";

    for (unsigned char byte : bytes)
    {
        std::cout << std::hex
                  << std::uppercase
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(byte)
                  << ' ';
    }

    std::cout << std::dec << '\n';

    return true;
}
