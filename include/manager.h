#pragma once
#include <vector>
#include "task.h"
#include "history.h"
#include "observer.h"

class TaskManager
{
    std::vector<Task*> taskList; // Lista de task-uri si categorii
    History<Task*> undoStack;    // Stiva pentru  Undo
    std::vector<Observer*> observers; // Colectie de observatori (Pattern Observer)
    
    // Notifica observatorii inregistrati
    void notifyObservers(const std::string& message);
    
    // Metoda privata ajutatoare pentru stergere recursiva in interiorul categoriilor
    bool findAndRemoveRecursively(Task* target, const std::vector<Task*>& searchList);

public:
    void addObserver(Observer* obs);
    void removeTask(Task* t);
    void addTask(Task* t);
    void undoLastAction();
    void afiseazaToateTaskurile() const;
    const std::vector<Task*>& getTasks() const { return taskList; }

    // Metoda template pentru filtrarea task-urilor dupa un criteriu (predicat)
    template <typename Predicate>
    std::vector<Task*> filterTasks(Predicate p) const {
        std::vector<Task*> results;
        for (Task* t : taskList) {
            if (p(t)) {
                results.push_back(t);
            }
            // Cautare recursiva in categorii (Composite Pattern)
            Category* cat = dynamic_cast<Category*>(t);
            if (cat) {
                for (Task* sub : cat->getSubTasks()) {
                    if (p(sub)) {
                        results.push_back(sub);
                    }
                }
            }
        }
        return results;
    }

    ~TaskManager();
};
