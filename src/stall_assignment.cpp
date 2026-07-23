// ============================================================
// stall_assignment.cpp — Circular Queue Implementation
// Task 2: Stall Assignment Module
// By: Sherwin A/L Jesudass (TP075823)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================
// This file implements a Circular Queue from scratch (no STL).
//
// WHY CIRCULAR QUEUE?
// The stall assignment follows a round-robin pattern: orders
// rotate through stalls 1 → 2 → 3 → 4 → 1 → 2 → ...
// A circular queue naturally wraps around using modulo
// arithmetic:  nextIndex = (current + 1) % count
// This avoids resetting the pointer and ensures fair, balanced
// distribution of orders across all stalls.
//
// CIRCULAR QUEUE DIAGRAM:
//   ┌──────┬──────┬──────┬──────┐
//   │ ST01 │ ST02 │ ST03 │ ST04 │  ← fixed-size array
//   └──────┴──────┴──────┴──────┘
//      ↑                    ↑
//    front                 rear
//           ↑
//        assignPtr (rotates after each assignment)
//
// When assignPtr reaches the end, it wraps to index 0.
// ============================================================

#include "stall_assignment.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "file_handler.h"

using namespace std;

// ---- Constructor ----
CircularQueue::CircularQueue() {
    front        = 0;
    rear         = -1;
    count        = 0;
    assignPtr    = 0;
    historyCount = 0;
}

// ============================================================
// CORE CIRCULAR QUEUE OPERATIONS
// ============================================================

// Enqueue — Add a stall to the rear of the circular queue.
// Time: O(1) | Space: O(1)
bool CircularQueue::enqueue(Stall* stall) {
    if (isFull()) {
        cout << "  [ERROR] Circular Queue is full. Cannot add stall." << endl;
        return false;
    }

    // Wrap rear index around using modulo (circular behaviour)
    rear = (rear + 1) % MAX_STALLS;
    queue[rear] = stall;
    count++;
    return true;
}

// Dequeue — Remove and return the stall at the front.
// Time: O(1) | Space: O(1)
Stall* CircularQueue::dequeue() {
    if (isEmpty()) {
        cout << "  [ERROR] Circular Queue is empty. Cannot dequeue." << endl;
        return nullptr;
    }

    Stall* removed = queue[front];

    // Advance front index with wrapping
    front = (front + 1) % MAX_STALLS;
    count--;

    // Keep assignPtr within valid range
    if (count > 0) {
        assignPtr = assignPtr % count;
    } else {
        assignPtr = 0;
    }

    return removed;
}

// Peek — View the stall at the front without removing it.
// Time: O(1) | Space: O(1)
Stall* CircularQueue::peek() const {
    if (isEmpty()) {
        cout << "  [ERROR] Circular Queue is empty." << endl;
        return nullptr;
    }
    return queue[front];
}

// Time: O(1)
bool CircularQueue::isFull() const {
    return count == MAX_STALLS;
}

// Time: O(1)
bool CircularQueue::isEmpty() const {
    return count == 0;
}

// Time: O(1)
int CircularQueue::getSize() const {
    return count;
}

// ============================================================
// ROUND-ROBIN STALL ASSIGNMENT
// ============================================================

