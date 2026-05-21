#include "../include/gui_manager.h"
#include "../include/task_factory.h"
#include <iostream>
#include <optional>
#include <algorithm>

// Constructor: seteaza task manager-ul si creeaza fereastra
GuiManager::GuiManager(TaskManager& manager) 
    : taskManager(manager), window(sf::VideoMode({800, 600}), "Tudu list") {}

// Initializare: incarca fontul si seteaza framerate-ul
bool GuiManager::init() {
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Eroare: Nu s-a putut incarca fontul.\n";
        return false;
    }
    window.setFramerateLimit(60);
    return true;
}

// Loop principal: proceseaza evenimente, actualizeaza si randeaza
void GuiManager::run() {
    while (window.isOpen()) {
        processEvents();
        render();
    }
}

// Proceseaza input (text, taste, click-uri)
void GuiManager::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        // Inchidere la apasarea butonului 'X'
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        
        // Logica pentru introducere text (doar daca suntem in Add/Edit sau Search)
        if (state != AppState::Viewing || searchFocused) {
            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode < 128) {
                    char c = static_cast<char>(textEntered->unicode);
                    std::string* target;
                    
                    // Identificam unde trebuie sa scriem textul
                    if (searchFocused) target = &searchQuery;
                    else {
                        if (activeField == 0) target = &newTitle;
                        else if (activeField == 1) target = &newDesc;
                        else if (activeField == 2) target = &newExtra;
                        else target = &newInterval;
                    }

                    // Stergere (Backspace)
                    if (c == '\b') {
                        if (searchFocused) { if (!searchQuery.empty()) searchQuery.pop_back(); }
                        else { if (cursorPos > 0) { target->erase(cursorPos - 1, 1); cursorPos--; } }
                    } 
                    // Trecere la campul urmator (Enter)
                    else if (c == '\r' || c == '\n') {
                        if (searchFocused) searchFocused = false;
                        else {
                            bool isRec = (selectedType == 2 || (state == AppState::EditingTask && dynamic_cast<RecurringTask*>(targetTask)));
                            int maxFields = isRec ? 4 : 3;
                            activeField = (activeField + 1) % maxFields; 
                            cursorPos = 0;
                        }
                    } 
                    // Introducere caracter normal
                    else if (c >= 32) {
                        if (searchFocused) searchQuery += c;
                        else { target->insert(cursorPos, 1, c); cursorPos++; }
                    }
                }
            }

            // Navigare in interiorul textului cu sageti (doar pt Add/Edit)
            if (!searchFocused) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Left && cursorPos > 0) cursorPos--;
                    if (keyPressed->code == sf::Keyboard::Key::Right) {
                        std::string* target = (activeField == 0) ? &newTitle : (activeField == 1) ? &newDesc : (activeField == 2) ? &newExtra : &newInterval;
                        if (cursorPos < (int)target->length()) cursorPos++;
                    }
                }
            }
        }

        // Click stanga
        if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseBtn->button == sf::Mouse::Button::Left) handleMouseClick(mouseBtn->position);
        }

        // Taste globale: Escape (iesire/anulare) si Ctrl+Z (undo)
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                if (state != AppState::Viewing) state = AppState::Viewing;
                else if (searchFocused) searchFocused = false;
                else window.close();
            }
            if (keyPressed->code == sf::Keyboard::Key::Z && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                taskManager.undoLastAction();
            }
        }
    }
}

