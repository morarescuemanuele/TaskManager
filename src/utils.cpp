#include "../include/utils.h"

using Time = std::chrono::system_clock::time_point;

// Parsarea string-ului de data folosind get_time
Time TimeUtils::citesteData(const std::string& inputData) {
    std::tm timp_struct = {};
    timp_struct.tm_isdst = -1;
    std::istringstream flux_text(inputData);
    
    flux_text >> std::get_time(&timp_struct, "%d/%m/%Y %H:%M");
    
    // Daca formatul e gresit, returnam momentul actual ca fallback
    if (flux_text.fail()) {
        std::cout << "[Eroare] Format invalid! Setam deadline-ul pentru momentul curent.\n";
        return std::chrono::system_clock::now();
    }
    std::time_t timp_c = std::mktime(&timp_struct);
    return std::chrono::system_clock::from_time_t(timp_c);
}

// Formatarea timpului pentru afisare in GUI
std::string TimeUtils::scrieData(Time t) {
    std::time_t t_c = std::chrono::system_clock::to_time_t(t);
    std::tm* local_t = std::localtime(&t_c);
    std::stringstream ss;
    ss << std::put_time(local_t, "%d/%m/%Y %H:%M");
    return ss.str();
}