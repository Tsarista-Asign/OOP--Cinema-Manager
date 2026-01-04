#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <regex>

using json = nlohmann::json;
using namespace std;

const string BASE_PATH = "C:/Users/Orias.ASUS/Documents/Code/1. C-C++/src/Cinema/";

class Customer {
public:
    string cccd;
    string name;
    string phone;

    Customer(const json& j) {
        cccd = j["CCCD"];
        name = j["Tên khách"];
        phone = j["Số điện thoại"];
    }

    void display() const {
        cout << "CCCD: " << cccd << ", Tên: " << name << ", Số điện thoại: " << phone << endl;
    }
};

class Movie {
public:
    string id;
    string title;
    int duration;

    Movie(const json& j) {
        id = j["Mã phim"];
        title = j["Tên phim"];
        duration = j["Thời lượng"];
    }

    void display() const {
        cout << "Mã phim: " << id << ", Tên phim: " << title << ", Thời lượng: " << duration << " phút" << endl;
    }
};

class Showtime {
public:
    string id;
    string movie_id;
    string room;
    string date;
    string time;
    int ticket_price;

    Showtime(const json& j) {
        id = j["Mã suất chiếu"];
        movie_id = j["Mã phim"];
        room = j["Phòng"];
        date = j["Ngày chiếu"];
        time = j["Giờ chiếu"];
        ticket_price = j.value("Giá vé", 100000); // Giá mặc định nếu không có
    }

    void display(const vector<Movie>& movies, int booked_seats) const {
        string movie_title = "Unknown";
        for (const auto& movie : movies) {
            if (movie.id == movie_id) {
                movie_title = movie.title;
                break;
            }
        }
        cout << "Mã suất chiếu: " << id << ", Mã phim: " << movie_id 
             << ", Tên phim: " << movie_title << ", Phòng: " << room 
             << ", Ngày chiếu: " << date << ", Giờ chiếu: " << time 
             << ", Giá vé: " << ticket_price << " VND"
             << (booked_seats >= 36 ? " - Đã hết vé!!" : "") << endl;
    }
};

class Booking {
public:
    string ticket_id;
    string cccd;
    string showtime_id;
    string seat;

    Booking(const json& j) {
        ticket_id = j["Mã vé"];
        cccd = j["CCCD"];
        showtime_id = j["Suất chiếu"];
        seat = j["Ghế"];
    }

    void display() const {
        cout << "Mã vé: " << ticket_id << ", CCCD: " << cccd 
             << ", Mã suất chiếu: " << showtime_id << ", Ghế: " << seat << endl;
    }
};

class CinemaManager {
private:
    vector<Customer> customers;
    vector<Movie> movies;
    vector<Showtime> showtimes;
    vector<Booking> bookings;

    json readJsonFile(const string& filename) {
        ifstream file(BASE_PATH + filename);
        if (!file.is_open()) {
            throw runtime_error("Không thể mở file: " + filename);
        }
        json j;
        file >> j;
        file.close();
        return j;
    }

    void writeJsonFile(const string& filename, const json& j) {
        ofstream file(BASE_PATH + filename);
        if (!file.is_open()) {
            throw runtime_error("Không thể ghi vào file: " + filename);
        }
        file << j.dump(4);
        file.close();
    }

    string getCurrentDateTime() {
        time_t now = std::time(nullptr);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << put_time(ltm, "%d/%m/%Y %H:%M");
        return ss.str();
    }