void GuiManager::handleMouseClick(sf::Vector2i mousePos) {
    // CAZ 1: Add/Edit
    if (state == AppState::AddingTask || state == AppState::EditingTask) {
        // Verificam daca click-ul este in interiorul casetei popup
        if (mousePos.x >= 175 && mousePos.x <= 625 && mousePos.y >= 50 && mousePos.y <= 575) {
            
            // Selectarea campului de text activ in functie de coordonata Y
            if (mousePos.y >= 150 && mousePos.y <= 235) { activeField = 0; cursorPos = newTitle.length(); }
            if (mousePos.y >= 225 && mousePos.y <= 310) { activeField = 1; cursorPos = newDesc.length(); }
            if (mousePos.y >= 300 && mousePos.y <= 385) { activeField = 2; cursorPos = newExtra.length(); }
            
            bool isRec = (selectedType == 2 || (state == AppState::EditingTask && dynamic_cast<RecurringTask*>(targetTask)));
            if (isRec && mousePos.y >= 375 && mousePos.y <= 460) { activeField = 3; cursorPos = newInterval.length(); }
            
            // Selectare tip task
            if (state == AppState::AddingTask && mousePos.y >= 110 && mousePos.y <= 140) {
                if (mousePos.x < 280) selectedType = 0;
                else if (mousePos.x < 380) selectedType = 1;
                else if (mousePos.x < 480) selectedType = 2;
                else selectedType = 3;
            }

            // Logica SAVE si CLOSE
            float btnY = isRec ? 485.f : 440.f;
            if (mousePos.y >= btnY && mousePos.y <= btnY + 45.f) {
                if (mousePos.x >= 400 && mousePos.x <= 520) { // SAVE
                    int interval = 0;
                    try { if (!newInterval.empty()) interval = std::stoi(newInterval); } catch(...) { interval = 3600; }

                    if (state == AppState::AddingTask) {
                        // Creare task nou
                        TaskFactory::TaskType type = (selectedType == 0) ? TaskFactory::TaskType::Simple : 
                                                    (selectedType == 1) ? TaskFactory::TaskType::Deadline : 
                                                    (selectedType == 2) ? TaskFactory::TaskType::Recurring :
                                                                          TaskFactory::TaskType::Category;
                        Task* newTask = TaskFactory::createTask(type, newTitle, newDesc, newExtra, interval);
                        if (parentCategory) parentCategory->addTask(newTask);
                        else taskManager.addTask(newTask);
                    } else {
                        // Actualizare task existent
                        targetTask->setTitle(newTitle);
                        targetTask->setDescription(newDesc);
                        DeadlineTask* dt = dynamic_cast<DeadlineTask*>(targetTask);
                        if (dt) dt->setDeadline(TimeUtils::citesteData(newExtra));
                        RecurringTask* rt = dynamic_cast<RecurringTask*>(targetTask);
                        if (rt) rt->setInterval(Duration(interval));
                    }
                    state = AppState::Viewing;
                }
                if (mousePos.x >= 280 && mousePos.x <= 400) state = AppState::Viewing; // CLOSE
            }
            return;
        }
    }

    // CAZ 2: Interfata principala
    searchFocused = false;
    // Click Search
    if (mousePos.x >= 300 && mousePos.x <= 600 && mousePos.y >= 15 && mousePos.y <= 55) { searchFocused = true; return; }
    
    // Click "+ NEW TASK"
    if (mousePos.x >= 650 && mousePos.x <= 770 && mousePos.y >= 15 && mousePos.y <= 55) {
        state = AppState::AddingTask; parentCategory = nullptr;
        newTitle = ""; newDesc = ""; newExtra = ""; newInterval = "3600";
        activeField = 0; cursorPos = 0; return;
    }

    // Verifica interactiunea cu fiecare task din lista
    float yOffset = 100.f;
    for (Task* t : taskManager.getTasks()) {
        if (checkTaskInteraction(t, mousePos, yOffset, 0)) return;
    }
}

// Verifica daca am apasat pe checkbox, buton de editare sau stergere al unui task
bool GuiManager::checkTaskInteraction(Task* task, sf::Vector2i mousePos, float& yOffset, int indent) {
    float xPos = 40.f + (indent * 35.f);
    float cardWidth = 720.f - (indent * 35.f);
    float cardHeight = 65.f;

    if (mousePos.y >= yOffset && mousePos.y <= yOffset + cardHeight &&
        mousePos.x >= xPos && mousePos.x <= xPos + cardWidth) 
    {
        if (mousePos.x <= xPos + 50.f) { task->markAsDone(); return true; }
        
        // Buton '+' pt categorii
        Category* cat = dynamic_cast<Category*>(task);
        if (cat && mousePos.x >= xPos + cardWidth - 120.f && mousePos.x <= xPos + cardWidth - 90.f) {
            state = AppState::AddingTask;
            parentCategory = cat;
            newTitle = ""; newDesc = ""; newExtra = ""; newInterval = "3600";
            activeField = 0; cursorPos = 0;
            return true;
        }

        if (mousePos.x >= xPos + cardWidth - 85.f && mousePos.x <= xPos + cardWidth - 45.f) {
            state = AppState::EditingTask; targetTask = task;
            newTitle = task->getTitle(); newDesc = task->getDescription();
            newExtra = ""; newInterval = "";
            DeadlineTask* dt = dynamic_cast<DeadlineTask*>(task);
            if (dt) newExtra = TimeUtils::scrieData(dt->getDeadline());
            RecurringTask* rt = dynamic_cast<RecurringTask*>(task);
            if (rt) newInterval = std::to_string(rt->getInterval().count());
            activeField = 0; cursorPos = newTitle.length();
            return true;
        }
        if (mousePos.x >= xPos + cardWidth - 45.f) { taskManager.removeTask(task); return true; }
    }
    yOffset += cardHeight + 12.f;
    Category* cat = dynamic_cast<Category*>(task);
    if (cat) {
        for (Task* sub : cat->getSubTasks()) {
            if (checkTaskInteraction(sub, mousePos, yOffset, indent + 1)) return true;
        }
    }
    return false;
}

