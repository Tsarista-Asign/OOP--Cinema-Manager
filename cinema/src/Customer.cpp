#include "Customer.h"
#include <iostream>

Customer::Customer(const json& j) {
    cccd = j["CCCD"];
    name = j["Tên khách"];
    phone = j["Số điện thoại"];
}

void Customer::display() const {
    std::cout << "CCCD: " << cccd << ", Tên: " << name << ", Số điện thoại: " << phone << std::endl;
}

json Customer::toJson() const {
    return {
        {"CCCD", cccd},
        {"Tên khách", name},
        {"Số điện thoại", phone}
    };
}