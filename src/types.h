#ifndef TYPES_H
#define TYPES_H

typedef struct{
  char* name;
  char* exec;
  char* icon;
}AppEntry;

typedef struct{
  AppEntry* apps;
  int count;
  int capacity;
}AppList;

#endif
