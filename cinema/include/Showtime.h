#ifndef SHOWTIME_H
#define SHOWTIME_H

#include "DataEntity.h"
#include <string>
#include <vector>

class Movie;

class Showtime : public DataEntity {
public:
    std::string id;
    std::string movie_id;
    std::string room;
    std::string date;
    std::string time;
    int ticket_price;

    Showtime(const json& j);
    void display(const std::vector<Movie>& movies, int booked_seats) const;
    json toJson() const override;
};

#endif // SHOWTIME_H