#include <stdlib.h>
#include <string.h>
#include "search.h"
#include "fuzzy_match.h" 

#include <stdio.h>

typedef struct{
  AppEntry* app;
  int32_t score;
}SearchResult;

static int compareResults(const void* a,const void* b){
  return ((SearchResult*)b)->score-((SearchResult*)a)->score; 
}

static char* escapeJsonStr(const char* str){
  if(!str)return strdup("");
  char* esc=malloc(strlen(str)*2+1);
  if(!esc)abort();
  int j=0;
  for(int i=0;str[i]!='\0';i++){
    if(str[i]=='"'||str[i]=='\\')esc[j++]='\\';
    esc[j++]=str[i];
  }
  esc[j]='\0';
  return esc;
}

char* executeSearch(const char* query,AppList* list){
  if(!list||!query)return NULL;

  SearchResult* results=malloc(sizeof(SearchResult)*list->count);
  if(!results)abort();
  int matchCount=0;
  for(int i=0;i<list->count;i++){
    int32_t score=fuzzy_match(query,list->apps[i].name);
    if(score!=INT32_MIN){ 
      results[matchCount].app=&list->apps[i];
      results[matchCount].score=score;
      matchCount++;
    }
  }
  qsort(results,matchCount,sizeof(SearchResult),compareResults);
  int size=4096,len=1;
  char* json=malloc(size);
  if(!json)abort();
  json[0]='[';
  for(int i=0;i<matchCount;i++){
    char item[1024];
    char* safeName=escapeJsonStr(results[i].app->name);
    char* safeExec=escapeJsonStr(results[i].app->exec);
    char* safeIcon=escapeJsonStr(results[i].app->icon);
    int itemLen=snprintf(item,sizeof(item),"{\"name\":\"%s\",\"exec\":\"%s\",\"icon\":\"%s\"},",
      safeName,safeExec,safeIcon);
    free(safeName);
    free(safeExec);
    free(safeIcon);
    if(itemLen>=sizeof(item)){
      fprintf(stderr,"app %s too long, skipping it.\n",results[i].app->name);
      continue;
    }
    if(len+itemLen>=size){
      size=size+size/2;
      json=realloc(json,size);
      if(!json)abort();
    }
    memcpy(json+len,item,itemLen);
    len+=itemLen;
  }
  if(len>1)json[len-1]=']';
  else json[len++]=']';
  json[len]='\0';
  free(results);
  return json;
}
