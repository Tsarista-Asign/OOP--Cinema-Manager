#ifndef CINEMA_SYSTEM_H
#define CINEMA_SYSTEM_H

#include "DataManager.h"
#include "CustomerManager.h"
#include "MovieManager.h"
#include "ShowtimeManager.h"
#include "BookingManager.h"
#include <vector>

class CinemaSystem {
private:
    DataManager data_manager;
    std::vector<Customer> customers;
    std::vector<Movie> movies;
    std::vector<Showtime> showtimes;
    std::vector<Booking> bookings;
    CustomerManager customer_manager;
    MovieManager movie_manager;
    ShowtimeManager showtime_manager;
    BookingManager booking_manager;

public:
    CinemaSystem();
    void run();
};

#endif // CINEMA_SYSTEM_H