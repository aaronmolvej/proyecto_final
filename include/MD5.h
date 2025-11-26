#ifndef MD5_H
#define MD5_H

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <wincrypt.h>
    #pragma comment(lib, "advapi32.lib")
#else
    #include <openssl/md5.h>
#endif

class MD5 {
public:
    static std::string hashFile(const std::string& filepath);
    
private:
    static std::string bytesToHex(const unsigned char* data, size_t len);
};

#endif
