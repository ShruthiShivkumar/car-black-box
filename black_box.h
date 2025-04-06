#include <xc.h>
#include "adc.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "uart.h"
#include "ds1307.h"
#include "i2c.h"
#include "external_EEPROM.h"


/* Enum for maintaining the app state */
typedef enum {
    e_dashboard, e_main_menu, e_view_log, e_set_time, e_download_log, e_clear_log
} State_t;


extern State_t state; // App state

//Function declarations

//Dashboard function declaration
void view_dashboard(void);

//Storing events function declaration
void event_store(void);

//Password function declaration
void password(void);

//main menu function declaration
void display_main_menu(void);

//View log function declaration
void view_log(void);

//Reading events function declaration
void event_reader(void);

//Change password function declaration
void change_password(void);

//Set time function declaration
void set_time(void);

//Download log function _declaration
void download_log(void);

//Clear log function declaration
void clear_log(void);

//function to read the events from the EEPROM
void read_event(void);