// assignOrder — The core function of this module.
//
// Uses a rotating pointer (assignPtr) to distribute orders
// fairly across all stalls. The pointer advances after each
// assignment, wrapping back to the start (circular behaviour).
//
// LOGIC:
//   1. Start from the current assignPtr position
//   2. Check if the stall is OPEN and has CAPACITY
//   3. If yes → assign order, update capacity, advance pointer
//   4. If no  → skip to next stall, repeat
//   5. If all stalls checked → system overload error
//
// Time: O(n) worst case (all stalls full, must check every one)
// Space: O(1)
bool CircularQueue::assignOrder(Order& order) {
    if (isEmpty()) {
        cout << "  [ERROR] No stalls in queue. Load stalls first." << endl;
        return false;
    }

    int stallsChecked = 0;
    int startPtr = assignPtr;  // Remember start in case of failure

    cout << "\n  [Round-Robin Assignment]" << endl;
    cout << "  Starting from position " << assignPtr << endl;

    while (stallsChecked < count) {
        // Calculate the actual array index using circular wrapping
        int actualIdx = (front + assignPtr) % MAX_STALLS;
        Stall* stall  = queue[actualIdx];

        cout << "  -> Checking " << stall->stallName
             << " (" << stall->stallID << "): ";

        // --- Skip closed stalls ---
        if (!stall->opening) {
            cout << "CLOSED - Skipping" << endl;
            assignPtr = (assignPtr + 1) % count;
            stallsChecked++;
            continue;
        }

        // --- Skip full stalls (capacity boundary check) ---
        if (stall->maxCapacity <= 0) {
            cout << "FULL (0 capacity) - Skipping" << endl;
            assignPtr = (assignPtr + 1) % count;
            stallsChecked++;
            continue;
        }

        // --- Stall is available — assign the order ---
        cout << "AVAILABLE (capacity: " << stall->maxCapacity
             << ") - ASSIGNED!" << endl;

        // Set the order's assigned stall
        strcpy(order.stallID, stall->stallID);

        // Symmetrical synchronization:
        // maxCapacity-- and currentQueueLength++ must happen together
        stall->maxCapacity--;
        stall->currentQueueLength++;

        // Record this assignment in the history log
        if (historyCount < MAX_STALL_HISTORY) {
            strcpy(history[historyCount].orderID, order.orderID);
            strcpy(history[historyCount].stallID, stall->stallID);
            strcpy(history[historyCount].stallName, stall->stallName);
            history[historyCount].timestamp = order.timeStamp;
            historyCount++;
        }

        cout << "\n  Result: Order " << order.orderID
             << " -> " << stall->stallName << endl;
        cout << "  Updated: Capacity=" << stall->maxCapacity
             << " | Queue Length=" << stall->currentQueueLength << endl;

        // Advance pointer for the NEXT call (round-robin rotation)
        assignPtr = (assignPtr + 1) % count;

        return true;
    }

    // All stalls were either closed or at full capacity
    cout << "\n  [WARNING] System overload! All stalls are closed or at"
         << " full capacity." << endl;
    cout << "  Order " << order.orderID << " could not be assigned." << endl;
    assignPtr = startPtr;  // Reset pointer since no assignment was made
    return false;
}

// ============================================================
// STALL MANAGEMENT
// ============================================================

// Find a stall's array index by its ID.
// Returns the actual queue array index, or -1 if not found.
// Time: O(n)
int CircularQueue::findStall(const char* stallID) const {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        if (strcmp(queue[idx]->stallID, stallID) == 0) {
            return idx;
        }
    }
    return -1;
}

// Update a stall's capacity by a delta value.
// delta = -1 when an order is assigned (decrease capacity)
// delta = +1 when an order is cancelled (restore capacity)
// Time: O(n)
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

// Cancel an order's stall assignment — restore one capacity slot.
// This implements the "Cancellation and Recovery Wipes" rule
// from the dataset dictionary.
// Time: O(n)
bool CircularQueue::cancelOrderAssignment(const char* stallID) {
    return updateCapacity(stallID, 1);  // Restore +1 slot
}

// ============================================================
// DISPLAY OPERATIONS
// ============================================================

// Display all stalls currently in the circular queue.
// Shows the internal queue state (front, rear, assignPtr).
// Time: O(n)
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

        // Mark the current assignment pointer position
        if (i == assignPtr) {
            cout << " <-- NEXT";
        }
        cout << endl;
    }

    cout << "  ====================================================" << endl;
    cout << "  Queue Size: " << count
         << " | Front: " << front
         << " | Rear: " << rear
         << " | Assign Ptr: " << assignPtr << endl;
    cout << "  ====================================================" << endl;
}

