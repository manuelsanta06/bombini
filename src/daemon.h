#ifndef DAEMON_H
#define DAEMON_H

#include "types.h"

#ifndef SOCKET_PATH
#define SOCKET_PATH "/tmp/bombini.sock"
#endif

/*
 * Starts the daemon
 * Return on error
 */
void startDaemon(AppList* list);

/*
 * Try to connect to an existing daemon
 * Return json on succces or null if no daemon exists
 */
char* askDaemon(const char* query);

#endif
