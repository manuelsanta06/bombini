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

char* executeSearch(const char* query,AppList* list,const Config* conf){
  if(!list||!query||!conf)return NULL;

  SearchResult* results=malloc(sizeof(SearchResult)*list->count);
  if(!results)abort();
  int matchCount=0;
  for(size_t i=0;i<list->count;i++){
    int32_t score=fuzzy_match(query,list->apps[i].name);
    if(score!=INT32_MIN){ 
      results[matchCount].app=&list->apps[i];
      results[matchCount].score=score;
      matchCount++;
    }
  }
  qsort(results,matchCount,sizeof(SearchResult),compareResults);
  int size=4096,len=0;
  char* output=malloc(size);
  if(!output)abort();
  if(conf->format==FORMAT_JSON){
    output[0]='[';
    len=1;
  }
  for(int i=0;i<matchCount;i++){
    char finalExec[1024];
    if(results[i].app->terminal&&conf->terminalWrapper){
      snprintf(finalExec,sizeof(finalExec),"%s %s",conf->terminalWrapper,results[i].app->exec);
    }else{
      snprintf(finalExec,sizeof(finalExec),"%s",results[i].app->exec);
    }

    char item[1024];
    int itemLen=0;
    if(conf->format==FORMAT_JSON){
      char* safeName=escapeJsonStr(results[i].app->name);
      char* safeExec=escapeJsonStr(finalExec);
      char* safeIcon=escapeJsonStr(results[i].app->icon);
      itemLen=snprintf(item,sizeof(item),"{\"name\":\"%s\",\"exec\":\"%s\",\"icon\":\"%s\",\"terminal\":%s},",
        safeName,safeExec,safeIcon,results[i].app->terminal?"true":"false");
      free(safeName);
      free(safeExec);
      free(safeIcon);
    }else{
      itemLen=snprintf(item,sizeof(item),"%s\t%s\t%s\t%s\n",
        results[i].app->name,finalExec,results[i].app->icon?results[i].app->icon:"",results[i].app->terminal?"true":"false");
    }
    if(itemLen<0||(size_t)itemLen>=sizeof(item)){
      fprintf(stderr,"app %s too long, skipping it.\n",results[i].app->name);
      continue;
    }
    if(len+itemLen>=size){
      size=size+size/2;
      output=realloc(output,size);
      if(!output)abort();
    }
    memcpy(output+len,item,itemLen);
    len+=itemLen;
  }
  if(conf->format==FORMAT_JSON){
    if(len>1) output[len-1]=']';
    else output[len++]=']';
  }
  output[len]='\0';
  free(results);
  return output;
}
