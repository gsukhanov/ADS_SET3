#include <random>
#include <cmath>
#include <fstream>

int main(int argv, char* argc[]) {
    std::ofstream table;
    char option = '1';
    if (argv > 1 && (argc[1][0] == '1' || argc[1][0] == '2')) option = argc[1][0];
    std::mt19937 g(120);
    std::uniform_real_distribution<double> dist;
    std::string filename = "results";
    filename += option;
    filename += ".csv";
    table.open(filename);
    table << "Dots;Area;Difference\n";
    double space, target = M_PI_4 + 1.25 * std::asin(0.8) - 1;
    if (option == '1') dist = std::uniform_real_distribution<double>(0, 2 + std::sqrt(5) / 2);
    if (option == '2') dist = std::uniform_real_distribution<double>(0.8, 2.2);
    for (int dots = 100; dots < 100000; dots += 500) {
        if (option == '1') space = (2 + std::sqrt(5) / 2) * (2 + std::sqrt(5) / 2);
        if (option == '2') space = 1.96;
        int hits = 0;
        for (int i = 0; i < dots; i++) {
            bool A = false, B = false, C = false;
            double dot_x = dist(g), dot_y = dist(g);
            if ((dot_x - 1) * (dot_x - 1) + (dot_y - 1) * (dot_y - 1) < 1) A = true;
            if ((dot_x - 1.5) * (dot_x - 1.5) + (dot_y - 2) * (dot_y - 2) < 1.25) B = true;
            if ((dot_x - 2) * (dot_x - 2) + (dot_y - 1.5) * (dot_y - 1.5) < 1.25) C = true;
            if (A && B && C) hits++;
        }
        space *= (static_cast<double>(hits) / dots);
        double diff = (space - target) / target;
        table << dots << ";" << space << ";" << diff << "\n";
    }
    return 0;
}