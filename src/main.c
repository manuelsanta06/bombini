#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "types.h"
#include "indexer.h"
#include "daemon.h"
#include "search.h"

void printHelp(char* name){
  printf("Usage: %s [OPTION]... [QUERY]\n",name);
  printf("Options:\n");
  printf("  -h, --help          shows this help message\n");
  printf("  -d, --daemon        work as a daemon for following ejecutions\n");
  printf("  -S, --standAlone    searchs the given query without trying to connect with a daemon\n");
  printf("  -R, --reload        tells any running daemon to reload its configuration file and apps list\n");
  printf("  -P, --plain         plain text output\n");
  printf("  -c, --setPath PATH  override .desktop files path. use : between paths for more than one\n");
  printf("  -c, --addPath PATH  concatenate a path for .desktop's. use : between paths for more than one\n");
}

int main(int argc, char** argv){
  Config conf={0};
  conf.format=FORMAT_JSON;
  char *home=getenv("HOME");
  if(home){
    static char defaultPath[PATH_MAX]; 
    int len=snprintf(defaultPath,sizeof(defaultPath),"%s/.local/share/applications:/usr/share/applications/",home);
    if(len<0){
      fprintf(stderr,"Error parsing default .desktop's path\n");
      abort();
    }else if((size_t)len>=sizeof(defaultPath)){
      fprintf(stderr,"HOME path too long.\n");
      exit(1);
    }
    conf.desktopDirs=strdup(defaultPath);
  }else{
    fprintf(stderr,"Error getting HOME directory for default .desktop's path\n");
    exit(1);
  }

  int opt;

  static struct option long_options[]={
    {"daemon",      0,0,'d'},
    {"help",        0,0,'h'},
    {"reload",      0,0,'R'},
    {"standAlone",  0,0,'S'},
    {"plain",       0,0,'P'},
    {"setPath",     1,0,'p'},
    {"addPath",     1,0,'a'},
    {0,0,0,0}
  };
  int option_index=0;
  while((opt=getopt_long(argc,argv,"dhp:a:RSP",long_options,&option_index))!=-1){
    switch(opt){
      case 'd':
        if(conf.standAlone)break;
        conf.daemondMode=true;
        break;
      case 'p':
        if(conf.standAlone)break;
        if(conf.desktopDirs)free(conf.desktopDirs);
        conf.desktopDirs=strdup(optarg);
        printf("%s\n\n",conf.desktopDirs);
        break;
      case 'a':
        if(conf.standAlone)break;
        size_t oldLen=strlen(conf.desktopDirs);
        size_t appendLen=strlen(optarg);
        conf.desktopDirs=realloc(conf.desktopDirs,oldLen+appendLen+2);
        if(!conf.desktopDirs)exit(1);
        conf.desktopDirs[oldLen]=':';
        memcpy(conf.desktopDirs+oldLen+1,optarg,appendLen+1);
        break;
      case 'S':
        if(!conf.daemondMode)conf.standAlone=true;
        break;
      case 'P':
        conf.format=FORMAT_PLAIN;
        break;
      case 'R':
        reloadDaemon();
        exit(0);
        break;
      case '?':
      case 'h':
        printHelp(argv[0]);
        exit(0);
      default:
        exit(1);
    }
  }

  if(conf.daemondMode){
    startDaemon(&conf);
    exit(1);
  }else{
    char* query=optind>=argc?"":argv[optind];
    char* results=NULL;
    if(!conf.standAlone)results=askDaemon(query,conf.format);
    if(!results){
      AppList* list=buildAppList(conf.desktopDirs);
      results=executeSearch(query,list,conf.format);
      freeAppList(list);
    }

    if(results){
      printf("%s\n",results);
      free(results);
    }
  }
  if(conf.desktopDirs)free(conf.desktopDirs);
  return 0;
}
