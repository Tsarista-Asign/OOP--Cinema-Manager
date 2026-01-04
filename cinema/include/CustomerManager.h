#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "DataManager.h"
#include "Customer.h"
#include <vector>
#include <string>

class CustomerManager {
private:
    DataManager& data_manager;
    std::vector<Customer>& customers;

public:
    CustomerManager(DataManager& dm, std::vector<Customer>& c);
    void loadCustomers();
    void displayCustomers() const;
    Customer* findCustomer(const std::string& cccd);
    void addCustomer(const std::string& cccd, const std::string& name, const std::string& phone);
    void updateCustomer(const std::string& cccd, const std::string& name, const std::string& phone);
};

#endif // CUSTOMER_MANAGER_H