    bool isShowtimeValid(const string& date, const string& time) {
        std::time_t now = std::time(nullptr);
        tm current_tm = *localtime(&now);

        regex date_regex(R"(\d{2}/\d{2}/\d{4})");
        regex time_regex(R"(\d{2}:\d{2}\s*-\s*\d{2}:\d{2})");
        if (!regex_match(date, date_regex) || !regex_match(time, time_regex)) {
            return false;
        }

        int day, month, year;
        try {
            day = stoi(date.substr(0, 2));
            month = stoi(date.substr(3, 2));
            year = stoi(date.substr(6, 4));
        } catch (const exception& e) {
            return false;
        }

        int hour, minute;
        try {
            string start_time = time.substr(0, 5);
            hour = stoi(start_time.substr(0, 2));
            minute = stoi(start_time.substr(3, 2));
        } catch (const exception& e) {
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

        time_t showtime = mktime(&showtime_tm);
        if (showtime == -1) {
            return false;
        }

        return showtime > now;
    }

public:
    CinemaManager() {
        loadData();
    }

    void loadData() {
        try {
            json customer_json = readJsonFile("Customer.json");
            customers.clear();
            for (const auto& item : customer_json) {
                customers.emplace_back(item);
            }

            json movie_json = readJsonFile("Movies.json");
            movies.clear();
            for (const auto& item : movie_json) {
                movies.emplace_back(item);
            }

            json showtime_json = readJsonFile("Showtime.json");
            showtimes.clear();
            for (const auto& item : showtime_json) {
                showtimes.emplace_back(item);
            }

            json booking_json = readJsonFile("Booking.json");
            bookings.clear();
            for (const auto& [showtime_id, booking_array] : booking_json.items()) {
                for (const auto& item : booking_array) {
                    bookings.emplace_back(item);
                }
            }
        } catch (const exception& e) {
            cout << "Lỗi khi tải dữ liệu: " << e.what() << endl;
        }
    }

    void displayCustomers() {
        cout << "\nDanh sách khách hàng:\n";
        for (const auto& customer : customers) {
            customer.display();
        }
    }

    void displayMovies() {
        cout << "\nDanh sách phim:\n";
        for (const auto& movie : movies) {
            movie.display();
        }
    }

    void displayShowtimes() {
        cout << "\nLịch chiếu phim:\n";
        for (const auto& showtime : showtimes) {
            int booked_seats = count_if(bookings.begin(), bookings.end(),
                [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
            showtime.display(movies, booked_seats);
        }
    }

    void addMovie() {
        int max_id = 0;
        for (const auto& movie : movies) {
            try {
                string num_str = movie.id.substr(3);
                int num = stoi(num_str);
                if (num > max_id) max_id = num;
            } catch (const exception& e) {
                cout << "Lỗi khi chuyển đổi mã phim: " << e.what() << endl;
                continue;
            }
        }
        string new_id = "MOV" + string(4 - to_string(max_id + 1).length(), '0') + to_string(max_id + 1);

        string title;
        int duration;

        cin.ignore();
        cout << "Nhập tên phim (nhập '0' để hủy): ";
        getline(cin, title);
        if (title == "0") {
            cout << "Hủy thêm phim.\n";
            return;
        }
        if (title.empty()) {
            cout << "Tên phim không được để trống!\n";
            return;
        }
        cout << "Nhập thời lượng (phút, nhập '0' để hủy): ";
        if (!(cin >> duration)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Thời lượng không hợp lệ!\n";
            return;
        }
        if (duration == 0) {
            cout << "Hủy thêm phim.\n";
            return;
        }
        if (duration < 0) {
            cout << "Thời lượng phải lớn hơn 0!\n";
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
            movie_json.push_back({
                {"Mã phim", movie.id},
                {"Tên phim", movie.title},
                {"Thời lượng", movie.duration}
            });
        }

        try {
            writeJsonFile("Movies.json", movie_json);
            cout << "Thêm phim thành công! Mã phim: " << new_id << endl;
        } catch (const exception& e) {
            cout << "Lỗi khi lưu phim: " << e.what() << endl;
        }
    }

    void editMovie() {
        string id;
        cout << "\nNhập mã phim cần sửa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy sửa phim.\n";
            return;
        }

        auto it = find_if(movies.begin(), movies.end(), [&id](const Movie& m) { return m.id == id; });
        if (it == movies.end()) {
            cout << "Phim không tồn tại!" << endl;
            return;
        }

        string title;
        int duration;
        cin.ignore();
        cout << "Nhập tên phim mới (nhập '0' để hủy): ";
        getline(cin, title);
        if (title == "0") {
            cout << "Hủy sửa phim.\n";
            return;
        }
        if (title.empty()) {
            cout << "Tên phim không được để trống!\n";
            return;
        }
        cout << "Nhập thời lượng mới (phút, nhập '0' để hủy): ";
        if (!(cin >> duration)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Thời lượng không hợp lệ!\n";
            return;
        }
        if (duration == 0) {
            cout << "Hủy sửa phim.\n";
            return;
        }
        if (duration < 0) {
            cout << "Thời lượng phải lớn hơn 0!\n";
            return;
        }

        it->title = title;
        it->duration = duration;

        json movie_json = json::array();
        for (const auto& movie : movies) {
            movie_json.push_back({
                {"Mã phim", movie.id},
                {"Tên phim", movie.title},
                {"Thời lượng", movie.duration}
            });
        }

        try {
            writeJsonFile("Movies.json", movie_json);
            cout << "Sửa phim thành công!" << endl;
        } catch (const exception& e) {
            cout << "Lỗi khi lưu phim: " << e.what() << endl;
        }
    }

    void deleteMovie() {
        string id;
        cout << "\nNhập mã phim cần xóa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy xóa phim.\n";
            return;
        }

        for (const auto& showtime : showtimes) {
            if (showtime.movie_id == id) {
                cout << "Không thể xóa phim vì đang có suất chiếu!" << endl;
                return;
            }
        }

        auto it = find_if(movies.begin(), movies.end(), [&id](const Movie& m) { return m.id == id; });
        if (it == movies.end()) {
            cout << "Phim không tồn tại!" << endl;
            return;
        }

        movies.erase(it);

        json movie_json = json::array();
        for (const auto& movie : movies) {
            movie_json.push_back({
                {"Mã phim", movie.id},
                {"Tên phim", movie.title},
                {"Thời lượng", movie.duration}
            });
        }

        try {
            writeJsonFile("Movies.json", movie_json);
            cout << "Xóa phim thành công!" << endl;
        } catch (const exception& e) {
            cout << "Lỗi khi xóa phim: " << e.what() << endl;
        }
    }

    void addShowtime() {
        string movie_id, room, date, time;
        int ticket_price;
        const int MAX_ATTEMPTS = 3; // Giới hạn số lần thử nhập sai
        int attempts = 0;
    
        while (attempts < MAX_ATTEMPTS) {
            cout << "\nNhập mã phim (nhập '0' để hủy): ";
            cin >> movie_id;
            if (movie_id == "0") {
                cout << "Hủy thêm suất chiếu.\n";
                return;
            }
    
            if (none_of(movies.begin(), movies.end(), [&movie_id](const Movie& m) { return m.id == movie_id; })) {
                cout << "Phim không tồn tại!" << endl;
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            cout << "Nhập phòng (e.g., R01, nhập '0' để hủy): ";
            cin >> room;
            if (room == "0") {
                cout << "Hủy thêm suất chiếu.\n";
                return;
            }
    
            cout << "Nhập ngày chiếu (dd/mm/yyyy, nhập '0' để hủy): ";
            cin >> date;
            if (date == "0") {
                cout << "Hủy thêm suất chiếu.\n";
                return;
            }
    
            // Kiểm tra định dạng ngày
            regex date_regex(R"(\d{2}/\d{2}/\d{4})");
            if (!regex_match(date, date_regex)) {
                cout << "Định dạng ngày không hợp lệ! Vui lòng nhập theo dạng dd/mm/yyyy.\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            cout << "Nhập giờ bắt đầu (hh:mm, nhập '0' để hủy): ";
            cin >> time;
            if (time == "0") {
                cout << "Hủy thêm suất chiếu.\n";
                return;
            }
    
            // Kiểm tra định dạng giờ
            regex time_input_regex(R"(\d{2}:\d{2})");
            if (!regex_match(time, time_input_regex)) {
                cout << "Định dạng giờ không hợp lệ! Vui lòng nhập theo dạng hh:mm.\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            // Kiểm tra giá trị giờ hợp lệ
            int start_hour, start_minute;
            try {
                start_hour = stoi(time.substr(0, 2));
                start_minute = stoi(time.substr(3, 2));
                if (start_hour < 0 || start_hour > 23 || start_minute < 0 || start_minute > 59) {
                    cout << "Giờ hoặc phút không hợp lệ!\n";
                    attempts++;
                    if (attempts >= MAX_ATTEMPTS) {
                        cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                        return;
                    }
                    continue;
                }
            } catch (const exception& e) {
                cout << "Lỗi định dạng giờ: " << e.what() << endl;
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            cout << "Nhập giá vé (VND, nhập '0' để hủy): ";
            if (!(cin >> ticket_price)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Giá vé không hợp lệ!\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
            if (ticket_price == 0) {
                cout << "Hủy thêm suất chiếu.\n";
                return;
            }
            if (ticket_price < 0) {
                cout << "Giá vé phải lớn hơn 0!\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            // Kiểm tra xung đột suất chiếu
            bool conflict = false;
            int new_start_minutes = start_hour * 60 + start_minute;
            int new_end_minutes = new_start_minutes + 180; // Giả sử thời lượng phim tối đa 3 giờ
    
            for (const auto& showtime : showtimes) {
                if (showtime.room == room && showtime.date == date) {
                    // Lấy thời gian bắt đầu và kết thúc của suất chiếu hiện có
                    string existing_time = showtime.time.substr(0, 5); // Lấy hh:mm
                    int existing_start_hour, existing_start_minute;
                    try {
                        existing_start_hour = stoi(existing_time.substr(0, 2));
                        existing_start_minute = stoi(existing_time.substr(3, 2));
                    } catch (const exception& e) {
                        cout << "Lỗi định dạng thời gian suất chiếu hiện có: " << e.what() << endl;
                        continue;
                    }
    
                    int existing_start_minutes = existing_start_hour * 60 + existing_start_minute;
                    int existing_end_minutes = existing_start_minutes + 180; // Giả sử thời lượng 3 giờ
    
                    // Kiểm tra giao nhau thời gian
                    if ((new_start_minutes <= existing_end_minutes && new_end_minutes >= existing_start_minutes)) {
                        conflict = true;
                        cout << "Xung đột suất chiếu! Phòng " << room << " đã được sử dụng vào ngày " << date
                             << " từ " << existing_time << ".\n";
                        break;
                    }
                }
            }
    
            if (conflict) {
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            // Nếu không có xung đột, tạo mã suất chiếu và lưu
            string date_code = date.substr(0, 2) + date.substr(3, 2) + date.substr(8, 2);
            string time_code = time.substr(0, 2) + time.substr(3, 2);
            string showtime_id = movie_id + room + date_code + time_code;
    
            json new_showtime = {
                {"Mã suất chiếu", showtime_id},
                {"Mã phim", movie_id},
                {"Phòng", room},
                {"Ngày chiếu", date},
                {"Giờ chiếu", time + " - " + to_string(start_hour + 3) + time.substr(2, 3)},
                {"Giá vé", ticket_price}
            };
    
            showtimes.emplace_back(new_showtime);
    
            json showtime_json = json::array();
            for (const auto& showtime : showtimes) {
                showtime_json.push_back({
                    {"Mã suất chiếu", showtime.id},
                    {"Mã phim", showtime.movie_id},
                    {"Phòng", showtime.room},
                    {"Ngày chiếu", showtime.date},
                    {"Giờ chiếu", showtime.time},
                    {"Giá vé", showtime.ticket_price}
                });
            }
    
            try {
                writeJsonFile("Showtime.json", showtime_json);
                cout << "Thêm suất chiếu thành công! Mã suất chiếu: " << showtime_id << endl;
                return;
            } catch (const exception& e) {
                cout << "Lỗi khi lưu suất chiếu: " << e.what() << endl;
                return;
            }
        }
    
        cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
    }

    void editShowtime() {
        string id;
        const int MAX_ATTEMPTS = 3; // Giới hạn số lần thử nhập sai
        int attempts = 0;
    
        cout << "\nNhập mã suất chiếu cần sửa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy sửa suất chiếu.\n";
            return;
        }
    
        auto it = find_if(showtimes.begin(), showtimes.end(), [&id](const Showtime& s) { return s.id == id; });
        if (it == showtimes.end()) {
            cout << "Suất chiếu không tồn tại!" << endl;
            return;
        }
    
        string movie_id, room, date, time;
        int ticket_price;
    
        while (attempts < MAX_ATTEMPTS) {
            cout << "Nhập mã phim mới (nhập '0' để hủy): ";
            cin >> movie_id;
            if (movie_id == "0") {
                cout << "Hủy sửa suất chiếu.\n";
                return;
            }
            if (none_of(movies.begin(), movies.end(), [&movie_id](const Movie& m) { return m.id == movie_id; })) {
                cout << "Phim không tồn tại!" << endl;
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            cout << "Nhập phòng mới (e.g., R01, nhập '0' để hủy): ";
            cin >> room;
            if (room == "0") {
                cout << "Hủy sửa suất chiếu.\n";
                return;
            }
    
            cout << "Nhập ngày chiếu mới (dd/mm/yyyy, nhập '0' để hủy): ";
            cin >> date;
            if (date == "0") {
                cout << "Hủy sửa suất chiếu.\n";
                return;
            }
    
            // Kiểm tra định dạng ngày
            regex date_regex(R"(\d{2}/\d{2}/\d{4})");
            if (!regex_match(date, date_regex)) {
                cout << "Định dạng ngày không hợp lệ! Vui lòng nhập theo dạng dd/mm/yyyy.\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            cout << "Nhập giờ bắt đầu mới (hh:mm, nhập '0' để hủy): ";
            cin >> time;
            if (time == "0") {
                cout << "Hủy sửa suất chiếu.\n";
                return;
            }
    
            // Kiểm tra định dạng giờ
            regex time_input_regex(R"(\d{2}:\d{2})");
            if (!regex_match(time, time_input_regex)) {
                cout << "Định dạng giờ không hợp lệ! Vui lòng nhập theo dạng hh:mm.\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            // Kiểm tra giá trị giờ hợp lệ
            int start_hour, start_minute;
            try {
                start_hour = stoi(time.substr(0, 2));
                start_minute = stoi(time.substr(3, 2));
                if (start_hour < 0 || start_hour > 23 || start_minute < 0 || start_minute > 59) {
                    cout << "Giờ hoặc phút không hợp lệ!\n";
                    attempts++;
                    if (attempts >= MAX_ATTEMPTS) {
                        cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                        return;
                    }
                    continue;
                }
            } catch (const exception& e) {
                cout << "Lỗi định dạng giờ: " << e.what() << endl;
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            cout << "Nhập giá vé mới (VND, nhập '0' để hủy): ";
            if (!(cin >> ticket_price)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Giá vé không hợp lệ!\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
            if (ticket_price == 0) {
                cout << "Hủy sửa suất chiếu.\n";
                return;
            }
            if (ticket_price < 0) {
                cout << "Giá vé phải lớn hơn 0!\n";
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            // Kiểm tra xung đột suất chiếu
            bool conflict = false;
            int new_start_minutes = start_hour * 60 + start_minute;
            int new_end_minutes = new_start_minutes + 180; // Giả sử thời lượng phim tối đa 3 giờ
    
            for (const auto& showtime : showtimes) {
                if (showtime.id == id) continue; // Bỏ qua suất chiếu đang sửa
                if (showtime.room == room && showtime.date == date) {
                    // Lấy thời gian bắt đầu của suất chiếu hiện có
                    string existing_time = showtime.time.substr(0, 5); // Lấy hh:mm
                    int existing_start_hour, existing_start_minute;
                    try {
                        existing_start_hour = stoi(existing_time.substr(0, 2));
                        existing_start_minute = stoi(existing_time.substr(3, 2));
                    } catch (const exception& e) {
                        cout << "Lỗi định dạng thời gian suất chiếu hiện có: " << e.what() << endl;
                        continue;
                    }
    
                    int existing_start_minutes = existing_start_hour * 60 + existing_start_minute;
                    int existing_end_minutes = existing_start_minutes + 180; // Giả sử thời lượng 3 giờ
    
                    // Kiểm tra giao nhau thời gian
                    if ((new_start_minutes <= existing_end_minutes && new_end_minutes >= existing_start_minutes)) {
                        conflict = true;
                        cout << "Xung đột suất chiếu! Phòng " << room << " đã được sử dụng vào ngày " << date
                             << " từ " << existing_time << ".\n";
                        break;
                    }
                }
            }
    
            if (conflict) {
                attempts++;
                if (attempts >= MAX_ATTEMPTS) {
                    cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                    return;
                }
                continue;
            }
    
            // Tạo mã suất chiếu mới: Mã phim + Phòng + Ngày chiếu (ddmmyy) + Giờ chiếu (hhmm)
            string date_code = date.substr(0, 2) + date.substr(3, 2) + date.substr(8, 2);
            string time_code = time.substr(0, 2) + time.substr(3, 2);
            string new_id = movie_id + room + date_code + time_code;
    
            // Cập nhật thông tin suất chiếu
            it->id = new_id;
            it->movie_id = movie_id;
            it->room = room;
            it->date = date;
            it->time = time + " - " + to_string(start_hour + 3) + time.substr(2, 3);
            it->ticket_price = ticket_price;
    
            // Cập nhật showtime_id trong bookings nếu có
            for (auto& booking : bookings) {
                if (booking.showtime_id == id) {
                    booking.showtime_id = new_id;
                    booking.ticket_id = new_id + booking.seat; // Cập nhật mã vé
                }
            }
    
            // Lưu vào Showtime.json
            json showtime_json = json::array();
            for (const auto& showtime : showtimes) {
                showtime_json.push_back({
                    {"Mã suất chiếu", showtime.id},
                    {"Mã phim", showtime.movie_id},
                    {"Phòng", showtime.room},
                    {"Ngày chiếu", showtime.date},
                    {"Giờ chiếu", showtime.time},
                    {"Giá vé", showtime.ticket_price}
                });
            }
    
            // Lưu vào Booking.json
            json booking_json = json::object();
            for (const auto& booking : bookings) {
                string showtime_id = booking.showtime_id;
                if (booking_json.find(showtime_id) == booking_json.end()) {
                    booking_json[showtime_id] = json::array();
                }
                booking_json[showtime_id].push_back({
                    {"Mã vé", booking.ticket_id},
                    {"CCCD", booking.cccd},
                    {"Suất chiếu", booking.showtime_id},
                    {"Ghế", booking.seat}
                });
            }
    
            try {
                writeJsonFile("Showtime.json", showtime_json);
                writeJsonFile("Booking.json", booking_json);
                cout << "Sửa suất chiếu thành công! Mã suất chiếu mới: " << new_id << endl;
                return;
            } catch (const exception& e) {
                cout << "Lỗi khi lưu suất chiếu: " << e.what() << endl;
                return;
            }
        }
    
        cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
    }

    void deleteShowtime() {
        string id;
        cout << "\nNhập mã suất chiếu cần xóa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy xóa suất chiếu.\n";
            return;
        }

        for (const auto& booking : bookings) {
            if (booking.showtime_id == id) {
                cout << "Không thể xóa suất chiếu vì đã có vé được đặt!" << endl;
                return;
            }
        }

        auto it = find_if(showtimes.begin(), showtimes.end(), [&id](const Showtime& s) { return s.id == id; });
        if (it == showtimes.end()) {
            cout << "Suất chiếu không tồn tại!" << endl;
            return;
        }

        showtimes.erase(it);

        json showtime_json = json::array();
        for (const auto& showtime : showtimes) {
            showtime_json.push_back({
                {"Mã suất chiếu", showtime.id},
                {"Mã phim", showtime.movie_id},
                {"Phòng", showtime.room},
                {"Ngày chiếu", showtime.date},
                {"Giờ chiếu", showtime.time},
                {"Giá vé", showtime.ticket_price}
            });
        }

        try {
            writeJsonFile("Showtime.json", showtime_json);
            cout << "Xóa suất chiếu thành công!" << endl;
        } catch (const exception& e) {
            cout << "Lỗi khi xóa suất chiếu: " << e.what() << endl;
        }
    }

    void bookTicket(string initial_movie_id = "") {
        string cccd;
        bool customer_selected = false;
        Customer* selected_customer = nullptr;
    
        // Vòng lặp chọn khách hàng
        while (!customer_selected) {
            cout << "\nNhập CCCD khách hàng (nhập '0' để thoát): ";
            cin >> cccd;
            if (cccd == "0") {
                cout << "Hủy đặt vé.\n";
                return;
            }
            if (cccd.empty()) {
                cout << "CCCD không được để trống!\n";
                continue;
            }
    
            auto customer_it = find_if(customers.begin(), customers.end(),
                [&cccd](const Customer& c) { return c.cccd == cccd; });
    
            if (customer_it != customers.end()) {
                bool customer_handled = false;
                while (!customer_handled) {
                    cout << "\nKhách hàng đã tồn tại:\n";
                    customer_it->display();
                    cout << "1. Xác nhận sử dụng thông tin này\n";
                    cout << "2. Sửa thông tin khách hàng\n";
                    cout << "3. Nhập CCCD khác\n";
                    cout << "0. Thoát\n";
                    cout << "Nhập lựa chọn: ";
    
                    int choice;
                    if (!(cin >> choice)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Lựa chọn không hợp lệ!\n";
                        continue;
                    }
    
                    switch (choice) {
                        case 1:
                            selected_customer = &(*customer_it);
                            customer_selected = true;
                            customer_handled = true;
                            break;
                        case 2: {
                            string name, phone, confirm;
                            cin.ignore();
                            cout << "Nhập tên khách hàng mới (nhập '0' để hủy): ";
                            getline(cin, name);
                            if (name == "0") {
                                cout << "Hủy sửa thông tin khách hàng.\n";
                                break;
                            }
                            if (name.empty()) {
                                cout << "Tên không được để trống!\n";
                                continue;
                            }
                            cout << "Nhập số điện thoại mới (nhập '0' để hủy): ";
                            getline(cin, phone);
                            if (phone == "0") {
                                cout << "Hủy sửa thông tin khách hàng.\n";
                                break;
                            }
                            if (phone.empty()) {
                                cout << "Số điện thoại không được để trống!\n";
                                continue;
                            }
                            cout << "Xác nhận cập nhật thông tin? (y/n): ";
                            cin >> confirm;
    
                            if (tolower(confirm[0]) == 'y') {
                                customer_it->name = name;
                                customer_it->phone = phone;
    
                                json customer_json = json::array();
                                for (const auto& customer : customers) {
                                    customer_json.push_back({
                                        {"CCCD", customer.cccd},
                                        {"Tên khách", customer.name},
                                        {"Số điện thoại", customer.phone}
                                    });
                                }
    
                                try {
                                    writeJsonFile("Customer.json", customer_json);
                                    cout << "Cập nhật thông tin khách hàng thành công!\n";
                                    selected_customer = &(*customer_it);
                                    customer_selected = true;
                                    customer_handled = true;
                                } catch (const exception& e) {
                                    cout << "Lỗi khi lưu khách hàng: " << e.what() << endl;
                                    return;
                                }
                            } else {
                                cout << "Hủy cập nhật thông tin.\n";
                            }
                            break;
                        }
                        case 3:
                            customer_handled = true;
                            break;
                        case 0:
                            cout << "Hủy đặt vé.\n";
                            return;
                        default:
                            cout << "Lựa chọn không hợp lệ!\n";
                    }
                }
            } else {
                string name, phone, confirm;
                cin.ignore();
                cout << "Khách hàng không tồn tại. Nhập tên khách hàng (nhập '0' để hủy): ";
                getline(cin, name);
                if (name == "0") {
                    cout << "Hủy lưu thông tin khách hàng.\n";
                    continue;
                }
                if (name.empty()) {
                    cout << "Tên không được để trống!\n";
                    continue;
                }
                cout << "Nhập số điện thoại (nhập '0' để hủy): ";
                getline(cin, phone);
                if (phone == "0") {
                    cout << "Hủy lưu thông tin khách hàng.\n";
                    continue;
                }
                if (phone.empty()) {
                    cout << "Số điện thoại không được để trống!\n";
                    continue;
                }
                cout << "Xác nhận lưu thông tin khách hàng mới? (y/n): ";
                cin >> confirm;
    
                if (tolower(confirm[0]) == 'y') {
                    json new_customer = {
                        {"CCCD", cccd},
                        {"Tên khách", name},
                        {"Số điện thoại", phone}
                    };
                    customers.emplace_back(new_customer);
                    selected_customer = &customers.back();
    
                    json customer_json = json::array();
                    for (const auto& customer : customers) {
                        customer_json.push_back({
                            {"CCCD", customer.cccd},
                            {"Tên khách", customer.name},
                            {"Số điện thoại", customer.phone}
                        });
                    }
    
                    try {
                        writeJsonFile("Customer.json", customer_json);
                        cout << "Lưu thông tin khách hàng thành công!" << endl;
                        customer_selected = true;
                    } catch (const exception& e) {
                        cout << "Lỗi khi lưu khách hàng: " << e.what() << endl;
                        continue;
                    }
                } else {
                    cout << "Hủy lưu thông tin khách hàng." << endl;
                    continue;
                }
            }
        }
    
        // Chọn phim
        Movie* selected_movie = nullptr;
        if (!initial_movie_id.empty()) {
            auto it = find_if(movies.begin(), movies.end(),
                [&initial_movie_id](const Movie& m) { return m.id == initial_movie_id; });
            if (it == movies.end()) {
                cout << "Phim không tồn tại!\n";
                return;
            }
            selected_movie = &(*it);
        }
    
        bool movie_selected = (selected_movie != nullptr);
        while (!movie_selected) {
            cout << "\nDanh sách phim:\n";
            for (size_t i = 0; i < movies.size(); ++i) {
                int showtime_count = 0;
                int booked_seats = 0;
                for (const auto& showtime : showtimes) {
                    if (showtime.movie_id == movies[i].id && isShowtimeValid(showtime.date, showtime.time)) {
                        showtime_count++;
                        booked_seats += count_if(bookings.begin(), bookings.end(),
                            [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
                    }
                }
                int total_seats = showtime_count * 36;
                bool is_sold_out = (total_seats > 0 && booked_seats >= total_seats);
                cout << i + 1 << ". " << movies[i].title
                     << (is_sold_out ? " - Đã hết vé!!" : "") << endl;
            }
    
            int movie_choice;
            cout << "Nhập số thứ tự phim (0 để thoát): ";
            if (!(cin >> movie_choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }
            if (movie_choice == 0) {
                cout << "Hủy đặt vé.\n";
                return;
            }
            if (movie_choice < 1 || movie_choice > static_cast<int>(movies.size())) {
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }
    
            selected_movie = &movies[movie_choice - 1];
    
            // Kiểm tra nếu phim đã hết vé
            int showtime_count = 0;
            int booked_seats = 0;
            for (const auto& showtime : showtimes) {
                if (showtime.movie_id == selected_movie->id && isShowtimeValid(showtime.date, showtime.time)) {
                    showtime_count++;
                    booked_seats += count_if(bookings.begin(), bookings.end(),
                        [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
                }
            }
            int total_seats = showtime_count * 36;
            if (total_seats > 0 && booked_seats >= total_seats) {
                cout << "Phim này đã hết vé!" << endl;
                selected_movie = nullptr;
                continue;
            }
            movie_selected = true;
        }
    
        // Chọn suất chiếu
        vector<Showtime> valid_showtimes;
        for (const auto& showtime : showtimes) {
            if (showtime.movie_id == selected_movie->id && isShowtimeValid(showtime.date, showtime.time)) {
                int booked_seats = count_if(bookings.begin(), bookings.end(),
                    [&showtime](const Booking& b) { return b.showtime_id == showtime.id; });
                if (booked_seats < 36) {
                    valid_showtimes.push_back(showtime);
                }
            }
        }
    
        if (valid_showtimes.empty()) {
            cout << "Không còn suất chiếu nào có chỗ trống cho phim này!" << endl;
            return;
        }
    
        Showtime* selected_showtime = nullptr;
        bool showtime_selected = false;
        while (!showtime_selected) {
            cout << "\nDanh sách suất chiếu khả dụng:\n";
            for (size_t i = 0; i < valid_showtimes.size(); ++i) {
                int booked_seats = count_if(bookings.begin(), bookings.end(),
                    [&valid_showtimes, i](const Booking& b) { return b.showtime_id == valid_showtimes[i].id; });
                cout << i + 1 << ". ";
                valid_showtimes[i].display(movies, booked_seats);
            }
    
            int showtime_choice;
            cout << "Nhập số thứ tự suất chiếu (0 để quay lại): ";
            if (!(cin >> showtime_choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }
            if (showtime_choice == 0) {
                if (!initial_movie_id.empty()) return;
                movie_selected = false;
                selected_movie = nullptr;
                break;
            }
            if (showtime_choice < 1 || showtime_choice > static_cast<int>(valid_showtimes.size())) {
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }
    
            selected_showtime = &valid_showtimes[showtime_choice - 1];
            showtime_selected = true;
    
            // Chọn ghế
            bool seats_selected = false;
            vector<string> selected_seats;
            while (!seats_selected) {
                cout << "\nSơ đồ ghế (Trống = số ghế, Full = đã đặt):\n";
                vector<string> rows = {"A", "B", "C", "D", "E", "F"};
                for (const auto& row : rows) {
                    cout << row << " ";
                    for (int col = 1; col <= 6; ++col) {
                        string seat = row + (col < 10 ? "0" + to_string(col) : to_string(col));
                        bool booked = any_of(bookings.begin(), bookings.end(),
                            [&seat, &selected_showtime](const Booking& b) {
                                return b.showtime_id == selected_showtime->id && b.seat == seat;
                            });
                        cout << setw(8) << (booked ? "Full" : seat + " ");
                    }
                    cout << endl;
                }
    
                int num_seats;
                cout << "\nNhập số ghế muốn đặt (0 để quay lại): ";
                if (!(cin >> num_seats)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Số ghế không hợp lệ!\n";
                    continue;
                }
                if (num_seats == 0) {
                    showtime_selected = false;
                    break;
                }
                if (num_seats < 0 || num_seats > 36) {
                    cout << "Số ghế không hợp lệ! Vui lòng nhập số ghế từ 1 đến 36.\n";
                    continue;
                }
    
                selected_seats.clear();
                for (int i = 0; i < num_seats; ++i) {
                    string seat;
                    cout << "Nhập ghế thứ " << i + 1 << " (e.g., A01, nhập '0' để hủy): ";
                    cin >> seat;
                    if (seat == "0") {
                        cout << "Hủy chọn ghế.\n";
                        selected_seats.clear();
                        break;
                    }
    
                    if (seat.length() != 3 || find(rows.begin(), rows.end(), seat.substr(0, 1)) == rows.end() ||
                        stoi(seat.substr(1)) < 1 || stoi(seat.substr(1)) > 6) {
                        cout << "Ghế không hợp lệ!" << endl;
                        --i;
                        continue;
                    }
    
                    if (any_of(bookings.begin(), bookings.end(),
                        [&seat, &selected_showtime](const Booking& b) {
                            return b.showtime_id == selected_showtime->id && b.seat == seat;
                        })) {
                        cout << "Ghế đã được đặt!" << endl;
                        --i;
                        continue;
                    }
    
                    if (find(selected_seats.begin(), selected_seats.end(), seat) != selected_seats.end()) {
                        cout << "Ghế đã được chọn!" << endl;
                        --i;
                        continue;
                    }
    
                    selected_seats.push_back(seat);
                }
                if (selected_seats.empty()) continue;
    
                cout << "\nXác nhận đặt vé cho các ghế: ";
                for (const auto& seat : selected_seats) cout << seat << " ";
                cout << "(y/n): ";
                string confirm;
                cin >> confirm;
    
                if (tolower(confirm[0]) == 'y') {
                    // Lưu thông tin vé
                    vector<pair<string, string>> ticket_info;
                    for (const auto& seat : selected_seats) {
                        string ticket_id = selected_showtime->id + seat;
                        ticket_info.emplace_back(ticket_id, seat);
                        json new_booking = {
                            {"Mã vé", ticket_id},
                            {"CCCD", cccd},
                            {"Suất chiếu", selected_showtime->id},
                            {"Ghế", seat}
                        };
                        bookings.emplace_back(new_booking);
                    }
    
                    // Ghi vào Booking.json với cấu trúc nhóm theo showtime_id
                    json booking_json = json::object();
                    for (const auto& booking : bookings) {
                        string showtime_id = booking.showtime_id;
                        if (booking_json.find(showtime_id) == booking_json.end()) {
                            booking_json[showtime_id] = json::array();
                        }
                        booking_json[showtime_id].push_back({
                            {"Mã vé", booking.ticket_id},
                            {"CCCD", booking.cccd},
                            {"Suất chiếu", booking.showtime_id},
                            {"Ghế", booking.seat}
                        });
                    }
    
                    try {
                        writeJsonFile("Booking.json", booking_json);
    
                        // Hiển thị hóa đơn
                        cout << "\n=== HÓA ĐƠN ĐẶT VÉ ===\n";
                        cout << "Tên khách hàng: " << selected_customer->name << endl;
                        cout << "Tên phim: " << selected_movie->title << endl;
                        cout << "Giờ chiếu: " << selected_showtime->time << endl;
                        cout << "Ngày chiếu: " << selected_showtime->date << endl;
                        cout << "Số vé: " << selected_seats.size() << endl;
                        cout << "Thông tin vé:\n";
                        for (const auto& [ticket_id, seat] : ticket_info) {
                            cout << "Mã vé: " << ticket_id << ", Ghế: " << seat << endl;
                        }
                        cout << "Tổng giá: " << (selected_seats.size() * selected_showtime->ticket_price) << " VND\n";
                        cout << "======================\n";
                        return;
                    } catch (const exception& e) {
                        cout << "Lỗi khi lưu đặt vé: " << e.what() << endl;
                        return;
                    }
                } else {
                    cout << "Hủy đặt vé. Quay lại bước chọn ghế." << endl;
                    continue;
                }
            }
        }
    }

    void manageMovies() {
        while (true) {
            displayMovies();
            cout << "\nQuản lý phim\n";
            cout << "1. Thêm phim mới\n";
            cout << "2. Sửa thông tin phim\n";
            cout << "3. Xóa phim\n";
            cout << "4. Đặt vé cho phim\n";
            cout << "0. Quay về menu chính\n";
            cout << "Nhập lựa chọn: ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }

            switch (choice) {
                case 1:
                    addMovie();
                    break;
                case 2:
                    editMovie();
                    break;
                case 3:
                    deleteMovie();
                    break;
                case 4: {
                    cout << "\nNhập mã phim để đặt vé (nhập '0' để hủy): ";
                    string movie_id;
                    cin >> movie_id;
                    if (movie_id == "0") {
                        cout << "Hủy đặt vé.\n";
                        break;
                    }
                    if (none_of(movies.begin(), movies.end(), 
                        [&movie_id](const Movie& m) { return m.id == movie_id; })) {
                        cout << "Phim không tồn tại!\n";
                        break;
                    }
                    bookTicket(movie_id);
                    break;
                }
                case 0:
                    return;
                default:
                    cout << "Lựa chọn không hợp lệ!\n";
            }
        }
    }

    void manageShowtimes() {
        while (true) {
            displayShowtimes();
            cout << "\nQuản lý lịch chiếu phim\n";
            cout << "1. Thêm suất chiếu\n";
            cout << "2. Sửa thông tin suất chiếu\n";
            cout << "3. Xóa suất chiếu\n";
            cout << "0. Quay về menu chính\n";
            cout << "Nhập lựa chọn: ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }

            switch (choice) {
                case 1:
                    addShowtime();
                    break;
                case 2:
                    editShowtime();
                    break;
                case 3:
                    deleteShowtime();
                    break;
                case 0:
                    return;
                default:
                    cout << "Lựa chọn không hợp lệ!\n";
            }
        }
    }

    void run() {
        while (true) {
            cout << "\nHệ thống quản lý rạp chiếu phim\n";
            cout << "1. Xem danh sách khách hàng\n";
            cout << "2. Xem danh sách phim\n";
            cout << "3. Xem lịch chiếu phim\n";
            cout << "4. Đặt vé\n";
            cout << "0. Thoát\n";
            cout << "Nhập lựa chọn: ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Lựa chọn không hợp lệ!\n";
                continue;
            }

            switch (choice) {
                case 1:
                    displayCustomers();
                    break;
                case 2:
                    manageMovies();
                    break;
                case 3:
                    manageShowtimes();
                    break;
                case 4:
                    bookTicket();
                    break;
                case 0:
                    cout << "Đang thoát...\n";
                    return;
                default:
                    cout << "Lựa chọn không hợp lệ!\n";
            }
        }
    }
};

int main() {
    CinemaManager manager;
    manager.run();
    return 0;
}