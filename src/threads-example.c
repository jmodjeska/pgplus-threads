/*
 * Playground+ - threads-example.c
 * Example of command threading
 * (c) 2023 by Raindog (Jeremy Modjeska)
 * https://github.com/jmodjeska/pgplus-threads
 * ---------------------------------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include "include/config.h"
#include "include/player.h"
#include "include/proto.h"

/* you can put this in proto.h to avoid repeating it */
#define CLEAR(arr) memset( arr, '\0', sizeof(arr) );

/* import struct from threads.c */
extern struct thread_task task_list[MAX_THREADS];

/* Threaded commands consist of two parts: a sending function and a
 * receiving function. The sending function opens a thread; the
 * thread manager calls the receiving function when the work is done.
 */

/* SEND
 *
 * Send a long-running task to the thread manager.
 * Sending player *p to the thread manager is optional; for a talker
 * system command that doesn't output back to the player, you can
 * substitute NULL.
 */

void send_threaded_command(player *p)
{
  int q;
  char cmd[128];
  ENTERFUNCTION;

  sprintf(cmd, "%s", "sleep 5; stat /");
  q = add_thread_task(p, cmd, receive_threaded_command);

  if ( q >= 0 ) { tell_player(p, "-=> Queueing threaded command ...\n"); }
  else { tell_player(p, "Couldn't queue threaded command!\n"); }
  CLEAR(cmd);
  EXITFUNCTION;
}

/* RECEIVE
 *
 * Receive a long-running task from the thread manager.
 * Note that task->ret contains the output of the task.
 */

int receive_threaded_command(struct thread_task *task)
{
  char *oldstack = stack;
  ENTERFUNCTION;

  stack += sprintf(stack, "\n");
  pstack_mid(task->p, "Threaded Command Results");
  stack += sprintf(stack, "%s", task->ret);
  pstack_mid(task->p, NULL);
  stack = end_string(stack);
  tell_player(task->p, oldstack);
  stack = oldstack;

  EXITFUNCTION;
  return 1;
}
