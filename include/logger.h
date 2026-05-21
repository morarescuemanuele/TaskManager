#pragma once
#include "observer.h"
#include "history.h"
#include <string>
#include <vector>

class Logger : public Observer {
    History<std::string> logHistory; // Stocheaza ultimele actiuni ca text

public:
    Logger(int maxEntries = 50);
    
    // Implementarea notificari (adauga mesajul in istoric si il scrie in consola)
    void onEvent(const std::string& message) override;
    
    // Afiseaza toate evenimentele salvate in log
    void showLogs() const;
    
    ~Logger() override = default;
};
