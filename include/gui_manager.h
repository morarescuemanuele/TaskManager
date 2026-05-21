#pragma once
#include "manager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class GuiManager {
    TaskManager& taskManager;
    sf::RenderWindow window;
    sf::Font font;
    
    sf::Color bgColor = sf::Color(30, 30, 30);
    sf::Color cardColor = sf::Color(50, 50, 50);
    sf::Color textColor = sf::Color::White;
    sf::Color accentColor = sf::Color(0, 120, 215);

public:
    GuiManager(TaskManager& manager);
    // Initializeaza fereastra si fontul
    bool init();
    // Porneste loop-ul principal al aplicatiei
    void run();

private:
    enum class AppState { Viewing, AddingTask, EditingTask };
    AppState state = AppState::Viewing;

    Task* targetTask = nullptr;
    Category* parentCategory = nullptr; // Unde adaugam task-ul nou
    int activeField = 0; 
    int selectedType = 0; // 0:Simple, 1:Deadline, 2:Recurring, 3:Category
    int cursorPos = 0;

    std::string newTitle = "";
    std::string newDesc = "";
    std::string newExtra = "";
    std::string newInterval = "";

    std::string searchQuery = "";
    bool searchFocused = false;

    // Proceseaza evenimentele de la utilizator (tastatura, mouse)
    void processEvents();
    // Deseneaza toate elementele in fereastra
    void render();
    // Deseneaza fereastra de tip popup pentru adaugare/editare
    void drawPopup();
    // Deseneaza un task individual si subtask-urile sale
    void drawTask(Task* task, float& yOffset, int indent = 0);
    
    // Gestioneaza click-urile de mouse in functie de stare
    void handleMouseClick(sf::Vector2i mousePos);
    // Verifica daca utilizatorul a interactionat cu un anumit task
    bool checkTaskInteraction(Task* task, sf::Vector2i mousePos, float& yOffset, int indent);
};
