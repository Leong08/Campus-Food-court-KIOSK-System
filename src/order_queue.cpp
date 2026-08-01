// ============================================================
// order_queue.cpp — Linked Queue Implementation
// Task 1: Order Queue Management Module
// CT077-3-2-DSTR | Lab Work #2
// ============================================================

#include "order_queue.h"
#include "file_handler.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// 跨平台原生延时，规避 MinGW 编译器 Bug
static void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// 终端无污染高速清屏
static void clearScreen() {
    for (int i = 0; i < 45; i++) {
        cout << "\n";
    }
}

// 炫酷的进度条加载过渡
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

static void formatTime(long timestamp, char* output) {
    time_t temp = (time_t)timestamp;
    struct tm* t = localtime(&temp);
    if (t != nullptr) {
        strftime(output, 20, "%H:%M:%S", t);
    } else {
        strcpy(output, "N/A");
    }
}

// ============================================================
// QUEUE CLASS IMPLEMENTATION
// ============================================================

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

bool OrderQueue::isEmpty() const { return front == nullptr; }
int OrderQueue::getQueueSize() const { return count; }

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

Order* OrderQueue::dequeue() {
    if (isEmpty()) return nullptr;

    OrderNode* temp = front;
    Order* processedOrder = temp->data;

    front = front->next;
    if (front == nullptr) rear = nullptr;
    count--;

    temp->next = historyHead;
    historyHead = temp;

    return processedOrder;
}

Order* OrderQueue::peek() const {
    if (isEmpty()) return nullptr;
    return front->data;
}

void OrderQueue::displayPendingOrders() const {
    if (isEmpty()) {
        cout << "\n  [INFO] The order queue is currently empty." << endl;
        return;
    }

    cout << "\n  =========================================================================================================" << endl;
    cout << "                                         PENDING KIOSK ORDER QUEUE                                         " << endl;
    cout << "  =========================================================================================================" << endl;
    cout << "  " << left 
         << setw(5)  << "Pos" 
         << setw(13) << "Order ID" 
         << setw(12) << "Order Time" 
         << setw(12) << "Student ID" 
         << setw(26) << "Item Name" 
         << setw(6)  << "Qty" 
         << setw(12) << "Total(RM)" 
         << setw(10) << "Priority" 
         << setw(10) << "Payment" << endl;
    cout << "  ---------------------------------------------------------------------------------------------------------" << endl;

    OrderNode* current = front;
    int position = 1;
    char timeStr[20];

    while (current != nullptr) {
        formatTime(current->data->timeStamp, timeStr);
        
        cout << "  " << left 
             << setw(5)  << position 
             << setw(13) << current->data->orderID 
             << setw(12) << timeStr 
             << setw(12) << current->data->studentID 
             << setw(26) << current->data->itemName 
             << setw(6)  << current->data->quantity 
             << "RM" << fixed << setprecision(2) << setw(10) << current->data->totalPrice 
             << setw(10) << (current->data->priorityFlag == 1 ? "HIGH" : "Normal") 
             << setw(10) << current->data->paymentStatus << endl;
        current = current->next;
        position++;
    }
    cout << "  =========================================================================================================" << endl;
    cout << "  Total Pending in Queue: " << count << endl;
}

void OrderQueue::displayCompletedOrders() const {
    if (historyHead == nullptr) {
        cout << "\n  [INFO] No completed orders in history." << endl;
        return;
    }
    cout << "\n  ========================================================================" << endl;
    cout << "                         RECENTLY PROCESSED ORDERS                        " << endl;
    cout << "  ========================================================================" << endl;
    
    OrderNode* current = historyHead;
    while (current != nullptr) {
        cout << "  - Order ID: " << current->data->orderID 
             << " | Item: " << current->data->itemName
             << " | Status: " << current->data->orderStatus << endl;
        current = current->next;
    }
    cout << "  ========================================================================" << endl;
}

// ============================================================
// YK Task 1 Module UI (Auto-Load & Dynamic Item Menu)
// ============================================================
OrderQueue globalOrderQueue;

