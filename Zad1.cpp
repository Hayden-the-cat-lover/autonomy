#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

// Definiujemy jak wygląda pachołek w pamięci komputera
struct Pacholek {
    int id{};
    double x{};
    double y{};
    std::vector<int> sklejone_z; // lista starych numerów
    bool usuniety = false;       // znacznik, czy pachołek został zastąpiony nowym
};

// Funkcja pomocnicza: Obliczanie zwykłej odległości
double oblicz_odleglosc(const Pacholek& a, const Pacholek& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Funkcja pomocnicza dla otoczki: Sprawdza czy skręcamy w lewo
// Zwraca wartość dodatnią, jeśli idąc z A do B, a potem do C, skręcamy w LEWO
double skret_w_lewo(const Pacholek& a, const Pacholek& b, const Pacholek& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

int main() {
    // Wczytywanie danych
    std::ifstream plik("example_input.txt");
    if (!plik.is_open()) {
        std::cout << "Blad: Nie znaleziono pliku example_input.txt!" << std::endl;
        return 1;
    }

    int liczba_pacholkow;
    double max_odleglosc;
    plik >> liczba_pacholkow >> max_odleglosc;

    std::vector<Pacholek> lista;
    int najwyzsze_id = 0;

    for (int i = 0; i < liczba_pacholkow; ++i) {
        Pacholek p;
        plik >> p.id >> p.x >> p.y;
        lista.push_back(p);
        if (p.id > najwyzsze_id) najwyzsze_id = p.id;
    }
    plik.close();

    // Deduplikacja
    int nowe_id = najwyzsze_id + 1;
    int rozmiar = lista.size();

    for (int i = 0; i < rozmiar; ++i) {
        if (lista[i].usuniety) continue;

        // Szukamy wszystkich pachołków, które są blisko pachołka "i"
        std::vector<int> do_sklejenia;
        do_sklejenia.push_back(i);

        for (int j = i + 1; j < rozmiar; ++j) {
            if (lista[j].usuniety) continue;

            if (oblicz_odleglosc(lista[i], lista[j]) <= max_odleglosc) {
                do_sklejenia.push_back(j);
            }
        }

        // Jeśli znaleźliśmy bliskich sąsiadów (więcej niż sam pachołek "i")
        if (do_sklejenia.size() > 1) {
            double suma_x = 0;
            double suma_y = 0;
            Pacholek nowy_pacholek;
            nowy_pacholek.id = nowe_id++;

            for (int indeks : do_sklejenia) {
                suma_x += lista[indeks].x;
                suma_y += lista[indeks].y;
                lista[indeks].usuniety = true; // Kasujemy stary pachołek
                nowy_pacholek.sklejone_z.push_back(lista[indeks].id);
            }

            // Średnia pozycja (środek ciężkości)
            nowy_pacholek.x = suma_x / do_sklejenia.size();
            nowy_pacholek.y = suma_y / do_sklejenia.size();

            lista.push_back(nowy_pacholek); // Dodajemy nowy złączony pachołek na koniec listy

            // Wypisujemy wynik w wymaganym formacie
            std::cout << "Pacholek " << nowy_pacholek.id << " "
                      << std::fixed << std::setprecision(3) << nowy_pacholek.x << " " << nowy_pacholek.y << " [";
            std::sort(nowy_pacholek.sklejone_z.begin(), nowy_pacholek.sklejone_z.end());
            for (size_t k = 0; k < nowy_pacholek.sklejone_z.size(); ++k) {
                std::cout << nowy_pacholek.sklejone_z[k] << (k == nowy_pacholek.sklejone_z.size() - 1 ? "" : ",");
            }
            std::cout << "]\n";
        }
    }

    // Tworzymy nową listę, zostawiając tylko aktywne pachołki
    std::vector<Pacholek> aktywne;
    for (Pacholek p : lista) {
        if (!p.usuniety) aktywne.push_back(p);
    }

    // Otoczka
    if (aktywne.size() < 3) return 0;

    // Znajdujemy pachołek najbardziej po lewej stronie (najmniejsze X)
    int startowy = 0;
    for (size_t i = 1; i < aktywne.size(); ++i) {
        if (aktywne[i].x < aktywne[startowy].x) {
            startowy = i;
        }
    }

    std::vector<Pacholek> otoczka;
    int obecny = startowy;
    int nastepny;

    // Idziemy dookoła, dopóki nie wrócimy do punktu startowego
    do {
        otoczka.push_back(aktywne[obecny]);
        nastepny = (obecny + 1) % aktywne.size();

        for (size_t i = 0; i < aktywne.size(); ++i) {
            // Szukamy punktu, który jest najbardziej "po lewej stronie" naszej drogi
            if (skret_w_lewo(aktywne[obecny], aktywne[i], aktywne[nastepny]) > 0) {
                nastepny = i;
            }
        }
        obecny = nastepny;
    } while (obecny != startowy);

    // Obliczamy obwód
    double obwod = 0;
    std::vector<int> identyfikatory_otoczki;

    for (size_t i = 0; i < otoczka.size(); ++i) {
        Pacholek a = otoczka[i];
        Pacholek b = otoczka[(i + 1) % otoczka.size()];
        obwod += oblicz_odleglosc(a, b);
        identyfikatory_otoczki.push_back(a.id);
    }

    // Wypisujemy wynik otoczki
    std::cout << "Otoczka " << std::fixed << std::setprecision(3) << obwod << " [";
    for (size_t i = 0; i < identyfikatory_otoczki.size(); ++i) {
        std::cout << identyfikatory_otoczki[i] << (i == identyfikatory_otoczki.size() - 1 ? "" : ",");
    }
    std::cout << "]\n";

    return 0;
}
