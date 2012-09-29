#include <l4io.h>

#include "taskserver.h" 
#include "root.h"

L4_ThreadId_t last_thread_id = L4_nilthread;

void taskserver_init() {
    /* Announce task service */
    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[TASK] Registering", &env);

    nameserver_register("/task");

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[TASK] Registered...", &env);
    
    last_thread_id = pagerid;

}

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env) {
    L4_ThreadId_t threadid = L4_GlobalId(L4_ThreadNo(last_thread_id) + 1, 1);
    last_thread_id = threadid;
    
    // 1. Task erstellen (ThreadControl, SpaceControl)

    // 2. Taskserver -> Fileserver: Datei laden und zwischspeichern
    // (Fileserver gibt komplette ELF-Datei zurück)

    // 3. ELF-Datei dekodieren (siehe load_elfimage im Rootserver) =>
    // man erhält eine Liste ("Sections") von (path, offset, size, realsize), "path"
    // ist bei allen Elementen gleich.
    
    // 4. Für jedes Element der Sections-Liste einen
    // map_file_pages-Aufruf an den Memory-Server senden => Sections
    // sind im Task eingeblendet
    
    // 5. Eintrag in Task-Liste erstellen für neu erstellen Thread
    
    // 6. MemoryServer: StartupIPC senden

    L4_ThreadId_t t;
    return t;

}
