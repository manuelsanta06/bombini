#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <sys/inotify.h>

#include "types.h"
#include "search.h"
#include "indexer.h"
#include "daemon.h"

int startDaemon(Config* conf){
  //main socket fd
  unlink(SOCKET_PATH);
  int server_fd=socket(AF_UNIX,SOCK_STREAM,0);
  if(server_fd<0)return 1;
  struct sockaddr_un address;
  address.sun_family=AF_UNIX;
  strcpy(address.sun_path,SOCKET_PATH);

  int returned=bind(server_fd,(struct sockaddr*)&address,sizeof(address));
  if(returned){
    close(server_fd);
    return 1;
  }
  listen(server_fd,10);

  //inotify fd
  int inotify_fd=inotify_init();
  if(inotify_fd<0){
    perror("Error while inotify init, no autoreload");
  }else{
    char* dirs_copy=strdup(conf->desktopDirs);
    if(!dirs_copy)return 1;
    char* saveptr;

    //register all .desktop's paths
    char* dir=strtok_r(dirs_copy,":",&saveptr);
    while(dir!=NULL){
      if(inotify_add_watch(inotify_fd, dir,IN_CREATE|IN_DELETE|IN_MODIFY)==-1)
        fprintf(stderr,"Path '%s' cant be accessed\n",dir);
      dir=strtok_r(NULL,":",&saveptr);
    }
    free(dirs_copy);
  }
  struct pollfd fds[2];
  int nfds=1;

  fds[0].fd=server_fd;
  fds[0].events=POLLIN;

  if(inotify_fd>=0){
    fds[1].fd=inotify_fd;
    fds[1].events=POLLIN;
    nfds++;
  }

  AppList* list=buildAppList(conf->desktopDirs);

  while(1){
    int poll_count=poll(fds,nfds,-1);
    if(poll_count<0){
      perror("Error en poll");
      return 1;
    }

    //main socket
    if (fds[0].revents&POLLIN){
      int client_fd=accept(server_fd,NULL,NULL);
      if(client_fd<0)continue;
      char query[512];
      int bytesRead=read(client_fd,query,sizeof(query)-1);
      if(bytesRead>0){
        query[bytesRead]='\0';
        OutputFormat format=FORMAT_JSON;
        char* actualQuery=query;

        if(query[0]=='J' && query[1]==':'){
          format=FORMAT_JSON;
          actualQuery=query+2;
        }else if(query[0]=='P' && query[1]==':'){
          format=FORMAT_PLAIN;
          actualQuery=query+2;
        }else if(query[0]=='S' && query[1]==':'){
          format=FORMAT_SYS;
          actualQuery=query+2;
        }
        if(format==FORMAT_SYS&&strcmp(actualQuery,RELOAD_CMD)==0){
          printf("Reload command received. Rebuilding cache...\n");
          freeAppList(list);
          list=buildAppList(conf->desktopDirs);
          write(client_fd,"OK",2); 
        }else{
          printf("Searching for \"%s\", Mode %d\n",actualQuery,format);
          char* json=executeSearch(actualQuery,list,format);
          if(json){
            write(client_fd,json,strlen(json));
            free(json);
          }
        }
      }
      close(client_fd);
    }

    //inotify
    if(nfds>1&&(fds[1].revents&POLLIN)){
      char inotify_buffer[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
      int bytes_read=read(inotify_fd,inotify_buffer,sizeof(inotify_buffer));
      
      if(bytes_read<=0)continue;
      printf("inotify:rebuilding app list automaticaly\n");

      //50ms delay to prevent multiple rebuilding applist many times
      //editors like vim/nano generate 5 events to modify one file
      struct pollfd drain_fd={inotify_fd,POLLIN,0};
      while(poll(&drain_fd,1,50)>0)
        read(inotify_fd,inotify_buffer,sizeof(inotify_buffer));

      freeAppList(list);
      list=buildAppList(conf->desktopDirs);
    }
  }
  freeAppList(list);
  close(inotify_fd);
  close(server_fd);
  unlink(SOCKET_PATH);
  return 0;
}
char* askDaemon(const char* query,OutputFormat format){
  int cli_fd=socket(AF_UNIX,SOCK_STREAM,0);
  struct sockaddr_un address;
  address.sun_family=AF_UNIX;
  strcpy(address.sun_path,SOCKET_PATH);
  int returned=connect(cli_fd,(struct sockaddr*)&address,sizeof(address));
  if(returned){
    close(cli_fd);
    return NULL;
  }
  char request[514];
  char formatChar='J';
  if(format==FORMAT_JSON)formatChar='J';
  else if(format==FORMAT_PLAIN)formatChar='P';
  else if(format==FORMAT_SYS)formatChar='S';
  snprintf(request,sizeof(request),"%c:%s",formatChar,query);
  write(cli_fd,request,strlen(request)+1);
  int size=4096,len=0;
  char* output=malloc(size);
  if(!output){
    close(cli_fd);
    return NULL;
  }
  char buffer[1024];
  while((returned=read(cli_fd,buffer,sizeof(buffer)))>0){
    if(len+returned+1>=size){
      size=size+size/2;
      char* tmp=realloc(output,size);
      if(!tmp){
        close(cli_fd);
        free(output);
        return NULL;
      }
      output=tmp;
    }
    memcpy(output+len,buffer,returned);
    len+=returned;
  }
  close(cli_fd);
  if(len==0){
    free(output);
    return NULL;
  }
  output[len]='\0';
  return output;
}


void reloadDaemon(void){
  char* response=askDaemon(RELOAD_CMD,FORMAT_SYS);
  if(response){
    printf("Daemon recargado exitosamente.\n");
    free(response);
  }else
    fprintf(stderr,"Error: El daemon no esta corriendo.\n");
}
