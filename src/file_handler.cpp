#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

// CSV PARSING UTILITIES
// Trim leading/trailing whitespace, tabs, newlines, carriage returns.
void trimWhitespace(char* str) {
    // Trim leading whitespace
    char* start = str;
    while (*start == ' ' || *start == '\t') {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    // Trim trailing whitespace and control characters
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' '  || str[len - 1] == '\t' ||
                        str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

// Split a CSV line by commas into an array of field strings.
// Each field is trimmed of whitespace.
// Returns the number of fields found.
int parseLine(const char* line, char fields[][256], int maxFields) {
    int fieldCount = 0;
    const char* pos = line;

    while (*pos && fieldCount < maxFields) {
        // Find the next comma or end of string, respecting double quotes
        const char* comma = pos;
        bool inQuotes = false;
        while (*comma && *comma != '\n' && *comma != '\r') {
            if (*comma == '"') {
                inQuotes = !inQuotes;
            } else if (*comma == ',' && !inQuotes) {
                break; // Found an unquoted comma
            }
            comma++;
        }

        // Copy this field
        int len = comma - pos;
        if (len > 255) len = 255;
        strncpy(fields[fieldCount], pos, len);
        fields[fieldCount][len] = '\0';
        trimWhitespace(fields[fieldCount]);
        fieldCount++;

        // Advance past the comma (or stop if end of string)
        if (*comma == ',') {
            pos = comma + 1;
        } else {
            break;
        }
    }

    return fieldCount;
}

// LOAD STALLS (stalls.csv)
int loadStalls(const char* filename, Stall stalls[], int maxSize) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open file: " << filename << endl;
        return 0;
    }

    char line[1024];
    int count   = 0;
    int lineNum = 0;

    // Skip the header row
    file.getline(line, 1024);
    lineNum++;

    while (file.getline(line, 1024) && count < maxSize) {
        lineNum++;
        char fields[10][256];
        int fieldCount = parseLine(line, fields, 10);

        // Validate: stalls.csv must have exactly 6 fields
        if (fieldCount != 6) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Malformed (expected 6 fields, got "
                 << fieldCount << "). Skipping." << endl;
            continue;
        }

        // Parse fields into the Stall struct
        strcpy(stalls[count].stallID, fields[0]);
        strcpy(stalls[count].stallName, fields[1]);
        strcpy(stalls[count].cuisineType, fields[2]);
        stalls[count].opening           = (atoi(fields[3]) == 1);
        stalls[count].maxCapacity       = atoi(fields[4]);
        stalls[count].currentQueueLength = atoi(fields[5]);

        // Validate: capacity must not be negative
        if (stalls[count].maxCapacity < 0) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Negative capacity. Setting to 0." << endl;
            stalls[count].maxCapacity = 0;
        }

        count++;
    }

    file.close();
    return count;
}

// LOAD ORDERS (orders.csv)
int loadOrders(const char* filename, Order orders[], int maxSize) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open file: " << filename << endl;
        return 0;
    }

    char line[1024];
    int count   = 0;
    int lineNum = 0;

    file.getline(line, 1024);
    lineNum++;

    while (file.getline(line, 1024) && count < maxSize) {
        lineNum++;
        char fields[15][256];
        int fieldCount = parseLine(line, fields, 15);

        // Validate: orders.csv must have exactly 11 fields
        if (fieldCount != 11) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Malformed order (expected 11 fields, got "
                 << fieldCount << "). Skipping." << endl;
            continue;
        }

        strcpy(orders[count].orderID, fields[0]);
        orders[count].timeStamp = atol(fields[1]);
        strcpy(orders[count].studentID, fields[2]);
        strcpy(orders[count].stallID, fields[3]);
        strcpy(orders[count].itemID, fields[4]);
        orders[count].quantity    = atoi(fields[5]);
        orders[count].totalPrice  = atof(fields[6]);
        strcpy(orders[count].paymentStatus, fields[7]);
        orders[count].priorityFlag = atoi(fields[8]);
        orders[count].pickupTime   = atol(fields[9]);
        strcpy(orders[count].orderStatus, fields[10]);

        // Validate: quantity must be >= 1
        if (orders[count].quantity < 1) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Invalid quantity (" << orders[count].quantity
                 << "). Skipping." << endl;
            continue;
        }

        // Validate: price must not be negative
        if (orders[count].totalPrice < 0) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Negative price. Skipping." << endl;
            continue;
        }

        count++;
    }

    file.close();
    return count;
}

// LOAD STUDENTS (students.csv)
int loadStudents(const char* filename, Student students[], int maxSize) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open file: " << filename << endl;
        return 0;
    }

    char line[1024];
    int count   = 0;
    int lineNum = 0;

    file.getline(line, 1024);
    lineNum++;

    while (file.getline(line, 1024) && count < maxSize) {
        lineNum++;
        char fields[10][256];
        int fieldCount = parseLine(line, fields, 10);

        // Validate: students.csv must have exactly 5 fields
        if (fieldCount != 5) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Malformed student record. Skipping." << endl;
            continue;
        }

        strcpy(students[count].studentID, fields[0]);
        strcpy(students[count].fullName, fields[1]);
        strcpy(students[count].contactNumber, fields[2]);
        students[count].accBalance = atof(fields[3]);
        strcpy(students[count].status, fields[4]);

        // Validate: balance must not be negative
        if (students[count].accBalance < 0) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Negative balance. Setting to 0." << endl;
            students[count].accBalance = 0.0;
        }

        count++;
    }

    file.close();
    return count;
}

// LOAD MENU ITEMS (menu_items.csv)
int loadMenuItems(const char* filename, MenuItem items[], int maxSize) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open file: " << filename << endl;
        return 0;
    }

    char line[1024];
    int count   = 0;
    int lineNum = 0;

    file.getline(line, 1024);
    lineNum++;

    while (file.getline(line, 1024) && count < maxSize) {
        lineNum++;
        char fields[10][256];
        int fieldCount = parseLine(line, fields, 10);

        // Validate: menu_items.csv must have exactly 7 fields
        if (fieldCount != 7) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Malformed menu item. Skipping." << endl;
            continue;
        }

        strcpy(items[count].itemID, fields[0]);
        strcpy(items[count].itemName, fields[1]);
        strcpy(items[count].category, fields[2]);
        items[count].availability = (atoi(fields[3]) == 1);
        items[count].prepTime     = atoi(fields[4]);
        strcpy(items[count].stallID, fields[5]);
        items[count].price = atof(fields[6]);

        // Validate: price must be positive
        if (items[count].price <= 0) {
            cout << "  [WARNING] Line " << lineNum
                 << ": Invalid price (" << items[count].price
                 << "). Skipping." << endl;
            continue;
        }

        count++;
    }

    file.close();
    return count;
}