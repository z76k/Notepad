#include "FileSystem.hpp"
#include <iostream>
#include <sys/stat.h>

std::string FileSystem::publicPath = "Notes/Public/";
std::string FileSystem::vaultPath = ".sys_cache/";

void FileSystem::init() {
    std::filesystem::create_directories(publicPath);
    std::filesystem::create_directories(vaultPath);
}

std::vector<std::string> FileSystem::listFiles(bool vaultUnlocked) {
    std::vector<std::string> files;
    
    // List Public
    for (const auto& entry : std::filesystem::directory_iterator(publicPath)) {
        files.push_back(entry.path().filename().string());
    }

    // List Vault if unlocked
    if (vaultUnlocked) {
        for (const auto& entry : std::filesystem::directory_iterator(vaultPath)) {
            files.push_back("[LOCKED] " + entry.path().filename().string());
        }
    }

    return files;
}

void FileSystem::privatizeFile(const std::string& filename) {
    std::string sourcePath = publicPath + filename;
    if (!std::filesystem::exists(sourcePath)) return;

    // Read content
    std::ifstream inFile(sourcePath);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    // Encrypt
    std::string encrypted = xorCipher(content);

    // Generate new name
    std::string newName = randomHexString(3) + ".dat";
    std::string destPath = vaultPath + newName;

    // Write to Vault
    std::ofstream outFile(destPath);
    outFile << encrypted;
    outFile.close();

    // Delete original
    std::filesystem::remove(sourcePath);
    std::cout << "File privatized to: " << destPath << std::endl;
}

std::string FileSystem::randomHexString(int length) {
    std::stringstream ss;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    ss << "0x";
    for (int i = 0; i < length; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

std::string FileSystem::xorCipher(const std::string& input) {
    std::string output = input;
    char key = 0x5A; // Simple key
    for (size_t i = 0; i < input.length(); ++i) {
        output[i] = input[i] ^ key;
    }
    return output;
}

std::string FileSystem::readFile(const std::string& filename, bool isVault) {
    std::string path = (isVault ? vaultPath : publicPath) + filename;
    std::ifstream inFile(path);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    
    if (isVault) {
        return xorCipher(content);
    }
    return content;
}

void FileSystem::saveFile(const std::string& filename, const std::string& content, bool isVault) {
    std::string path = (isVault ? vaultPath : publicPath) + filename;
    std::ofstream outFile(path);
    if (isVault) {
        outFile << xorCipher(content);
    } else {
        outFile << content;
    }
}
