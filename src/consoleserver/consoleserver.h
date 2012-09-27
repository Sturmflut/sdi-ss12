#ifndef _CONSOLESERVER_H_
#define _CONSOLESERVER_H_

typedef struct {
        char x;
        char y;
} cursor_t;

typedef struct {
        char character;
        char attribute;
} glyph_t __attribute__((packed));

typedef struct {
	char key;
	char modifier;
} keypress_t;

typedef struct {
	char 		count;
	keypress_t	keys[10];
} keybuffer_t;



extern L4_ThreadId_t loggerid;


void  consoleserver_server(void);
void consoleserver_init(void);

void  consoleserver_interrupt_impl();

void  consoleserver_putchar_impl(CORBA_Object  _caller, const CORBA_char  character, const CORBA_char  attrib);
void  consoleserver_putcharat_impl(CORBA_Object  _caller, const CORBA_long  x, const CORBA_long  y, const CORBA_char  character, const CORBA_char  attrib);

void  consoleserver_putstring_impl(CORBA_Object  _caller, const consolestring_t  text, const CORBA_char  attrib);
void consoleserver_putstringat_impl(CORBA_Object  _caller, const CORBA_long  x, const CORBA_long  y, const consolestring_t text, const CORBA_char  attrib);

void  consoleserver_setactivethread_impl(CORBA_Object  _caller, const CORBA_long  console, const L4_ThreadId_t * thread);

CORBA_long consoleserver_getconsolenum_impl(CORBA_Object  _caller);

CORBA_boolean consoleserver_getKey_impl(CORBA_Object  _caller, CORBA_char * key, CORBA_char * modifier);

#endif 
