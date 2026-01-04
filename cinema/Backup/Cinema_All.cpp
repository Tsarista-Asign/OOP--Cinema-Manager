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

// Lớp quản lý đọc/ghi file JSON
class FileManager {
public:
    static json readJsonFile(const string& filename) {
        ifstream file(BASE_PATH + filename);
        if (!file.is_open()) throw runtime_error("Không thể mở file: " + filename);
        json j;
        file >> j;
        file.close();
        return j;
    }

    static void writeJsonFile(const string& filename, const json& j) {
        ofstream file(BASE_PATH + filename);
        if (!file.is_open()) throw runtime_error("Không thể ghi vào file: " + filename);
        file << j.dump(4);
        file.close();
    }
};

// Lớp cơ sở trừu tượng cho các thực thể
class CinemaEntity {
protected:
    virtual void fromJson(const json& j) = 0;
public:
    virtual json toJson() const = 0;
    virtual void display() const = 0;
    virtual void display(bool detailed) const = 0;
    virtual ~CinemaEntity() = default;
};

class Customer : public CinemaEntity {
private:
    string cccd;
    string name;
    string phone;

protected:
    void fromJson(const json& j) override {
        cccd = j["CCCD"];
        name = j["Tên khách"];
        phone = j["Số điện thoại"];
    }

public:
    json toJson() const override {
        return {
            {"CCCD", cccd},
            {"Tên khách", name},
            {"Số điện thoại", phone}
        };
    }

    Customer(const json& j) {
        fromJson(j);
    }

    Customer(const string& cccd, const string& name, const string& phone) {
        setCCCD(cccd);
        setName(name);
        setPhone(phone);
    }

    // Getters
    string getCCCD() const { return cccd; }
    string getName() const { return name; }
    string getPhone() const { return phone; }

    // Setters với kiểm tra hợp lệ
    void setCCCD(const string& newCCCD) {
        if (newCCCD.empty()) throw runtime_error("CCCD không được để trống!");
        cccd = newCCCD;
    }

    void setName(const string& newName) {
        if (newName.empty()) throw runtime_error("Tên không được để trống!");
        name = newName;
    }

    void setPhone(const string& newPhone) {
        if (newPhone.empty()) throw runtime_error("Số điện thoại không được để trống!");
        regex phone_regex(R"(\d{10})");
        if (!regex_match(newPhone, phone_regex)) throw runtime_error("Số điện thoại phải có 10 chữ số!");
        phone = newPhone;
    }

    void display() const override {
        cout << "CCCD: " << cccd << ", Tên: " << name << ", Số điện thoại: " << phone << endl;
    }

    void display(bool detailed) const override {
        if (detailed) {
            display();
        } else {
            cout << "Tên: " << name << endl;
        }
    }
};

class Movie : public CinemaEntity {
private:
    string id;
    string title;
    int duration;

protected:
    void fromJson(const json& j) override {
        id = j["Mã phim"];
        title = j["Tên phim"];
        duration = j["Thời lượng"];
    }

public:
    json toJson() const override {
        return {
            {"Mã phim", id},
            {"Tên phim", title},
            {"Thời lượng", duration}
        };
    }

    Movie(const json& j) {
        fromJson(j);
    }

    Movie(const string& id, const string& title, int duration) {
        setId(id);
        setTitle(title);
        setDuration(duration);
    }

    // Getters
    string getId() const { return id; }
    string getTitle() const { return title; }
    int getDuration() const { return duration; }

    // Setters với kiểm tra hợp lệ
    void setId(const string& newId) {
        if (newId.empty()) throw runtime_error("Mã phim không được để trống!");
        id = newId;
    }

    void setTitle(const string& newTitle) {
        if (newTitle.empty()) throw runtime_error("Tên phim không được để trống!");
        title = newTitle;
    }

    void setDuration(int newDuration) {
        if (newDuration <= 0) throw runtime_error("Thời lượng phải lớn hơn 0!");
        duration = newDuration;
    }

    void setDuration(const string& newDuration) {
        int dur;
        try {
            dur = stoi(newDuration);
        } catch (const exception& e) {
            throw runtime_error("Thời lượng không hợp lệ!");
        }
        setDuration(dur);
    }

    void display() const override {
        cout << "Mã phim: " << id << ", Tên phim: " << title << ", Thời lượng: " << duration << " phút" << endl;
    }

    void display(bool detailed) const override {
        if (detailed) {
            display();
        } else {
            cout << "Tên phim: " << title << endl;
        }
    }
};

