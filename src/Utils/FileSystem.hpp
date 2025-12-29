#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>

class FileSystem {
public:
    static void init();
    static std::vector<std::string> listFiles(bool vaultUnlocked);
    static void privatizeFile(const std::string& filename);
    static std::string readFile(const std::string& filename, bool isVault);
    static void saveFile(const std::string& filename, const std::string& content, bool isVault);

private:
    static std::string publicPath;
    static std::string vaultPath;
    
    static std::string randomHexString(int length);
    static std::string xorCipher(const std::string& input);
};
