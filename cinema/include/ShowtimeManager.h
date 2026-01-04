#ifndef SHOWTIME_MANAGER_H
#define SHOWTIME_MANAGER_H

#include "DataManager.h"
#include "Showtime.h"
#include "Movie.h"
#include "Booking.h"
#include <vector>
#include <string>
#include <regex>

class ShowtimeManager {
private:
    DataManager& data_manager;
    std::vector<Showtime>& showtimes;
    const std::vector<Movie>& movies;
    std::vector<Booking>& bookings;

    bool isShowtimeValid(const std::string& date, const std::string& time) const;

public:
    ShowtimeManager(DataManager& dm, std::vector<Showtime>& s, const std::vector<Movie>& m, std::vector<Booking>& b);
    void loadShowtimes();
    void displayShowtimes() const;
    void addShowtime();
    void editShowtime();
    void deleteShowtime();
};

#endif // SHOWTIME_MANAGER_H