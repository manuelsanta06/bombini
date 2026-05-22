#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

typedef struct{
  char* filename;
  char* name;
  char* exec;
  char* icon;
}AppEntry;

typedef struct{
  AppEntry* apps;
  int count;
  int capacity;
}AppList;

typedef struct{
  char* configFilePath;
  char* desktopDirs;
  bool standAlone;
  bool daemondMode;
}Config;

#endif
