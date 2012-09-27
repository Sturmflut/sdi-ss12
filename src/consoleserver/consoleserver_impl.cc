#include <sdi/sdi.h>
#include <sdi/constants.h>
#include <sdi/io.h>

#include <l4io.h>

#include <if/iflogging.h>
#include <if/iftaskserver.h>

#include "consoleserver.h"

#include "keyconsts.h"
#include <sdi/keyboard.h>


unsigned char modifier_status = 0x00;

char keycodes[128] = "\000\0331234567890-=\177\t"                    /* 0x00 - 0x0f */
        "qwertyuiop[]\r\000as"                          /* 0x10 - 0x1f */
        "dfghjkl;'`\000\\zxcv"                          /* 0x20 - 0x2f */
        "bnm,./\000*\000 \000\201\202\203\204\205"      /* 0x30 - 0x3f */
        "\206\207\210\211\212\000\000789-456+1"         /* 0x40 - 0x4f */
        "230\177\000\000\213\214\000\000\000\000\000\000\000\000\000\000" /* 0x50 - 0x5f */
        "\r\000/";
char keycodes_shift[128] = "\000\033!@#$%^&*()_+\177\t"                    /* 0x00 - 0x0f */
        "QEWRTZUIOP[]\r\000AS"                          /* 0x10 - 0x1f */
        "DFGHJKL;'`\000\\ZXCV"                          /* 0x20 - 0x2f */
        "BNM<>?\000*\000 \000\201\202\203\204\205"      /* 0x30 - 0x3f */
        "\206\207\210\211\212\000\000789-456+1"         /* 0x40 - 0x4f */
        "230\177\000\000\213\214\000\000\000\000\000\000\000\000\000\000" /* 0x50 - 0x5f */
        "\r\000/";;


/** Backbuffer */
glyph_t backbuffer[SDI_CONSOLESERVER_NUM_CONSOLES][80 * 25];

/** Cursors */
cursor_t cursors[SDI_CONSOLESERVER_NUM_CONSOLES] = {0, 0};

/** Keybuffers */
keybuffer_t keybuffers[SDI_CONSOLESERVER_NUM_CONSOLES];

/** Thread mapping */
L4_ThreadId_t active_threads[SDI_CONSOLESERVER_NUM_CONSOLES];

/** Video memory */
static glyph_t *vidmem = (glyph_t *)0xb8000;

/** Currently active console */
int active_console = 0;


/**
 Find the console for a given thread
*/
int find_console_for_thread(L4_ThreadId_t threadid)
{
        for(int i = 0; i < SDI_CONSOLESERVER_NUM_CONSOLES; i++)
                if(active_threads[i] == threadid)
                        return i;

        return -1;
}


/**
 Blit the given backbuffer to the screen
*/
void blit_backbuffer(int num)
{
        memcpy(vidmem, backbuffer[num], 80 * 25 * sizeof(glyph_t));
}


/**
 Put the given character to the given position
*/
void putcharat(int consolenum, int x, int y, char character, char attribute)
{
        if(x < 80 && y < 25)
        {
                int index = y * 80 + x;

                backbuffer[consolenum][index].character = character;
                backbuffer[consolenum][index].attribute = attribute;

		// If the character was put to the current console, put
		// It there
                if(consolenum == active_console)
                {
                        (vidmem + index)->character = character;
                        (vidmem + index)->attribute = attribute;
                }
        }
}


void putchar(int consolenum, char character, char attribute)
{
        if(consolenum < 0 || consolenum >= SDI_CONSOLESERVER_NUM_CONSOLES)
                return;

        putcharat(consolenum, cursors[consolenum].x, cursors[consolenum].y, character, attribute);

        cursors[consolenum].x++;

        /** Line wrap */
        if(cursors[consolenum].x >= 80)
        {
                cursors[consolenum].x = 0;
                cursors[consolenum].y++;

                /** Scroll */
                if(cursors[consolenum].y >= 25)
                {
                        cursors[consolenum].y = 24;

                        for(int i = 0; i < (80 * 24); i++)
                        {
                                        backbuffer[consolenum][i] = backbuffer[consolenum][i + 80];
                        }

                        if(consolenum == active_console)
                                blit_backbuffer(active_console);
                }
        }
}


/**
 Blank all consoles
*/
void init_consoles()
{
        for(int i = 0; i < SDI_CONSOLESERVER_NUM_CONSOLES;i++)
        {
                // Blank the screen and set attribute to bright white
                for(int j = 0; j < 2000; j++)
                {
                        backbuffer[i][j].character = ' ';
                        backbuffer[i][j].attribute = 0xf;
                }

                // Reset the cursor
                cursors[i].x = 0;
                cursors[i].y = 0;

                /** Reset thread mappings */
                active_threads[i] = L4_nilthread;
        }
}

/**
 Clear all keyboard buffers
*/
void init_keybuffers()
{
        for(int i = 0; i < SDI_CONSOLESERVER_NUM_CONSOLES;i++)
        {
		keybuffers[i].count = 0;

		for(int j = 0; j < 10; j++)
		{
			keybuffers[i].keys[j].key = '\0';
			keybuffers[i].keys[j].modifier = 0;
		}
	}
}



