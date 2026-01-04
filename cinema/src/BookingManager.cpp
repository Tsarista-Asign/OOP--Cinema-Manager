#include "BookingManager.h"
#include "CustomerManager.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>

BookingManager::BookingManager(DataManager& dm, std::vector<Customer>& c, std::vector<Movie>& m,
                               std::vector<Showtime>& s, std::vector<Booking>& b)
    : data_manager(dm), customers(c), movies(m), showtimes(s), bookings(b) {}

void BookingManager::loadBookings() {
    try {
        json booking_json = data_manager.readJsonFile("Booking.json");
        bookings.clear();
        for (const auto& [showtime_id, booking_array] : booking_json.items()) {
            for (const auto& item : booking_array) {
                bookings.emplace_back(item);
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi tải đặt vé: " << e.what() << std::endl;
    }
}

void BookingManager::bookTicket(const std::string& initial_movie_id) {
    CustomerManager customer_manager(data_manager, customers);
    std::string cccd;
    bool customer_selected = false;
    Customer* selected_customer = nullptr;

    while (!customer_selected) {
        std::cout << "\nNhập CCCD khách hàng (nhập '0' để thoát): ";
        std::cin >> cccd;
        if (cccd == "0") {
            std::cout << "Hủy đặt vé.\n";
            return;
        }
        if (cccd.empty()) {
            std::cout << "CCCD không được để trống!\n";
            continue;
        }

        selected_customer = customer_manager.findCustomer(cccd);

        if (selected_customer) {
            bool customer_handled = false;
            while (!customer_handled) {
                std::cout << "\nKhách hàng đã tồn tại:\n";
                selected_customer->display();
                std::cout << "1. Xác nhận sử dụng thông tin này\n";
                std::cout << "2. Sửa thông tin khách hàng\n";
                std::cout << "3. Nhập CCCD khác\n";
                std::cout << "0. Thoát\n";
                std::cout << "Nhập lựa chọn: ";

                int choice;
                if (!(std::cin >> choice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Lựa chọn không hợp lệ!\n";
                    continue;
                }

                switch (choice) {
                    case 1:
                        customer_selected = true;
                        customer_handled = true;
                        break;
                    case 2: {
                        std::string name, phone, confirm;
                        std::cin.ignore();
                        std::cout << "Nhập tên khách hàng mới (nhập '0' để hủy): ";
                        std::getline(std::cin, name);
                        if (name == "0") {
                            std::cout << "Hủy sửa thông tin khách hàng.\n";
                            break;
                        }
                        if (name.empty()) {
                            std::cout << "Tên không được để trống!\n";
                            continue;
                        }
                        std::cout << "Nhập số điện thoại mới (nhập '0' để hủy): ";
                        std::getline(std::cin, phone);
                        if (phone == "0") {
                            std::cout << "Hủy sửa thông tin khách hàng.\n";
                            break;
                        }
                        if (phone.empty()) {
                            std::cout << "Số điện thoại không được để trống!\n";
                            continue;
                        }
                        std::cout << "Xác nhận cập nhật thông tin? (y/n): ";
                        std::cin >> confirm;

                        if (std::tolower(confirm[0]) == 'y') {
                            customer_manager.updateCustomer(cccd, name, phone);
                            selected_customer = customer_manager.findCustomer(cccd);
                            customer_selected = true;
                            customer_handled = true;
                        } else {
                            std::cout << "Hủy cập nhật thông tin.\n";
                        }
                        break;
                    }
                    case 3:
                        customer_handled = true;
                        break;
                    case 0:
                        std::cout << "Hủy đặt vé.\n";
                        return;
                    default:
                        std::cout << "Lựa chọn không hợp lệ!\n";
                }
            }
        } else {
            std::string name, phone, confirm;
            std::cin.ignore();
            std::cout << "Khách hàng không tồn tại. Nhập tên khách hàng (nhập '0' để hủy): ";
            std::getline(std::cin, name);
            if (name == "0") {
                std::cout << "Hủy lưu thông tin khách hàng.\n";
                continue;
            }
            if (name.empty()) {
                std::cout << "Tên không được để trống!\n";
                continue;
            }
            std::cout << "Nhập số điện thoại (nhập '0' để hủy): ";
            std::getline(std::cin, phone);
            if (phone == "0") {
                std::cout << "Hủy lưu thông tin khách hàng.\n";
                continue;
            }
            if (phone.empty()) {
                std::cout << "Số điện thoại không được để trống!\n";
                continue;
            }
            std::cout << "Xác nhận lưu thông tin khách hàng mới? (y/n): ";
            std::cin >> confirm;

            if (std::tolower(confirm[0]) == 'y') {
                customer_manager.addCustomer(cccd, name, phone);
                selected_customer = customer_manager.findCustomer(cccd);
                customer_selected = true;
            } else {
                std::cout << "Hủy lưu thông tin khách hàng." << std::endl;
                continue;
            }
        }
    }

    Movie* selected_movie = nullptr;
    if (!initial_movie_id.empty()) {
        auto it = std::find_if(movies.begin(), movies.end(),
            [&initial_movie_id](const Movie& m) { return m.id == initial_movie_id; });
        if (it == movies.end()) {
            std::cout << "Phim không tồn tại!\n";
            return;
        }
        selected_movie = &(*it);
    }

    bool movie_selected = (selected_movie != nullptr);
    while (!movie_selected) {
        std::cout << "\nDanh sách phim:\n";
        for (size_t i = 0; i < movies.size(); ++i) {
            int showtime_count = 0;
            int booked_seats = 0;
            for (const auto& showtime : showtimes) {
                if (showtime.movie_id == movies[i].id) {
                    showtime_count++;
                    booked_seats += std::count_if(bookings.begin(), bookings.end(),
                        [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
                }
            }
            int total_seats = showtime_count * 36;
            bool is_sold_out = (total_seats > 0 && booked_seats >= total_seats);
            std::cout << i + 1 << ". " << movies[i].title
                      << (is_sold_out ? " - Đã hết vé!!" : "") << std::endl;
        }

        int movie_choice;
        std::cout << "Nhập số thứ tự phim (0 để thoát): ";
        if (!(std::cin >> movie_choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Lựa chọn không hợp lệ!\n";
            continue;
        }
        if (movie_choice == 0) {
            std::cout << "Hủy đặt vé.\n";
            return;
        }
        if (movie_choice < 1 || movie_choice > static_cast<int>(movies.size())) {
            std::cout << "Lựa chọn không hợp lệ!\n";
            continue;
        }

        selected_movie = &movies[movie_choice - 1];

        int showtime_count = 0;
        int booked_seats = 0;
        for (const auto& showtime : showtimes) {
            if (showtime.movie_id == selected_movie->id) {
                showtime_count++;
                booked_seats += std::count_if(bookings.begin(), bookings.end(),
                    [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
            }
        }
        int total_seats = showtime_count * 36;
        if (total_seats > 0 && booked_seats >= total_seats) {
            std::cout << "Phim này đã hết vé!" << std::endl;
            selected_movie = nullptr;
            continue;
        }
        movie_selected = true;
    }

    std::vector<Showtime> valid_showtimes;
    for (const auto& showtime : showtimes) {
        if (showtime.movie_id == selected_movie->id) {
            int booked_seats = std::count_if(bookings.begin(), bookings.end(),
                [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
            if (booked_seats < 36) {
                valid_showtimes.push_back(showtime);
            }
        }
    }

    if (valid_showtimes.empty()) {
        std::cout << "Không còn suất chiếu nào có chỗ trống cho phim này!" << std::endl;
        return;
    }

    Showtime* selected_showtime = nullptr;
    bool showtime_selected = false;
    while (!showtime_selected) {
        std::cout << "\nDanh sách suất chiếu khả dụng:\n";
        for (size_t i = 0; i < valid_showtimes.size(); ++i) {
            int booked_seats = std::count_if(bookings.begin(), bookings.end(),
                [&valid_showtimes, i](const Booking& b) { return b.showtime_id == valid_showtimes[i].id; });
            std::cout << i + 1 << ". ";
            valid_showtimes[i].display(movies, booked_seats);
        }

        int showtime_choice;
        std::cout << "Nhập số thứ tự suất chiếu (0 để quay lại): ";
        if (!(std::cin >> showtime_choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Lựa chọn không hợp lệ!\n";
            continue;
        }
        if (showtime_choice == 0) {
            if (!initial_movie_id.empty()) return;
            movie_selected = false;
            selected_movie = nullptr;
            break;
        }
        if (showtime_choice < 1 || showtime_choice > static_cast<int>(valid_showtimes.size())) {
            std::cout << "Lựa chọn không hợp lệ!\n";
            continue;
        }

        selected_showtime = &valid_showtimes[showtime_choice - 1];
        showtime_selected = true;

        bool seats_selected = false;
        std::vector<std::string> selected_seats;
        while (!seats_selected) {
            std::cout << "\nSơ đồ ghế (Trống = số ghế, Full = đã đặt):\n";
            std::vector<std::string> rows = {"A", "B", "C", "D", "E", "F"};
            for (const auto& row : rows) {
                std::cout << row << " ";
                for (int col = 1; col <= 6; ++col) {
                    std::string seat = row + (col < 10 ? "0" + std::to_string(col) : std::to_string(col));
                    bool booked = std::any_of(bookings.begin(), bookings.end(),
                        [&seat, &selected_showtime](const Booking& b) {
                            return b.showtime_id == selected_showtime->id && b.seat == seat;
                        });
                    std::cout << std::setw(8) << (booked ? "Full" : seat + " ");
                }
                std::cout << std::endl;
            }

            int num_seats;
            std::cout << "\nNhập số ghế muốn đặt (0 để quay lại): ";
            if (!(std::cin >> num_seats)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Số ghế không hợp lệ!\n";
                continue;
            }
            if (num_seats == 0) {
                showtime_selected = false;
                break;
            }
            if (num_seats < 0 || num_seats > 36) {
                std::cout << "Số ghế không hợp lệ! Vui lòng nhập số ghế từ 1 đến 36.\n";
                continue;
            }

            selected_seats.clear();
            for (int i = 0; i < num_seats; ++i) {
                std::string seat;
                std::cout << "Nhập ghế thứ " << i + 1 << " (e.g., A01, nhập '0' để hủy): ";
                std::cin >> seat;
                if (seat == "0") {
                    std::cout << "Hủy chọn ghế.\n";
                    selected_seats.clear();
                    break;
                }

                if (seat.length() != 3 || std::find(rows.begin(), rows.end(), seat.substr(0, 1)) == rows.end() ||
                    std::stoi(seat.substr(1)) < 1 || std::stoi(seat.substr(1)) > 6) {
                    std::cout << "Ghế không hợp lệ!" << std::endl;
                    --i;
                    continue;
                }

                if (std::any_of(bookings.begin(), bookings.end(),
                    [&seat, &selected_showtime](const Booking& b) {
                        return b.showtime_id == selected_showtime->id && b.seat == seat;
                    })) {
                    std::cout << "Ghế đã được đặt!" << std::endl;
                    --i;
                    continue;
                }

                if (std::find(selected_seats.begin(), selected_seats.end(), seat) != selected_seats.end()) {
                    std::cout << "Ghế đã được chọn!" << std::endl;
                    --i;
                    continue;
                }

                selected_seats.push_back(seat);
            }
            if (selected_seats.empty()) continue;

            std::cout << "\nXác nhận đặt vé cho các ghế: ";
            for (const auto& seat : selected_seats) std::cout << seat << " ";
            std::cout << "(y/n): ";
            std::string confirm;
            std::cin >> confirm;

            if (std::tolower(confirm[0]) == 'y') {
                std::vector<std::pair<std::string, std::string>> ticket_info;
                for (const auto& seat : selected_seats) {
                    std::string ticket_id = selected_showtime->id + seat;
                    ticket_info.emplace_back(ticket_id, seat);
                    json new_booking = {
                        {"Mã vé", ticket_id},
                        {"CCCD", cccd},
                        {"Suất chiếu", selected_showtime->id},
                        {"Ghế", seat}
                    };
                    bookings.emplace_back(new_booking);
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
                    data_manager.writeJsonFile("Booking.json", booking_json);

                    std::cout << "\n=== HÓA ĐƠN ĐẶT VÉ ===\n";
                    std::cout << "Tên khách hàng: " << selected_customer->name << std::endl;
                    std::cout << "Tên phim: " << selected_movie->title << std::endl;
                    std::cout << "Giờ chiếu: " << selected_showtime->time << std::endl;
                    std::cout << "Ngày chiếu: " << selected_showtime->date << std::endl;
                    std::cout << "Số vé: " << selected_seats.size() << std::endl;
                    std::cout << "Thông tin vé:\n";
                    for (const auto& [ticket_id, seat] : ticket_info) {
                        std::cout << "Mã vé: " << ticket_id << ", Ghế: " << seat << std::endl;
                    }
                    std::cout << "Tổng giá: " << (selected_seats.size() * selected_showtime->ticket_price) << " VND\n";
                    std::cout << "======================\n";
                    return;
                } catch (const std::exception& e) {
                    std::cout << "Lỗi khi lưu đặt vé: " << e.what() << std::endl;
                    return;
                }
            } else {
                std::cout << "Hủy đặt vé. Quay lại bước chọn ghế." << std::endl;
                continue;
            }
        }
    }
}