// Bài 2.24: Phân số (Hàm tạo, Hàm tạo sao chép, Hàm bạn)

// Lớp PhanSo:
// Thành phần dữ liệu: Tử số, Mẫu số.
// Hàm thành phần: Nhập, Hiện, Cộng phân số, Rút gọn phân số, và Tìm ước chung lớn nhất.

// Chương trình:
// Tạo mảng các phân số.
// Tính tổng các phân số và hiển thị kết quả dạng phân số rút gọn.

#include <iostream>
using namespace std;

class PhanSo {
private:
    int tuSo;
    int mauSo;

public:

    // Hàm tạo
    PhanSo() : tuSo(1), mauSo(1) {}

    PhanSo(int tu, int mau) {
        tuSo = tu;
        mauSo = (mau != 0) ? mau : 1;
    }

    //Hàm tạo sao chép
    PhanSo(const PhanSo &ps) {
        tuSo = ps.tuSo;
        mauSo = ps.mauSo;
    }

    void Nhap() {
        cout << "Nhập tử số: ";
        cin >> tuSo;
        cout << "Nhập mẫu số (khác 0): ";
        do {
            cin >> mauSo;
            if (mauSo == 0) {
                cout << "Mẫu số không được bằng 0. Vui lòng nhập lại: ";
            }
        } while (mauSo == 0);
    }

    void HienThi() {
        if (mauSo == 1) {
            cout << tuSo << endl;
        } else {
            cout << tuSo << "/" << mauSo << endl;
        }
    }

    int TimUCLN(int a, int b) {
        a = abs(a);
        b = abs(b);
        while (b != 0) {
            int r = a % b;
            a = b;
            b = r;
        }
        return a;
    }

    void RutGon() {
        int ucln = TimUCLN(tuSo, mauSo);
        tuSo /= ucln;
        mauSo /= ucln;
    }

    // Hàm bạn
    friend PhanSo CongPhanSo(const PhanSo &ps1, const PhanSo &ps2);
};

PhanSo CongPhanSo(const PhanSo &ps1, const PhanSo &ps2) {
    PhanSo tong;
    tong.tuSo = ps1.tuSo * ps2.mauSo + ps1.mauSo * ps2.tuSo;
    tong.mauSo = ps1.mauSo * ps2.mauSo;
    tong.RutGon();
    return tong;
}

int main() {
    int n;
    cout << "Nhập số lượng phân số: ";
    cin >> n;

    PhanSo *danhSachPhanSo = new PhanSo[n];

    for (int i = 0; i < n; i++) {
        cout << "\nNhập phân số thứ " << i + 1 << ":\n";
        danhSachPhanSo[i].Nhap();
    }

    PhanSo tong = danhSachPhanSo[0];
    for (int i = 1; i < n; i++) {
        tong = CongPhanSo(tong, danhSachPhanSo[i]);
    }

    cout << "\nTổng các phân số: ";
    tong.HienThi();

    delete[] danhSachPhanSo;
    return 0;
}