void set_active_console(int num)
{
        if(num < SDI_CONSOLESERVER_NUM_CONSOLES)
        {
                active_console = num;

                blit_backbuffer(num);
        }
}



/**
*/
void  consoleserver_interrupt_impl()
{
        char logbuf[50];
        unsigned char scancode;
        unsigned char status;
	char* modstring;

	CORBA_Environment env(idl4_default_environment);

	status = inb(0x64);

	if(status & KBD_STAT_OBF)
	{
	        scancode = inb(0x60);

		// Handle modifiers
		if(scancode == KEY_SHIFT_DOWN)
			modifier_status |= KEYMASK_SHIFT;

		if(scancode == KEY_SHIFT_UP)
			modifier_status &= (~KEYMASK_SHIFT);

		if(scancode == KEY_CTRL_DOWN)
			modifier_status |= KEYMASK_CTRL;

		if(scancode == KEY_CTRL_UP)
			modifier_status &= (~KEYMASK_CTRL);

		if(scancode == KEY_ALT_DOWN)
			modifier_status |= KEYMASK_ALT;

		if(scancode == KEY_ALT_UP)
			modifier_status &= (~KEYMASK_ALT);


		// Handle keys
		if(scancode >= 0x01 && scancode <= 0x58)
		{
			char key;

			if(modifier_status & KEYMASK_SHIFT)
				key = keycodes_shift[scancode];
			else
				key = keycodes[scancode];

			if(key != 0)
			{
				// Magic key? Switch consoles
				if(modifier_status == KEYMASK_CTRL && (key >= '1' && key <= '8'))
				{
					set_active_console(key-'1');
					return;
				}
		
				// Put the keypress into the buffer
				if(keybuffers[active_console].count >= 10)
				{
					IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[KEYBOARD] Buffer full!", &env);
					return;
				}

				keybuffers[active_console].keys[keybuffers[active_console].count].key = key;
				keybuffers[active_console].keys[keybuffers[active_console].count].modifier = modifier_status;
				keybuffers[active_console].count++;

				// Debugging output
			        snprintf(logbuf, sizeof(logbuf), "[KEYBOARD] Interrupt! %s + %c (%2x)", modstring, key, key);
			        IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);
			}
		}
	}
}


/**
*/
void  consoleserver_putchar_impl(CORBA_Object  _caller, const CORBA_char  character, const CORBA_char  attrib)
{
    /** Find matching console */
    int console = find_console_for_thread(_caller);

    if(console != -1)
        putchar(console, character, attrib);

    return;
}


/**
*/
void  consoleserver_putcharat_impl(CORBA_Object  _caller, const CORBA_long  x, const CORBA_long  y, const CORBA_char  character, const CORBA_char  attrib)
{
    /** Find matching console */
    int console = find_console_for_thread(_caller);

    if(console != -1)
        putcharat(console, x,y, character, attrib);

    return;
}


/**
*/
void  consoleserver_putstring_impl(CORBA_Object  _caller, const consolestring_t  text, const CORBA_char  attrib)
{
    /** Find matching console */
        int console = find_console_for_thread(_caller);

        if(console != -1)
        {
                for(int i = 0; i < strlen(text); i++)
                        putchar(console, text[i], attrib);
        }
}


/**
*/
void consoleserver_putstringat_impl(CORBA_Object  _caller, const CORBA_long  x, const CORBA_long  y, const consolestring_t text, const CORBA_char  attrib)
{
    /** Find matching console */
        int console = find_console_for_thread(_caller);

        if(console != -1)
        {
                for(int i = 0; i < strlen(text); i++)
                        putcharat(console, x + i, y, text[i], attrib);
        }

        return;
}


/**
*/
void  consoleserver_setactivethread_impl(CORBA_Object  _caller, const CORBA_long  console, const L4_ThreadId_t * thread)
{
        if(console <0 || console >= SDI_CONSOLESERVER_NUM_CONSOLES)
                return;

        active_threads[console] = *thread;
}


/**
*/
CORBA_long consoleserver_getconsolenum_impl(CORBA_Object  _caller)
{
	return active_console;
}


/**
*/
CORBA_boolean consoleserver_getKey_impl(CORBA_Object  _caller, CORBA_char * key, CORBA_char * modifier)
{
	/** Find matching console */
        int console = find_console_for_thread(_caller);

        if(console != -1 && keybuffers[console].count > 0)
        {
		// Return data
		*key = keybuffers[console].keys[0].key;
		*modifier = keybuffers[console].keys[0].modifier;

		// Move buffer
		for(int i = 1; i < 10; i++)
			keybuffers[console].keys[i-1] = keybuffers[console].keys[i];

		// Correct count
		keybuffers[console].count--;

		return true;
	}

	return false;
}


void consoleserver_init()
{
	CORBA_Environment env(idl4_default_environment);
	
	// Init the consoles	
        init_consoles();
        set_active_console(0);

	// Init the keybuffers
	init_keybuffers();
	
	// Register the keyboard interrupt
        L4_ThreadId_t taskserverid = L4_nilthread;

        while (L4_IsNilThread(taskserverid))
                taskserverid = nameserver_lookup("/task");

        // Attach keyboard interrupt
        IF_TASKSERVER_attach_interrupt((CORBA_Object)taskserverid, 0x01, &env);

        IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[KEYBOARDDRIVER] Registered Interrupt...", &env);
}

