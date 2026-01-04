#include "ShowtimeManager.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <limits>

ShowtimeManager::ShowtimeManager(DataManager& dm, std::vector<Showtime>& s, const std::vector<Movie>& m, std::vector<Booking>& b)
    : data_manager(dm), showtimes(s), movies(m), bookings(b) {}

bool ShowtimeManager::isShowtimeValid(const std::string& date, const std::string& time) const {
    std::time_t now = std::time(nullptr);
    tm current_tm = *std::localtime(&now);

    std::regex date_regex(R"(\d{2}/\d{2}/\d{4})");
    std::regex time_regex(R"(\d{2}:\d{2}\s*-\s*\d{2}:\d{2})");
    if (!std::regex_match(date, date_regex) || !std::regex_match(time, time_regex)) {
        return false;
    }

    int day, month, year;
    try {
        day = std::stoi(date.substr(0, 2));
        month = std::stoi(date.substr(3, 2));
        year = std::stoi(date.substr(6, 4));
    } catch (const std::exception& e) {
        return false;
    }

    int hour, minute;
    try {
        std::string start_time = time.substr(0, 5);
        hour = std::stoi(start_time.substr(0, 2));
        minute = std::stoi(start_time.substr(3, 2));
    } catch (const std::exception& e) {
        return false;
    }

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return false;
    }

    tm showtime_tm = {};
    showtime_tm.tm_mday = day;
    showtime_tm.tm_mon = month - 1;
    showtime_tm.tm_year = year - 1900;
    showtime_tm.tm_hour = hour;
    showtime_tm.tm_min = minute;

    std::time_t showtime = std::mktime(&showtime_tm);
    if (showtime == -1) {
        return false;
    }

    return showtime > now;
}

void ShowtimeManager::loadShowtimes() {
    try {
        json showtime_json = data_manager.readJsonFile("Showtime.json");
        showtimes.clear();
        for (const auto& item : showtime_json) {
            showtimes.emplace_back(item);
        }
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi tải suất chiếu: " << e.what() << std::endl;
    }
}

