#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "types.h"
#include "loader.h"
#include "indexer.h"
#include "daemon.h"
#include "search.h"

void printHelp(char* name){
  printf("Usage: %s [OPTION]... [QUERY]\n",name);
  printf("Options:\n");
  printf("  -h, --help           shows this help message\n");
  printf("  -d, --daemon         work as a daemon for following ejecutions\n");
  printf("  -d, --reload         tells any running daemon to reload its configuration file and apps list\n");
  printf("  -d, --standAlone     searchs the given query without trying to connect with a daemon\n");
  printf("  -c, --config FILE    Config file path\n");
}

int main(int argc, char** argv){
  Config conf={0};
  conf.format=FORMAT_JSON;
  conf.desktopDirs="/home/santa/.local/share/applications:/usr/share/applications/";
  int opt;

  static struct option long_options[]={
    {"daemon",      0,0,'d'},
    {"help",        0,0,'h'},
    {"reload",      0,0,'R'},
    {"standAlone",  0,0,'S'},
    {"plain",       0,0,'P'},
    {"config",      1,0,'c'},
    {0,0,0,0}
  };
  int option_index=0;
  while((opt=getopt_long(argc,argv,"dhc:RSP",long_options,&option_index))!=-1){
    switch(opt){
      case 'd':
        if(!conf.standAlone)conf.daemondMode=true;
        break;
      case 'c':
        conf.configFilePath=optarg;
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
        abort();
    }
  }

  if(!conf.configFilePath){
    char *home=getenv("HOME");
    if(home){
      static char defaultPath[PATH_MAX]; 
      int len=snprintf(defaultPath,sizeof(defaultPath),"%s/.config/bombini/conf.ini",home);
      if(len<0){
        fprintf(stderr,"Error parsing default config path.\n");
        abort();
      }else if((size_t)len>=sizeof(defaultPath)){
        fprintf(stderr,"HOME path too long.\n");
        abort();
      }
      conf.configFilePath=defaultPath;
    }else{
      fprintf(stderr,"Error getting HOME directory for default config path\n");
      abort();
    }
  }
  if(conf.daemondMode){
    printf("Loading config file: %s\n",conf.configFilePath);
    //loadConfig(conf);
    
    startDaemon(&conf);
    abort();
  }else{
    char* query=optind>=argc?"":argv[optind];
    char* results=NULL;
    if(!conf.standAlone)results=askDaemon(query,conf.format);
    if(!results){
      //loadConfig(conf);
      AppList* list=buildAppList(conf.desktopDirs);
      results=executeSearch(query,list,conf.format);
      freeAppList(list);
    }

    if(results){
      printf("%s\n",results);
      free(results);
    }
  }
  return 0;
}
