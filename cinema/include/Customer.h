#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "DataEntity.h"
#include <string>

class Customer : public DataEntity {
public:
    std::string cccd;
    std::string name;
    std::string phone;

    Customer(const json& j);
    void display() const override;
    json toJson() const override;
};

#endif // CUSTOMER_H