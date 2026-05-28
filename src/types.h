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

typedef enum{
  FORMAT_SYS,
  FORMAT_JSON,
  FORMAT_PLAIN
} OutputFormat;

typedef struct{
  char* desktopDirs;
  OutputFormat format;
  bool standAlone;
  bool daemondMode;
}Config;

#endif
