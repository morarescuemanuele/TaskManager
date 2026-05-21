#pragma once
#include <string>
#include "task.h"
#include <iostream>
#include <sstream>
// Functii utilitare pentru manipularea timpului si conversia datelor
class TimeUtils {
public:
    // Transforma un string de forma "DD/MM/YYYY HH:MM" intr-un obiect Time (chrono)
    static Time citesteData(const std::string& inputData);
    
    // Transforma un obiect Time intr-un string lizibil pentru interfata grafica
    static std::string scrieData(Time t);
};