void runOrderQueueModule() {
    static bool autoLoaded = false;
    
    clearScreen();
    if (!autoLoaded) {
        playTransition("Initializing Self-Order Kiosk Queue & Syncing Datasets...");
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
        autoLoaded = true;
        cout << "  [SYSTEM] Connection successful! " << added << " active orders fetched." << endl;
        sleepMs(1200);
        clearScreen();
    } else {
        playTransition("Opening Order Queue Management Terminal...");
        clearScreen();
    }

    int choice;
    bool running = true;

    while (running) {
        cout << "\n  ============================================" << endl;
        cout << "     TASK 1: ORDER QUEUE MANAGEMENT MODULE    " << endl;
        cout << "  ============================================" << endl;
        cout << "    1. Accept & Record NEW Student Order (Kiosk)" << endl;
        cout << "    2. Display Pending Orders (Queue)" << endl;
        cout << "    3. Process Next Order (Dequeue)" << endl;
        cout << "    4. Display Processed History" << endl;
        cout << "    0. Return to Main Menu" << endl;
        cout << "  ============================================" << endl;
        cout << "    Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            
            clearScreen();
            playTransition("Handling input buffer error...");
            clearScreen();
            cout << "\n  [ERROR] Invalid menu choice. Please type a valid option number." << endl;
            sleepMs(1500);
            clearScreen();
            continue;
        }

        switch (choice) {
            case 1: {
                clearScreen();
                playTransition("Launching Student Ordering Interface...");
                clearScreen();

                if (orderCount >= MAX_ORDERS) {
                    cout << "\n  [ERROR] Maximum limit of orders reached in system database." << endl;
                    sleepMs(1500);
                    clearScreen();
                    break;
                }
                if (studentCount == 0) studentCount = loadStudents("datasets/students.csv", students, MAX_STUDENTS);
                if (menuItemCount == 0) menuItemCount = loadMenuItems("datasets/menu_items.csv", menuItems, MAX_MENU_ITEMS);

                Order newOrder;
                memset(&newOrder, 0, sizeof(Order));

                int maxOrderNum = 0;
                for (int i = 0; i < orderCount; i++) {
                    int num = atoi(orders[i].orderID + 4);
                    if (num > maxOrderNum) maxOrderNum = num;
                }
                sprintf(newOrder.orderID, "ORD_%05d", maxOrderNum + 1);
                newOrder.timeStamp = (long)time(NULL);

                cout << "\n  [KIOSK SIMULATION] Create New Order" << endl;
                cout << "  Enter Student ID (e.g. TP076118): ";
                cin >> setw(20) >> newOrder.studentID;

                cout << "\n  ------ SELECT ITEM FROM KIOSK MENU ------" << endl;
                for (int i = 0; i < menuItemCount; i++) {
                    cout << "    " << left << setw(3) << (i + 1) << ". " 
                         << setw(28) << menuItems[i].itemName 
                         << "RM" << fixed << setprecision(2) << setw(8) << menuItems[i].price 
                         << " (" << (menuItems[i].availability ? "In Stock" : "Out of Stock") << ")" << endl;
                }
                cout << "  -----------------------------------------" << endl;

                int itemChoice;
                while (true) {
                    cout << "  Select Item Number (1-" << menuItemCount << "): ";
                    if (cin >> itemChoice && itemChoice >= 1 && itemChoice <= menuItemCount) {
                        if (menuItems[itemChoice - 1].availability) {
                            break;
                        } else {
                            cout << "  [ERROR] This item is currently out of stock! Choose another." << endl;
                        }
                    } else {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "  [ERROR] Invalid select. Please enter a valid number." << endl;
                    }
                }

                MenuItem& selected = menuItems[itemChoice - 1];
                strcpy(newOrder.itemID, selected.itemID);
                strcpy(newOrder.itemName, selected.itemName);
                double unitPrice = selected.price;

                cout << "  Enter quantity: ";
                cin >> newOrder.quantity;
                while (newOrder.quantity < 1) {
                    cout << "  [ERROR] Quantity must be at least 1: ";
                    cin >> newOrder.quantity;
                }

                newOrder.totalPrice = newOrder.quantity * unitPrice;
                strcpy(newOrder.paymentStatus, "PAID");
                newOrder.priorityFlag = 0;
                strcpy(newOrder.orderStatus, "PENDING");
                strcpy(newOrder.stallID, "");

                orders[orderCount] = newOrder;
                
                clearScreen();
                playTransition("Authorising transaction & registering order...");
                clearScreen();
                
                globalOrderQueue.enqueue(&orders[orderCount]);
                orderCount++;

                cout << "  [SUCCESS] Order " << newOrder.orderID << " (" << selected.itemName << " x" << newOrder.quantity << ") accepted & enqueued!" << endl;
                sleepMs(2000);
                
                clearScreen();
                playTransition("Returning to Queue Controller...");
                clearScreen();
                break;
            }
            case 2:
                clearScreen();
                playTransition("Decrypting and loading live Queue table...");
                clearScreen();
                
                globalOrderQueue.displayPendingOrders();
                pauseScreen();
                
                clearScreen();
                playTransition("Packing data and closing report...");
                clearScreen();
                break;
            case 3: {
                clearScreen();
                playTransition("Allocating pipeline bandwidth and routing order to stall...");
                clearScreen();

                Order* nextOrder = globalOrderQueue.dequeue();
                if (nextOrder != nullptr) {
                    cout << "  [SYSTEM] Order " << nextOrder->orderID << " has been successfully processed." << endl;
                    cout << "  >> Signal sent: Marked status as [PREPARING] in master directory." << endl;
                    strcpy(nextOrder->orderStatus, "PREPARING");
                } else {
                    cout << "  [ERROR] Dequeue failed. Pending Queue is currently empty." << endl;
                }
                sleepMs(2500);
                
                clearScreen();
                playTransition("Refreshing controller panel...");
                clearScreen();
                break;
            }
            case 4:
                clearScreen();
                playTransition("Scanning transaction registers for completed records...");
                clearScreen();

                globalOrderQueue.displayCompletedOrders();
                pauseScreen();

                clearScreen();
                playTransition("Returning to Queue Controller...");
                clearScreen();
                break;
            case 0:
                clearScreen();
                playTransition("Safely disconnecting Order Queue Module...");
                clearScreen();
                running = false;
                break;
            default:
                clearScreen();
                cout << "\n  [ERROR] Invalid option choice. Self-redirecting to Menu in 1.5 seconds..." << endl;
                sleepMs(1500);
                clearScreen();
                break;
        }
    }
}