// Display detailed stall status with visual capacity bars.
// Time: O(n)
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

        // Visual capacity bar [####------] style
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
         << " (" << (totalCapacity > 0 ? (totalUsed * 100 / totalCapacity) : 0)
         << "%)" << endl;
    cout << "  ====================================================" << endl;
}

// Display the assignment history log.
// Shows every order-to-stall assignment made during this session.
// Time: O(n)
void CircularQueue::displayHistory() const {
    if (historyCount == 0) {
        cout << "\n  [INFO] No assignments recorded yet." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "            ASSIGNMENT HISTORY LOG                     " << endl;
    cout << "  ====================================================" << endl;
    cout << "  " << left
         << setw(5)  << "#"
         << setw(15) << "Order ID"
         << setw(12) << "Stall ID"
         << setw(20) << "Stall Name" << endl;
    cout << "  ----------------------------------------------------" << endl;

    for (int i = 0; i < historyCount; i++) {
        cout << "  " << left
             << setw(5)  << (i + 1)
             << setw(15) << history[i].orderID
             << setw(12) << history[i].stallID
             << setw(20) << history[i].stallName << endl;
    }

    cout << "  ====================================================" << endl;
    cout << "  Total Assignments: " << historyCount << endl;
    cout << "  ====================================================" << endl;
}


// ============================================================
// MODULE ENTRY POINT & MENU FUNCTIONS
// ============================================================
CircularQueue stallQueue;
bool dataLoaded = false;
void displayBanner();
void loadAllData();
void displayOrders();
void displayPendingOrders();
void assignNextPending();
void assignAllPending();
void createNewOrder();
void cancelOrder();

// ============================================================
// MAIN FUNCTION
// ============================================================
void runStallModule() {
displayBanner();

    int choice;
    bool running = true;

    while (running) {
        cout << "\n  ============================================" << endl;
        cout << "    STALL ASSIGNMENT MODULE - MAIN MENU       " << endl;
        cout << "  ============================================" << endl;
        cout << "    1. Load All Data from CSV Files" << endl;
        cout << "    2. Display Stalls in Circular Queue" << endl;
        cout << "    3. Display Stall Status & Capacity" << endl;
        cout << "    4. Assign Next Pending Order" << endl;
        cout << "    5. Assign All Pending Orders" << endl;
        cout << "    6. Create New Order" << endl;
        cout << "    7. Cancel Order (Restore Capacity)" << endl;
        cout << "    8. Display All Orders" << endl;
        cout << "    9. Display Assignment History" << endl;
        cout << "    0. Exit" << endl;
        cout << "  ============================================" << endl;
        cout << "    Enter choice: ";

        // Handle invalid input (non-integer)
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\n  [ERROR] Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1: loadAllData();            break;
            case 2: stallQueue.displayAll();  break;
            case 3: stallQueue.displayStatus(); break;
            case 4: assignNextPending();      break;
            case 5: assignAllPending();       break;
            case 6: createNewOrder();         break;
            case 7: cancelOrder();            break;
            case 8: displayOrders();          break;
            case 9: stallQueue.displayHistory(); break;
            case 0:
                cout << "\n  Thank you for using the Campus Food Court"
                     << " Kiosk System!" << endl;
                cout << "  Goodbye!\n" << endl;
                running = false;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice. Please enter 0-9." << endl;
        }
    }

    
}

// ============================================================
// DISPLAY BANNER
// ============================================================
void displayBanner() {
    cout << endl;
    cout << "  ====================================================" << endl;
    cout << "  |                                                  |" << endl;
    cout << "  |   CAMPUS FOOD COURT SELF-ORDER KIOSK SYSTEM      |" << endl;
    cout << "  |   Task 2: Stall Assignment Module                |" << endl;
    cout << "  |   Data Structure: Circular Queue                 |" << endl;
    cout << "  |                                                  |" << endl;
    cout << "  |   By: Sherwin A/L Jesudass (TP075823)            |" << endl;
    cout << "  |   CT077-3-2-DSTR | Lab Work #2                   |" << endl;
    cout << "  |                                                  |" << endl;
    cout << "  ====================================================" << endl;
    cout << endl;
}

