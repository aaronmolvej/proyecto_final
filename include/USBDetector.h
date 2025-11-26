#ifndef USBDETECTOR_H
#define USBDETECTOR_H

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class USBDetector {
private:
    std::vector<std::string> usbPaths;
    
    void scanWindowsDrives();
    void scanLinuxDrives();
    
public:
    USBDetector();
    ~USBDetector();
    
    bool detectUSBDrives();
    std::vector<std::string> getUSBPaths() const;
};

#endif
