#include "session_history.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

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

bool SessionHistoryStack::isEmpty() const {
    return topNode == nullptr;
}

int SessionHistoryStack::getSize() const {
    return count;
}

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
    cout << "  Total steps recorded: " << count << endl;
}

void runHistoryModule() {
    SessionHistoryStack sessionStack;
    bool running = true;

    sessionStack.push("System Login", "Student logged into the Kiosk successfully.");

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
            continue;
        }

        switch (choice) {
            case 1:
                sessionStack.push("Menu Browsed", "User is viewing the Food & Beverage categories.");
                cout << "\n  [SYSTEM] Action recorded." << endl;
                break;
            case 2:
                sessionStack.push("Item Selected", "User clicked on an item to view details.");
                cout << "\n  [SYSTEM] Action recorded." << endl;
                break;
            case 3:
                sessionStack.push("Quantity Updated", "User changed item quantity from 1 to 2.");
                cout << "\n  [SYSTEM] Action recorded." << endl;
                break;
            case 4:
                sessionStack.push("Checkout Screen", "User reached payment confirmation screen.");
                cout << "\n  [SYSTEM] Action recorded." << endl;
                break;
            case 5: {
                SessionStep undoneStep;
                if (sessionStack.pop(undoneStep)) {
                    cout << "\n  [NAVIGATE BACKWARD] Undid action: " << undoneStep.actionName << endl;
                    cout << "  Returning to previous state..." << endl;
                } else {
                    cout << "\n  [ERROR] Cannot navigate back. History is empty!" << endl;
                }
                break;
            }
            case 6:
                sessionStack.displayHistoryTrace();
                break;
            case 7:
                sessionStack.clear();
                cout << "\n  [SYSTEM] Session cleared. User logged out." << endl;
                break;
            case 0:
                running = false;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice." << endl;
        }
    }
}
