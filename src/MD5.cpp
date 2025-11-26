#include "MD5.h"
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

std::string MD5::bytesToHex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

std::string MD5::hashFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: No se puede abrir archivo para MD5: " << filepath << std::endl;
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Error: No se pudo leer archivo para MD5" << std::endl;
        return "";
    }
    file.close();

#ifdef _WIN32
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[16];
    DWORD cbHash = 16;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        std::cerr << "Error CryptAcquireContext" << std::endl;
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        std::cerr << "Error CryptCreateHash" << std::endl;
        return "";
    }

    if (!CryptHashData(hHash, reinterpret_cast<BYTE*>(buffer.data()), size, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        std::cerr << "Error CryptHashData" << std::endl;
        return "";
    }

    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        std::cerr << "Error CryptGetHashParam" << std::endl;
        return "";
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return bytesToHex(rgbHash, cbHash);
#else
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    MD5_Update(&md5Context, buffer.data(), size);
    MD5_Final(result, &md5Context);
    return bytesToHex(result, MD5_DIGEST_LENGTH);
#endif
}
