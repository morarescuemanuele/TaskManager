#include "../include/logger.h"
#include <iostream>

Logger::Logger(int maxEntries) : logHistory(maxEntries) {}

// Aceasta metoda este apelata automat de TaskManager prin Observer Pattern
void Logger::onEvent(const std::string& message) {
    logHistory.add(message); // Salvare in clasa template History<string>
    std::cout << "[LOGGER]: " << message << std::endl;
}

void Logger::showLogs() const {
    std::cout << "\n========== ACTIVITY LOG ==========\n";
    if (logHistory.isEmpty()) {
        std::cout << "No activity recorded.\n";
    } else {
        // Iteram prin mesajele salvate in History
        for (const auto& log : logHistory.getItems()) {
            std::cout << "- " << log << std::endl;
        }
    }
    std::cout << "==================================\n";
}
