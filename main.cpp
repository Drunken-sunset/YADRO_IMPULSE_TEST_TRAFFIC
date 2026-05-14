#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <variant>
#include <iomanip>

// fixed-pitch model
struct UniformTraffic {
    double interval;
    double size;

    std::pair<double, double> generate() {
        return {interval, size};
    }
    std::string get_name() const { return "Uniform (Fixed)"; }
};

// Poisson model, exponent size
struct PoissonTraffic {
    std::mt19937 engine{std::random_device{}()};
    std::poisson_distribution<int> dist_interval;
    std::exponential_distribution<double> dist_size;

    PoissonTraffic(double mean_int, double mean_size) 
        : dist_interval(mean_int), dist_size(1.0 / mean_size) {}

    std::pair<double, double> generate() {
        return { static_cast<double>(dist_interval(engine)), dist_size(engine) };
    }
    std::string get_name() const {return "Poisson/Exponential";}
};

using ModelVariant = std::variant<UniformTraffic, PoissonTraffic>;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error: No config file provided!" << std::endl;
        return 1;
    }

    std::ifstream config(argv[1]);
    if (!config) return 1;

    double limit_time;
    std::string type;
    config >> limit_time >> type;

    ModelVariant model;
    if (type.find("equal") != std::string::npos) {
        double i, s;
        config >> i >> s;
        model = UniformTraffic{i, s};
    } else {
        double i, s;
        config >> i >> s;
        model = PoissonTraffic{i, s};
    }

    std::ofstream out("output.csv");
    out << "Time, Size, ModelName\n";

    double current_time = 0;
    
    std::visit([&](auto& m) {
        while (current_time < limit_time) {
            auto [dt, sz] = m.generate();
            current_time += dt;
            if (current_time > limit_time) break;
            out << current_time << "," << sz << "," << m.get_name() << "\n";
        }
    }, model);

    return 0;
}