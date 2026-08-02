#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstring>

const int MAX_STUDENTS    = 50;
const int MAX_STALLS      = 50;
const int MAX_MENU_ITEMS  = 100;
const int MAX_ORDERS      = 200;
const int MAX_STALL_HISTORY = 500;

struct Student {
    char studentID[20];
    char fullName[100];
    char contactNumber[20];
    double accBalance;
    char status[20];
};

struct Stall {
    char stallID[20];
    char stallName[50];
    char cuisineType[20];
    bool opening;
    int  maxCapacity;
    int  currentQueueLength;
};

struct MenuItem {
    char   itemID[20];
    char   itemName[100];
    char   category[20];
    bool   availability;
    int    prepTime;
    char   stallID[20];
    double price;
};

struct Order {
    char   orderID[20];
    long   timeStamp;
    char   studentID[20];
    char   stallID[20];
    char   itemID[20];
    char   itemName[100];
    int    quantity;
    double totalPrice;
    char   paymentStatus[20];
    int    priorityFlag;
    long   pickupTime;
    char   orderStatus[20];
};

struct AssignmentRecord {
    char orderID[20];
    char stallID[20];
    char stallName[50];
    long timestamp;
};

// Session navigation step (Task 3 stack history)
struct SessionStep {
    char stepType[20];        // one of the STEP_* labels in session_history.h
    char studentID[20];       // who performed the step
    char itemID[20];          // item involved, "" if none
    int  quantity;            // qty at this step, 0 if n/a
    long timeStamp;           // Unix epoch when step occurred
    char stateSnapshot[100];  // short human-readable state description
};

extern Student students[MAX_STUDENTS];
extern Stall stallArray[MAX_STALLS];
extern MenuItem menuItems[MAX_MENU_ITEMS];
extern Order orders[MAX_ORDERS];

extern int studentCount;
extern int stallCount;
extern int menuItemCount;
extern int orderCount;

#endif // STRUCTURES_H
