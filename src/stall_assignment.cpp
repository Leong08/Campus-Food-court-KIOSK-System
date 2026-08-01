#include "stall_assignment.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "file_handler.h"

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

CircularQueue::CircularQueue() {
    front        = 0;
    rear         = -1;
    count        = 0;
    assignPtr    = 0;
    historyCount = 0;
}

bool CircularQueue::enqueue(Stall* stall) {
    if (isFull()) {
        cout << "  [ERROR] Circular Queue is full. Cannot add stall." << endl;
        return false;
    }
    rear = (rear + 1) % MAX_STALLS;
    queue[rear] = stall;
    count++;
    return true;
}

Stall* CircularQueue::dequeue() {
    if (isEmpty()) {
        cout << "  [ERROR] Circular Queue is empty. Cannot dequeue." << endl;
        return nullptr;
    }
    Stall* removed = queue[front];
    front = (front + 1) % MAX_STALLS;
    count--;
    if (count > 0) {
        assignPtr = assignPtr % count;
    } else {
        assignPtr = 0;
    }
    return removed;
}

Stall* CircularQueue::peek() const {
    if (isEmpty()) {
        cout << "  [ERROR] Circular Queue is empty." << endl;
        return nullptr;
    }
    return queue[front];
}

bool CircularQueue::isFull() const { return count == MAX_STALLS; }
bool CircularQueue::isEmpty() const { return count == 0; }
int CircularQueue::getSize() const { return count; }

bool CircularQueue::assignOrder(Order& order) {
    if (isEmpty()) {
        cout << "  [ERROR] No stalls in queue. Load stalls first." << endl;
        return false;
    }

    int stallsChecked = 0;
    int startPtr = assignPtr;  

    cout << "\n  [Round-Robin Assignment]" << endl;
    cout << "  Starting from position " << assignPtr << endl;

    while (stallsChecked < count) {
        int actualIdx = (front + assignPtr) % MAX_STALLS;
        Stall* stall  = queue[actualIdx];

        cout << "  -> Checking " << stall->stallName << " (" << stall->stallID << "): ";

        if (!stall->opening) {
            cout << "CLOSED - Skipping" << endl;
            assignPtr = (assignPtr + 1) % count;
            stallsChecked++;
            continue;
        }

        if (stall->maxCapacity <= 0) {
            cout << "FULL (0 capacity) - Skipping" << endl;
            assignPtr = (assignPtr + 1) % count;
            stallsChecked++;
            continue;
        }

        cout << "AVAILABLE (capacity: " << stall->maxCapacity << ") - ASSIGNED!" << endl;
        strcpy(order.stallID, stall->stallID);
        stall->maxCapacity--;
        stall->currentQueueLength++;

        if (historyCount < MAX_STALL_HISTORY) {
            strcpy(history[historyCount].orderID, order.orderID);
            strcpy(history[historyCount].stallID, stall->stallID);
            strcpy(history[historyCount].stallName, stall->stallName);
            history[historyCount].timestamp = order.timeStamp;
            historyCount++;
        }

        cout << "\n  Result: Order " << order.orderID << " -> " << stall->stallName << endl;
        cout << "  Updated: Capacity=" << stall->maxCapacity << " | Queue Length=" << stall->currentQueueLength << endl;
        assignPtr = (assignPtr + 1) % count;
        return true;
    }

    cout << "\n  [WARNING] System overload! All stalls are closed or at full capacity." << endl;
    cout << "  Order " << order.orderID << " could not be assigned." << endl;
    assignPtr = startPtr;  
    return false;
}

int CircularQueue::findStall(const char* stallID) const {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        if (strcmp(queue[idx]->stallID, stallID) == 0) {
            return idx;
        }
    }
    return -1;
}

bool CircularQueue::updateCapacity(const char* stallID, int delta) {
    int idx = findStall(stallID);
    if (idx == -1) {
        cout << "  [ERROR] Stall " << stallID << " not found in queue." << endl;
        return false;
    }

    int newCap = queue[idx]->maxCapacity + delta;
    int newLen = queue[idx]->currentQueueLength - delta;

    if (newCap > 15) newCap = 15;
    if (newCap < 0) newCap = 0;
    if (newLen > 15) newLen = 15;
    if (newLen < 0) newLen = 0;

    queue[idx]->maxCapacity = newCap;
    queue[idx]->currentQueueLength = newLen;
    return true;
}

