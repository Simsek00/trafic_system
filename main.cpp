#include <iostream>

using namespace std;

int main() {
    double num1, num2;
    char op;

    cout << "Birinci sayiyi girin: ";
    cin >> num1;

    cout << "Islem s (+, -, *, /): ";
    cin >> op;

    cout << "Ikinci sayiyi girin: ";
    cin >> num2;

    double sonuc;

    switch(op) {
        case '+':
            sonuc = num1 + num2;
            break;
        case '-':
            sonuc = num1 - num2;
            break;
        case '*':
            sonuc = num1 * num2;
            break;
        case '/':
            if (num2 != 0)
                sonuc = num1 / num2;
            else {
                cout << "Hata: 0'a bolme yapilamaz!" << endl;
                return 1;
            }
            break;
        default:
            cout << "Gecersiz islem!" << endl;
            return 1;
    }

    cout << "Sonuc: " << sonuc << endl;

    cout << "Devam etmek icin bir tusa basin..."; // Kullanıcıya bilgi ver
    cin.ignore(); // Önceki girdiyi temizle
    cin.get();
    return 0;
}