class Showtime : public CinemaEntity {
private:
    string id;
    string movie_id;
    string room;
    string date;
    string time;
    int ticket_price;

protected:
    void fromJson(const json& j) override {
        id = j["Mã suất chiếu"];
        movie_id = j["Mã phim"];
        room = j["Phòng"];
        date = j["Ngày chiếu"];
        time = j["Giờ chiếu"];
        ticket_price = j.value("Giá vé", 100000);
    }

public:
    json toJson() const override {
        return {
            {"Mã suất chiếu", id},
            {"Mã phim", movie_id},
            {"Phòng", room},
            {"Ngày chiếu", date},
            {"Giờ chiếu", time},
            {"Giá vé", ticket_price}
        };
    }

    Showtime(const json& j) {
        fromJson(j);
    }

    Showtime(const string& id, const string& movie_id, const string& room,
             const string& date, const string& time, int ticket_price) {
        setId(id);
        setMovieId(movie_id);
        setRoom(room);
        setDate(date);
        setTime(time);
        setTicketPrice(ticket_price);
    }

    // Getters
    string getId() const { return id; }
    string getMovieId() const { return movie_id; }
    string getRoom() const { return room; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    int getTicketPrice() const { return ticket_price; }

    // Setters với kiểm tra hợp lệ
    void setId(const string& newId) {
        if (newId.empty()) throw runtime_error("Mã suất chiếu không được để trống!");
        id = newId;
    }

    void setMovieId(const string& newMovieId) {
        if (newMovieId.empty()) throw runtime_error("Mã phim không được để trống!");
        movie_id = newMovieId;
    }

    void setRoom(const string& newRoom) {
        if (newRoom.empty()) throw runtime_error("Phòng không được để trống!");
        regex room_regex(R"(R\d{2})");
        if (!regex_match(newRoom, room_regex)) throw runtime_error("Phòng phải có định dạng Rxx (VD: R01)!");
        room = newRoom;
    }

    void setDate(const string& newDate) {
        regex date_regex(R"(\d{2}/\d{2}/\d{4})");
        if (!regex_match(newDate, date_regex)) throw runtime_error("Định dạng ngày không hợp lệ (dd/mm/yyyy)!");
        date = newDate;
    }

    void setTime(const string& newTime) {
        regex time_regex(R"(\d{2}:\d{2}\s*-\s*\d{2}:\d{2})");
        if (!regex_match(newTime, time_regex)) throw runtime_error("Định dạng giờ không hợp lệ (hh:mm - hh:mm)!");
        time = newTime;
    }

    void setTicketPrice(int newPrice) {
        if (newPrice <= 0) throw runtime_error("Giá vé phải lớn hơn 0!");
        ticket_price = newPrice;
    }

    void setTicketPrice(const string& newPrice) {
        int price;
        try {
            price = stoi(newPrice);
        } catch (const exception& e) {
            throw runtime_error("Giá vé không hợp lệ!");
        }
        setTicketPrice(price);
    }

    void display() const override {
        cout << "Mã suất chiếu: " << id << ", Mã phim: " << movie_id
             << ", Phòng: " << room << ", Ngày chiếu: " << date
             << ", Giờ chiếu: " << time << ", Giá vé: " << ticket_price << " VND" << endl;
    }

    void display(bool detailed) const override {
        if (detailed) {
            display();
        } else {
            cout << "Ngày chiếu: " << date << ", Giờ chiếu: " << time << endl;
        }
    }

    void display(const vector<Movie>& movies, int booked_seats) const {
        string movie_title = "Unknown";
        for (const auto& movie : movies) {
            if (movie.getId() == movie_id) {
                movie_title = movie.getTitle();
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

class Booking : public CinemaEntity {
private:
    string ticket_id;
    string cccd;
    string showtime_id;
    string seat;

protected:
    void fromJson(const json& j) override {
        ticket_id = j["Mã vé"];
        cccd = j["CCCD"];
        showtime_id = j["Suất chiếu"];
        seat = j["Ghế"];
    }

public:
    json toJson() const override {
        return {
            {"Mã vé", ticket_id},
            {"CCCD", cccd},
            {"Suất chiếu", showtime_id},
            {"Ghế", seat}
        };
    }

    Booking(const json& j) {
        fromJson(j);
    }

    Booking(const string& ticket_id, const string& cccd, const string& showtime_id, const string& seat) {
        setTicketId(ticket_id);
        setCCCD(cccd);
        setShowtimeId(showtime_id);
        setSeat(seat);
    }

    // Getters
    string getTicketId() const { return ticket_id; }
    string getCCCD() const { return cccd; }
    string getShowtimeId() const { return showtime_id; }
    string getSeat() const { return seat; }

    // Setters với kiểm tra hợp lệ
    void setTicketId(const string& newTicketId) {
        if (newTicketId.empty()) throw runtime_error("Mã vé không được để trống!");
        ticket_id = newTicketId;
    }

    void setCCCD(const string& newCCCD) {
        if (newCCCD.empty()) throw runtime_error("CCCD không được để trống!");
        cccd = newCCCD;
    }

    void setShowtimeId(const string& newShowtimeId) {
        if (newShowtimeId.empty()) throw runtime_error("Mã suất chiếu không được để trống!");
        showtime_id = newShowtimeId;
    }

    void setSeat(const string& newSeat) {
        if (!isValidSeat(newSeat)) {
            throw runtime_error("Ghế không hợp lệ (VD: A01)!");
        }
        seat = newSeat;
    }

    // Hàm tĩnh kiểm tra định dạng ghế
    static bool isValidSeat(const string& newSeat) {
        if (newSeat.length() != 3) return false;
        if (newSeat[0] < 'A' || newSeat[0] > 'F') return false;
        try {
            int col = stoi(newSeat.substr(1));
            if (col < 1 || col > 6) return false;
        } catch (const exception& e) {
            return false;
        }
        return true;
    }

    void display() const override {
        cout << "Mã vé: " << ticket_id << ", CCCD: " << cccd
             << ", Mã suất chiếu: " << showtime_id << ", Ghế: " << seat << endl;
    }

    void display(bool detailed) const override {
        if (detailed) {
            display();
        } else {
            cout << "Mã vé: " << ticket_id << ", Ghế: " << seat << endl;
        }
    }
};

// Lớp quản lý khách hàng
class CustomerManager {
private:
    vector<Customer> customers;

public:
    void loadCustomers() {
        try {
            json customer_json = FileManager::readJsonFile("Customer.json");
            customers.clear();
            for (const auto& item : customer_json) {
                customers.emplace_back(item);
            }
        } catch (const exception& e) {
            cout << "Lỗi khi tải dữ liệu khách hàng: " << e.what() << endl;
        }
    }

    void saveCustomers() {
        json customer_json = json::array();
        for (const auto& customer : customers) {
            customer_json.push_back(customer.toJson());
        }
        try {
            FileManager::writeJsonFile("Customer.json", customer_json);
        } catch (const exception& e) {
            cout << "Lỗi khi lưu khách hàng: " << e.what() << endl;
        }
    }

    void displayCustomers() const {
        cout << "\nDanh sách khách hàng:\n";
        for (const auto& customer : customers) {
            customer.display();
        }
    }

    Customer* findCustomer(const string& cccd) {
        auto it = find_if(customers.begin(), customers.end(),
            [&cccd](const Customer& c) { return c.getCCCD() == cccd; });
        return (it != customers.end()) ? &(*it) : nullptr;
    }

    void addCustomer(const Customer& customer) {
        customers.push_back(customer);
        saveCustomers();
    }

    void editCustomer(const string& cccd, const string& newName, const string& newPhone) {
        Customer* customer = findCustomer(cccd);
        if (!customer) {
            cout << "Khách hàng không tồn tại!\n";
            return;
        }
        try {
            customer->setName(newName);
            customer->setPhone(newPhone);
            saveCustomers();
            cout << "Sửa thông tin khách hàng thành công!\n";
        } catch (const exception& e) {
            cout << "Lỗi khi sửa thông tin khách hàng: " << e.what() << endl;
        }
    }

    const vector<Customer>& getCustomers() const { return customers; }
};

// Lớp quản lý phim
class MovieManager {
private:
    vector<Movie> movies;

public:
    void loadMovies() {
        try {
            json movie_json = FileManager::readJsonFile("Movies.json");
            movies.clear();
            for (const auto& item : movie_json) {
                movies.emplace_back(item);
            }
        } catch (const exception& e) {
            cout << "Lỗi khi tải dữ liệu phim: " << e.what() << endl;
        }
    }

    void saveMovies() {
        json movie_json = json::array();
        for (const auto& movie : movies) {
            movie_json.push_back(movie.toJson());
        }
        try {
            FileManager::writeJsonFile("Movies.json", movie_json);
        } catch (const exception& e) {
            cout << "Lỗi khi lưu phim: " << e.what() << endl;
        }
    }

    void displayMovies() const {
        cout << "\nDanh sách phim:\n";
        for (const auto& movie : movies) {
            movie.display();
        }
    }

    Movie* findMovie(const string& id) {
        auto it = find_if(movies.begin(), movies.end(),
            [&id](const Movie& m) { return m.getId() == id; });
        return (it != movies.end()) ? &(*it) : nullptr;
    }

    void addMovie(const Movie& movie) {
        movies.push_back(movie);
        saveMovies();
    }

    void editMovie(const string& id, const string& newTitle, int newDuration) {
        Movie* movie = findMovie(id);
        if (!movie) {
            cout << "Phim không tồn tại!\n";
            return;
        }
        try {
            movie->setTitle(newTitle);
            movie->setDuration(newDuration);
            saveMovies();
            cout << "Sửa phim thành công!\n";
        } catch (const exception& e) {
            cout << "Lỗi khi sửa phim: " << e.what() << endl;
        }
    }

    void removeMovie(const string& id) {
        auto it = find_if(movies.begin(), movies.end(),
            [&id](const Movie& m) { return m.getId() == id; });
        if (it != movies.end()) {
            movies.erase(it);
            saveMovies();
            cout << "Xóa phim thành công!\n";
        } else {
            cout << "Phim không tồn tại!\n";
        }
    }

    const vector<Movie>& getMovies() const { return movies; }
};

// Lớp quản lý suất chiếu
class ShowtimeManager {
private:
    vector<Showtime> showtimes;
    MovieManager& movieManager;

    string getCurrentDateTime() const {
        time_t now = time(nullptr);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << put_time(ltm, "%d/%m/%Y %H:%M");
        return ss.str();
    }

    bool isShowtimeValid(const string& date, const string& time) const {
        std::time_t now = std::time(nullptr);
        tm current_tm = *localtime(&now);

        regex date_regex(R"(\d{2}/\d{2}/\d{4})");
        regex time_regex(R"(\d{2}:\d{2})");
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
            hour = stoi(time.substr(0, 2));
            minute = stoi(time.substr(3, 2));
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
        if (showtime == -1) return false;

        return showtime > now;
    }

public:
    ShowtimeManager(MovieManager& mm) : movieManager(mm) {}

    void loadShowtimes() {
        try {
            json showtime_json = FileManager::readJsonFile("Showtime.json");
            showtimes.clear();
            for (const auto& item : showtime_json) {
                showtimes.emplace_back(item);
            }
        } catch (const exception& e) {
            cout << "Lỗi khi tải dữ liệu suất chiếu: " << e.what() << endl;
        }
    }

    void saveShowtimes() {
        json showtime_json = json::array();
        for (const auto& showtime : showtimes) {
            showtime_json.push_back(showtime.toJson());
        }
        try {
            FileManager::writeJsonFile("Showtime.json", showtime_json);
        } catch (const exception& e) {
            cout << "Lỗi khi lưu suất chiếu: " << e.what() << endl;
        }
    }

    void displayShowtimes(const vector<Booking>& bookings) const {
        cout << "\nLịch chiếu phim:\n";
        for (const auto& showtime : showtimes) {
            int booked_seats = count_if(bookings.begin(), bookings.end(),
                [&showtime](const Booking& b) { return b.getShowtimeId() == showtime.getId(); });
            showtime.display(movieManager.getMovies(), booked_seats);
        }
    }

    Showtime* findShowtime(const string& id) {
        auto it = find_if(showtimes.begin(), showtimes.end(),
            [&id](const Showtime& s) { return s.getId() == id; });
        return (it != showtimes.end()) ? &(*it) : nullptr;
    }

    bool addShowtime(const string& movie_id, const string& room, const string& date,
                     const string& time, int ticket_price) {
        if (!movieManager.findMovie(movie_id)) {
            cout << "Phim không tồn tại!\n";
            return false;
        }

        int start_hour, start_minute;
        try {
            start_hour = stoi(time.substr(0, 2));
            start_minute = stoi(time.substr(3, 2));
        } catch (const exception& e) {
            cout << "Lỗi định dạng giờ: " << e.what() << endl;
            return false;
        }

        // Tính thời gian kết thúc (giả sử suất chiếu kéo dài 3 giờ)
        int end_hour = (start_hour + 3) % 24;
        int end_minute = start_minute;
        stringstream end_time_ss;
        end_time_ss << setfill('0') << setw(2) << end_hour << ":" << setw(2) << end_minute;
        string full_time = time + " - " + end_time_ss.str();

        // Kiểm tra xung đột suất chiếu
        int new_start_minutes = start_hour * 60 + start_minute;
        int new_end_minutes = new_start_minutes + 180;  // 3 giờ = 180 phút

        for (const auto& showtime : showtimes) {
            if (showtime.getRoom() == room && showtime.getDate() == date) {
                string existing_time = showtime.getTime().substr(0, 5);
                int existing_start_hour, existing_start_minute;
                try {
                    existing_start_hour = stoi(existing_time.substr(0, 2));
                    existing_start_minute = stoi(existing_time.substr(3, 2));
                } catch (const exception& e) {
                    cout << "Lỗi định dạng thời gian suất chiếu hiện có: " << e.what() << endl;
                    continue;
                }

                int existing_start_minutes = existing_start_hour * 60 + existing_start_minute;
                int existing_end_minutes = existing_start_minutes + 180;

                if (new_start_minutes <= existing_end_minutes && new_end_minutes >= existing_start_minutes) {
                    cout << "Xung đột suất chiếu! Phòng " << room << " đã được sử dụng vào ngày " << date
                         << " từ " << existing_time << ".\n";
                    return false;
                }
            }
        }

        string date_code = date.substr(0, 2) + date.substr(3, 2) + date.substr(8, 2);
        string time_code = time.substr(0, 2) + time.substr(3, 2);
        string showtime_id = movie_id + room + date_code + time_code;

        Showtime new_showtime(showtime_id, movie_id, room, date, full_time, ticket_price);
        showtimes.push_back(new_showtime);
        saveShowtimes();
        return true;
    }

    bool editShowtime(const string& id, const string& movie_id, const string& room,
                      const string& date, const string& time, int ticket_price, vector<Booking>& bookings) {
        Showtime* showtime = findShowtime(id);
        if (!showtime) {
            cout << "Suất chiếu không tồn tại!\n";
            return false;
        }

        if (!movieManager.findMovie(movie_id)) {
            cout << "Phim không tồn tại!\n";
            return false;
        }

        int start_hour, start_minute;
        try {
            start_hour = stoi(time.substr(0, 2));
            start_minute = stoi(time.substr(3, 2));
        } catch (const exception& e) {
            cout << "Lỗi định dạng giờ: " << e.what() << endl;
            return false;
        }

        // Tính thời gian kết thúc
        int end_hour = (start_hour + 3) % 24;
        int end_minute = start_minute;
        stringstream end_time_ss;
        end_time_ss << setfill('0') << setw(2) << end_hour << ":" << setw(2) << end_minute;
        string full_time = time + " - " + end_time_ss.str();

        // Kiểm tra xung đột suất chiếu
        int new_start_minutes = start_hour * 60 + start_minute;
        int new_end_minutes = new_start_minutes + 180;

        for (const auto& s : showtimes) {
            if (s.getId() == id) continue;
            if (s.getRoom() == room && s.getDate() == date) {
                string existing_time = s.getTime().substr(0, 5);
                int existing_start_hour, existing_start_minute;
                try {
                    existing_start_hour = stoi(existing_time.substr(0, 2));
                    existing_start_minute = stoi(existing_time.substr(3, 2));
                } catch (const exception& e) {
                    cout << "Lỗi định dạng thời gian suất chiếu hiện có: " << e.what() << endl;
                    continue;
                }

                int existing_start_minutes = existing_start_hour * 60 + existing_start_minute;
                int existing_end_minutes = existing_start_minutes + 180;

                if (new_start_minutes <= existing_end_minutes && new_end_minutes >= existing_start_minutes) {
                    cout << "Xung đột suất chiếu! Phòng " << room << " đã được sử dụng vào ngày " << date
                         << " từ " << existing_time << ".\n";
                    return false;
                }
            }
        }

        string date_code = date.substr(0, 2) + date.substr(3, 2) + date.substr(8, 2);
        string time_code = time.substr(0, 2) + time.substr(3, 2);
        string new_id = movie_id + room + date_code + time_code;

        // Cập nhật showtime_id trong bookings
        for (auto& booking : bookings) {
            if (booking.getShowtimeId() == id) {
                booking.setShowtimeId(new_id);
                booking.setTicketId(new_id + booking.getSeat());
            }
        }

        showtime->setId(new_id);
        showtime->setMovieId(movie_id);
        showtime->setRoom(room);
        showtime->setDate(date);
        showtime->setTime(full_time);
        showtime->setTicketPrice(ticket_price);

        saveShowtimes();
        return true;
    }

    void removeShowtime(const string& id) {
        auto it = find_if(showtimes.begin(), showtimes.end(),
            [&id](const Showtime& s) { return s.getId() == id; });
        if (it != showtimes.end()) {
            showtimes.erase(it);
            saveShowtimes();
            cout << "Xóa suất chiếu thành công!\n";
        } else {
            cout << "Suất chiếu không tồn tại!\n";
        }
    }

    bool isShowtimeValid(const Showtime& showtime) const {
        string time = showtime.getTime().substr(0, 5); // Lấy hh:mm
        return isShowtimeValid(showtime.getDate(), time);
    }

    const vector<Showtime>& getShowtimes() const { return showtimes; }
};

// Lớp quản lý đặt vé
class BookingManager {
private:
    vector<Booking> bookings;

public:
    void loadBookings() {
        try {
            json booking_json = FileManager::readJsonFile("Booking.json");
            bookings.clear();
            for (const auto& [showtime_id, booking_array] : booking_json.items()) {
                for (const auto& item : booking_array) {
                    bookings.emplace_back(item);
                }
            }
        } catch (const exception& e) {
            cout << "Lỗi khi tải dữ liệu đặt vé: " << e.what() << endl;
        }
    }

    void saveBookings() {
        json booking_json = json::object();
        for (const auto& booking : bookings) {
            string showtime_id = booking.getShowtimeId();
            if (booking_json.find(showtime_id) == booking_json.end()) {
                booking_json[showtime_id] = json::array();
            }
            booking_json[showtime_id].push_back(booking.toJson());
        }
        try {
            FileManager::writeJsonFile("Booking.json", booking_json);
        } catch (const exception& e) {
            cout << "Lỗi khi lưu đặt vé: " << e.what() << endl;
        }
    }

    void addBooking(const Booking& booking) {
        bookings.push_back(booking);
        saveBookings();
    }

    vector<Booking>& getBookings() {
        return bookings;
    }

    const vector<Booking>& getBookings() const {
        return bookings;
    }
};

// Lớp quản lý chính
class CinemaManager {
private:
    CustomerManager customerManager;
    MovieManager movieManager;
    ShowtimeManager showtimeManager;
    BookingManager bookingManager;

public:
    CinemaManager() : showtimeManager(movieManager) {
        customerManager.loadCustomers();
        movieManager.loadMovies();
        showtimeManager.loadShowtimes();
        bookingManager.loadBookings();
    }

    void displayCustomers() {
        customerManager.displayCustomers();
    }

    void displayMovies() {
        movieManager.displayMovies();
    }

    void displayShowtimes() {
        showtimeManager.displayShowtimes(bookingManager.getBookings());
    }

    void addMovie() {
        int max_id = 0;
        for (const auto& movie : movieManager.getMovies()) {
            try {
                string num_str = movie.getId().substr(3);
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

        try {
            Movie new_movie(new_id, title, duration);
            movieManager.addMovie(new_movie);
            cout << "Thêm phim thành công! Mã phim: " << new_id << endl;
        } catch (const exception& e) {
            cout << "Lỗi khi thêm phim: " << e.what() << endl;
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

        string title;
        int duration;
        cin.ignore();
        cout << "Nhập tên phim mới (nhập '0' để hủy): ";
        getline(cin, title);
        if (title == "0") {
            cout << "Hủy sửa phim.\n";
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

        movieManager.editMovie(id, title, duration);
    }

    void deleteMovie() {
        string id;
        cout << "\nNhập mã phim cần xóa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy xóa phim.\n";
            return;
        }

        for (const auto& showtime : showtimeManager.getShowtimes()) {
            if (showtime.getMovieId() == id) {
                cout << "Không thể xóa phim vì đang có suất chiếu!\n";
                return;
            }
        }

        movieManager.removeMovie(id);
    }

    void addShowtime() {
        string movie_id, room, date, time;
        int ticket_price;
        const int MAX_ATTEMPTS = 3;
        int attempts = 0;

        while (attempts < MAX_ATTEMPTS) {
            cout << "\nNhập mã phim (nhập '0' để hủy): ";
            cin >> movie_id;
            if (movie_id == "0") {
                cout << "Hủy thêm suất chiếu.\n";
                return;
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

            cout << "Nhập giờ bắt đầu (hh:mm, nhập '0' để hủy): ";
            cin >> time;
            if (time == "0") {
                cout << "Hủy thêm suất chiếu.\n";
                return;
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

            try {
                if (showtimeManager.addShowtime(movie_id, room, date, time, ticket_price)) {
                    cout << "Thêm suất chiếu thành công!\n";
                    return;
                }
            } catch (const exception& e) {
                cout << "Lỗi: " << e.what() << endl;
            }
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                cout << "Đã vượt quá số lần thử. Hủy thêm suất chiếu.\n";
                return;
            }
        }
    }

    void editShowtime() {
        string id;
        const int MAX_ATTEMPTS = 3;
        int attempts = 0;

        cout << "\nNhập mã suất chiếu cần sửa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy sửa suất chiếu.\n";
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

            cout << "Nhập giờ bắt đầu mới (hh:mm, nhập '0' để hủy): ";
            cin >> time;
            if (time == "0") {
                cout << "Hủy sửa suất chiếu.\n";
                return;
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

            try {
                if (showtimeManager.editShowtime(id, movie_id, room, date, time, ticket_price, bookingManager.getBookings())) {
                    bookingManager.saveBookings();
                    cout << "Sửa suất chiếu thành công!\n";
                    return;
                }
            } catch (const exception& e) {
                cout << "Lỗi: " << e.what() << endl;
            }
            attempts++;
            if (attempts >= MAX_ATTEMPTS) {
                cout << "Đã vượt quá số lần thử. Hủy sửa suất chiếu.\n";
                return;
            }
        }
    }

    void deleteShowtime() {
        string id;
        cout << "\nNhập mã suất chiếu cần xóa (nhập '0' để hủy): ";
        cin >> id;
        if (id == "0") {
            cout << "Hủy xóa suất chiếu.\n";
            return;
        }

        for (const auto& booking : bookingManager.getBookings()) {
            if (booking.getShowtimeId() == id) {
                cout << "Không thể xóa suất chiếu vì đã có vé được đặt!\n";
                return;
            }
        }

        showtimeManager.removeShowtime(id);
    }

    void bookTicket(string initial_movie_id = "") {
        string cccd;
        bool customer_selected = false;
        Customer* selected_customer = nullptr;

        while (!customer_selected) {
            cout << "\nNhập CCCD khách hàng (nhập '0' để thoát): ";
            cin >> cccd;
            if (cccd == "0") {
                cout << "Hủy đặt vé.\n";
                return;
            }

            selected_customer = customerManager.findCustomer(cccd);
            if (selected_customer) {
                bool customer_handled = false;
                while (!customer_handled) {
                    cout << "\nKhách hàng đã tồn tại:\n";
                    selected_customer->display();
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
                            cout << "Nhập số điện thoại mới (nhập '0' để hủy): ";
                            getline(cin, phone);
                            if (phone == "0") {
                                cout << "Hủy sửa thông tin khách hàng.\n";
                                break;
                            }
                            cout << "Xác nhận cập nhật thông tin? (y/n): ";
                            cin >> confirm;

                            if (tolower(confirm[0]) == 'y') {
                                customerManager.editCustomer(cccd, name, phone);
                                selected_customer = customerManager.findCustomer(cccd);
                                customer_selected = true;
                                customer_handled = true;
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
                cout << "Nhập số điện thoại (nhập '0' để hủy): ";
                getline(cin, phone);
                if (phone == "0") {
                    cout << "Hủy lưu thông tin khách hàng.\n";
                    continue;
                }
                cout << "Xác nhận lưu thông tin khách hàng mới? (y/n): ";
                cin >> confirm;

                if (tolower(confirm[0]) == 'y') {
                    try {
                        Customer new_customer(cccd, name, phone);
                        customerManager.addCustomer(new_customer);
                        selected_customer = customerManager.findCustomer(cccd);
                        cout << "Lưu thông tin khách hàng thành công!\n";
                        customer_selected = true;
                    } catch (const exception& e) {
                        cout << "Lỗi khi lưu khách hàng: " << e.what() << endl;
                        continue;
                    }
                } else {
                    cout << "Hủy lưu thông tin khách hàng.\n";
                    continue;
                }
            }
        }

        Movie* selected_movie = nullptr;
        if (!initial_movie_id.empty()) {
            selected_movie = movieManager.findMovie(initial_movie_id);
            if (!selected_movie) {
                cout << "Phim không tồn tại!\n";
                return;
            }
        }

        bool movie_selected = (selected_movie != nullptr);
        while (!movie_selected) {
            cout << "\nDanh sách phim:\n";
            const auto& movies = movieManager.getMovies();
            for (size_t i = 0; i < movies.size(); ++i) {
                int showtime_count = 0;
                int booked_seats = 0;
                for (const auto& showtime : showtimeManager.getShowtimes()) {
                    if (showtime.getMovieId() == movies[i].getId() && showtimeManager.isShowtimeValid(showtime)) {
                        showtime_count++;
                        booked_seats += count_if(bookingManager.getBookings().begin(), bookingManager.getBookings().end(),
                            [&showtime](const Booking& b) { return b.getShowtimeId() == showtime.getId(); });
                    }
                }
                int total_seats = showtime_count * 36;
                bool is_sold_out = (total_seats > 0 && booked_seats >= total_seats);
                cout << i + 1 << ". " << movies[i].getTitle()
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

            selected_movie = const_cast<Movie*>(&movies[movie_choice - 1]);

            int showtime_count = 0;
            int booked_seats = 0;
            for (const auto& showtime : showtimeManager.getShowtimes()) {
                if (showtime.getMovieId() == selected_movie->getId() && showtimeManager.isShowtimeValid(showtime)) {
                    showtime_count++;
                    booked_seats += count_if(bookingManager.getBookings().begin(), bookingManager.getBookings().end(),
                        [&showtime](const Booking& b) { return b.getShowtimeId() == showtime.getId(); });
                }
            }
            int total_seats = showtime_count * 36;
            if (total_seats > 0 && booked_seats >= total_seats) {
                cout << "Phim này đã hết vé!\n";
                selected_movie = nullptr;
                continue;
            }
            movie_selected = true;
        }

        vector<Showtime> valid_showtimes;
        for (const auto& showtime : showtimeManager.getShowtimes()) {
            if (showtime.getMovieId() == selected_movie->getId() && showtimeManager.isShowtimeValid(showtime)) {
                int booked_seats = count_if(bookingManager.getBookings().begin(), bookingManager.getBookings().end(),
                    [&showtime](const Booking& b) { return b.getShowtimeId() == showtime.getId(); });
                if (booked_seats < 36) {
                    valid_showtimes.push_back(showtime);
                }
            }
        }

        if (valid_showtimes.empty()) {
            cout << "Không còn suất chiếu nào có chỗ trống cho phim này!\n";
            return;
        }

        Showtime* selected_showtime = nullptr;
        bool showtime_selected = false;
        while (!showtime_selected) {
            cout << "\nDanh sách suất chiếu khả dụng:\n";
            for (size_t i = 0; i < valid_showtimes.size(); ++i) {
                int booked_seats = count_if(bookingManager.getBookings().begin(), bookingManager.getBookings().end(),
                    [&valid_showtimes, i](const Booking& b) { return b.getShowtimeId() == valid_showtimes[i].getId(); });
                cout << i + 1 << ". ";
                valid_showtimes[i].display(movieManager.getMovies(), booked_seats);
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

            bool seats_selected = false;
            vector<string> selected_seats;
            while (!seats_selected) {
                cout << "\nSơ đồ ghế (Trống = số ghế, Full = đã đặt):\n";
                vector<string> rows = {"A", "B", "C", "D", "E", "F"};
                for (const auto& row : rows) {
                    cout << row << " ";
                    for (int col = 1; col <= 6; ++col) {
                        string seat = row + (col < 10 ? "0" + to_string(col) : to_string(col));
                        bool booked = any_of(bookingManager.getBookings().begin(), bookingManager.getBookings().end(),
                            [&seat, &selected_showtime](const Booking& b) {
                                return b.getShowtimeId() == selected_showtime->getId() && b.getSeat() == seat;
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

                    if (!Booking::isValidSeat(seat)) {
                        cout << "Ghế không hợp lệ (VD: A01)!\n";
                        --i;
                        continue;
                    }

                    if (any_of(bookingManager.getBookings().begin(), bookingManager.getBookings().end(),
                        [&seat, &selected_showtime](const Booking& b) {
                            return b.getShowtimeId() == selected_showtime->getId() && b.getSeat() == seat;
                        })) {
                        cout << "Ghế đã được đặt!\n";
                        --i;
                        continue;
                    }

                    if (find(selected_seats.begin(), selected_seats.end(), seat) != selected_seats.end()) {
                        cout << "Ghế đã được chọn!\n";
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
                    vector<pair<string, string>> ticket_info;
                    for (const auto& seat : selected_seats) {
                        string ticket_id = selected_showtime->getId() + seat;
                        ticket_info.emplace_back(ticket_id, seat);
                        Booking new_booking(ticket_id, cccd, selected_showtime->getId(), seat);
                        bookingManager.addBooking(new_booking);
                    }

                    cout << "\n=== HÓA ĐƠN ĐẶT VÉ ===\n";
                    cout << "Tên khách hàng: " << selected_customer->getName() << endl;
                    cout << "Tên phim: " << selected_movie->getTitle() << endl;
                    cout << "Giờ chiếu: " << selected_showtime->getTime() << endl;
                    cout << "Ngày chiếu: " << selected_showtime->getDate() << endl;
                    cout << "Số vé: " << selected_seats.size() << endl;
                    cout << "Thông tin vé:\n";
                    for (const auto& [ticket_id, seat] : ticket_info) {
                        cout << "Mã vé: " << ticket_id << ", Ghế: " << seat << endl;
                    }
                    cout << "Tổng giá: " << (selected_seats.size() * selected_showtime->getTicketPrice()) << " VND\n";
                    cout << "======================\n";
                    return;
                } else {
                    cout << "Hủy đặt vé. Quay lại bước chọn ghế.\n";
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
                    if (!movieManager.findMovie(movie_id)) {
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