// ============================================================
// main.cpp — Driver Program
// Campus Food Court Self-Order Kiosk System (Part 2)
// Task 2: Stall Assignment Module (Circular Queue)
// By: Sherwin A/L Jesudass (TP075823)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================
// This driver program demonstrates the Circular Queue-based
// Stall Assignment Module. It loads data from CSV files,
// lets the user assign orders to stalls via round-robin,
// and handles cancellations with capacity recovery.
// ============================================================

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "structures.h"
#include "stall_assignment.h"
#include "file_handler.h"

using namespace std;

// ============================================================
// GLOBAL DATA ARRAYS (no STL containers)
// ============================================================
Student  students[MAX_STUDENTS];
Stall    stallArray[MAX_STALLS];
MenuItem menuItems[MAX_MENU_ITEMS];
Order    orders[MAX_ORDERS];

int studentCount  = 0;
int stallCount    = 0;
int menuItemCount = 0;
int orderCount    = 0;

// The circular queue for stall assignment
CircularQueue stallQueue;

// Flag to track whether data has been loaded
bool dataLoaded = false;

// ============================================================
// FUNCTION DECLARATIONS
// ============================================================
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
int main() {
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

    return 0;
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
    stallCount = loadStalls("../datasets/stalls.csv", stallArray, MAX_STALLS);
    cout << "  Stalls loaded:     " << stallCount << endl;

    // Populate the circular queue with stalls
    for (int i = 0; i < stallCount; i++) {
        stallQueue.enqueue(stallArray[i]);
    }
    cout << "  Stalls enqueued to Circular Queue." << endl;

    // Load orders
    orderCount = loadOrders("../datasets/orders.csv", orders, MAX_ORDERS);
    cout << "  Orders loaded:     " << orderCount << endl;

    // Load students
    studentCount = loadStudents("../datasets/students.csv", students, MAX_STUDENTS);
    cout << "  Students loaded:   " << studentCount << endl;

    // Load menu items
    menuItemCount = loadMenuItems("../datasets/menu_items.csv", menuItems, MAX_MENU_ITEMS);
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
    cin >> newOrder.studentID;

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
    cin >> newOrder.itemID;

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
    cin >> cancelID;

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
