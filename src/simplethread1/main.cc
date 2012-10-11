//
// File:  src/test/main.cc
//
// Description: Testclient
//

#include <l4io.h>

#include <sdi/sdi.h>
#include <sdi/types.h>
#include <sdi/constants.h>

#include <if/iflogging.h>
#include <if/ifnaming.h>
#include <if/ifconsoleserver.h>
#include <if/iftaskserver.h>

#include <sdi/console_attributes.h>


L4_ThreadId_t loggerid = L4_nilthread;
L4_ThreadId_t consoleid = L4_nilthread;
L4_ThreadId_t taskserverid = L4_nilthread;
CORBA_Environment env(idl4_default_environment);


L4_Word_t second_thread_stack[1024];

void print_charat(int x, int y, char character, char attrib)
{
	IF_CONSOLESERVER_putcharat((CORBA_Object)consoleid, x, y, character, attrib, &env); 
}


void print_stringat(int x, int y, char* text, char attrib)
{
	IF_CONSOLESERVER_putstringat((CORBA_Object)consoleid, x, y, text, attrib, &env); 
}


void thread_loop() {
    while(42) {
        sleep(5000);

        if( loggerid != L4_nilthread) {
            log_printf(loggerid, "Thread is running...");
        }
    }
}


void animation_loop()
{
	int x = 85;
	const char* banner = "Karlsruhe Institute of Technology";
	char buf[30];

	/** Print logo */
	print_stringat(23, 3, " ____  ____ ___       ___  ____  ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 4, "/ ___||  _ \\_ _|     / _ \\/ ___| ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 5, "\\___ \\| | | | |_____| | | \\___ \\ ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 6, " ___) | |_| | |_____| |_| |___) |", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 7, "|____/|____/___|     \\___/|____/ ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);

	void *p1, *p2, *p3;
	char key, modifier;

	while(1)
	{
		// Read key
		if(IF_CONSOLESERVER_getKey((CORBA_Object)consoleid, &key, &modifier, &env))
		{
			snprintf(buf, sizeof(buf), "%x %x", key, modifier);
			print_stringat(1, 1, buf, SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE);
		}
		

		/* Scrolling step */
		x--;
		if(x < -strlen(banner))
			x = 85;

		/* Blank line */
		print_stringat(0, 23, "                                                                                ", SDI_CONSOLE_ATTRIBUTE_FGBLACK | SDI_CONSOLE_ATTRIBUTE_BGBLACK);

		/* Print banner */
		for(int i = 0; i < strlen(banner); i++)
			if(x + i > 0)
				print_charat(x + i, 23, banner[i], SDI_CONSOLE_ATTRIBUTE_FGLIGHTRED);

		sleep(150);
	}
}


int main()
{
	L4_Msg_t msg;
	L4_MsgTag_t tag;

	char buf[256];

	CORBA_Environment env(idl4_default_environment);


	while (L4_IsNilThread(loggerid))
		loggerid = nameserver_lookup("/server/logger");


	/* Print some stuff on the console */
	while (L4_IsNilThread(consoleid))
		consoleid = nameserver_lookup("/server/console");

    while (L4_IsNilThread(taskserverid))
        taskserverid = nameserver_lookup("/task");

    L4_ThreadId_t myself = L4_Myself();
	IF_CONSOLESERVER_setactivethread((CORBA_Object)consoleid, 1, &myself, &env);

    /* Starting a second thread */
    IF_TASKSERVER_create_thread(
            (CORBA_Object) taskserverid, 
            (L4_Word_t)&thread_loop, 
            (L4_Word_t)&second_thread_stack[1023], 
            &env);

	/* And now.... action! */
	animation_loop();

	/* Spin forever */
	while (42) ;

	return 0;
}

