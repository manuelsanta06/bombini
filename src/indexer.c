#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "indexer.h"
#include "types.h"


static void growAppList(AppList *list){
  if(!list)abort();
  if(!list->capacity){
    list->apps=malloc(sizeof(AppEntry)*64);
    if(!list->apps){
      fprintf(stderr,"Error: Failed to allocate memory for AppList.\n");
      abort();
    }
    list->capacity=64;
    list->count=0;
    return;
  }
  list->capacity+=list->capacity/2;
  list->apps=realloc(list->apps,sizeof(AppEntry)*list->capacity);
  if(!list->apps){
    fprintf(stderr,"Error reallocating AppList\n");
    abort();
  }
}

AppList* buildAppList(const char* dirs){
  AppList* list=malloc(sizeof(AppList));
  if(!list){
    fprintf(stderr,"Error allocating AppList\n");
    abort();
  }
  list->capacity=0;
  growAppList(list);
  struct dirent *entry;
  DIR* dp=opendir(dirs);
  if(!dp)return list;
  while((entry=readdir(dp))!=NULL){
    size_t len=strlen(entry->d_name);
    if(entry->d_name[0]=='.'||
      len<8||
      strcmp(entry->d_name+(len-8),".desktop")!=0)
        continue;
    char fullPath[PATH_MAX];
    snprintf(fullPath,sizeof(fullPath),"%s/%s",dirs,entry->d_name);
    FILE* deskFile=fopen(fullPath,"r");
    if(!deskFile){
      fprintf(stderr,"Error openning file %s.\n",fullPath);
      continue;
    }
    list->count++;
    if(list->count>=list->capacity)growAppList(list);
    char buffer[512];
    while(fgets(buffer,sizeof(buffer),deskFile)){
      //TODO: parsing
    }
    fclose(deskFile);
  }
  closedir(dp);
  return list;
}
