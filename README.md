# PG+ Threading [WIP]
Simple threading support for PG+ talkers. Tested on AMI Linux (RHEL). YMMV.

## Requirements

A functioning [Playground Plus](https://github.com/talkers/pgplus) talk server.

## Installation

_NOTE: These instructions require more testing with out-of-the-box PG+._

1. Add `src/threads.c` provided in this repo to your talker's `src` directory.

1. Add `src/threads-example.c` provided in this repo to your talker's `src` directory. This contains an example of a command that takes advantage of threads.

1. Replace `src/configure/Configure.help` with the one provided in this repo (or add the help items for THREADS and MAX_THREADS at the end).

1. Update `src/configure/Configure.options` by inserting the following code at ~[line 35](https://github.com/talkers/pgplus/blob/master/src/configure/Configure.options#L35):

    ```
    #
    comment 'Threaded commands'
    bool 'Allow development of threaded commands' THREADS
    if [ "$THREADS" = "y" ]; then
      int '  Max threads' MAX_THREADS
    fi
    ```

1. Update `src/include/proto.h` by inserting the following code at ~[line 380](https://github.com/talkers/pgplus/blob/master/src/include/proto.h#L380):

    ```
    #ifdef THREADS
    extern void threads_version(void);
    #endif /* THREADS */
    ```

1. Update `src/include/proto.h` by inserting the following code at ~[line 498](https://github.com/talkers/pgplus/blob/master/src/include/proto.h#L498):

    ```
    #ifdef THREADS
    struct thread_task 
    {
      int status;
      player *p;
      char cmd[512];
      char ret[2048];
      int (*report)(struct thread_task*);
    };
    ```

1. Update `src/version.c` by inserting the following code at ~[line 161](https://github.com/talkers/pgplus/blob/master/src/version.c#L161):

    ```
    #ifdef THREADS
    threads_version();
    #endif /* THREADS */
    ```

1. Update `src/glue.c` by inserting the following code at ~[line 269](https://github.com/talkers/pgplus/blob/master/src/glue.c#L269):

    ```
    #ifdef THREADS
    #include "threads.c"
    #endif /* THREADS */
    ```

## Configuration

During `make config`, set the `MAX_THREADS` option as you see fit. You shouldn't need more than 4-8 unless you're really going crazy with threading.

1. Recompile using the standard PG+ compilation process.

## You might also like
* [PG+ Short Link Generator](https://github.com/jmodjeska/pgplus-shortlink)
* [PG+ Cocktail Recipe](https://github.com/jmodjeska/pgplus-cocktail)
* [PG+ Test Harness](https://github.com/jmodjeska/pgplus-test)
