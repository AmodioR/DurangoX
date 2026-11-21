
#include "durangox/BinaryInspector.h"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace
{
    bool readUInt16(std::ifstream& file, std::uint16_t& value)
    {
        unsigned char bytes[2];

        if (!file.read(reinterpret_cast<char*>(bytes), 2))
            return false;

        value = static_cast<std::uint16_t>(
            bytes[0] | (static_cast<std::uint16_t>(bytes[1]) << 8)
        );

        return true;
    }

    bool readUInt32(std::ifstream& file, std::uint32_t& value)
    {
        unsigned char bytes[4];

        if (!file.read(reinterpret_cast<char*>(bytes), 4))
            return false;

        value = static_cast<std::uint32_t>(bytes[0])
              | (static_cast<std::uint32_t>(bytes[1]) << 8)
              | (static_cast<std::uint32_t>(bytes[2]) << 16)
              | (static_cast<std::uint32_t>(bytes[3]) << 24);

        return true;
    }

    std::string getArchitecture(std::uint16_t machine)
    {
        switch (machine)
        {
            case 0x014C: return "Intel x86";
            case 0x8664: return "AMD64 (x64)";
            case 0xAA64: return "ARM64";
            case 0x01C4: return "ARM Thumb-2";
            default: return "Unknown";
        }
    }
}

bool BinaryInspector::inspect(const std::string& filePath)
{
    std::cout << "\nDURANGOX / BINARY INSPECTOR\n";
    std::cout << "Analyzing: " << filePath << "\n\n";

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cerr << "[ERROR] Could not open file.\n";
        return false;
    }

    const std::streamoff fileSize = file.tellg();

    if (fileSize < 64)
    {
        std::cerr << "[ERROR] File is too small for a PE header.\n";
        return false;
    }

    // Read DOS signature
    file.seekg(0, std::ios::beg);

    std::uint16_t dosSignature = 0;

    if (!readUInt16(file, dosSignature) || dosSignature != 0x5A4D)
    {
        std::cerr << "[ERROR] Invalid DOS signature.\n";
        return false;
    }

    std::cout << "[OK] DOS Signature: MZ\n";

    // Find PE header offset
    file.seekg(0x3C, std::ios::beg);

    std::uint32_t peOffset = 0;

    if (!readUInt32(file, peOffset))
    {
        std::cerr << "[ERROR] Could not read PE offset.\n";
        return false;
    }

    // PE signature (4 bytes) + COFF header (20 bytes)
    if (peOffset < 64 ||
        static_cast<std::uint64_t>(peOffset) + 24 >
        static_cast<std::uint64_t>(fileSize))
    {
        std::cerr << "[ERROR] Invalid PE header offset.\n";
        return false;
    }

    file.seekg(peOffset, std::ios::beg);

    std::uint32_t peSignature = 0;

    if (!readUInt32(file, peSignature) ||
        peSignature != 0x00004550)
    {
        std::cerr << "[ERROR] Invalid PE signature.\n";
        return false;
    }

    std::cout << "[OK] PE Signature: PE\n";

    // Read COFF file header
    std::uint16_t machine = 0;
    std::uint16_t sections = 0;

    if (!readUInt16(file, machine) ||
        !readUInt16(file, sections))
    {
        std::cerr << "[ERROR] Could not read COFF header.\n";
        return false;
    }

    std::cout << "[OK] Architecture: "
              << getArchitecture(machine) << "\n";

    // SizeOfOptionalHeader is at PE offset + 20
    file.seekg(static_cast<std::streamoff>(peOffset) + 20,
               std::ios::beg);

    std::uint16_t optionalHeaderSize = 0;

    if (!readUInt16(file, optionalHeaderSize))
    {
        std::cerr << "[ERROR] Could not read optional header size.\n";
        return false;
    }

    const std::uint64_t optionalHeaderStart =
        static_cast<std::uint64_t>(peOffset) + 24;

    if (optionalHeaderSize < 2 ||
        optionalHeaderStart + optionalHeaderSize >
        static_cast<std::uint64_t>(fileSize))
    {
        std::cerr << "[ERROR] Invalid optional header size.\n";
        return false;
    }

    file.seekg(static_cast<std::streamoff>(optionalHeaderStart),
               std::ios::beg);

    std::uint16_t magic = 0;

    if (!readUInt16(file, magic))
    {
        std::cerr << "[ERROR] Could not read PE format.\n";
        return false;
    }

    if (magic == 0x020B)
        std::cout << "[OK] Format: PE32+\n";
    else if (magic == 0x010B)
        std::cout << "[OK] Format: PE32\n";
    else
    {
        std::cerr << "[ERROR] Unsupported PE format.\n";
        return false;
    }

    const std::uint64_t sectionTableEnd =
        optionalHeaderStart + optionalHeaderSize +
        static_cast<std::uint64_t>(sections) * 40;

    if (sectionTableEnd > static_cast<std::uint64_t>(fileSize))
    {
        std::cerr << "[ERROR] Section table exceeds file size.\n";
        return false;
    }

    std::cout << "[OK] Sections: " << sections << "\n";
    std::cout << "\nAnalysis completed successfully.\n";

    return true;
}
