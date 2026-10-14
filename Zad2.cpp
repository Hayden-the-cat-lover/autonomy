#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// Struktura przechowująca jeden wpis z czujnika
struct Pomiar {
    double czas;
    double x;
    double y;
};

int main() {
    // Wczytywanie danych
    std::ifstream plik("sensor1.csv");
    if (!plik.is_open()) {
        std::cout << "Blad: Nie mozna otworzyc pliku sensor1.csv!" << std::endl;
        return 1;
    }

    std::vector<Pomiar> baza_danych;
    std::string linia;

    // Pomijamy nagłówek pliku CSV
    std::getline(plik, linia);

    // Czytamy plik linia po linii
    while (std::getline(plik, linia)) {
        std::stringstream ss(linia);
        std::string komorka_czas, komorka_x, komorka_y;

        // Rozdzielamy tekst po przecinkach
        if (std::getline(ss, komorka_czas, ',') &&
            std::getline(ss, komorka_x, ',') &&
            std::getline(ss, komorka_y, ',')) {

            Pomiar p;
            p.czas = std::stod(komorka_czas);
            p.x = std::stod(komorka_x);
            p.y = std::stod(komorka_y);
            baza_danych.push_back(p);
        }
    }
    plik.close();

    if (baza_danych.empty()) {
        std::cout << "Blad: Brak danych w pliku!" << std::endl;
        return 1;
    }

    // Pętla - pytanie o T
    double T;
    while (true) {
        std::cout << "Podaj T" << std::endl;
        std::cout << ">> ";

        // Jeśli użytkownik zamknie program lub wpisze coś niepoprawnego, kończymy
        if (!(std::cin >> T)) {
            break;
        }

        // Sprawdzamy wymagany zakres 0.000 - 33.000
        if (T < 0.000 || T > 33.000) {
            std::cout << "Wartosc spoza zakresu!" << std::endl;
            continue; // Wraca na początek pętli i pyta znowu
        }

        // Szukamy odpowiedniego momentu
        bool znaleziono = false;

        for (size_t i = 0; i < baza_danych.size() - 1; ++i) {
            Pomiar p_przed = baza_danych[i];
            Pomiar p_po = baza_danych[i + 1];

            // Szukamy momentu, gdzie nasz czas T znajduje się pomiędzy dwoma pomiarami
            if (T >= p_przed.czas && T <= p_po.czas) {
                // Wyliczamy proporcję
                double przedzial_czasu = p_po.czas - p_przed.czas;
                double waga = (T - p_przed.czas) / przedzial_czasu;

                // Wyliczamy pozycję X i Y na podstawie proporcji
                double wyliczone_x = p_przed.x + waga * (p_po.x - p_przed.x);
                double wyliczone_y = p_przed.y + waga * (p_po.y - p_przed.y);

                // Wypisujemy wynik z dokładnością do 3 miejsc po przecinku
                std::cout << "<< " << std::fixed << std::setprecision(3)
                          << wyliczone_x << " " << wyliczone_y << std::endl;

                znaleziono = true;
                break; // Znaleźliśmy, więc przerywamy przeszukiwanie bazy dla tego T
            }
        }

        if (!znaleziono) {
            std::cout << "Blad: Nie udalo sie dopasowac czasu w bazie danych." << std::endl;
        }
    }

    return 0;
}
