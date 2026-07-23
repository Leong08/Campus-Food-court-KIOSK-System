// ============================================================
// file_handler.h — CSV File Loading Declarations
// Campus Food Court Self-Order Kiosk System (Part 2)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "structures.h"

// ---- CSV parsing utilities ----
// Splits a CSV line by commas into an array of field strings.
// Returns the number of fields found.
int  parseLine(const char* line, char fields[][256], int maxFields);

// Trims leading/trailing whitespace and newline characters.
void trimWhitespace(char* str);

// ---- Data loading functions ----
// Each function opens a CSV file, skips the header row, parses
// each data row into the appropriate struct, validates fields,
// and skips malformed lines with a warning log.
// Returns: the number of records successfully loaded.

int loadStalls(const char* filename, Stall stalls[], int maxSize);
int loadOrders(const char* filename, Order orders[], int maxSize);
int loadStudents(const char* filename, Student students[], int maxSize);
int loadMenuItems(const char* filename, MenuItem items[], int maxSize);

#endif // FILE_HANDLER_H
