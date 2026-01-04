#ifndef DATA_ENTITY_H
#define DATA_ENTITY_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class DataEntity {
public:
    virtual void display() const = 0;
    virtual json toJson() const = 0;
    virtual ~DataEntity() = default;
};

#endif // DATA_ENTITY_H