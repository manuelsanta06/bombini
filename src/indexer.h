#ifndef INDEXER_H
#define INDEXER_H

#include "types.h"

void cleanAppEntry(AppEntry* entry);
void freeAppList(AppList* list);
AppList* buildAppList(const char* dirs);

#endif
