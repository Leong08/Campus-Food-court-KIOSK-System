#include <iostream>
#include "structures.h"
#include "order_queue.h"        // Task 1: YK
#include "stall_assignment.h"   // Task 2: Sherwin
#include "session_history.h"    // Task 3: Max
#include "menu_bst.h"           // Task 4: Carrie

using namespace std;

// Actual instantiation of the globally shared data sets
Student students[MAX_STUDENTS];
Stall stallArray[MAX_STALLS];
MenuItem menuItems[MAX_MENU_ITEMS];
Order orders[MAX_ORDERS];

int studentCount = 0;
int stallCount = 0;
int menuItemCount = 0;
int orderCount = 0;

int main() {
    bool running = true;
    while (running) {
        cout << "\n============================================\n";
        cout << "CAMPUS FOOD COURT SYSTEM - MAIN MENU\n";
        cout << "============================================\n";
        cout << "1. Task 1: Order Queue Management (YK)\n";
        cout << "2. Task 2: Stall Assignment Module (Sherwin)\n";
        cout << "3. Task 3: Session History Stack (Max)\n";
        cout << "4. Task 4: Menu Item Management BST (Carrie)\n";
        cout << "0. Exit System\n";
        cout << "Enter choice: ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid choice. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: runOrderQueueModule(); break;
            case 2: runStallModule();      break;
            case 3: runHistoryModule();    break;
            case 4: runMenuSearchModule(); break;
            case 0: running = false;       break;
            default: cout << "Invalid choice.\n"; break;
        }
    }
    cout << "\nSystem terminated successfully.\n";
    return 0;
}