bool CircularQueue::cancelOrderAssignment(const char* stallID) {
    return updateCapacity(stallID, 1);  
}

void CircularQueue::displayAll() const {
    if (isEmpty()) {
        cout << "\n  [INFO] Circular Queue is empty. No stalls loaded." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "           STALLS IN CIRCULAR QUEUE                    " << endl;
    cout << "  ====================================================" << endl;
    cout << "  " << left
         << setw(4)  << "Pos"
         << setw(12) << "Stall ID"
         << setw(18) << "Name"
         << setw(10) << "Cuisine"
         << setw(8)  << "Open"
         << setw(10) << "Capacity"
         << setw(10) << "QueueLen"
         << endl;
    cout << "  ----------------------------------------------------" << endl;

    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        const Stall& s = *queue[idx];

        cout << "  " << left
             << setw(4)  << i
             << setw(12) << s.stallID
             << setw(18) << s.stallName
             << setw(10) << s.cuisineType
             << setw(8)  << (s.opening ? "Yes" : "No")
             << setw(10) << s.maxCapacity
             << setw(10) << s.currentQueueLength;

        if (i == assignPtr) {
            cout << " <-- NEXT";
        }
        cout << endl;
    }

    cout << "  ====================================================" << endl;
    cout << "  Queue Size: " << count << " | Assign Ptr: " << assignPtr << endl;
    cout << "  ====================================================" << endl;
}

void CircularQueue::displayStatus() const {
    if (isEmpty()) {
        cout << "\n  [INFO] No stalls loaded." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "              STALL STATUS OVERVIEW                    " << endl;
    cout << "  ====================================================" << endl;

    int totalCapacity = 0;
    int totalUsed     = 0;

    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        const Stall& s = *queue[idx];

        cout << "\n  " << s.stallName << " (" << s.stallID << ")" << endl;
        cout << "    Status:    " << (s.opening ? "OPEN" : "CLOSED") << endl;
        cout << "    Cuisine:   " << s.cuisineType << endl;
        cout << "    Capacity:  " << s.maxCapacity << " remaining / 15 total" << endl;
        cout << "    In Queue:  " << s.currentQueueLength << " orders" << endl;

        cout << "    Load:      [";
        int maxSlots = 15;
        int filled   = s.currentQueueLength;
        if (filled > maxSlots) filled = maxSlots;

        for (int j = 0; j < maxSlots; j++) {
            if (j < filled) cout << "#";
            else            cout << "-";
        }
        cout << "] " << s.currentQueueLength << "/" << maxSlots << endl;

        totalCapacity += 15;
        totalUsed     += s.currentQueueLength;
    }

    cout << "\n  ----------------------------------------------------" << endl;
    cout << "  Overall Load: " << totalUsed << "/" << totalCapacity
         << " (" << (totalCapacity > 0 ? (totalUsed * 100 / totalCapacity) : 0) << "%)" << endl;
    cout << "  ====================================================" << endl;
}

