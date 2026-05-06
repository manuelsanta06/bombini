#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

void printHelp(char* name){
  printf("Uso: %s [OPCIONES]\n",name);
  printf("Opciones:\n");
  printf("  -d, --daemon         Ejecutar como daemon en segundo plano\n");
  printf("  -c, --config FILE    Ruta al archivo de configuración\n");
  printf("  -h, --help           Muestra este mensaje de ayuda\n");
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
        fprintf(stderr,"Error al formatear la ruta.\n");
        abort();
      }else if(len>=sizeof(defaultPath)){
        fprintf(stderr,"Advertencia: La ruta de configuración es demasiado larga.\n");
        abort();
      }
      configFile=defaultPath;
    }else{
      fprintf(stderr,"Error getting HOME directory\n");
      abort();
    }
  }
  if(configFile){
    if(!daemonMode)printf("Loading plugins from: %s\n",configFile);
  }
  if(daemonMode){
    printf("Running as daemon...\n");
    return 0;
  }

  return 0;
}
