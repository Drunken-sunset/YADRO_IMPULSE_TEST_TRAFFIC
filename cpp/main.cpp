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
    if (!config) {
        std::cerr << "Error: Could not open config file!" << std::endl;
        return 1;
    }

    double limit_time, intensity, packet_size;
    std::string type;
    config >> limit_time >> type;

    if (!(config >> limit_time >> type >> intensity >> packet_size)) {
        // Если не получилось считать 4 значения, пробуем считать хотя бы 2
        config.clear();
        config.seekg(0);
        if (!(config >> intensity >> limit_time)) {
            std::cerr << "Error: Invalid config format!" << std::endl;
            return 1;
        }
    }

    ModelVariant model = PoissonTraffic{intensity, limit_time};

    std::ofstream out("output.csv");
    out << "Time, Size, ModelName\n";

    double current_time = 0;
    
    std::visit([&](auto& m) {
        while (current_time < limit_time) {
            auto [dt, sz] = m.generate();
            current_time += dt;
            if (current_time > limit_time) break;
            out << current_time << "," << sz << "," << m.get_name() << "\n";
            out.flush();
        }
    }, model);

    return 0;
}