// ============================================================
// LOAD ALL DATA FROM CSV FILES
// ============================================================
void loadAllData() {
    // Prevent duplicate loading
    if (dataLoaded) {
        cout << "\n  [INFO] Data already loaded. Reload? (y/n): ";
        char ch;
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "  Reload cancelled." << endl;
            return;
        }
        // Reset the circular queue before reloading
        while (!stallQueue.isEmpty()) {
            stallQueue.dequeue();
        }
        stallCount    = 0;
        orderCount    = 0;
        studentCount  = 0;
        menuItemCount = 0;
    }

    cout << "\n  --- Loading Data from CSV Files ---" << endl;

    // Load stalls
    stallCount = loadStalls("datasets/stalls.csv", stallArray, MAX_STALLS);
    cout << "  Stalls loaded:     " << stallCount << endl;

    // Populate the circular queue with stalls
    for (int i = 0; i < stallCount; i++) {
        stallQueue.enqueue(&stallArray[i]);
    }
    cout << "  Stalls enqueued to Circular Queue." << endl;

    // Load orders
    orderCount = loadOrders("datasets/orders.csv", orders, MAX_ORDERS);
    cout << "  Orders loaded:     " << orderCount << endl;

    // Load students
    studentCount = loadStudents("datasets/students.csv", students, MAX_STUDENTS);
    cout << "  Students loaded:   " << studentCount << endl;

    // Load menu items
    menuItemCount = loadMenuItems("datasets/menu_items.csv", menuItems, MAX_MENU_ITEMS);
    cout << "  Menu items loaded: " << menuItemCount << endl;

    dataLoaded = true;
    cout << "\n  All data loaded successfully!" << endl;
}

// ============================================================
// DISPLAY ALL ORDERS
// ============================================================
void displayOrders() {
    if (orderCount == 0) {
        cout << "\n  [INFO] No orders loaded." << endl;
        return;
    }

    cout << "\n  ====================================================================" << endl;
    cout << "                            ALL ORDERS                                " << endl;
    cout << "  ====================================================================" << endl;
    cout << "  " << left
         << setw(13) << "Order ID"
         << setw(11) << "Student"
         << setw(11) << "Stall"
         << setw(11) << "Item"
         << setw(5)  << "Qty"
         << setw(10) << "Total(RM)"
         << setw(11) << "Payment"
         << setw(12) << "Status" << endl;
    cout << "  --------------------------------------------------------------------" << endl;

    for (int i = 0; i < orderCount; i++) {
        cout << "  " << left
             << setw(13) << orders[i].orderID
             << setw(11) << orders[i].studentID
             << setw(11) << orders[i].stallID
             << setw(11) << orders[i].itemID
             << setw(5)  << orders[i].quantity
             << fixed << setprecision(2) << setw(10) << orders[i].totalPrice
             << setw(11) << orders[i].paymentStatus
             << setw(12) << orders[i].orderStatus << endl;
    }

    cout << "  ====================================================================" << endl;
    cout << "  Total orders: " << orderCount << endl;
}

// ============================================================
// DISPLAY PENDING ORDERS ONLY
// ============================================================
void displayPendingOrders() {
    cout << "\n  --- Pending Orders ---" << endl;
    bool found = false;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "  " << orders[i].orderID
                 << " | Student: " << orders[i].studentID
                 << " | Item: " << orders[i].itemID
                 << " | Qty: " << orders[i].quantity
                 << " | RM" << fixed << setprecision(2) << orders[i].totalPrice
                 << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "  No pending orders." << endl;
    }
}

