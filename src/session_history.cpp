#include "session_history.h"
#include <iostream>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

static void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

static void clearScreen() {
    for (int i = 0; i < 45; i++) cout << "\n";
}

static void playTransition(const char* message) {
    cout << "\n  " << message << "\n  [";
    for (int i = 0; i < 15; i++) {
        sleepMs(50);
        cout << "#" << flush;
    }
    cout << "] 100% Complete!\n" << endl;
    sleepMs(150);
}

static void pauseScreen() {
    cout << "\n  Press [Enter] to return to the menu...";
    cin.ignore(); 
    cin.get();    
}

SessionHistoryStack::SessionHistoryStack() {
    topNode = nullptr;
    count = 0;
}

SessionHistoryStack::~SessionHistoryStack() {
    clear();
}

void SessionHistoryStack::push(const char* action, const char* details) {
    HistoryNode* newNode = new HistoryNode;
    strcpy(newNode->step.actionName, action);
    strcpy(newNode->step.details, details);
    
    newNode->next = topNode;
    topNode = newNode;
    count++;
}

bool SessionHistoryStack::pop(SessionStep& poppedStep) {
    if (isEmpty()) {
        return false;
    }
    HistoryNode* temp = topNode;
    poppedStep = temp->step;
    topNode = topNode->next;
    delete temp;
    count--;
    return true;
}

bool SessionHistoryStack::peek(SessionStep& currentStep) const {
    if (isEmpty()) {
        return false;
    }
    currentStep = topNode->step;
    return true;
}

bool SessionHistoryStack::isEmpty() const { return topNode == nullptr; }
int SessionHistoryStack::getSize() const { return count; }

void SessionHistoryStack::clear() {
    SessionStep temp;
    while (!isEmpty()) {
        pop(temp);
    }
}

void SessionHistoryStack::displayCurrentState() const {
    cout << "\n  [Current Active Kiosk State]" << endl;
    if (isEmpty()) {
        cout << "  -> Main Screen (Home / No active session)" << endl;
    } else {
        cout << "  -> Action  : " << topNode->step.actionName << endl;
        cout << "  -> Details : " << topNode->step.details << endl;
    }
}

void SessionHistoryStack::displayHistoryTrace() const {
    if (isEmpty()) {
        cout << "\n  [INFO] Session history is completely empty." << endl;
        return;
    }

    cout << "\n  ========================================================" << endl;
    cout << "           SESSION HISTORY TRACE (NEWEST TO OLDEST)       " << endl;
    cout << "  ========================================================" << endl;
    
    HistoryNode* current = topNode;
    int stepNum = count;
    
    while (current != nullptr) {
        cout << "  Step " << left << setw(2) << stepNum;
        if (current == topNode) {
            cout << " [CURRENT] : ";
        } else {
            cout << "           : ";
        }
        cout << current->step.actionName << " - " << current->step.details << endl;
        current = current->next;
        stepNum--;
    }
    cout << "  ========================================================" << endl;
}

void runHistoryModule() {
    SessionHistoryStack sessionStack;
    clearScreen();
    
    playTransition("Syncing Session Memory registers with local variables...");
    sessionStack.push("System Login", "Student logged into the Kiosk successfully.");
    clearScreen();

    bool running = true;
    while (running) {
        cout << "\n  ======================================================" << endl;
        cout << "    TASK 3: KIOSK SESSION NAVIGATION (STACK LIFO)       " << endl;
        cout << "  ======================================================" << endl;
        sessionStack.displayCurrentState();
        cout << "  ------------------------------------------------------" << endl;
        cout << "    [Simulate User Actions]" << endl;
        cout << "    1. Browse Menu" << endl;
        cout << "    2. Select Item" << endl;
        cout << "    3. Update Quantity" << endl;
        cout << "    4. Proceed to Checkout" << endl;
        cout << "    --------------------------------------------------" << endl;
        cout << "    [Navigation Features]" << endl;
        cout << "    5. Navigate Backward / Undo (Pop Stack)" << endl;
        cout << "    6. View Full Navigation Trace" << endl;
        cout << "    7. Clear Session (Logout)" << endl;
        cout << "    0. Return to Main System Menu" << endl;
        cout << "  ======================================================" << endl;
        cout << "    Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            clearScreen();
            playTransition("Handling input error...");
            clearScreen();
            cout << "\n  [ERROR] Invalid input. Please select a valid option." << endl;
            sleepMs(1200);
            clearScreen();
            continue;
        }

        switch (choice) {
            case 1:
                clearScreen();
                playTransition("Recording screen transition to menu categories...");
                sessionStack.push("Menu Browsed", "User is viewing the Food & Beverage categories.");
                clearScreen();
                cout << "\n  [SYSTEM] Action successfully recorded in LIFO stack." << endl;
                sleepMs(1200);
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            case 2:
                clearScreen();
                playTransition("Recording dynamic selection vector...");
                sessionStack.push("Item Selected", "User clicked on an item to view details.");
                clearScreen();
                cout << "\n  [SYSTEM] Action successfully recorded in LIFO stack." << endl;
                sleepMs(1200);
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            case 3:
                clearScreen();
                playTransition("Recording quantity buffer updates...");
                sessionStack.push("Quantity Updated", "User changed item quantity from 1 to 2.");
                clearScreen();
                cout << "\n  [SYSTEM] Action successfully recorded in LIFO stack." << endl;
                sleepMs(1200);
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            case 4:
                clearScreen();
                playTransition("Redirecting and logging checkout pipeline...");
                sessionStack.push("Checkout Screen", "User reached payment confirmation screen.");
                clearScreen();
                cout << "\n  [SYSTEM] Action successfully recorded in LIFO stack." << endl;
                sleepMs(1200);
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            case 5: {
                clearScreen();
                playTransition("Initiating pop() backward navigation signal...");
                clearScreen();
                SessionStep undoneStep;
                if (sessionStack.pop(undoneStep)) {
                    cout << "\n  [NAVIGATE BACKWARD] Undid action: " << undoneStep.actionName << endl;
                    cout << "  Returning to previous state: " << undoneStep.details << endl;
                } else {
                    cout << "\n  [ERROR] Cannot navigate back. Navigation Trace is at main base step!" << endl;
                }
                pauseScreen();
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            }
            case 6:
                clearScreen();
                playTransition("Reading stack memory blocks from top to bottom...");
                clearScreen();
                sessionStack.displayHistoryTrace();
                pauseScreen();
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            case 7:
                clearScreen();
                playTransition("Frees stack memory and logging off user...");
                sessionStack.clear();
                clearScreen();
                cout << "\n  [SYSTEM] Session cleared. Safe state loaded." << endl;
                sleepMs(1200);
                clearScreen();
                playTransition("Returning to Session Controller...");
                clearScreen();
                break;
            case 0:
                clearScreen();
                playTransition("Safely freeing session variables & returning to Main Menu...");
                clearScreen();
                running = false;
                break;
            default:
                clearScreen();
                cout << "\n  [ERROR] Invalid choice. Safe-redirecting..." << endl;
                sleepMs(1200);
                clearScreen();
        }
    }
}