void CircularQueue::displayHistory() const {
    if (historyCount == 0) {
        cout << "\n  [INFO] No assignments recorded yet." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "            ASSIGNMENT HISTORY LOG                     " << endl;
    cout << "  ====================================================" << endl;
    cout << "  " << left << setw(5) << "#" << setw(15) << "Order ID" << setw(12) << "Stall ID" << setw(20) << "Stall Name" << endl;
    cout << "  ----------------------------------------------------" << endl;

    for (int i = 0; i < historyCount; i++) {
        cout << "  " << left << setw(5) << (i + 1) << setw(15) << history[i].orderID << setw(12) << history[i].stallID << setw(20) << history[i].stallName << endl;
    }

    cout << "  ====================================================" << endl;
    cout << "  Total Assignments: " << historyCount << endl;
    cout << "  ====================================================" << endl;
}

CircularQueue stallQueue;
bool dataLoaded = false;
void displayBanner();
void displayOrders();
void displayPendingOrders();
void assignNextPending();
void assignAllPending();
void createNewOrder();
void cancelOrder();

void runStallModule() {
    clearScreen();
    static bool autoLoaded = false;
    if (!autoLoaded) {
        playTransition("Initializing Stall Assignment Databases & Fetching Datasets...");
        stallCount = loadStalls("datasets/stalls.csv", stallArray, MAX_STALLS);
        for (int i = 0; i < stallCount; i++) {
            stallQueue.enqueue(&stallArray[i]);
        }
        orderCount = loadOrders("datasets/orders.csv", orders, MAX_ORDERS);
        studentCount = loadStudents("datasets/students.csv", students, MAX_STUDENTS);
        menuItemCount = loadMenuItems("datasets/menu_items.csv", menuItems, MAX_MENU_ITEMS);
        autoLoaded = true;
        dataLoaded = true;
        cout << "  [SYSTEM] Stall data synchronization completed successfully." << endl;
        sleepMs(1200);
        clearScreen();
    } else {
        playTransition("Opening Stall Assignment Terminal...");
        clearScreen();
    }
    
    displayBanner();
    int choice;
    bool running = true;

    while (running) {
        cout << "\n  ============================================" << endl;
        cout << "    STALL ASSIGNMENT MODULE - MAIN MENU       " << endl;
        cout << "  ============================================" << endl;
        cout << "    1. Display Stalls in Circular Queue" << endl;
        cout << "    2. Display Stall Status & Capacity" << endl;
        cout << "    3. Assign Next Pending Order" << endl;
        cout << "    4. Assign All Pending Orders" << endl;
        cout << "    5. Create New Order" << endl;
        cout << "    6. Cancel Order (Restore Capacity)" << endl;
        cout << "    7. Display All Orders" << endl;
        cout << "    8. Display Assignment History" << endl;
        cout << "    0. Return to Main Menu" << endl;
        cout << "  ============================================" << endl;
        cout << "    Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            clearScreen();
            playTransition("Handling input error...");
            clearScreen();
            cout << "\n  [ERROR] Invalid input. Please enter a valid option." << endl;
            sleepMs(1200);
            clearScreen();
            continue;
        }

        switch (choice) {
            case 1: 
                clearScreen();
                playTransition("Accessing circular queue registers...");
                clearScreen();
                stallQueue.displayAll();  
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 2: 
                clearScreen();
                playTransition("Scanning food court hardware load...");
                clearScreen();
                stallQueue.displayStatus(); 
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 3: 
                clearScreen();
                playTransition("Executing Round-Robin assignment algorithm...");
                clearScreen();
                assignNextPending();      
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 4: 
                clearScreen();
                playTransition("Executing batch assignment routing...");
                clearScreen();
                assignAllPending();       
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 5: 
                clearScreen();
                playTransition("Launching Kiosk Order Creation Interface...");
                clearScreen();
                createNewOrder();         
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 6: 
                clearScreen();
                playTransition("Fetching active cancellable order records...");
                clearScreen();
                cancelOrder();            
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 7: 
                clearScreen();
                playTransition("Requesting master order logs from memory...");
                clearScreen();
                displayOrders();          
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 8: 
                clearScreen();
                playTransition("Decrypting allocation session history...");
                clearScreen();
                stallQueue.displayHistory(); 
                pauseScreen();
                clearScreen();
                playTransition("Returning to Stall Controller...");
                clearScreen();
                break;
            case 0:
                clearScreen();
                playTransition("Disconnecting Stall Assignment Module...");
                clearScreen();
                running = false;
                break;
            default:
                clearScreen();
                cout << "\n  [ERROR] Invalid choice. Redirecting in 1.5s..." << endl;
                sleepMs(1500);
                clearScreen();
        }
    }
}

void displayBanner() {
    cout << "  ====================================================" << endl;
    cout << "  |   CAMPUS FOOD COURT SELF-ORDER KIOSK SYSTEM      |" << endl;
    cout << "  |   Task 2: Stall Assignment Module                |" << endl;
    cout << "  |   By: Sherwin A/L Jesudass (Circular Queue)      |" << endl;
    cout << "  ====================================================" << endl;
}

