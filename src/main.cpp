#include <iostream>
#include "structures.h"
#include "order_queue.h"        // Task 1: YK
#include "stall_assignment.h"   // Task 2: Sherwin
#include "session_history.h"    // Task 3: Max
#include "menu_bst.h"           // Task 4: Carrie

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// 全局基础数据实例化
Student students[MAX_STUDENTS];
Stall stallArray[MAX_STALLS];
MenuItem menuItems[MAX_MENU_ITEMS];
Order orders[MAX_ORDERS];

int studentCount = 0;
int stallCount = 0;
int menuItemCount = 0;
int orderCount = 0;

// 原生延时
static void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// 高级清屏
static void clearScreen() {
    for (int i = 0; i < 45; i++) cout << "\n";
}

// 进度条动画
static void playTransition(const char* message) {
    cout << "\n  " << message << "\n  [";
    for (int i = 0; i < 15; i++) {
        sleepMs(50);
        cout << "#" << flush;
    }
    cout << "] 100% Complete!\n" << endl;
    sleepMs(150);
}

int main() {
    // 【开机动画】
    clearScreen();
    playTransition("Booting APU Smart Kiosk OS & Securing Environment Link...");
    clearScreen();

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
        cout << "============================================\n";
        cout << "Enter choice: ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            clearScreen();
            cout << "Invalid choice. Please enter a number.\n";
            sleepMs(1200);
            clearScreen();
            continue;
        }

        switch (choice) {
            case 1:
                clearScreen();
                runOrderQueueModule();
                clearScreen();
                break;
            case 2:
                clearScreen();
                runStallModule();
                clearScreen();
                break;
            case 3:
                clearScreen();
                runSessionHistory();
                clearScreen();
                break;
            case 4:
                clearScreen();
                runMenuSearchModule();
                clearScreen();
                break;
            case 0:
                clearScreen();
                playTransition("Shutting down APU Smart Kiosk OS safely...");
                clearScreen();
                running = false;
                break;
            default:
                clearScreen();
                cout << "Invalid menu number. Please try again.\n";
                sleepMs(1200);
                clearScreen();
                break;
        }
    }
    return 0;
}