void ShowtimeManager::displayShowtimes() const {
    std::cout << "\nLịch chiếu phim:\n";
    for (const auto& showtime : showtimes) {
        int booked_seats = std::count_if(bookings.begin(), bookings.end(),
            [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
        showtime.display(movies, booked_seats);
    }
}

void ShowtimeManager::addShowtime() {
    std::string movie_id, room, date, time;
    int ticket_price;
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    while (attempts < MAX_ATTEMPTS) {
        std::cout << "\nNhập mã phim (nhập '0' để hủy): ";
        std::cin >> movie_id;
        if (movie_id == "0") {
            std::cout << "Hủy thêm suất chiếu.\n";
            return;
        }

        if (std::none_of(movies.begin(), movies.end(), [&movie_id](const Movie& m) { return m.id == movie_id; })) {
            std::cout << "Phim không tồn tại!" << std::endl;
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }

        std::cout << "Nhập phòng (e.g., R01, nhập '0' để hủy): ";
        std::cin >> room;
        if (room == "0") {
            std::cout << "Hủy thêm suất chiếu.\n";
            return;
        }

        std::cout << "Nhập ngày chiếu (dd/mm/yyyy, nhập '0' để hủy): ";
        std::cin >> date;
        if (date == "0") {
            std::cout << "\nHủy thêm suất chiếu.\n";
            return;
        }

        std::regex date_regex(R"(\d{2}/\d{2}/\d{4})");
        if (!std::regex_match(date, date_regex)) {
            std::cout << "Định dạng ngày không hợp lệ! Vui lòng nhập theo dạng dd/mm/yyyy.\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }

        std::cout << "Nhập giờ bắt đầu (hh:mm, nhập '0' để hủy): ";
        std::cin >> time;
        if (time == "0") {
            std::cout << "Hủy thêm suất chiếu.\n";
            return;
        }

        std::regex time_input_regex(R"(\d{2}:\d{2})");
        if (!std::regex_match(time, time_input_regex)) {
            std::cout << "Định dạng giờ không hợp lệ! Vui lòng nhập theo dạng hh:mm.\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }

        int start_hour, start_minute;
        try {
            start_hour = std::stoi(time.substr(0, 2));
            start_minute = std::stoi(time.substr(3, 2));
            if (start_hour < 0 || start_hour > 23 || start_minute < 0 || start_minute > 59) {
                std::cout << "Giờ hoặc phút không hợp lệ!\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
        } catch (const std::exception& e) {
            std::cout << "Lỗi định dạng giờ: " << e.what() << std::endl;
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }

        std::cout << "Nhập giá vé (VND, nhập '0' để hủy): ";
        if (!(std::cin >> ticket_price)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Giá vé không hợp lệ!\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }
        if (ticket_price == 0) {
            std::cout << "Hủy thêm suất chiếu.\n";
            return;
        }
        if (ticket_price < 0) {
            std::cout << "Giá vé phải lớn hơn 0!\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }

        bool conflict = false;
        int new_start_minutes = start_hour * 60 + start_minute;
        int new_end_minutes = new_start_minutes + 180;

        for (const auto& showtime : showtimes) {
            if (showtime.room == room && showtime.date == date) {
                std::string existing_time = showtime.time.substr(0, 5);
                int existing_start_hour, existing_start_minute;
                try {
                    existing_start_hour = std::stoi(existing_time.substr(0, 2));
                    existing_start_minute = std::stoi(existing_time.substr(3, 2));
                } catch (const std::exception& e) {
                    std::cout << "Lỗi định dạng thời gian suất chiếu hiện có: " << e.what() << std::endl;
                    continue;
                }

                int existing_start_minutes = existing_start_hour * 60 + existing_start_minute;
                int existing_end_minutes = existing_start_minutes + 180;

                if ((new_start_minutes <= existing_end_minutes && new_end_minutes >= existing_start_minutes)) {
                    conflict = true;
                    std::cout << "Xung đột suất chiếu! Phòng " << room << " đã được sử dụng vào ngày " << date
                              << " từ " << existing_time << ".\n";
                    break;
                }
            }
        }

        if (conflict) {
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
            continue;
        }

        std::string date_code = date.substr(0, 2) + date.substr(3, 2) + date.substr(8, 2);
        std::string time_code = time.substr(0, 2) + time.substr(3, 2);
        std::string showtime_id = movie_id + room + date_code + time_code;

        json new_showtime = {
            {"Mã suất chiếu", showtime_id},
            {"Mã phim", movie_id},
            {"Phòng", room},
            {"Ngày chiếu", date},
            {"Giờ chiếu", time + " - " + std::to_string(start_hour + 3) + time.substr(2, 3)},
            {"Giá vé", ticket_price}
        };

        showtimes.emplace_back(new_showtime);

        json showtime_json = json::array();
        for (const auto& showtime : showtimes) {
            showtime_json.push_back(showtime.toJson());
        }

        try {
            data_manager.writeJsonFile("Showtime.json", showtime_json);
            std::cout << "Thêm suất chiếu thành công! Mã suất chiếu: " << showtime_id << std::endl;
            return;
        } catch (const std::exception& e) {
            std::cout << "Lỗi khi lưu suất chiếu: " << e.what() << std::endl;
            return;
        }
    }

    std::cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
}

void ShowtimeManager::editShowtime() {
    std::string id;
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    std::cout << "\nNhập mã suất chiếu cần sửa (nhập '0' để hủy): ";
    std::cin >> id;
    if (id == "0") {
        std::cout << "Hủy sửa suất chiếu.\n";
        return;
    }

    auto it = std::find_if(showtimes.begin(), showtimes.end(), [&id](const Showtime& s) { return s.id == id; });
    if (it == showtimes.end()) {
        std::cout << "Suất chiếu không tồn tại!" << std::endl;
        return;
    }

    std::string movie_id, room, date, time;
    int ticket_price;

    while (attempts < MAX_ATTEMPTS) {
        std::cout << "Nhập mã phim mới (nhập '0' để hủy): ";
        std::cin >> movie_id;
        if (movie_id == "0") {
            std::cout << "Hủy sửa suất chiếu.\n";
            return;
        }
        if (std::none_of(movies.begin(), movies.end(), [&movie_id](const Movie& m) { return m.id == movie_id; })) {
            std::cout << "Phim không tồn tại!" << std::endl;
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }

        std::cout << "Nhập phòng mới (e.g., R01, nhập '0' để hủy): ";
        std::cin >> room;
        if (room == "0") {
            std::cout << "Hủy sửa suất chiếu.\n";
            return;
        }

        std::cout << "Nhập ngày chiếu mới (dd/mm/yyyy, nhập '0' để hủy): ";
        std::cin >> date;
        if (date == "0") {
            std::cout << "Hủy sửa suất chiếu.\n";
            return;
        }

        std::regex date_regex(R"(\d{2}/\d{2}/\d{4})");
        if (!std::regex_match(date, date_regex)) {
            std::cout << "Định dạng ngày không hợp lệ! Vui lòng nhập theo dạng dd/mm/yyyy.\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }

        std::cout << "Nhập giờ bắt đầu mới (hh:mm, nhập '0' để hủy): ";
        std::cin >> time;
        if (time == "0") {
            std::cout << "Hủy sửa suất chiếu.\n";
            return;
        }

        std::regex time_input_regex(R"(\d{2}:\d{2})");
        if (!std::regex_match(time, time_input_regex)) {
            std::cout << "Định dạng giờ không hợp lệ! Vui lòng nhập theo dạng hh:mm.\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }

        int start_hour, start_minute;
        try {
            start_hour = std::stoi(time.substr(0, 2));
            start_minute = std::stoi(time.substr(3, 2));
            if (start_hour < 0 || start_hour > 23 || start_minute < 0 || start_minute > 59) {
                std::cout << "Giờ hoặc phút không hợp lệ!\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
        } catch (const std::exception& e) {
            std::cout << "Lỗi định dạng giờ: " << e.what() << std::endl;
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }

        std::cout << "Nhập giá vé mới (VND, nhập '0' để hủy): ";
        if (!(std::cin >> ticket_price)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Giá vé không hợp lệ!\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }
        if (ticket_price == 0) {
            std::cout << "Hủy sửa suất chiếu.\n";
            return;
        }
        if (ticket_price < 0) {
            std::cout << "Giá vé phải lớn hơn 0!\n";
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }

        bool conflict = false;
        int new_start_minutes = start_hour * 60 + start_minute;
        int new_end_minutes = new_start_minutes + 180;

        for (const auto& showtime : showtimes) {
            if (showtime.id == id) continue;
            if (showtime.room == room && showtime.date == date) {
                std::string existing_time = showtime.time.substr(0, 5);
                int existing_start_hour, existing_start_minute;
                try {
                    existing_start_hour = std::stoi(existing_time.substr(0, 2));
                    existing_start_minute = std::stoi(existing_time.substr(3, 2));
                } catch (const std::exception& e) {
                    std::cout << "Lỗi định dạng thời gian suất chiếu hiện có: " << e.what() << std::endl;
                    continue;
                }

                int existing_start_minutes = existing_start_hour * 60 + existing_start_minute;
                int existing_end_minutes = existing_start_minutes + 180;

                if ((new_start_minutes <= existing_end_minutes && new_end_minutes >= existing_start_minutes)) {
                    conflict = true;
                    std::cout << "Xung đột suất chiếu! Phòng " << room << " đã được sử dụng vào ngày " << date
                              << " từ " << existing_time << ".\n";
                    break;
                }
            }
        }

        if (conflict) {
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
            continue;
        }

        std::string date_code = date.substr(0, 2) + date.substr(3, 2) + date.substr(8, 2);
        std::string time_code = time.substr(0, 2) + time.substr(3, 2);
        std::string new_id = movie_id + room + date_code + time_code;

        it->id = new_id;
        it->movie_id = movie_id;
        it->room = room;
        it->date = date;
        it->time = time + " - " + std::to_string(start_hour + 3) + time.substr(2, 3);
        it->ticket_price = ticket_price;

        for (auto& booking : bookings) {
            if (booking.showtime_id == id) {
                booking.showtime_id = new_id;
                booking.ticket_id = new_id + booking.seat;
            }
        }

        json showtime_json = json::array();
        for (const auto& showtime : showtimes) {
            showtime_json.push_back(showtime.toJson());
        }

        json booking_json = json::object();
        for (const auto& booking : bookings) {
            std::string showtime_id = booking.showtime_id;
            if (booking_json.find(showtime_id) == booking_json.end()) {
                booking_json[showtime_id] = json::array();
            }
            booking_json[showtime_id].push_back(booking.toJson());
        }

        try {
            data_manager.writeJsonFile("Showtime.json", showtime_json);
            data_manager.writeJsonFile("Booking.json", booking_json);
            std::cout << "Sửa suất chiếu thành công! Mã suất chiếu mới: " << new_id << std::endl;
            return;
        } catch (const std::exception& e) {
            std::cout << "Lỗi khi lưu suất chiếu: " << e.what() << std::endl;
            return;
        }
    }

    std::cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
}

void ShowtimeManager::deleteShowtime() {
    std::string id;
    std::cout << "\nNhập mã suất chiếu cần xóa (nhập '0' để hủy): ";
    std::cin >> id;
    if (id == "0") {
        std::cout << "Hủy xóa suất chiếu.\n";
        return;
    }

    for (const auto& booking : bookings) {
        if (booking.showtime_id == id) {
            std::cout << "Không thể xóa suất chiếu vì đã có vé được đặt!" << std::endl;
            return;
        }
    }

    auto it = std::find_if(showtimes.begin(), showtimes.end(), [&id](const Showtime& s) { return s.id == id; });
    if (it == showtimes.end()) {
        std::cout << "Suất chiếu không tồn tại!" << std::endl;
        return;
    }

    showtimes.erase(it);

    json showtime_json = json::array();
    for (const auto& showtime : showtimes) {
        showtime_json.push_back(showtime.toJson());
    }

    try {
        data_manager.writeJsonFile("Showtime.json", showtime_json);
        std::cout << "Xóa suất chiếu thành công!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi xóa suất chiếu: " << e.what() << std::endl;
    }
}