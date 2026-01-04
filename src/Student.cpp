// Bài 2.23: Sinh viên (Hàm tạo)

// Lớp SinhVien:
// Thành phần dữ liệu: Họ tên, Điểm học kỳ 1, Điểm học kỳ 2.
// Hàm thành phần: Nhập, Hiện, Kiểm tra xem điểm trung bình năm có trên 5 hay không (công thức: 
// (Đ𝑖ể𝑚 1 + 2 × Đ𝑖ể𝑚 2) / 3).

// Chương trình:
// Tạo mảng gồm n sinh viên, in ra thông tin chi tiết của họ.
// Liệt kê các sinh viên có điểm trung bình năm trên 5.

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

class SinhVien {
private:
    string hoTen;
    float diemHocKy1;
    float diemHocKy2;

public:
    SinhVien() : hoTen(""), diemHocKy1(0), diemHocKy2(0) {}

    void NhapThongTin() {
        cin.ignore();
        cout << "Nhập họ tên sinh viên: ";
        getline(cin, hoTen);
        cout << "Nhập điểm học kỳ 1: ";
        cin >> diemHocKy1;
        cout << "Nhập điểm học kỳ 2: ";
        cin >> diemHocKy2;
    }

    float TinhDiemTrungBinh() {
        return (diemHocKy1 + 2 * diemHocKy2) / 3;
    }

    bool KiemTraDiemTrungBinh() {
        return TinhDiemTrungBinh() > 5;
    }

    void HienThiThongTin() {
        cout << fixed << setprecision(2);
        cout << "\nHọ và tên: " << hoTen << "\n";
        cout << "Điểm HK 1: " << diemHocKy1 << "\n";
        cout << "Điểm HK 2: " << diemHocKy2 << "\n";
        cout << "Điểm TBC:  " << TinhDiemTrungBinh() << "\n";
    }
};

int main() {
    int n;
    cout << "Nhập số lượng sinh viên: ";
    cin >> n;

    SinhVien* danhSachSinhVien = new SinhVien[n];

    for (int i = 0; i < n; i++) {
        cout << "\nNhập thông tin sinh viên thứ " << i + 1 << ":\n";
        danhSachSinhVien[i].NhapThongTin();
    }

    cout << "\n --- Danh sách sinh viên --- \n";
    for (int i = 0; i < n; i++) {
        danhSachSinhVien[i].HienThiThongTin();
        cout << "\n---------------\n";
    }

    cout << "\n --- Sinh viên có điểm TBC trên 5 --- \n";
    for (int i = 0; i < n; i++) {
        if (danhSachSinhVien[i].KiemTraDiemTrungBinh()) {
            danhSachSinhVien[i].HienThiThongTin();
            cout << "\n---------------\n";
        }
    }

    delete[] danhSachSinhVien;
    return 0;
}