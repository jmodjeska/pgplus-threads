/*
 * Playground+ - threads.c
 * Simple threading for longer-running commands
 * (c) 2023 by Raindog (Jeremy Modjeska)
 * https://github.com/jmodjeska/pgplus-threads
 * ---------------------------------------------------------------------------
 */

#include <pthread.h>
#define CLEAR(arr) memset( arr, '\0', sizeof(arr) );

struct thread_task task_list[MAX_THREADS];

/* interns */
void* thread_handler(void *);

/* Create the version info */

void threads_version(void)
{
  ENTERFUNCTION;
  sprintf(stack, " -=> Command threading v0.5 (by Raindog) enabled.\n");
  stack = strchr(stack, 0);
  EXITFUNCTION;
}

/* Find an open spot on the task list */

int get_open_task_id(void)
{
  int id, i;
  ENTERFUNCTION;
  for ( i = 0; i < MAX_THREADS; i++ )
  {
    if ( task_list[i].status == 0 )
    {
      id = i;
      EXITFUNCTION;
      return id;
    }
  }
  EXITFUNCTION;
  return -1;
}

/* Add a task to the todo list */

int add_thread_task(player *p, char *cmd, int (*func)(struct thread_task *))
{
  ENTERFUNCTION;
  int id = get_open_task_id();
  if ( id >= 0 )
  {
    // add the task to the list
    task_list[id].p = p;
    strcpy(task_list[id].cmd, cmd);
    task_list[id].report = func;
    task_list[id].status = 1;

    // fire up a thread handler
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, thread_handler, (void *) (intptr_t) id);
    pthread_detach(thread_id);

    EXITFUNCTION;
    return id;
  }
  log("error", "Couldn't add threaded task; list is full.");

  EXITFUNCTION;
  return -1;
}

/* Execute tasks placed on the threaded task list */

void *thread_handler(void *arg)
{
  int chars = 0, id = (int)arg;
  char c, msg[512];
  char *cmd;
  FILE *f;

  if ( task_list[id].status == 1 )
  {
    cmd = task_list[id].cmd;

    // DEBUG --
    // char *pn;
    // pn = ( task_list[id].p == NULL ) ? "system" : task_list[id].p->name;
    // sprintf(msg, "THREAD: %s queued: '%s' (thread ID %d)", pn, cmd, id);
    // debug_wall(msg);
    // CLEAR(msg);
    // -- DEBUG

    // run task and capture output
    if ( (f = (FILE*)popen(cmd, "r")) == 0 )
    {
      sprintf(msg, "THREAD: popen() failed for '%s' (thread ID %d))",
        task_list[id].cmd, id);
      log("error", msg);
      CLEAR(msg);
      task_list[id].status = -1;
    }
    else
    {
      while ( fread(&c, sizeof c, 1, f) )
      {
        task_list[id].ret[chars] = c;
        chars++;
      }
      pclose(f);

      // send results back to the output function
      if ( task_list[id].report != NULL &&
        task_list[id].report(&task_list[id]) == 1 )
      {
        // output success; flag for cleanup
        task_list[id].status = 2;
      }
      else if ( task_list[id].report != NULL )
      {
        // output failed
        sprintf(msg, "THREAD: finished but couldn't report '%s' (thread ID %d)",
          cmd, id);
        log("error", msg);
        CLEAR(msg);
        task_list[id].status = -1;
      }
      else {
        // no report required
        task_list[id].status = 2;
      }
    }
  }

  // cleanup task
  if ( task_list[id].status == 2 || task_list[id].status == -1 )
  {
    CLEAR(task_list[id].cmd);
    CLEAR(task_list[id].ret);
    task_list[id].p = NULL;
    task_list[id].status = 0;
  }
  return 0;
}
