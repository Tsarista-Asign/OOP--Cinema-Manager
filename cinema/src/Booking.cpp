#include "Booking.h"
#include <iostream>

Booking::Booking(const json& j) {
    ticket_id = j["Mã vé"];
    cccd = j["CCCD"];
    showtime_id = j["Suất chiếu"];
    seat = j["Ghế"];
}

void Booking::display() const {
    std::cout << "Mã vé: " << ticket_id << ", CCCD: " << cccd 
              << ", Mã suất chiếu: " << showtime_id << ", Ghế: " << seat << std::endl;
}

json Booking::toJson() const {
    return {
        {"Mã vé", ticket_id},
        {"CCCD", cccd},
        {"Suất chiếu", showtime_id},
        {"Ghế", seat}
    };
}