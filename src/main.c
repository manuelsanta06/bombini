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
  printf("  -d, --daemon         work as a daemon for following ejecutions\n");
  printf("  -c, --config FILE    Config file path\n");
  printf("  -h, --help           shows this help message\n");
}

int main(int argc, char** argv){
  bool daemonMode=false;
  char* configFile=NULL;
  int opt;

  static struct option long_options[]={
    {"daemon",no_argument,      0,'d'},
    {"help",  no_argument,      0,'h'},
    {"config",required_argument,0,'c'},
    {0,0,0,0}
  };
  int option_index=0;
  while((opt=getopt_long(argc,argv,"dhc:",long_options,&option_index))!=-1){
    switch(opt){
      case 'd':
        daemonMode=true;
        break;
      case 'c':
        configFile=optarg;
        break;
      case '?':
      case 'h':
        printHelp(argv[0]);
        exit(0);
      default:
        abort();
    }
  }

  if(!configFile){
    char *home=getenv("HOME");
    if(home){
      static char defaultPath[PATH_MAX]; 
      int len=snprintf(defaultPath,sizeof(defaultPath),"%s/.config/bombini/conf.ini",home);
      if(len<0){
        fprintf(stderr,"Error parsing the path.\n");
        abort();
      }else if(len>=sizeof(defaultPath)){
        fprintf(stderr,"Config path too long, aborting.\n");
        abort();
      }
      configFile=defaultPath;
    }else{
      fprintf(stderr,"Error getting HOME directory\n");
      abort();
    }
  }
  Config* conf=malloc(sizeof(Config));
  conf->desktopDirs="/home/santa/.local/share/applications:/usr/share/applications/";
  AppList* list=buildAppList(conf->desktopDirs);
  if(configFile){
    if(daemonMode)printf("Loading plugins from: %s\n",configFile);
  }
  if(daemonMode){
    printf("Running as daemon...\n");
  }else{
    if(optind>=argc){
      fprintf(stderr,"Error: No query provided for direct search.\n");
      abort();
    }
    char* query=argv[optind];
    char* results=executeSearch(query,list);
    printf("%s\n",results);
  }
  freeAppList(list);
  return 0;
}
