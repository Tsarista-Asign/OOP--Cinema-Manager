#ifndef BOOKING_H
#define BOOKING_H

#include "DataEntity.h"
#include <string>

class Booking : public DataEntity {
public:
    std::string ticket_id;
    std::string cccd;
    std::string showtime_id;
    std::string seat;

    Booking(const json& j);
    void display() const override;
    json toJson() const override;
};

#endif // BOOKING_H