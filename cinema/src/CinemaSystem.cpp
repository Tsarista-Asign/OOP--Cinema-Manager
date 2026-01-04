#include "CinemaSystem.h"
#include <iostream>
#include <limits>

CinemaSystem::CinemaSystem()
    : customer_manager(data_manager, customers),
      movie_manager(data_manager, movies, showtimes),
      showtime_manager(data_manager, showtimes, movies, bookings),
      booking_manager(data_manager, customers, movies, showtimes, bookings) {
    customer_manager.loadCustomers();
    movie_manager.loadMovies();
    showtime_manager.loadShowtimes();
    booking_manager.loadBookings();
}

void CinemaSystem::run() {
    while (true) {
        std::cout << "\nHệ thống quản lý rạp chiếu phim\n";
        std::cout << "1. Xem danh sách khách hàng\n";
        std::cout << "2. Quản lý phim\n";
        std::cout << "3. Quản lý lịch chiếu phim\n";
        std::cout << "4. Đặt vé\n";
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
                customer_manager.displayCustomers();
                break;
            case 2: {
                while (true) {
                    movie_manager.displayMovies();
                    std::cout << "\nQuản lý phim\n";
                    std::cout << "1. Thêm phim mới\n";
                    std::cout << "2. Sửa thông tin phim\n";
                    std::cout << "3. Xóa phim\n";
                    std::cout << "4. Đặt vé cho phim\n";
                    std::cout << "0. Quay về menu chính\n";
                    std::cout << "Nhập lựa chọn: ";

                    int movie_choice;
                    if (!(std::cin >> movie_choice)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Lựa chọn không hợp lệ!\n";
                        continue;
                    }

                    switch (movie_choice) {
                        case 1:
                            movie_manager.addMovie();
                            break;
                        case 2:
                            movie_manager.editMovie();
                            break;
                        case 3:
                            movie_manager.deleteMovie();
                            break;
                        case 4: {
                            std::cout << "\nNhập mã phim để đặt vé (nhập '0' để hủy): ";
                            std::string movie_id;
                            std::cin >> movie_id;
                            if (movie_id == "0") {
                                std::cout << "Hủy đặt vé.\n";
                                break;
                            }
                            if (std::none_of(movies.begin(), movies.end(),
                                [&movie_id](const Movie& m) { return m.id == movie_id; })) {
                                std::cout << "Phim không tồn tại!\n";
                                break;
                            }
                            booking_manager.bookTicket(movie_id);
                            break;
                        }
                        case 0:
                            goto main_menu;
                        default:
                            std::cout << "Lựa chọn không hợp lệ!\n";
                    }
                }
            }
            case 3: {
                while (true) {
                    showtime_manager.displayShowtimes();
                    std::cout << "\nQuản lý lịch chiếu phim\n";
                    std::cout << "1. Thêm suất chiếu\n";
                    std::cout << "2. Sửa thông tin suất chiếu\n";
                    std::cout << "3. Xóa suất chiếu\n";
                    std::cout << "0. Quay về menu chính\n";
                    std::cout << "Nhập lựa chọn: ";

                    int showtime_choice;
                    if (!(std::cin >> showtime_choice)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Lựa chọn không hợp lệ!\n";
                        continue;
                    }

                    switch (showtime_choice) {
                        case 1:
                            showtime_manager.addShowtime();
                            break;
                        case 2:
                            showtime_manager.editShowtime();
                            break;
                        case 3:
                            showtime_manager.deleteShowtime();
                            break;
                        case 0:
                            goto main_menu;
                        default:
                            std::cout << "Lựa chọn không hợp lệ!\n";
                    }
                }
            }
            case 4:
                booking_manager.bookTicket();
                break;
            case 0:
                std::cout << "Đang thoát...\n";
                return;
            default:
                std::cout << "Lựa chọn không hợp lệ!\n";
        }
    main_menu:;
    }
}