// Randeaza grafica pentru un task card
void GuiManager::drawTask(Task* task, float& yOffset, int indent) {
    // Calculam pozitia si latimea in functie de nivelul de indentare (pt subtask-uri)
    float xPos = 40.f + (indent * 35.f);
    float cardWidth = 720.f - (indent * 35.f);
    float cardHeight = 65.f;

    // Detectam daca mouse-ul se afla peste acest task
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    bool isHovered = (mousePos.x >= xPos && mousePos.x <= xPos + cardWidth &&
                      mousePos.y >= yOffset && mousePos.y <= yOffset + cardHeight &&
                      state == AppState::Viewing);

    // Verificam daca task-ul are deadline-ul depasit
    bool isOverdue = false;
    DeadlineTask* dt = dynamic_cast<DeadlineTask*>(task);
    if (dt && !dt->isDone() && dt->remainingTime().count() < 0) isOverdue = true;

    // Umbra cardului
    sf::RectangleShape shadow({cardWidth, cardHeight});
    shadow.setPosition({xPos + 2.f, yOffset + 2.f});
    shadow.setFillColor(sf::Color(0, 0, 0, 50));
    window.draw(shadow);

    // Corpul principal al cardului
    sf::RectangleShape card({cardWidth, cardHeight});
    card.setPosition({xPos, yOffset});
    sf::Color baseColor = cardColor;
    if (isHovered) baseColor = sf::Color(65, 65, 80); // Culoare mai deschisa la hover
    card.setFillColor(baseColor);
    card.setOutlineThickness(1.5f);
    if (isOverdue) card.setOutlineColor(sf::Color(200, 50, 50)); // Contur rosu la overdue
    else card.setOutlineColor(isHovered ? accentColor : sf::Color(60, 60, 60));
    window.draw(card);

    // Bara verticala colorata in functie de tipul task-ului
    sf::RectangleShape typeBar({5.f, cardHeight});
    typeBar.setPosition({xPos, yOffset});
    sf::Color typeColor = sf::Color(150, 150, 150);
    Category* cat = dynamic_cast<Category*>(task);
    if (cat) typeColor = sf::Color(255, 165, 0); // Portocaliu - Categorii
    else if (dynamic_cast<RecurringTask*>(task)) typeColor = sf::Color(0, 200, 255); // Albastru - Recurente
    else if (dt) typeColor = sf::Color(200, 100, 255); // Mov - Deadline
    typeBar.setFillColor(typeColor);
    window.draw(typeBar);

    // Cerc pentru status
    sf::CircleShape status(12.f);
    status.setPosition({xPos + 20.f, yOffset + 20.f});
    status.setOutlineThickness(2.f);
    if (task->isDone()) { 
        status.setFillColor(sf::Color(46, 204, 113)); // Verde daca e gata
        status.setOutlineColor(sf::Color(39, 174, 96)); 
    } else { 
        status.setFillColor(sf::Color::Transparent); 
        status.setOutlineColor(isOverdue ? sf::Color::Red : sf::Color(150, 150, 150)); 
    }
    window.draw(status);

    // Titlul task-ului
    sf::Text titleText(font);
    titleText.setString(task->getTitle());
    titleText.setCharacterSize(19);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(task->isDone() ? sf::Color(150, 150, 150) : textColor);
    titleText.setPosition({xPos + 55.f, yOffset + 12.f});
    if (task->isDone()) titleText.setStyle(sf::Text::StrikeThrough); // Taiat daca e gata
    window.draw(titleText);

    // Descrierea si informatiile despre timp
    sf::Text desc(font);
    std::string info = task->getDescription();
    if (dt && !task->isDone()) {
        auto rem = dt->remainingTime().count();
        if (rem < 0) info += " | !!! OVERDUE";
        else info += " | Time: " + std::to_string(rem / 3600) + "h " + std::to_string((rem % 3600) / 60) + "m";
    }
    desc.setString(info);
    desc.setCharacterSize(13);
    desc.setFillColor(isOverdue ? sf::Color(255, 100, 100) : sf::Color(160, 160, 160));
    desc.setPosition({xPos + 55.f, yOffset + 38.f});
    window.draw(desc);

    // Helper pt desenarea butoanelor mici
    auto drawBtn = [&](const std::string& label, float offset, sf::Color col) {
        sf::Text btn(font);
        btn.setString(label);
        btn.setCharacterSize(18);
        btn.setFillColor(col);
        btn.setPosition({xPos + cardWidth - offset, yOffset + 20.f});
        window.draw(btn);
    };
    if (cat) drawBtn("+", 105.f, isHovered ? sf::Color::Green : sf::Color(50, 150, 50));
    drawBtn("E", 70.f, isHovered ? sf::Color::Cyan : sf::Color(100, 150, 200));
    drawBtn("X", 35.f, isHovered ? sf::Color::Red : sf::Color(180, 80, 80));

    // Crestem Y-ul pentru urmatorul element si desenam recursiv subtask-urile
    yOffset += cardHeight + 12.f;
    if (cat) {
        for (Task* sub : cat->getSubTasks()) {
            drawTask(sub, yOffset, indent + 1);
        }
    }
}

