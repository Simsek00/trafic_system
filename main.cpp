#include <iostream>

int main() {
    double sayi1, sayi2;
    char islem;

    std::cout << "Birinci sayiyi girin: ";
    std::cin >> sayi1;

    std::cout << "Islem secin (+, -, *, /): ";
    std::cin >> islem;

    std::cout << "Ikinci sayiyi girin: ";
    std::cin >> sayi2;

    double sonuc;

    switch (islem) {
        case '+':
            sonuc = sayi1 + sayi2;
            break;
        case '-':
            sonuc = sayi1 - sayi2;
            break;
        case '*':
            sonuc = sayi1 * sayi2;
            break;
        case '/':
            if (sayi2 != 0)
                sonuc = sayi1 / sayi2;
            else {
                std::cout << "Sifira bolme hatasi!" << std::endl;
                return 1;
            }
            break;
        default:
            std::cout << "Gecersiz islem!" << std::endl;
            return 1;
    }

    std::cout << "Sonuc: " << sonuc << std::endl;
    std::cin.get(); 
    return 0;
}

