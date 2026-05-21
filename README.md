# Task Manager Pro

A desktop Task Management application built in C++ using SFML for the graphical interface. 

This project demonstrates core Object-Oriented Programming (OOP) concepts and Design Patterns applied to a practical tool for organizing daily activities.

## Features

*   **Task Variety:** Create Simple tasks, tasks with Deadlines, and Recurring tasks.
*   **Categories:** Group related tasks together using Categories (Sub-tasks).
*   **Interactive UI:** A custom-built, modern graphical interface to view, add, edit, and interact with tasks using the mouse.
*   **Real-time Updates:** Deadline tasks dynamically display the remaining time or highlight in red when overdue.
*   **Search Functionality:** Quickly filter tasks by title using the built-in search bar.
*   **Activity History (Undo):** Safely undo the deletion of a task or category with a simple key press (`Ctrl+Z`).
*   **Action Logging:** An internal logging system tracks operations (adds, deletes, restores) using the Observer pattern.

## Technologies & Concepts Used

*   **Language:** C++
*   **Graphics:** SFML (Simple and Fast Multimedia Library)
*   **Design Patterns:**
    *   *Factory:* Centralized creation of different task types.
    *   *Observer:* System monitoring and logging.
    *   *Composite:* Hierarchical grouping of tasks within categories.

## How to Run

1.  **Prerequisites:** Ensure you have a C++ compiler (like GCC/MinGW) and SFML 3.x (or compatible) installed on your system.
2.  **Compilation:** Use the included `Makefile`. Open your terminal in the project root and run:
    ```bash
    make
    ```
3.  **Execution:** Run the compiled executable:
    ```bash
    ./app.exe
    ```

##  Controls

*   **Mouse Left-Click:** Interact with the UI (Add, Edit, Complete, Delete tasks).
*   **Keyboard:** Type in text fields, use `Backspace` to delete, and `Enter` to switch fields.
*   **Escape:** Cancel current action or close the application.
*   **Ctrl+Z:** Undo the last deleted task.
