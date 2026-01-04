#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class DataManager {
private:
    const std::string BASE_PATH = "C:/Users/Orias.ASUS/Documents/Code/1. C-C++/src/Cinema/";

public:
    json readJsonFile(const std::string& filename);
    void writeJsonFile(const std::string& filename, const json& j);
};

#endif // DATA_MANAGER_H