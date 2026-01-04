// Bài 2.25: Lớp bạn

// Xây dựng chương trình quản lý bán hàng, bao gồm:
// Lớp KhachHang:
// Thành phần dữ liệu: Họ tên, Tuổi.
// Hàm thành phần: Nhập, Hiện.

// Lớp MatHang:
// Thành phần dữ liệu: Mã hàng, Tên hàng.
// Hàm thành phần: Nhập, Hiện.

// Lớp HoaDon (là lớp bạn của KhachHang và MatHang):
// Dữ liệu quản lý: Họ tên khách hàng, Tuổi, Mã hàng, Tên hàng, Ngày bán.
// Hàm thành phần: Nhập, Hiện thông tin về quá trình bán hàng.

// Chương trình chính:
// Nhập thông tin n hóa đơn, in thông tin vừa nhập.
// Đếm số lần mặt hàng có mã x (x nhập từ bàn phím) bán được trong ngày 13/3/2017.

// 001 - 003
// 16/03/2025 - 18/03/2025

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

using namespace std;

class HoaDon;

class KhachHang {
private:
    string hoTen;
    int tuoi;
    friend class HoaDon;
};

class MatHang {
private:
    string maHang;
    string tenHang;
    friend class HoaDon;
};

class HoaDon {
private:
    KhachHang kh;
    MatHang mh;
    string ngayBan;

public:
    void Nhap() {
        cin.ignore();
        do {
            cout << "Nhập họ tên khách hàng: ";
            getline(cin, kh.hoTen);
        } while (kh.hoTen.empty());

        cout << "Nhập tuổi khách hàng: ";
        cin >> kh.tuoi;

        do {
            cout << "Nhập mã hàng: ";
            cin >> mh.maHang;
        } while (mh.maHang.empty());

        cin.ignore();
        do {
            cout << "Nhập tên hàng: ";
            getline(cin, mh.tenHang);
        } while (mh.tenHang.empty());

        regex ngayRegex("^([0-2][0-9]|3[0-1])/([0-1][0-9])/([0-9]{4})$");
        do {
            cout << "Nhập ngày bán (dd/mm/yyyy): ";
            cin >> ngayBan;
        } while (!regex_match(ngayBan, ngayRegex));
    }

    void HienThi() const {
        cout << "Khách hàng:  " << kh.hoTen << " - Tuổi: " << kh.tuoi << endl;
        cout << "Mã mặt hàng: " << mh.maHang << " - Tên hàng: " << mh.tenHang << endl;
        cout << "Ngày bán: " << ngayBan << endl;
    }

    static int DemSoLanBan(const vector<HoaDon>& danhSach, const string& maX, const string& ngayX) {
        return count_if(danhSach.begin(), danhSach.end(), [&](const HoaDon& hd) {
            return hd.mh.maHang == maX && hd.ngayBan == ngayX;
        });
    }
};

int main() {
    int n;
    cout << "Nhập số lượng hóa đơn: ";
    cin >> n;
    vector<HoaDon> danhSach(n);

    for (int i = 0; i < n; i++) {
        cout << "\n--- Nhập hóa đơn thứ " << i + 1 << " ---\n";
        danhSach[i].Nhap();
    }

    cout << "\n--- Thông tin các hóa đơn ---\n";
    for (const HoaDon& hd : danhSach) {
        hd.HienThi();
        cout << "--------------------------\n";
    }

    string maX, ngayX;
    cout << "\nNhập mã hàng cần kiểm tra: ";
    cin >> maX;
    cout << "Nhập ngày bán cần kiểm tra (dd/mm/yyyy): ";
    cin >> ngayX;

    int soLanBan = HoaDon::DemSoLanBan(danhSach, maX, ngayX);
    cout << "Mặt hàng mã " << maX << " đã bán " << soLanBan << " lần vào ngày " << ngayX << ".\n";

    return 0;
}