void GuiManager::drawPopup() {
    // Ajustam inaltimea popup-ului daca avem nevoie de campul suplimentar pt interval
    bool isRec = (selectedType == 2 || (state == AppState::EditingTask && dynamic_cast<RecurringTask*>(targetTask)));
    float popupHeight = isRec ? 520.f : 450.f;

    // Overlay intunecat pe tot ecranul
    sf::RectangleShape overlay({800.f, 600.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    // Caseta centrala
    sf::RectangleShape box({450.f, popupHeight});
    box.setPosition({175.f, 50.f});
    box.setFillColor(sf::Color(45, 45, 55));
    box.setOutlineThickness(3.f);
    box.setOutlineColor(accentColor);
    window.draw(box);

    // Titlu popup (NEW TASK / EDIT TASK)
    sf::Text titleText(font);
    std::string headerStr = (state == AppState::AddingTask) ? "NEW TASK" : "EDIT TASK";
    if (parentCategory) headerStr += " IN " + parentCategory->getTitle();
    titleText.setString(headerStr);
    titleText.setCharacterSize(20);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition({200.f, 70.f});
    titleText.setFillColor(accentColor);
    window.draw(titleText);

    // Selector pentru tipul de task (doar la adaugare)
    if (state == AppState::AddingTask) {
        std::string types[] = {"Simple", "Deadline", "Recurring", "Category"};
        for (int i = 0; i < 4; i++) {
            sf::Text t(font); t.setString(types[i]); t.setCharacterSize(12);
            t.setPosition({200.f + (i * 95.f), 110.f});
            t.setFillColor(selectedType == i ? sf::Color::Yellow : sf::Color::White);
            window.draw(t);
        }
    } else {
        // La editare afisam doar tipul curent
        std::string typeLabel = "Type: ";
        if (dynamic_cast<RecurringTask*>(targetTask)) typeLabel += "Recurring";
        else if (dynamic_cast<DeadlineTask*>(targetTask)) typeLabel += "Deadline";
        else if (dynamic_cast<Category*>(targetTask)) typeLabel += "Category";
        else typeLabel += "Simple";
        sf::Text t(font); t.setString(typeLabel); t.setCharacterSize(14);
        t.setPosition({200.f, 110.f}); t.setFillColor(sf::Color::Yellow);
        window.draw(t);
    }

    // Desenarea campurilor de input
    std::string labels[] = {"Title", "Description", "Deadline/Date", "Interval (s)"};
    std::string* vals[] = {&newTitle, &newDesc, &newExtra, &newInterval};
    int numFields = isRec ? 4 : (selectedType == 3 ? 2 : 3);

    for (int i = 0; i < numFields; i++) {
        sf::Text l(font); l.setString(labels[i]); l.setCharacterSize(11);
        l.setFillColor(sf::Color(180, 180, 180));
        l.setPosition({200.f, 150.f + (i * 75.f)});
        window.draw(l);

        sf::RectangleShape f({400.f, 35.f});
        f.setPosition({200.f, 170.f + (i * 75.f)});
        f.setFillColor(sf::Color(30, 30, 35));
        f.setOutlineThickness(activeField == i ? 2.f : 1.f);
        f.setOutlineColor(activeField == i ? sf::Color::Cyan : sf::Color(80, 80, 80));
        window.draw(f);

        // Adaugam cursorul vizual '|' in campul activ
        std::string d = *vals[i];
        if (activeField == i) d.insert(cursorPos, "|");
        sf::Text v(font); v.setString(d); v.setCharacterSize(14);
        v.setPosition({210.f, 177.f + (i * 75.f)});
        window.draw(v);
    }

    // SAVE si CLOSE
    float btnY = isRec ? 485.f : 440.f;
    auto drawBigBtn = [&](const std::string& text, float x, sf::Color col) {
        sf::RectangleShape b({120.f, 45.f});
        b.setPosition({x, btnY});
        b.setFillColor(col);
        window.draw(b);
        sf::Text t(font); t.setString(text); t.setCharacterSize(16);
        t.setStyle(sf::Text::Bold); t.setPosition({x + 35.f, btnY + 10.f});
        window.draw(t);
    };
    drawBigBtn("SAVE", 400.f, sf::Color(39, 174, 96));
    drawBigBtn("CLOSE", 280.f, sf::Color(192, 57, 43));
}

void GuiManager::render() {
    window.clear(sf::Color(25, 25, 30));
    
    // Desenare fundal Header
    sf::RectangleShape hb({800.f, 70.f}); hb.setFillColor(sf::Color(35, 35, 45)); window.draw(hb);

    // Titlu aplicatie
    sf::Text h(font); h.setString("Tudu List"); h.setCharacterSize(22);
    h.setStyle(sf::Text::Bold); h.setFillColor(sf::Color::White); h.setPosition({30.f, 20.f});
    window.draw(h);

    // Search bar
    sf::RectangleShape sb({300.f, 40.f}); sb.setPosition({300.f, 15.f});
    sb.setFillColor(sf::Color(45, 45, 55)); sb.setOutlineThickness(searchFocused ? 2.f : 1.f);
    sb.setOutlineColor(searchFocused ? sf::Color::Cyan : sf::Color(100, 100, 100)); window.draw(sb);

    sf::Text st(font); std::string ds = searchQuery.empty() ? "Search..." : searchQuery;
    if (searchFocused) ds += "|"; st.setString(ds); st.setCharacterSize(14);
    st.setFillColor(searchQuery.empty() ? sf::Color(150, 150, 150) : sf::Color::White); st.setPosition({315.f, 25.f});
    window.draw(st);

    // Butonul "+ NEW TASK"
    sf::RectangleShape ab({120.f, 40.f}); ab.setPosition({650.f, 15.f});
    ab.setFillColor(accentColor); window.draw(ab);
    sf::Text at(font); at.setString("+ NEW TASK"); at.setCharacterSize(14);
    at.setStyle(sf::Text::Bold); at.setPosition({665.f, 25.f}); window.draw(at);

    // Lista de task-uri (filtrata sau nu)
    float yOffset = 100.f;
    std::vector<Task*> dl;
    if (searchQuery.empty()) dl = taskManager.getTasks();
    else dl = taskManager.filterTasks([this](Task* t) {
        // Filtrare case-insensitive dupa titlu
        std::string title = t->getTitle(); std::string search = searchQuery;
        std::transform(title.begin(), title.end(), title.begin(), ::tolower);
        std::transform(search.begin(), search.end(), search.begin(), ::tolower);
        return title.find(search) != std::string::npos;
    });

    // Desenam fiecare task din lista curenta
    for (Task* t : dl) drawTask(t, yOffset, 0);
    
    // Daca suntem in stare de adaugare/editare, suprapunem Popup-ul
    if (state != AppState::Viewing) drawPopup();
    
    window.display();
}
