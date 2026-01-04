// Bài 2.26: Dữ liệu static

// Lớp Bạn đọc:
// Thành phần dữ liệu: Mã bạn đọc (số nguyên 5 chữ số, tự động tăng), Họ tên, Địa chỉ, Số điện thoại, Loại bạn đọc (Sinh viên, Học viên cao học, Giáo viên).
// Hàm thành phần: Nhập, Xuất các dữ liệu trên.

// Chương trình:
// Tạo một mảng gồm n bạn đọc.
// In danh sách các bạn đọc là "Sinh viên".

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

class BanDoc {
private:
    static int maBanDocTuDong; // Biến static
    int maBanDoc;
    string hoTen;
    string diaChi;
    string soDienThoai;
    string loaiBanDoc;

public:

    BanDoc() {
        maBanDoc = maBanDocTuDong++;
    }

    void Nhap() {
        int t;
        cin.ignore();
        cout << "Nhập họ tên bạn đọc: ";
        getline(cin, hoTen);

        cout << "Nhập địa chỉ: ";
        getline(cin, diaChi);

        cout << "Nhập số điện thoại: ";
        getline(cin, soDienThoai);

        cout << "Nhập loại bạn đọc (1 - 3): \n" << "1. Sinh viên\n" << "2. Học viên cao học\n" << "3. Giáo viên\n";
        cin >> t;
        do {
            switch (t) {
                case 1:
                    loaiBanDoc = "Sinh viên";
                    break;
                case 2:
                    loaiBanDoc = "Học viên cao học";
                    break;
                case 3:
                    loaiBanDoc = "Giáo viên";
                    break;
                default:
                    cout << "Loại bạn đọc không hợp lệ.\n";
            }
        } while (t < 1 || t > 3);
    }

    void Xuat() const {
        cout << setw(10) << maBanDoc << setw(20) << hoTen
             << setw(20) << diaChi << setw(20) << soDienThoai
             << setw(20) << loaiBanDoc << endl;
    }

    bool LaSinhVien() const {
        return loaiBanDoc == "Sinh viên";
    }
};

// Khởi tạo static
int BanDoc::maBanDocTuDong = 10000;

int main() {
    int n;
    cout << "Nhập số lượng bạn đọc: ";
    cin >> n;

    vector<BanDoc> danhSach(n);

    for (int i = 0; i < n; i++) {
        cout << "\n--- Nhập thông tin bạn đọc thứ " << i + 1 << " ---\n";
        danhSach[i].Nhap();
    }

    cout << "\n--- Danh sách bạn đọc ---\n";
    cout << setw(10) << "Mã BD" << setw(20) << "Họ Tên"
         << setw(20) << "Địa Chỉ" << setw(20) << "SĐT"
         << setw(20) << "Loại Bạn Đọc" << endl;



    cout << "\n--- Danh sách bạn đọc là Sinh viên ---\n";
    cout << setw(10) << "Mã BD" << setw(20) << "Họ Tên"
         << setw(20) << "Địa Chỉ" << setw(20) << "SĐT"
         << setw(20) << "Loại Bạn Đọc" << endl;
    
    for (const BanDoc& bd : danhSach) {
        if (bd.LaSinhVien()) {
            bd.Xuat();
        }
    }

    return 0;
}