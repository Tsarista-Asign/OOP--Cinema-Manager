#include "Showtime.h"
#include "Movie.h"
#include <iostream>
#include <algorithm>

Showtime::Showtime(const json& j) {
    id = j["Mã suất chiếu"];
    movie_id = j["Mã phim"];
    room = j["Phòng"];
    date = j["Ngày chiếu"];
    time = j["Giờ chiếu"];
    ticket_price = j.value("Giá vé", 100000);
}

void Showtime::display(const std::vector<Movie>& movies, int booked_seats) const {
    std::string movie_title = "Unknown";
    for (const auto& movie : movies) {
        if (movie.id == movie_id) {
            movie_title = movie.title;
            break;
        }
    }
    std::cout << "Mã suất chiếu: " << id << ", Mã phim: " << movie_id 
              << ", Tên phim: " << movie_title << ", Phòng: " << room 
              << ", Ngày chiếu: " << date << ", Giờ chiếu: " << time 
              << ", Giá vé: " << ticket_price << " VND"
              << (booked_seats >= 36 ? " - Đã hết vé!!" : "") << std::endl;
}

json Showtime::toJson() const {
    return {
        {"Mã suất chiếu", id},
        {"Mã phim", movie_id},
        {"Phòng", room},
        {"Ngày chiếu", date},
        {"Giờ chiếu", time},
        {"Giá vé", ticket_price}
    };
}