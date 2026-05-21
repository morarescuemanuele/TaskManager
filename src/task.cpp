#include "../include/task.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, const Task& task) {
    task.display(os);
    return os;
}

int Task::generatorId = 0;

Task::Task(std::string t, std::string d, bool s)
    : title(t), description(d), status(s), id(generatorId++) {}

void Task::setStatus(bool s) { status = s; }
void Task::setTitle(std::string t) { title = t; }
void Task::setDescription(std::string d) { description = d; }

int Task::getId() const { return id; }
std::string Task::getTitle() const { return title; }
std::string Task::getDescription() const { return description; }
bool Task::isDone() const { return status; }

void Task::display(std::ostream& os) const {
    os << "#" << id << " [" << (status ? "X" : " ") << "] " << title << ": " << description;
}

SimpleTask::SimpleTask(std::string t, std::string d, bool s) : Task(t, d, s) {}

// Pentru task-ul simplu, doar inversam statusul
void SimpleTask::markAsDone() {
    setStatus(!isDone());
}

void SimpleTask::display(std::ostream& os) const {
    Task::display(os);
    os << " (Task Simplu)\n";
}

DeadlineTask::DeadlineTask(std::string t, std::string d, Time dead, bool s)
    : Task(t, d, s), deadline(dead) {}

Time DeadlineTask::getDeadline() const { return deadline; }
void DeadlineTask::setDeadline(Time d) { deadline = d; }

// Seteaza deadline-ul folosind componente de data/ora
Time DeadlineTask::setDeadline(int an, int luna, int zi, int ora, int minut) {
    std::tm t = {};
    t.tm_year = an - 1900;
    t.tm_mon = luna - 1;
    t.tm_mday = zi;
    t.tm_hour = ora;
    t.tm_min = minut;
    t.tm_isdst = -1;
    
    std::time_t tt = std::mktime(&t);
    this->deadline = std::chrono::system_clock::from_time_t(tt);
    return this->deadline;
}

// Diferenta dintre deadline si momentul actual
Duration DeadlineTask::remainingTime() const { 
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<Duration>(deadline - now);
}

void DeadlineTask::display(std::ostream& os) const {
    Task::display(os);
    std::time_t t_c = std::chrono::system_clock::to_time_t(deadline);
    std::tm* local_t = std::localtime(&t_c);
    os << " | Deadline: " << std::put_time(local_t, "%d-%m-%Y %H:%M") << "\n";
}

void DeadlineTask::markAsDone() {
    setStatus(!isDone());
}

RecurringTask::RecurringTask(std::string t, std::string d, Time dead, Duration i, bool s) 
    : DeadlineTask(t, d, dead, s), interval(i) {}

Duration RecurringTask::getInterval() const { return interval; }
void RecurringTask::setInterval(Duration i) { interval = i; }

// Cand un task recurent e gata, ii mutam deadline-ul in viitor
void RecurringTask::markAsDone() {
    auto now = std::chrono::system_clock::now();
    while (getDeadline() <= now) {
        setDeadline(getDeadline() + interval);
    }
    setStatus(false);
}

void RecurringTask::display(std::ostream& os) const {
    DeadlineTask::display(os);
    os << "   [Se repeta la fiecare " << (int)interval.count() << " secunde]\n";
}

Category::Category(std::string name, std::string description) 
    : Task(name, description) {}

void Category::addTask(Task* t) {
    subTasks.push_back(t);
}

void Category::removeTask(Task* t) {
    auto it = std::find(subTasks.begin(), subTasks.end(), t);
    if (it != subTasks.end()) {
        subTasks.erase(it);
    }
}

// Propagarea actiunii catre toti copiii (daca categoria e gata, toate task-urile sunt gata)
void Category::markAsDone() {
    bool currentStatus = isDone();
    setStatus(!currentStatus);
    for (Task* t : subTasks) {
        if (t->isDone() != !currentStatus) {
            t->markAsDone();
        }
    }
}

void Category::display(std::ostream& os) const {
    os << "\n>>> Category: " << getTitle() << " (" << getDescription() << ") <<<\n";
    if (subTasks.empty()) {
        os << "   (no tasks)\n";
    } else {
        for (Task* t : subTasks) {
            os << "  |-- ";
            t->display(os);
        }
    }
    os << "------------------------------------------\n";
}

const std::vector<Task*>& Category::getSubTasks() const { return subTasks; }


Category::~Category() {
    for (Task* t : subTasks) {
        delete t;
    }
    subTasks.clear();
}
