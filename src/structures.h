// ============================================================
// structures.h — Shared Data Structures
// Campus Food Court Self-Order Kiosk System (Part 2)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================
// This header defines all the structs used across the system,
// matching the CSV schemas from the dataset dictionary.
// ============================================================

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstring>

// ---- Array size limits (no STL containers allowed) ----
const int MAX_STUDENTS    = 50;
const int MAX_STALLS      = 50;
const int MAX_MENU_ITEMS  = 100;
const int MAX_ORDERS      = 200;
const int MAX_STALL_HISTORY = 500;

// ---- Student record (students.csv) ----
struct Student {
    char studentID[20];       // Primary Key, e.g. "TP076118"
    char fullName[100];
    char contactNumber[20];   // e.g. "012-3456789"
    double accBalance;        // Digital wallet balance (>= 0.00)
    char status[20];          // Active, Suspended, Graduated
};

// ---- Stall record (stalls.csv) ----
struct Stall {
    char stallID[20];         // Primary Key, e.g. "STALL_01"
    char stallName[50];
    char cuisineType[20];     // Malay, Chinese, Western, Indian
    bool opening;             // true = Open, false = Closed
    int  maxCapacity;         // Slots remaining (starts at 15)
    int  currentQueueLength;  // Active items being processed
};

// ---- Menu item record (menu_items.csv) ----
struct MenuItem {
    char   itemID[20];        // Primary Key, e.g. "ITEM_M01"
    char   itemName[100];
    char   category[20];      // Food, Beverage, Dessert
    bool   availability;      // true = In Stock
    int    prepTime;          // Minutes
    char   stallID[20];       // Foreign Key -> stalls.csv
    double price;             // Must be > 0.00
};

// ---- Order record (orders.csv) ----
struct Order {
    char   orderID[20];       // Primary Key, e.g. "ORD_00001"
    long   timeStamp;         // Unix epoch timestamp
    char   studentID[20];     // Foreign Key -> students.csv
    char   stallID[20];       // Foreign Key -> stalls.csv (assigned by module)
    char   itemID[20];        // Foreign Key -> menu_items.csv
    int    quantity;          // Must be >= 1
    double totalPrice;        // quantity * item price
    char   paymentStatus[20]; // PAID, REFUNDED, FAILED
    int    priorityFlag;      // 1 = Priority/Express, 0 = Normal
    long   pickupTime;        // Unix epoch for food collection
    char   orderStatus[20];   // PENDING, PREPARING, READY, FULFILLED, CANCELLED
};

// ---- Assignment history record (for Task 2 tracking) ----
struct AssignmentRecord {
    char orderID[20];
    char stallID[20];
    char stallName[50];
    long timestamp;
};

// ---- Session navigation step (for Task 3 stack history) ----
struct SessionStep {
    char stepType[20];        // one of the STEP_* labels in session_history.h
    char studentID[20];       // who performed the step
    char itemID[20];          // item involved, "" if none
    int  quantity;            // qty at this step, 0 if n/a
    long timeStamp;           // Unix epoch when step occurred
    char stateSnapshot[100];  // short human-readable state description
};

#endif // STRUCTURES_H

// ---- Shared Global Arrays (Appended for cross-module integration) ----
extern Student students[MAX_STUDENTS];
extern Stall stallArray[MAX_STALLS];
extern MenuItem menuItems[MAX_MENU_ITEMS];
extern Order orders[MAX_ORDERS];

extern int studentCount;
extern int stallCount;
extern int menuItemCount;
extern int orderCount;
