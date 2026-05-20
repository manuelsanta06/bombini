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
      }else if((size_t)len>=sizeof(defaultPath)){
        fprintf(stderr,"Config path too long, aborting.\n");
        abort();
      }
      configFile=defaultPath;
    }else{
      fprintf(stderr,"Error getting HOME directory\n");
      abort();
    }
  }
  if(daemonMode){
    printf("Running as daemon...\n");
    Config* conf=malloc(sizeof(Config));
    conf->desktopDirs="/home/santa/.local/share/applications:/usr/share/applications/";
    if(configFile){
      if(daemonMode)printf("Loading plugins from: %s\n",configFile);
    }
    AppList* list=buildAppList(conf->desktopDirs);

    startDaemon(list);

    freeAppList(list);
    free(conf);
    abort();
  }else{
    char* query=optind>=argc?"":argv[optind];
    char* results=askDaemon(query);
    if(!results){
      printf("Fallback\n");
      Config* conf=malloc(sizeof(Config));
      conf->desktopDirs="/home/santa/.local/share/applications:/usr/share/applications/";
      if(configFile){
        if(daemonMode)printf("Loading plugins from: %s\n",configFile);
      }
      AppList* list=buildAppList(conf->desktopDirs);
      results=executeSearch(query,list);
      freeAppList(list);
      free(conf);
    }

    if(results){
      printf("%s\n",results);
      free(results);
    }
  }
  return 0;
}
