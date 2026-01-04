#include "MovieManager.h"
#include "Showtime.h"
#include <iostream>
#include <algorithm>
#include <limits>

MovieManager::MovieManager(DataManager& dm, std::vector<Movie>& m, const std::vector<Showtime>& s)
    : data_manager(dm), movies(m), showtimes(s) {}

void MovieManager::loadMovies() {
    try {
        json movie_json = data_manager.readJsonFile("Movies.json");
        movies.clear();
        for (const auto& item : movie_json) {
            movies.emplace_back(item);
        }
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi tải phim: " << e.what() << std::endl;
    }
}

void MovieManager::displayMovies() const {
    std::cout << "\nDanh sách phim:\n";
    for (const auto& movie : movies) {
        movie.display();
    }
}

void MovieManager::addMovie() {
    int max_id = 0;
    for (const auto& movie : movies) {
        try {
            std::string num_str = movie.id.substr(3);
            int num = std::stoi(num_str);
            if (num > max_id) max_id = num;
        } catch (const std::exception& e) {
            std::cout << "Lỗi khi chuyển đổi mã phim: " << e.what() << std::endl;
            continue;
        }
    }
    std::string new_id = "MOV" + std::string(4 - std::to_string(max_id + 1).length(), '0') + std::to_string(max_id + 1);

    std::string title;
    int duration;

    std::cin.ignore();
    std::cout << "Nhập tên phim (nhập '0' để hủy): ";
    std::getline(std::cin, title);
    if (title == "0") {
        std::cout << "Hủy thêm phim.\n";
        return;
    }
    if (title.empty()) {
        std::cout << "Tên phim không được để trống!\n";
        return;
    }
    std::cout << "Nhập thời lượng (phút, nhập '0' để hủy): ";
    if (!(std::cin >> duration)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Thời lượng không hợp lệ!\n";
        return;
    }
    if (duration == 0) {
        std::cout << "Hủy thêm phim.\n";
        return;
    }
    if (duration < 0) {
        std::cout << "Thời lượng phải lớn hơn 0!\n";
        return;
    }

    json new_movie = {
        {"Mã phim", new_id},
        {"Tên phim", title},
        {"Thời lượng", duration}
    };

    movies.emplace_back(new_movie);

    json movie_json = json::array();
    for (const auto& movie : movies) {
        movie_json.push_back(movie.toJson());
    }

    try {
        data_manager.writeJsonFile("Movies.json", movie_json);
        std::cout << "Thêm phim thành công! Mã phim: " << new_id << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi lưu phim: " << e.what() << std::endl;
    }
}

void MovieManager::editMovie() {
    std::string id;
    std::cout << "\nNhập mã phim cần sửa (nhập '0' để hủy): ";
    std::cin >> id;
    if (id == "0") {
        std::cout << "Hủy sửa phim.\n";
        return;
    }

    auto it = std::find_if(movies.begin(), movies.end(), [&id](const Movie& m) { return m.id == id; });
    if (it == movies.end()) {
        std::cout << "Phim không tồn tại!" << std::endl;
        return;
    }

    std::string title;
    int duration;
    std::cin.ignore();
    std::cout << "Nhập tên phim mới (nhập '0' để hủy): ";
    std::getline(std::cin, title);
    if (title == "0") {
        std::cout << "Hủy sửa phim.\n";
        return;
    }
    if (title.empty()) {
        std::cout << "Tên phim không được để trống!\n";
        return;
    }
    std::cout << "Nhập thời lượng mới (phút, nhập '0' để hủy): ";
    if (!(std::cin >> duration)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Thời lượng không hợp lệ!\n";
        return;
    }
    if (duration == 0) {
        std::cout << "Hủy sửa phim.\n";
        return;
    }
    if (duration < 0) {
        std::cout << "Thời lượng phải lớn hơn 0!\n";
        return;
    }

    it->title = title;
    it->duration = duration;

    json movie_json = json::array();
    for (const auto& movie : movies) {
        movie_json.push_back(movie.toJson());
    }

    try {
        data_manager.writeJsonFile("Movies.json", movie_json);
        std::cout << "Sửa phim thành công!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi lưu phim: " << e.what() << std::endl;
    }
}

void MovieManager::deleteMovie() {
    std::string id;
    std::cout << "\nNhập mã phim cần xóa (nhập '0' để hủy): ";
    std::cin >> id;
    if (id == "0") {
        std::cout << "Hủy xóa phim.\n";
        return;
    }

    for (const auto& showtime : showtimes) {
        if (showtime.movie_id == id) {
            std::cout << "Không thể xóa phim vì đang có suất chiếu!" << std::endl;
            return;
        }
    }

    auto it = std::find_if(movies.begin(), movies.end(), [&id](const Movie& m) { return m.id == id; });
    if (it == movies.end()) {
        std::cout << "Phim không tồn tại!" << std::endl;
        return;
    }

    movies.erase(it);

    json movie_json = json::array();
    for (const auto& movie : movies) {
        movie_json.push_back(movie.toJson());
    }

    try {
        data_manager.writeJsonFile("Movies.json", movie_json);
        std::cout << "Xóa phim thành công!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Lỗi khi xóa phim: " << e.what() << std::endl;
    }
}   