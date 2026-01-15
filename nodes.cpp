#include "nodes.hxx"

void PreferencjeOdbiorców::dodaj_odbiorcę(IOdbiorcaPaczki *odbiorca) {
    double liczba_odbiorców_przed = static_cast<double>(preferencje_.size());

    if (liczba_odbiorców_przed == 0) {
        preferencje_[odbiorca] = 1.0;
    } else {
        double nowe_prawdopodobieństwo = 1.0 / (liczba_odbiorców_przed + 1);

        for (auto &para : preferencje_) {
            para.second = nowe_prawdopodobieństwo;
        }
        preferencje_[odbiorca] = nowe_prawdopodobieństwo;
    }
}

void PreferencjeOdbiorców::usuń_odbiorcę(IOdbiorcaPaczki *odbiorca) {
    double liczba_odbiorców_przed = static_cast<double>(preferencje_.size());

    if (liczba_odbiorców_przed > 1) {
        double nowe_prawdopodobieństwo = 1.0 / (liczba_odbiorców_przed - 1);

        for (auto &para : preferencje_) {
            if (para.first != odbiorca) {
                para.second = nowe_prawdopodobieństwo;
            }
        }
    }

    preferencje_.erase(odbiorca);
}