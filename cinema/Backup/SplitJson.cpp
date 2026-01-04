#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // 1. Đọc file JSON đầu vào
    std::ifstream input_file("C:/Users/Orias.ASUS/Documents/Code/1. C-C++/src/Cinema/Booking.json");
    if (!input_file.is_open()) {
        std::cerr << "Không thể mở file đầu vào" << std::endl;
        return 1;
    }

    json tickets;
    try {
        input_file >> tickets;
    } catch (const json::parse_error& e) {
        std::cerr << "Lỗi parse JSON: " << e.what() << std::endl;
        input_file.close();
        return 1;
    }
    input_file.close();

    // 2. Nhóm vé theo suất chiếu
    json grouped_tickets;
    for (const auto& ticket : tickets) {
        std::string showtime = ticket["Suất chiếu"].get<std::string>();
        grouped_tickets[showtime].push_back(ticket);
    }

    // 3. Ghi kết quả vào file đầu ra với định dạng tùy chỉnh
    std::ofstream output_file("C:/Users/Orias.ASUS/Documents/Code/1. C-C++/src/Cinema/Booking2.json");
    if (!output_file.is_open()) {
        std::cerr << "Không thể mở file đầu ra: GroupedTicketsByShowtime.json" << std::endl;
        return 1;
    }

    try {
        output_file << "{\n"; // Bắt đầu đối tượng JSON
        bool first_showtime = true;
        for (auto it = grouped_tickets.begin(); it != grouped_tickets.end(); ++it) {
            if (!first_showtime) {
                output_file << ",\n";
            }
            first_showtime = false;

            // Ghi khóa suất chiếu
            output_file << "  \"" << it.key() << "\": [\n";

            // Ghi từng vé, mỗi vé trên một dòng
            bool first_ticket = true;
            for (const auto& ticket : it.value()) {
                if (!first_ticket) {
                    output_file << ",\n";
                }
                first_ticket = false;
                output_file << "    " << ticket.dump(); // Ghi vé trên một dòng, thụt lề 4 khoảng trắng
            }

            output_file << "\n  ]";
        }
        output_file << "\n}";
    } catch (const std::exception& e) {
        std::cerr << "Lỗi khi ghi file: " << e.what() << std::endl;
        output_file.close();
        return 1;
    }
    output_file.close();

    return 0;
}