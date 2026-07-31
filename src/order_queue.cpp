#include "order_queue.h"
#include "file_handler.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;


OrderQueue::OrderQueue() {
    front = nullptr;
    rear = nullptr;
    historyHead = nullptr;
    count = 0;
}


OrderQueue::~OrderQueue() {

    OrderNode* current = front;
    while (current != nullptr) {
        OrderNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }

    current = historyHead;
    while (current != nullptr) {
        OrderNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

bool OrderQueue::isEmpty() const {
    return front == nullptr;
}

int OrderQueue::getQueueSize() const {
    return count;
}

// ---------------------------------------------------------
// Enquene
// ---------------------------------------------------------
void OrderQueue::enqueue(Order* newOrder) {
    OrderNode* newNode = new OrderNode;
    newNode->data = newOrder;
    newNode->next = nullptr;

    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    count++;
}

// ---------------------------------------------------------
// Dequeue
// ---------------------------------------------------------
Order* OrderQueue::dequeue() {
    if (isEmpty()) {
        return nullptr;
    }

    OrderNode* temp = front;
    Order* processedOrder = temp->data;

    front = front->next;
    

    if (front == nullptr) {
        rear = nullptr;
    }
    count--;


    temp->next = historyHead;
    historyHead = temp;

    return processedOrder;
}

Order* OrderQueue::peek() const {
    if (isEmpty()) return nullptr;
    return front->data;
}

// ---------------------------------------------------------
// UI and Display
// ---------------------------------------------------------
void OrderQueue::displayPendingOrders() const {
    if (isEmpty()) {
        cout << "\n  [INFO] The order queue is currently empty." << endl;
        return;
    }

    cout << "\n  ========================================================" << endl;
    cout << "                    PENDING ORDER QUEUE                   " << endl;
    cout << "  ========================================================" << endl;
    cout << "  " << left 
         << setw(5)  << "Pos" 
         << setw(15) << "Order ID" 
         << setw(12) << "Student ID" 
         << setw(10) << "Item ID" 
         << setw(5)  << "Qty" << endl;
    cout << "  --------------------------------------------------------" << endl;

    OrderNode* current = front;
    int position = 1;
    while (current != nullptr) {
        cout << "  " << left 
             << setw(5)  << position 
             << setw(15) << current->data->orderID 
             << setw(12) << current->data->studentID 
             << setw(10) << current->data->itemID 
             << setw(5)  << current->data->quantity << endl;
        current = current->next;
        position++;
    }
    cout << "  ========================================================" << endl;
    cout << "  Total Pending: " << count << endl;
}

void OrderQueue::displayCompletedOrders() const {
    if (historyHead == nullptr) {
        cout << "\n  [INFO] No completed orders in history." << endl;
        return;
    }
    cout << "\n  ========================================================" << endl;
    cout << "                 RECENTLY PROCESSED ORDERS                " << endl;
    cout << "  ========================================================" << endl;
    
    OrderNode* current = historyHead;
    while (current != nullptr) {
        cout << "  - Order ID: " << current->data->orderID 
             << " | Processed Status: " << current->data->orderStatus << endl;
        current = current->next;
    }
    cout << "  ========================================================" << endl;
}

// ============================================================
// TUI
// ============================================================
OrderQueue globalOrderQueue;

void runOrderQueueModule() {
    int choice;
    bool running = true;

    while (running) {
        cout << "\n  ============================================" << endl;
        cout << "     TASK 1: ORDER QUEUE MANAGEMENT MODULE    " << endl;
        cout << "  ============================================" << endl;
        cout << "    1. Load PENDING Orders from Dataset" << endl;
        cout << "    2. Display Pending Orders (Queue)" << endl;
        cout << "    3. Process Next Order (Dequeue)" << endl;
        cout << "    4. Display Processed History" << endl;
        cout << "    0. Return to Main Menu" << endl;
        cout << "  ============================================" << endl;
        cout << "    Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\n  [ERROR] Invalid input." << endl;
            continue;
        }

        switch (choice) {
            case 1: {

                if (orderCount == 0) {
                    orderCount = loadOrders("datasets/orders.csv", orders, MAX_ORDERS);
                }
                int added = 0;
                for (int i = 0; i < orderCount; i++) {

                    if (strcmp(orders[i].orderStatus, "PENDING") == 0) {
                        globalOrderQueue.enqueue(&orders[i]);
                        added++;
                    }
                }
                cout << "\n  [SUCCESS] " << added << " pending orders enqueued." << endl;
                break;
            }
            case 2:
                globalOrderQueue.displayPendingOrders();
                break;
            case 3: {
                Order* nextOrder = globalOrderQueue.dequeue();
                if (nextOrder != nullptr) {
                    cout << "\n  [SYSTEM] Order " << nextOrder->orderID << " has been dequeued." << endl;
                    cout << "  >> Routing to Stall Assignment Module..." << endl;
                    

                    strcpy(nextOrder->orderStatus, "PREPARING");
                } else {
                    cout << "\n  [ERROR] Queue is empty. No orders to process!" << endl;
                }
                break;
            }
            case 4:
                globalOrderQueue.displayCompletedOrders();
                break;
            case 0:
                running = false;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice." << endl;
        }
    }
}