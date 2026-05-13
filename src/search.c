#include <stdlib.h>
#include <string.h>
#include "search.h"
#include "fuzzy_match.h" 

typedef struct{
  AppEntry* app;
  int32_t score;
}SearchResult;

static int compareResults(const void* a,const void* b){
  return ((SearchResult*)b)->score-((SearchResult*)a)->score; 
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
  //TODO:format data
  free(results);
  return "";
}
