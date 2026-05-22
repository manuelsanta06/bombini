#ifndef DAEMON_H
#define DAEMON_H

#include "types.h"

#ifndef SOCKET_PATH
#define SOCKET_PATH "/tmp/bombini.sock"
#endif
#define RELOAD_CMD ":RELOAD:"

/*
 * Starts the daemon
 * Return on error
 */
void startDaemon(Config* conf);

/*
 * Try to connect to an existing daemon
 * Return json on succces or null if no daemon exists
 */
char* askDaemon(const char* query);

void reloadDaemon(void);


#endif
