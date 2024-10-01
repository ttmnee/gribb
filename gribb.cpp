#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <iomanip>

// do czytania danych binarnych
template<typename T>
T read(std::ifstream &file) {
    T value;
    file.read(reinterpret_cast<char*>(&value), sizeof(T));
    return value;
}

// do czytania danych binarnych w postaci wektora bajtów
std::vector<uint8_t> readBytes(std::ifstream &file, size_t size) {
    std::vector<uint8_t> bytes(size);
    file.read(reinterpret_cast<char*>(bytes.data()), size);
    return bytes;
}

// do konwersji bajtów na string
std::string bytesToString(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

int main() {
    std::ifstream file("all.grib", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    // wszystkie wartości w bajtach
    std::vector<uint8_t> indicatorSection = readBytes(file, 8);
    std::string codingInfo = bytesToString({indicatorSection.begin(), indicatorSection.begin() + 4});
    uint32_t gribLength = *reinterpret_cast<uint32_t*>(&indicatorSection[4]);
    
    std::cout << "Coding info: " << codingInfo << "\n";
    std::cout << "==========================\n";
    std::cout << "== General Message Info ==\n";
    std::cout << "==========================\n";
    std::cout << "Whole Message length: " << gribLength << "\n";

    file.seekg(8); 
    uint16_t section1Length = read<uint16_t>(file);
    std::vector<uint8_t> section1Data = readBytes(file, section1Length - 3); 

    uint8_t tableVersion = section1Data[0];
    uint8_t centre = section1Data[1];
    uint8_t processID = section1Data[2];
    uint8_t gridID = section1Data[3];
    uint8_t flag = section1Data[4];
    uint8_t unitParam = section1Data[5];
    uint8_t levelType = section1Data[6];
    uint16_t levelValue = *reinterpret_cast<uint16_t*>(&section1Data[7]);
    std::string dateTime = std::to_string(section1Data[9]) + "/" + std::to_string(section1Data[10]) + "/" + std::to_string(section1Data[11]) + "|" + std::to_string(section1Data[12]) + ":" + std::to_string(section1Data[13]);

    std::cout << "======================\n";
    std::cout << "==  Section 1 Data  ==\n";
    std::cout << "======================\n";
    std::cout << "Section 1 length: " << section1Length << "\n";
    std::cout << "Parameter table Version: " << static_cast<int>(tableVersion) << "\n";
    std::cout << "Identification of Centre: " << static_cast<int>(centre) << "\n";
    std::cout << "process ID number: " << static_cast<int>(processID) << "\n";
    std::cout << "Grid Identification: " << static_cast<int>(gridID) << "\n";
    std::cout << "GDS and BMS: " << (flag & 0x80 ? "GDS Included" : "GDS Omitted") << " " << (flag & 0x40 ? "BMS Included" : "BMS Omitted") << "\n";
    std::cout << "Unit parameters: " << static_cast<int>(unitParam) << "\n";
    std::cout << "Indicator of type of level or layer: " << static_cast<int>(levelType) << "\n";
    std::cout << "Height, pressure, etc. of the level or layer: " << levelValue << "\n";
    std::cout << "YY/MM/DD|HH:MM  :  " << dateTime << "\n";

    file.close();
    return 0;
}