// ============================================================
// ASSIGN NEXT PENDING ORDER (one at a time)
// ============================================================
void assignNextPending() {
    if (!dataLoaded) {
        cout << "\n  [ERROR] Load data first (Option 1)." << endl;
        return;
    }

    // Find the first PENDING order
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "\n  Found pending order: " << orders[i].orderID
                 << " (Student: " << orders[i].studentID
                 << ", Item: " << orders[i].itemID << ")" << endl;

            if (stallQueue.assignOrder(orders[i])) {
                strcpy(orders[i].orderStatus, "PREPARING");
                cout << "  Order status updated to: PREPARING" << endl;
            }
            return;
        }
    }

    cout << "\n  [INFO] No pending orders found." << endl;
}

// ============================================================
// ASSIGN ALL PENDING ORDERS (batch)
// ============================================================
void assignAllPending() {
    if (!dataLoaded) {
        cout << "\n  [ERROR] Load data first (Option 1)." << endl;
        return;
    }

    int assigned = 0;
    int failed   = 0;

    cout << "\n  --- Batch Assignment of All Pending Orders ---" << endl;

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
    cout << "  Assigned: " << assigned
         << " | Failed: " << failed << endl;
}

// ============================================================
// CREATE A NEW ORDER (for demo purposes)
// ============================================================
void createNewOrder() {
    if (!dataLoaded) {
        cout << "\n  [ERROR] Load data first (Option 1)." << endl;
        return;
    }

    if (orderCount >= MAX_ORDERS) {
        cout << "\n  [ERROR] Maximum number of orders reached." << endl;
        return;
    }

    Order newOrder;
    memset(&newOrder, 0, sizeof(Order));

    // ---- Auto-generate Order ID ----
    // Find the highest existing order number and increment
    int maxOrderNum = 0;
    for (int i = 0; i < orderCount; i++) {
        // Extract the number after "ORD_"
        int num = atoi(orders[i].orderID + 4);
        if (num > maxOrderNum) maxOrderNum = num;
    }
    sprintf(newOrder.orderID, "ORD_%05d", maxOrderNum + 1);

    // ---- Set timestamp ----
    newOrder.timeStamp = (long)time(NULL);

    // ---- Show available students ----
    cout << "\n  --- Active Students ---" << endl;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].status, "Active") == 0) {
            cout << "  " << students[i].studentID
                 << " - " << students[i].fullName
                 << " (Balance: RM" << fixed << setprecision(2)
                 << students[i].accBalance << ")" << endl;
        }
    }

    // ---- Select student ----
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
        cout << "  [ERROR] Student account is "
             << students[studentIdx].status << ". Cannot place order." << endl;
        return;
    }

    // ---- Show available menu items ----
    cout << "\n  --- Available Menu Items ---" << endl;
    cout << "  " << left
         << setw(12) << "Item ID"
         << setw(28) << "Name"
         << setw(12) << "Category"
         << setw(10) << "Price(RM)"
         << setw(10) << "Stall" << endl;
    cout << "  --------------------------------------------------------" << endl;

    for (int i = 0; i < menuItemCount; i++) {
        if (menuItems[i].availability) {
            cout << "  " << left
                 << setw(12) << menuItems[i].itemID
                 << setw(28) << menuItems[i].itemName
                 << setw(12) << menuItems[i].category
                 << fixed << setprecision(2) << setw(10) << menuItems[i].price
                 << setw(10) << menuItems[i].stallID << endl;
        }
    }

    // ---- Select menu item ----
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

    // ---- Enter quantity ----
    cout << "  Enter quantity: ";
    cin >> newOrder.quantity;

    if (newOrder.quantity < 1) {
        cout << "  [ERROR] Quantity must be at least 1." << endl;
        return;
    }

    // ---- Calculate total price ----
    newOrder.totalPrice = newOrder.quantity * menuItems[itemIdx].price;

    // ---- Financial sufficiency check ----
    // (Dataset README: accBalance >= totalPrice before instantiating order)
    if (students[studentIdx].accBalance < newOrder.totalPrice) {
        cout << "  [ERROR] Insufficient balance!" << endl;
        cout << "  Required: RM" << fixed << setprecision(2) << newOrder.totalPrice
             << " | Available: RM" << students[studentIdx].accBalance << endl;

        // Record as failed order
        strcpy(newOrder.paymentStatus, "FAILED");
        strcpy(newOrder.orderStatus, "FAILED");
        strcpy(newOrder.stallID, "");
        orders[orderCount] = newOrder;
        orderCount++;
        return;
    }

    // ---- Deduct balance and create order ----
    students[studentIdx].accBalance -= newOrder.totalPrice;
    strcpy(newOrder.paymentStatus, "PAID");
    newOrder.priorityFlag = 0;
    newOrder.pickupTime   = 0;
    strcpy(newOrder.orderStatus, "PENDING");
    strcpy(newOrder.stallID, "");  // Will be assigned by round-robin

    // Add to the orders array
    orders[orderCount] = newOrder;
    orderCount++;

    cout << "\n  Order " << newOrder.orderID << " created successfully!" << endl;
    cout << "  Item: " << menuItems[itemIdx].itemName
         << " x" << newOrder.quantity << endl;
    cout << "  Total: RM" << fixed << setprecision(2) << newOrder.totalPrice << endl;
    cout << "  Remaining balance: RM" << students[studentIdx].accBalance << endl;

    // ---- Auto-assign to stall via Circular Queue ----
    cout << "\n  Assigning to stall via Circular Queue round-robin..." << endl;
    if (stallQueue.assignOrder(orders[orderCount - 1])) {
        strcpy(orders[orderCount - 1].orderStatus, "PREPARING");
        cout << "  Order status: PREPARING" << endl;
    } else {
        cout << "  Order remains PENDING — no stall available." << endl;
    }
}

