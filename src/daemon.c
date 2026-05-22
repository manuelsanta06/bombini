#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "types.h"
#include "search.h"
#include "indexer.h"
#include "daemon.h"

void startDaemon(Config* conf){
  unlink(SOCKET_PATH);
  int server_fd=socket(AF_UNIX,SOCK_STREAM,0);
  if(server_fd<0)return;
  struct sockaddr_un address;
  address.sun_family=AF_UNIX;
  strcpy(address.sun_path,SOCKET_PATH);

  int returned=bind(server_fd,(struct sockaddr*)&address,sizeof(address));
  if(returned){
    close(server_fd);
    return;
  }
  listen(server_fd,10);
  AppList* list=buildAppList(conf->desktopDirs);

  while(1){
    int client_fd=accept(server_fd,NULL,NULL);
    if(client_fd<0)continue;
    char query[512];
    int bytesRead=read(client_fd,query,sizeof(query)-1);
    if(bytesRead>0){
      query[bytesRead]='\0';
      if(strcmp(query,RELOAD_CMD)==0){
        printf("Reload command received. Rebuilding cache...\n");
        freeAppList(list);
        list=buildAppList(conf->desktopDirs);
        //loadConfig(conf);
        write(client_fd,"OK",2); 
      }else{
        printf("Searching for %s\n",query);
        char* json=executeSearch(query,list);
        if(json){
          write(client_fd,json,strlen(json));
          free(json);
        }
      }
    }
    close(client_fd);
  }
  freeAppList(list);
  close(server_fd);
  unlink(SOCKET_PATH);
  return;
}
char* askDaemon(const char* query){
  int cli_fd=socket(AF_UNIX,SOCK_STREAM,0);
  struct sockaddr_un address;
  address.sun_family=AF_UNIX;
  strcpy(address.sun_path,SOCKET_PATH);
  int returned=connect(cli_fd, (struct sockaddr*)&address, sizeof(address));
  if(returned){
    close(cli_fd);
    return NULL;
  }
  write(cli_fd,query,strlen(query)+1);
  int size=4096,len=0;
  char* json=malloc(size);
  if(!json){
    close(cli_fd);
    return NULL;
  }
  char buffer[1024];
  while((returned=read(cli_fd,buffer,sizeof(buffer)))>0){
    if(len+returned+1>=size){
      size=size+size/2;
      json=realloc(json,size);
      if(!json){
        close(cli_fd);
        return NULL;
      }
    }
    memcpy(json+len,buffer,returned);
    len+=returned;
  }
  close(cli_fd);
  if(len==0){
    free(json);
    return NULL;
  }
  json[len]='\0';
  return json;
}


void reloadDaemon(void){
  char* response=askDaemon(RELOAD_CMD);
  if(response){
    printf("Daemon recargado exitosamente.\n");
    free(response);
  }else
    fprintf(stderr,"Error: El daemon no esta corriendo.\n");
}
