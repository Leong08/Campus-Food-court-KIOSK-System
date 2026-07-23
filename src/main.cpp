// ============================================================
// main.cpp — Thin Shared Driver
// Campus Food Court Self-Order Kiosk System (Part 2)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================

#include <iostream>
#include "structures.h"
#include "stall_assignment.h"
#include "session_history.h"

using namespace std;

// Define global arrays (extern in structures.h)
Student  students[MAX_STUDENTS];
Stall    stallArray[MAX_STALLS];
MenuItem menuItems[MAX_MENU_ITEMS];
Order    orders[MAX_ORDERS];

int studentCount  = 0;
int stallCount    = 0;
int menuItemCount = 0;
int orderCount    = 0;

int main() {
    bool running = true;
    while (running) {
        cout << "\n============================================\n";
        cout << "CAMPUS FOOD COURT SYSTEM - MAIN MENU\n";
        cout << "============================================\n";
        cout << "1. Task 1: Module 1\n";
        cout << "2. Task 2: Stall Assignment Module\n";
        cout << "3. Task 3: Session History Stack\n";
        cout << "4. Task 4: Module 4\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid choice. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 2:
                runStallModule();
                break;
            case 3:
                runSessionHistory();
                break;
            case 0:
                running = false;
                break;
            default:
                cout << "Module not implemented yet or invalid choice.\n";
                break;
        }
    }
    return 0;
}
