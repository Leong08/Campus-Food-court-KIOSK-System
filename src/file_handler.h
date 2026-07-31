#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "structures.h"

// Splits a CSV line by commas into an array of field strings.
// Returns the number of fields found.
int  parseLine(const char* line, char fields[][256], int maxFields);

// Trims leading/trailing whitespace and newline characters.
void trimWhitespace(char* str);

// Data loading functions 
int loadStalls(const char* filename, Stall stalls[], int maxSize);
int loadOrders(const char* filename, Order orders[], int maxSize);
int loadStudents(const char* filename, Student students[], int maxSize);
int loadMenuItems(const char* filename, MenuItem items[], int maxSize);

#endif