#include <json.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using json = nlohmann::json;
using namespace std;

struct Booking {
    string user;
    vector<string> seats;
};

struct Movie {
    string title;
    string showtime;
    string room;
    vector<Booking> bookings;
};

vector<Movie> movies;

// Hàm đọc dữ liệu từ file JSON
void loadMovies(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        json j;
        file >> j;
        for (const auto& item : j["movies"]) {
            Movie m;
            m.title = item["title"];
            m.showtime = item["showtime"];
            m.room = item["room"];
            for (const auto& b : item["bookings"]) {
                Booking booking;
                booking.user = b["user"];
                for (const auto& seat : b["seats"]) {
                    booking.seats.push_back(seat);
                }
                m.bookings.push_back(booking);
            }
            movies.push_back(m);
        }
    }
}

// Hàm lưu dữ liệu ra file JSON
void saveMovies(const string& filename) {
    json j;
    for (const auto& m : movies) {
        json movieJson;
        movieJson["title"] = m.title;
        movieJson["showtime"] = m.showtime;
        movieJson["room"] = m.room;
        for (const auto& b : m.bookings) {
            json bookingJson;
            bookingJson["user"] = b.user;
            bookingJson["seats"] = b.seats;
            movieJson["bookings"].push_back(bookingJson);
        }
        j["movies"].push_back(movieJson);
    }
    ofstream file(filename);
    file << j.dump(4);
}

// Hàm đặt vé
void bookTicket() {
    string user, movieTitle, seat;
    cout << "Nhap ten nguoi dat: ";
    getline(cin, user);
    cout << "Nhap ten phim: ";
    getline(cin, movieTitle);
    
    for (auto& m : movies) {
        if (m.title == movieTitle) {
            Booking newBooking;
            newBooking.user = user;
            
            while (true) {
                cout << "Nhap ma ghe (VD: A01), hoac nhap 'x' de ket thuc: ";
                getline(cin, seat);
                if (seat == "x") break;
                newBooking.seats.push_back(seat);
            }
            
            m.bookings.push_back(newBooking);
            cout << "Dat ve thanh cong!\n";
            return;
        }
    }
    cout << "Khong tim thay phim!\n";
}

// Hàm hiển thị danh sách phim
void showMovies() {
    cout << "\nDanh sach phim:\n";
    for (const auto& m : movies) {
        cout << "Phim: " << m.title << ", Suat: " << m.showtime << ", Phong: " << m.room << "\n";
    }
}

int main() {
    string filename = "movies.json";
    loadMovies(filename);

    int choice;
    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Xem danh sach phim\n";
        cout << "2. Dat ve\n";
        cout << "3. Luu va thoat\n";
        cout << "Chon: ";
        cin >> choice;
        cin.ignore(); // bỏ ký tự enter sau cin
        
        switch (choice) {
            case 1: showMovies(); break;
            case 2: bookTicket(); break;
            case 3: saveMovies(filename); cout << "Da luu va thoat!\n"; break;
            default: cout << "Lua chon khong hop le.\n";
        }
    } while (choice != 3);

    return 0;
}
