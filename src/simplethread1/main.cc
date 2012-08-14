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

#include <sdi/console_attributes.h>


L4_ThreadId_t consoleid = L4_nilthread;
CORBA_Environment env(idl4_default_environment);


void print_charat(int x, int y, char character, char attrib)
{
	IF_CONSOLESERVER_putcharat((CORBA_Object)consoleid, x, y, character, attrib, &env); 
}


void print_stringat(int x, int y, char* text, char attrib)
{
	IF_CONSOLESERVER_putstringat((CORBA_Object)consoleid, x, y, text, attrib, &env); 
}


void animation_loop()
{
	int x = 85;
	const char* banner = "Karlsruhe Institute of Technology";

	/** Print logo */
	print_stringat(23, 3, " ____  ____ ___       ___  ____  ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 4, "/ ___||  _ \\_ _|     / _ \\/ ___| ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 5, "\\___ \\| | | | |_____| | | \\___ \\ ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 6, " ___) | |_| | |_____| |_| |___) |", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);
	print_stringat(23, 7, "|____/|____/___|     \\___/|____/ ", SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE | SDI_CONSOLE_ATTRIBUTE_BGBLUE);

	void *p1, *p2, *p3;

	while(1)
	{
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

		p1 = malloc(10);
		p2 = malloc(4097);
		p3 = malloc(10);
		sleep(150);
		free(p2);
		free(p3);
		free(p1);
	}
}


int main()
{
	L4_Msg_t msg;
	L4_MsgTag_t tag;

	char buf[256];

	CORBA_Environment env(idl4_default_environment);

	L4_ThreadId_t loggerid = L4_nilthread;

	while (L4_IsNilThread(loggerid))
		loggerid = nameserver_lookup("/server/logger");


	/* Print some stuff on the console */
	while (L4_IsNilThread(consoleid))
		consoleid = nameserver_lookup("/server/console");

	L4_ThreadId_t myself = L4_Myself();
	IF_CONSOLESERVER_setactivethread((CORBA_Object)consoleid, 0, &myself, &env);


	/* And now.... action! */
	animation_loop();

	/* Spin forever */
	while (42) ;

	return 0;
}
