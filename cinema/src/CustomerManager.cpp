#include "CustomerManager.h"
#include <iostream>
#include <algorithm>

CustomerManager::CustomerManager(DataManager& dm, std::vector<Customer>& c)
    : data_manager(dm), customers(c) {}

void CustomerManager::loadCustomers() {
    try {
        json customer_json = data_manager.readJsonFile("Customer.json");
        customers.clear();
        for (const auto& item : customer_json) {
            customers.emplace_back(item);
        }
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi tải khách hàng: " << e.what() << std::endl;
    }
}

void CustomerManager::displayCustomers() const {
    std::cout << "\nDanh sách khách hàng:\n";
    for (const auto& customer : customers) {
        customer.display();
    }
}

Customer* CustomerManager::findCustomer(const std::string& cccd) {
    auto it = std::find_if(customers.begin(), customers.end(),
        [&cccd](const Customer& c) { return c.cccd == cccd; });
    return it != customers.end() ? &(*it) : nullptr;
}

void CustomerManager::addCustomer(const std::string& cccd, const std::string& name, const std::string& phone) {
    json new_customer = {
        {"CCCD", cccd},
        {"Tên khách", name},
        {"Số điện thoại", phone}
    };
    customers.emplace_back(new_customer);

    json customer_json = json::array();
    for (const auto& customer : customers) {
        customer_json.push_back(customer.toJson());
    }

    try {
        data_manager.writeJsonFile("Customer.json", customer_json);
        std::cout << "Lưu thông tin khách hàng thành công!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi lưu khách hàng: " << e.what() << std::endl;
    }
}

void CustomerManager::updateCustomer(const std::string& cccd, const std::string& name, const std::string& phone) {
    auto it = std::find_if(customers.begin(), customers.end(),
        [&cccd](const Customer& c) { return c.cccd == cccd; });
    if (it != customers.end()) {
        it->name = name;
        it->phone = phone;

        json customer_json = json::array();
        for (const auto& customer : customers) {
            customer_json.push_back(customer.toJson());
        }

        try {
            data_manager.writeJsonFile("Customer.json", customer_json);
            std::cout << "Cập nhật thông tin khách hàng thành công!\n";
        } catch (const std::exception& e) {
            std::cout << "Lỗi khi lưu khách hàng: " << e.what() << std::endl;
        }
    }
}