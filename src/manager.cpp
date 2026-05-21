#include "../include/manager.h"
#include <iostream>
#include <algorithm>

// Observer Pattern: Inregistreaza un nou ascultator (ex: Logger)
void TaskManager::addObserver(Observer* obs) {
    observers.push_back(obs);
}

// Notifica toti observatorii despre un eveniment text
void TaskManager::notifyObservers(const std::string& message) {
    for (Observer* obs : observers) {
        obs->onEvent(message);
    }
}

// Adaugare task la nivelul principal
void TaskManager::addTask(Task* t){
    taskList.push_back(t);
    notifyObservers("Task added: " + t->getTitle());
}

// Cautare in adancime pentru a gasi si sterge un task dintr-o categorie
bool TaskManager::findAndRemoveRecursively(Task* target, const std::vector<Task*>& searchList) {
    for (Task* item : searchList) {
        Category* cat = dynamic_cast<Category*>(item);
        if (cat) {
            const auto& subs = cat->getSubTasks();
            auto subIt = std::find(subs.begin(), subs.end(), target);

            if (subIt != subs.end()) {
                undoStack.add(target); // Salvam in istoric inainte de stergere
                cat->removeTask(target);
                notifyObservers("Task deleted from category: " + target->getTitle());
                return true;
            }

            // continua cautarea in sub-categorii
            if (findAndRemoveRecursively(target, subs)) {
                return true;
            }
        }
    }
    return false;
}

// Stergere: verifica atat lista principala cat si sub-categoriile
void TaskManager::removeTask(Task* t) {
    // Verificare rapida in lista principala
    auto it = std::find(taskList.begin(), taskList.end(), t);
    if (it != taskList.end()) {
        std::string title = (*it)->getTitle();
        undoStack.add(*it); // Salvam in stiva de Undo
        taskList.erase(it);
        notifyObservers("Task deleted: " + title);
        return;
    }

    // Daca nu e in root, pornim cautarea recursiva in categorii
    findAndRemoveRecursively(t, taskList);
}

// Restaureaza ultimul task sters
void TaskManager::undoLastAction(){
    try {
        Task* restoredTask = undoStack.undo();
        taskList.push_back(restoredTask); // Restaurare in lista principala
        std::cout << "[Undo] Task '" << restoredTask->getTitle() << "' was restored.\n";
        notifyObservers("Undoed task: " + restoredTask->getTitle());
    } catch (const std::runtime_error& e) {
        std::cerr << "[Eroare Undo] " << e.what() << "\n"; // Istoricul e gol
    }
}

// Afiseaza structura arborescenta in consola
void TaskManager::afiseazaToateTaskurile() const {
    std::cout << "--- Task list ---\n";
    for (const Task* t : taskList) { 
        std::cout << *t << "\n"; 
    }
}

TaskManager::~TaskManager() {
    // Stergem task-urile active
    for (Task* t : taskList) {
        delete t;
    }
    // Stergem task-urile din stiva de Undo
    while(!undoStack.isEmpty()){
        delete undoStack.undo();
    }
}
