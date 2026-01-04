#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

#include "DataManager.h"
#include "Customer.h"
#include "Movie.h"
#include "Showtime.h"
#include "Booking.h"
#include <vector>
#include <string>

class BookingManager {
private:
    DataManager& data_manager;
    std::vector<Customer>& customers;
    std::vector<Movie>& movies;
    std::vector<Showtime>& showtimes;
    std::vector<Booking>& bookings;

public:
    BookingManager(DataManager& dm, std::vector<Customer>& c, std::vector<Movie>& m,
                   std::vector<Showtime>& s, std::vector<Booking>& b);
    void loadBookings();
    void bookTicket(const std::string& initial_movie_id = "");
};

#endif // BOOKING_MANAGER_H