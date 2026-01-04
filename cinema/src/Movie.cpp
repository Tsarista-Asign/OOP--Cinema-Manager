#include "Movie.h"
#include <iostream>

Movie::Movie(const json& j) {
    id = j["Mã phim"];
    title = j["Tên phim"];
    duration = j["Thời lượng"];
}

void Movie::display() const {
    std::cout << "Mã phim: " << id << ", Tên phim: " << title << ", Thời lượng: " << duration << " phút" << std::endl;
}

json Movie::toJson() const {
    return {
        {"Mã phim", id},
        {"Tên phim", title},
        {"Thời lượng", duration}
    };
}