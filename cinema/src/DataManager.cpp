#include "DataManager.h"
#include <fstream>
#include <stdexcept>

json DataManager::readJsonFile(const std::string& filename) {
    std::ifstream file(BASE_PATH + filename);
    if (!file.is_open()) {
        throw std::runtime_error("Không thể mở file: " + filename);
    }
    json j;
    file >> j;
    file.close();
    return j;
}

void DataManager::writeJsonFile(const std::string& filename, const json& j) {
    std::ofstream file(BASE_PATH + filename);
    if (!file.is_open()) {
        throw std::runtime_error("Không thể ghi vào file: " + filename);
    }
    file << j.dump(4);
    file.close();
}