// ============================================================
// CANCEL ORDER (with refund and capacity restoration)
// ============================================================
void cancelOrder() {
    if (!dataLoaded) {
        cout << "\n  [ERROR] Load data first (Option 1)." << endl;
        return;
    }

    // Show cancellable orders (PREPARING or PENDING)
    cout << "\n  --- Active Orders (Cancellable) ---" << endl;
    bool hasActive = false;

    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderStatus, "PREPARING") == 0 ||
            strcmp(orders[i].orderStatus, "PENDING") == 0) {
            cout << "  " << orders[i].orderID
                 << " | Student: " << orders[i].studentID
                 << " | Stall: " << orders[i].stallID
                 << " | RM" << fixed << setprecision(2) << orders[i].totalPrice
                 << " | Status: " << orders[i].orderStatus << endl;
            hasActive = true;
        }
    }

    if (!hasActive) {
        cout << "  No active orders to cancel." << endl;
        return;
    }

    // ---- Select order to cancel ----
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

    // ---- Cancel the order ----
    cout << "\n  Cancelling order " << cancelID << "..." << endl;
    strcpy(orders[orderIdx].orderStatus, "CANCELLED");
    strcpy(orders[orderIdx].paymentStatus, "REFUNDED");

    // ---- Restore stall capacity ----
    // (Dataset README: maxCapacity++ and currentQueueLength--)
    if (strlen(orders[orderIdx].stallID) > 0) {
        cout << "  Restoring capacity for stall "
             << orders[orderIdx].stallID << "..." << endl;
        stallQueue.cancelOrderAssignment(orders[orderIdx].stallID);
        cout << "  Stall capacity restored (+1 slot)." << endl;
    }

    // ---- Refund student ----
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].studentID, orders[orderIdx].studentID) == 0) {
            students[i].accBalance += orders[orderIdx].totalPrice;
            cout << "  Refunded RM" << fixed << setprecision(2)
                 << orders[orderIdx].totalPrice
                 << " to " << students[i].fullName << endl;
            cout << "  New balance: RM" << students[i].accBalance << endl;
            break;
        }
    }

    cout << "\n  Order " << cancelID << " cancelled successfully!" << endl;
}
