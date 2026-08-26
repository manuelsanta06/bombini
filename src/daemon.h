#ifndef DAEMON_H
#define DAEMON_H

#include "types.h"

#ifndef SOCKET_PATH
#define SOCKET_PATH "/tmp/bombini.sock"
#endif
#define RELOAD_CMD "S::RELOAD:"

enum BombiniStatus{
  BOMBINI_OK=0,
  BOMBINI_ERROR=1,
  BOMBINI_ALREADY_RUNNING=2
};

/*
 * Starts the daemon
 * Return on error
 */
int startDaemon(Config* conf);

/*
 * Try to connect to an existing daemon
 * Return search results on succces or null if no daemon exists
 */
char* askDaemon(const char* query,OutputFormat format);

void reloadDaemon(void);


#endif
