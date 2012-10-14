//
// File:  src/test/main.cc
//
// Description: Testclient
//

#include <l4io.h>
#include <stdlib.h>

#include <sdi/sdi.h>
#include <sdi/types.h>
#include <sdi/constants.h>

#include <if/iflogging.h>
#include <if/ifnaming.h>
#include <if/ifconsoleserver.h>

#include <sdi/console_attributes.h>
#include <sdi/io.h>

static int PADDLE_HEIGHT = 6;


L4_ThreadId_t consoleid = L4_nilthread;
L4_ThreadId_t loggerid = L4_nilthread;


CORBA_Environment env(idl4_default_environment);


void print_charat(int x, int y, char character, char attrib)
{
    if(x >= 0 && x < 80 && y >= 0 && y < 25)
        IF_CONSOLESERVER_putcharat((CORBA_Object)consoleid, x, y, character, attrib, &env);
}


void print_stringat(int x, int y, char* text, char attrib)
{
    IF_CONSOLESERVER_putstringat((CORBA_Object)consoleid, x, y, text, attrib, &env);
}


void blank_line(int y)
{
    print_stringat(0, y, (char*)"                                                                                ", SDI_CONSOLE_ATTRIBUTE_FGBLACK | SDI_CONSOLE_ATTRIBUTE_BGBLACK);
}


void clear_screen()
{
    for(int y = 0; y < 25; y++)
        blank_line(y);
}


void game_loop()
{
    // Dot parameters
    char dotx = 3;
    char doty = 12;
    bool dotgoesright = true;
    bool dotgoesdown = true;

    // Paddle parameters
    char paddley = 7;

    // Result
    int lostballs = 0;

    char key;
    char modifier;
    char ret;
    char strbuf[30];

    sleep(300);

    clear_screen();

    while(1)
    {
        // Handle paddle
        ret = IF_CONSOLESERVER_getKey((CORBA_Object)consoleid, &key, &modifier, &env);

        if(ret && modifier == 0)
        {
            if(key == 'w')
                paddley = paddley - 2;

            if(key == 's')
                paddley = paddley + 2;

            if(paddley < 1)
                paddley = 1;

            if(paddley > (23 - PADDLE_HEIGHT))
                paddley = 23 - PADDLE_HEIGHT;
        }


	// Handle vertical walls
        if(doty == 1 && !dotgoesdown)
            dotgoesdown = true;

	if(doty == 24 && dotgoesdown)
            dotgoesdown = false;

        // Handle right wall
        if(dotx >= 78 && dotgoesright)
            dotgoesright = false;

        // Handle paddle
        if(dotx == 3 && !dotgoesright)
        {
            // Paddle catches ball
            if(doty >= paddley && doty < (paddley + PADDLE_HEIGHT))
                dotgoesright = true;
            else
           {
               // Paddle does not catch ball
               lostballs++;
               dotx = 9;
               dotgoesright = true;

               print_stringat(36, 12, (char*)"BALL LOST", SDI_CONSOLE_ATTRIBUTE_FGLIGHTRED);
               sleep(2000);
               print_stringat(36, 12, (char*)"         ", 0);
           }             
        }

        // Advance dot
        if(dotgoesright)
            dotx = dotx + 2;
        else
            dotx = dotx - 2;

        if(dotgoesdown)
            doty++;
        else
            doty--;



        // Draw dot
        print_charat(dotx, doty, '*', SDI_CONSOLE_ATTRIBUTE_FGLIGHTGREEN);

        // Draw paddle
        for(int i = 0; i < PADDLE_HEIGHT; i++)
            print_charat(2, paddley + i, '|', SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE);

        // Draw result
        snprintf(strbuf, sizeof(strbuf), "Balls lost: %i", lostballs);
	print_stringat(0, 0, strbuf, SDI_CONSOLE_ATTRIBUTE_FGLIGHTYELLOW);
   
        sleep(150);


        // Clear dot
        print_charat(dotx, doty, ' ', 0);

        // Clear paddle
        for(int i = 0; i < PADDLE_HEIGHT; i++)
            print_charat(2, paddley + i, ' ', 0);
    }
}


int main()
{
    // Resolve logger
    while (L4_IsNilThread(loggerid))
        loggerid = nameserver_lookup((path_t)"/server/logger");

    // Resolve console
    while (L4_IsNilThread(consoleid))
        consoleid = nameserver_lookup((path_t)"/server/console");

    // Let's play!
    game_loop();
}