void displayOrders() {
    if (orderCount == 0) {
        cout << "\n  [INFO] No orders loaded." << endl;
        return;
    }

    cout << "\n  ====================================================================" << endl;
    cout << "                            ALL MASTER ORDERS                         " << endl;
    cout << "  ====================================================================" << endl;
    cout << "  " << left << setw(13) << "Order ID" << setw(11) << "Student" << setw(11) << "Stall" << setw(11) << "Item" << setw(5)  << "Qty" << setw(10) << "Total(RM)" << setw(11) << "Payment" << setw(12) << "Status" << endl;
    cout << "  --------------------------------------------------------------------" << endl;

    for (int i = 0; i < orderCount; i++) {
        cout << "  " << left << setw(13) << orders[i].orderID << setw(11) << orders[i].studentID << setw(11) << orders[i].stallID << setw(11) << orders[i].itemID << setw(5)  << orders[i].quantity << fixed << setprecision(2) << setw(10) << orders[i].totalPrice << setw(11) << orders[i].paymentStatus << setw(12) << orders[i].orderStatus << endl;
    }
    cout << "  ====================================================================" << endl;
}

void displayPendingOrders() {
    bool found = false;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "  " << orders[i].orderID << " | Student: " << orders[i].studentID << " | Item: " << orders[i].itemID << " | Qty: " << orders[i].quantity << " | RM" << fixed << setprecision(2) << orders[i].totalPrice << endl;
            found = true;
        }
    }
    if (!found) cout << "  No pending orders." << endl;
}

void assignNextPending() {
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "\n  Found pending order: " << orders[i].orderID << " (Student: " << orders[i].studentID << ", Item: " << orders[i].itemID << ")" << endl;
            if (stallQueue.assignOrder(orders[i])) {
                strcpy(orders[i].orderStatus, "PREPARING");
                cout << "  Order status updated to: PREPARING" << endl;
            }
            return;
        }
    }
    cout << "\n  [INFO] No pending orders found." << endl;
}

void assignAllPending() {
    int assigned = 0;
    int failed   = 0;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "\n  Processing: " << orders[i].orderID << endl;
            if (stallQueue.assignOrder(orders[i])) {
                strcpy(orders[i].orderStatus, "PREPARING");
                assigned++;
            } else {
                failed++;
                cout << "  Cannot assign — stalls full. Stopping." << endl;
                break;
            }
        }
    }
    cout << "\n  ---- Batch Assignment Complete ----" << endl;
    cout << "  Assigned: " << assigned << " | Failed: " << failed << endl;
}

void createNewOrder() {
    if (orderCount >= MAX_ORDERS) {
        cout << "\n  [ERROR] Maximum number of orders reached." << endl;
        return;
    }

    Order newOrder;
    memset(&newOrder, 0, sizeof(Order));

    int maxOrderNum = 0;
    for (int i = 0; i < orderCount; i++) {
        int num = atoi(orders[i].orderID + 4);
        if (num > maxOrderNum) maxOrderNum = num;
    }
    sprintf(newOrder.orderID, "ORD_%05d", maxOrderNum + 1);
    newOrder.timeStamp = (long)time(NULL);

    cout << "\n  --- Active Students ---" << endl;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].status, "Active") == 0) {
            cout << "  " << students[i].studentID << " - " << students[i].fullName << " (Balance: RM" << fixed << setprecision(2) << students[i].accBalance << ")" << endl;
        }
    }

    cout << "\n  Enter Student ID: ";
    cin >> setw(20) >> newOrder.studentID;

    int studentIdx = -1;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].studentID, newOrder.studentID) == 0) {
            studentIdx = i;
            break;
        }
    }

    if (studentIdx == -1) {
        cout << "  [ERROR] Student not found." << endl;
        return;
    }
    if (strcmp(students[studentIdx].status, "Active") != 0) {
        cout << "  [ERROR] Student account is " << students[studentIdx].status << ". Cannot place order." << endl;
        return;
    }

    cout << "\n  --- Available Menu Items ---" << endl;
    for (int i = 0; i < menuItemCount; i++) {
        if (menuItems[i].availability) {
            cout << "  " << left << setw(12) << menuItems[i].itemID << setw(28) << menuItems[i].itemName << fixed << setprecision(2) << setw(10) << menuItems[i].price << setw(10) << menuItems[i].stallID << endl;
        }
    }

    cout << "\n  Enter Item ID: ";
    cin >> setw(20) >> newOrder.itemID;

    int itemIdx = -1;
    for (int i = 0; i < menuItemCount; i++) {
        if (strcmp(menuItems[i].itemID, newOrder.itemID) == 0) {
            itemIdx = i;
            break;
        }
    }

    if (itemIdx == -1) {
        cout << "  [ERROR] Item not found." << endl;
        return;
    }
    if (!menuItems[itemIdx].availability) {
        cout << "  [ERROR] Item is out of stock." << endl;
        return;
    }

    cout << "  Enter quantity: ";
    cin >> newOrder.quantity;
    if (newOrder.quantity < 1) {
        cout << "  [ERROR] Quantity must be at least 1." << endl;
        return;
    }

    newOrder.totalPrice = newOrder.quantity * menuItems[itemIdx].price;

    if (students[studentIdx].accBalance < newOrder.totalPrice) {
        cout << "  [ERROR] Insufficient balance!" << endl;
        cout << "  Required: RM" << fixed << setprecision(2) << newOrder.totalPrice << " | Available: RM" << students[studentIdx].accBalance << endl;

        strcpy(newOrder.paymentStatus, "FAILED");
        strcpy(newOrder.orderStatus, "FAILED");
        strcpy(newOrder.stallID, "");
        strcpy(newOrder.itemName, menuItems[itemIdx].itemName);
        orders[orderCount] = newOrder;
        orderCount++;
        return;
    }

    students[studentIdx].accBalance -= newOrder.totalPrice;
    strcpy(newOrder.paymentStatus, "PAID");
    strcpy(newOrder.itemName, menuItems[itemIdx].itemName); 
    newOrder.priorityFlag = 0;
    newOrder.pickupTime   = 0;
    strcpy(newOrder.orderStatus, "PENDING");
    strcpy(newOrder.stallID, "");  

    orders[orderCount] = newOrder;
    orderCount++;

    cout << "\n  Order " << newOrder.orderID << " created successfully!" << endl;
    cout << "  Item: " << menuItems[itemIdx].itemName << " x" << newOrder.quantity << endl;
    cout << "  Total: RM" << fixed << setprecision(2) << newOrder.totalPrice << endl;

    cout << "\n  Assigning to stall via Circular Queue round-robin..." << endl;
    if (stallQueue.assignOrder(orders[orderCount - 1])) {
        strcpy(orders[orderCount - 1].orderStatus, "PREPARING");
        cout << "  Order status: PREPARING" << endl;
    } else {
        cout << "  Order remains PENDING — no stall available." << endl;
    }
}

