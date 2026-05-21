#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <iostream>

using Time = std::chrono::system_clock::time_point;
using Duration = std::chrono::seconds;

class Task
{
    int id;
    static int generatorId; // Generator static pentru ID-uri unice
    std::string title, description;
    bool status; // true = finalizat, false = nefinalizat :')
public:
    void setStatus(bool s);
    void setTitle(std::string t);
    void setDescription(std::string d);

    Task(std::string t, std::string d, bool s = false);
    
    // Metoda pur virtuala pentru marcarea ca finalizat
    virtual void markAsDone() = 0; 
    
    std::string getTitle() const;
    std::string getDescription() const;
    int getId() const;
    bool isDone() const;

    // Afisare specifica fiecarui tip de task
    virtual void display(std::ostream& os) const = 0;
    
    friend std::ostream& operator<<(std::ostream& os, const Task& task);
    
    virtual ~Task() = default;
};

// Task simplu
class SimpleTask : public Task
{
public:
    SimpleTask(std::string t, std::string d, bool s = false);
    void markAsDone() override;
    void display(std::ostream& os) const override;
};

// Task cu termen limita
class DeadlineTask : public Task
{
    Time deadline;
public:
    DeadlineTask(std::string t, std::string d, Time dead, bool s = false);
    Time getDeadline() const;
    void setDeadline(Time d);
    Time setDeadline(int an, int luna, int zi, int ora = 0, int minut = 0);
    Duration remainingTime() const; // Calculeaza timpul ramas pana la deadline
    void display(std::ostream& os) const override;
    void markAsDone() override;
};

// Task care se repeta la un anumit interval dupa ce este finalizat
class RecurringTask : public DeadlineTask {
    Duration interval;
public:
    RecurringTask(std::string t, std::string d, Time dead, Duration i, bool s = false);
    void markAsDone() override; // Devanseaza deadline-ul automat
    Duration getInterval() const;
    void setInterval(Duration i);
    void display(std::ostream& os) const override;
};

// Categorie care poate contine alte task-uri (Pattern Composite)
class Category : public Task {
    std::vector<Task*> subTasks;
public:
    Category(std::string name, std::string description = "");
    void addTask(Task* t);
    void removeTask(Task* t);
    void markAsDone() override; // Marcheaza toate subtask-urile ca finalizate
    void display(std::ostream& os) const override;
    const std::vector<Task*>& getSubTasks() const;
    ~Category() override;
};
