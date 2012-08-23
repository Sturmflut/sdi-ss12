#ifndef _CONSTANTS_H
#define _CONSTANTS_H 1


/* TASKSERVER */
/** Default nameserver thread id */
static const int SDI_TASKSERVER_DEFAULT_THREADID = 50;

/* MEMORYSERVER */
/** Default memoryserver thread id */
static const int SDI_MEMORYSERVER_DEFAULT_THREADID = 60;

/* FILESERVER */
/** Default fileserver thread id */
static const int SDI_FILESERVER_DEFAULT_THREADID = 70;

/* NAMESERVER */
/** Maximum number of entries in the nameserver database */
static const int SDI_NAMESERVER_MAX_ENTRIES = 16;

/** Maximum nameserver entrypath length */
static const int SDI_NAMESERVER_MAX_ENTRY_LEN = 64;

/** Default nameserver thread id */
static const int SDI_NAMESERVER_DEFAULT_THREADID = 80;




/* CONSOLESERVER */
static const int SDI_CONSOLESERVER_NUM_CONSOLES = 8;

#endif

