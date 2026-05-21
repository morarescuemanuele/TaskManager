#pragma once
#include <deque>
#include <stdexcept>

template <class T>
class History
{
    std::deque<T> list;
    int maxsize;
public:
    History(int size = 10) : maxsize(size) {}

    // Adauga un element nou si sterge cel mai vechi daca depasim limita
    void add(T item){
        if(list.size() >= (size_t)maxsize)
        {
            list.pop_front();
            list.push_back(item);
        }else{
            list.push_back(item);
        }
    }

    // Extrage si returneaza ultimul element adaugat (LIFO)
    T undo()
    {
        if(list.empty()){
            throw std::runtime_error("Istoric gol");
        }
        T lastItem = list.back();
        list.pop_back();
        return lastItem;
    }

    bool isEmpty() const
    {
        return list.empty();
    }

    // Returneaza referinta la lista pentru vizualizare / iterare
    const std::deque<T>& getItems() const {
        return list;
    }
};