void cancelOrder() {
    cout << "\n  --- Active Orders (Cancellable) ---" << endl;
    bool hasActive = false;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PREPARING") == 0 || strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "  " << orders[i].orderID << " | Student: " << orders[i].studentID << " | Stall: " << orders[i].stallID << " | RM" << fixed << setprecision(2) << orders[i].totalPrice << " | Status: " << orders[i].orderStatus << endl;
            hasActive = true;
        }
    }

    if (!hasActive) {
        cout << "  No active orders to cancel." << endl;
        return;
    }

    cout << "\n  Enter Order ID to cancel: ";
    char cancelID[20];
    cin >> setw(20) >> cancelID;

    int orderIdx = -1;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderID, cancelID) == 0) {
            orderIdx = i;
            break;
        }
    }

    if (orderIdx == -1) {
        cout << "  [ERROR] Order not found." << endl;
        return;
    }

    if (strcmp(orders[orderIdx].orderStatus, "CANCELLED") == 0) {
        cout << "  [ERROR] Order is already cancelled." << endl;
        return;
    }

    if (strcmp(orders[orderIdx].orderStatus, "FULFILLED") == 0) {
        cout << "  [ERROR] Cannot cancel a fulfilled order." << endl;
        return;
    }

    playTransition("Terminating assignment registry & executing refund sequence...");
    strcpy(orders[orderIdx].orderStatus, "CANCELLED");
    strcpy(orders[orderIdx].paymentStatus, "REFUNDED");

    if (strlen(orders[orderIdx].stallID) > 0) {
        cout << "  Restoring capacity for stall " << orders[orderIdx].stallID << "..." << endl;
        stallQueue.cancelOrderAssignment(orders[orderIdx].stallID);
        cout << "  Stall capacity restored (+1 slot)." << endl;
    }

    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].studentID, orders[orderIdx].studentID) == 0) {
            students[i].accBalance += orders[orderIdx].totalPrice;
            cout << "  Refunded RM" << fixed << setprecision(2) << orders[orderIdx].totalPrice << " to " << students[i].fullName << endl;
            break;
        }
    }
    cout << "\n  Order " << cancelID << " cancelled successfully!" << endl;
}