#include "include/manager.h"
#include "include/task_factory.h"
#include "include/gui_manager.h"
#include "include/logger.h"
#include <iostream>

// Deci stiu ca apare eroare ca nu gaseste headerele, dar daca dau compile le gaseste, nu stiu clar care e problema sincer
// insa aplicatia merge, asta conteaza =)

int main()
{
    TaskManager manager;
    Logger logger;
    manager.addObserver(&logger);
    Category* facultate = new Category("Facultate", "Proiecte semestrul 2");
    facultate->addTask(TaskFactory::createTask(TaskFactory::TaskType::Simple, "Tema POO", "Interfata SFML"));
    facultate->addTask(TaskFactory::createTask(TaskFactory::TaskType::Deadline, "Examen Analiza", "Serii si Integrale", "25/06/2026 10:00"));

    manager.addTask(facultate);
    manager.addTask(TaskFactory::createTask(TaskFactory::TaskType::Simple, "Cumparaturi", "Oua, Lapte, Mere"));
    manager.addTask(TaskFactory::createTask(TaskFactory::TaskType::Recurring, "Sala", "Cardio saptamanal", "15/05/2026 18:00", 604800));

    GuiManager gui(manager);
    if (gui.init()) {
        gui.run();
    } else {
        return -1;
    }

    return 0;
}
