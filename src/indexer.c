#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "indexer.h"
#include "types.h"


void cleanAppEntry(AppEntry* entry){
  if(!entry)return;
  if(entry->filename)free(entry->filename);
  if(entry->name)free(entry->name);
  if(entry->exec)free(entry->exec);
  if(entry->icon)free(entry->icon);
  entry->filename=NULL;
  entry->name=NULL;
  entry->exec=NULL;
  entry->icon=NULL;
}

void freeAppList(AppList* list){
  if(!list)return;
  for(int i=0;i< list->count;i++)cleanAppEntry(&list->apps[i]);
  if(list->apps)free(list->apps);
  list->apps=NULL;
  free(list);
}

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

static char* trimString(char* str){
  size_t len=strlen(str);
  while(len>0&&isspace((unsigned char)str[len-1])){
    len--;
    str[len]='\0';
  }
  while(*str&&isspace((unsigned char)*str))str++;
  return str;
}

static void cleanExecString(char* exec){
  if(!exec)return;
  for(int i=0;exec[i]!='\0';i++){
    if(exec[i]=='%'){
      bool spaceBefore=(i>0&&isspace((unsigned char)exec[i-1]));
      bool letterAfter=isalpha((unsigned char)exec[i+1]);
      bool spaceOrNullAfter=false;
      if(letterAfter){
        char after=exec[i+2];
        spaceOrNullAfter=(after=='\0'||isspace((unsigned char)after));
      }
      if(spaceBefore&&letterAfter&&spaceOrNullAfter){
        exec[i]=' ';
        exec[i+1]=' ';
      }
    }
  }
  size_t len=strlen(exec);
  while(len>0&&isspace((unsigned char)exec[len-1])){
    len--;
    exec[len]='\0';
  }
}

AppList* buildAppList(const char* colonSeparatedDirs){
  AppList* list=calloc(1,sizeof(AppList));
  if(!list)abort();
  growAppList(list);
  char* dirsCopy=strdup(colonSeparatedDirs);
  if(!dirsCopy)abort();

  char* saveptr;
  char* token=strtok_r(dirsCopy,":",&saveptr);
  while(token!=NULL){
    indexDirectory(list,token);
    token=strtok_r(NULL,":",&saveptr); 
  }
  free(dirsCopy);
  return list;
}

//TODO: hashes
void indexDirectory(AppList* list,const char* dir){
  if(!list)abort();
  struct dirent *entry;
  DIR* dp=opendir(dir);
  if(!dp)return;
  while((entry=readdir(dp))!=NULL){
    size_t len=strlen(entry->d_name);
    if(entry->d_name[0]=='.'||
      len<9||
      strcmp(entry->d_name+(len-8),".desktop")!=0)
        continue;
    bool isDuplicate=false;
    for(int i=0;i<list->count;i++){
      if(strcmp(list->apps[i].filename,entry->d_name)==0){
        isDuplicate=true;
        break;
      }
    }if(isDuplicate)continue;
    char fullPath[PATH_MAX];
    snprintf(fullPath,sizeof(fullPath),"%s/%s",dir,entry->d_name);
    FILE* deskFile=fopen(fullPath,"r");
    if(!deskFile){
      fprintf(stderr,"Error openning file %s.\n",fullPath);
      continue;
    }
    AppEntry tmp={0};
    tmp.filename=strdup(entry->d_name);
    bool inSection=false,skip=true;
    char buffer[512];
    while(fgets(buffer,sizeof(buffer),deskFile)){
      char* line=trimString(buffer);
      if(line[0]=='\0'||line[0]=='#'||line[0]==';')continue;
      len=strlen(line);
      if(strncmp(line,"[Desktop Entry]",15)==0)inSection=true;
      else if(line[0]=='[')inSection=false;
      if(inSection){
        if(strncmp(line,"NoDisplay=true",14)==0){
          skip=true;
          break;
        }else if(!tmp.name&&strncmp(line,"Name=",5)==0){
          tmp.name=strdup(line+5);
        }else if(!tmp.exec&&strncmp(line,"Exec=",5)==0){
          tmp.exec=strdup(line+5);
          cleanExecString(tmp.exec);
        }else if(!tmp.icon&&strncmp(line,"Icon=",5)==0){
          tmp.icon=strdup(line+5);
        }else if(strncmp(line,"Type=Application",16)==0){
          skip=false;
        }
      }
    }
    if(skip||!tmp.name||!tmp.exec){
      cleanAppEntry(&tmp);
    }else{
      list->apps[list->count]=tmp;
      list->count++;
      if(list->count>=list->capacity)growAppList(list);
    }
    fclose(deskFile);
  }
  closedir(dp);
  return;
}
