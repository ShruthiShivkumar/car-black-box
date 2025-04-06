
#include "black_box.h"
#include "adc.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "uart.h"
#include "i2c.h"
#include "ds1307.h"
#include "external_EEPROM.h"

int event_count;
extern unsigned char time[9];
unsigned char EV[9][3] = {"ON","GN","G1","G2","G3","G4","G5","GR","C_" } ;
unsigned char opt[4][16]={"View Log       ","Clear Log      ","Download Log   ","Set Time       "};
int star=0;
unsigned int speed = 0 ;
unsigned int index = 0 ;
unsigned char key;
int address=0;
int scrollcount=0;
int viewcount=0;
int readflag=0;
int event_flag = 0;
int neutral = 0;
int reverse = 0;
unsigned char read_ev[10][15];
int address_flag=0;
int i;
int blink_flag = 0;
int hour,min,sec;
unsigned int delay = 0;

void check_matrix_keypad(void)
{
   
    key = read_switches(STATE_CHANGE);

}
//view dashboard
void view_dashboard(void)
{
    //displaying details
    clcd_print("TIME     EV SP ", LINE1(0)); 
    check_matrix_keypad();
    clcd_print(time, LINE2(0)); 
    clcd_print(EV[index] , LINE2(9));
    clcd_putch('0' + (speed /10) , LINE2(12));
    clcd_putch('0' + (speed % 10), LINE2(13));
     speed = read_adc(CHANNEL4) / 10.33 ;
    //switch 1 for collision
    if(key == MK_SW1)
    {
        index = 8 ;
        neutral = 0;
        reverse = 0;
        if(event_flag==0)
        {
          event_store();
          event_flag = 1;
        }    
    }
     //switch 2 for incrementing gear
    else if(key == MK_SW2)
    {
        event_flag = 0;
        neutral = 0;
        if(index < 7)
        {
            index++ ;
        }
        if(index == 8)
        {
            index = 1 ;
        }
        if(reverse == 0 && index == 7)
        {
          event_store();
          reverse = 1;
        }
        if(reverse == 0)
        {
            event_store();
        }
    }
     //switch 3 for decrementing gear
    else if(key == MK_SW3)
    {
        event_flag = 0;
        reverse = 0;
        if(index > 1 && index < 8)
        {
            index-- ;
        }
        
        if(index == 8)
        {
            index = 1 ;
        }
        
        if(neutral == 0 && index == 1)
        {
          event_store();
          neutral = 1;
        }
        if(neutral == 0)
        {
            event_store();
        }
    }
     //switch 11 for main menu
    else if(key == MK_SW11)
    {
        state=e_main_menu;
         CLEAR_DISP_SCREEN;
        
    }
      
}

void event_store(void)
{
    
    //CLEAR_DISP_SCREEN;
    
    event_count++;
    int add1=0;
    int z;
    int add2 = 12;
    char read;
    //storing only 10 events
    if(event_count > 10)
    {
        event_count--;
        for(i=1;i<10;i++)
        {
            if(i==9)
            {
                address = add2;
            }
            for(z=0;z<12;z++)
            {
                read = read_external_EEPROM(add2++);
                write_external_EEPROM(add1++,read);
            }
        }
        
    }
    
    char array[13];
    
    int j=address;
    
    int i;
    //writing in eeprom
    for(i=0;i<8;i++)
    {
        write_external_EEPROM(address++,time[i]);
    }
    
    for(i=0;i<2;i++)
    {
        write_external_EEPROM(address++,EV[index][i]);
        
    }
    
    write_external_EEPROM(address++,'0' + speed/10);
    write_external_EEPROM(address++,'0'+ speed%10);
    
    //reading from eeprom and displaying
    for(i=0;i<12;i++)
    {
        
        array[i]=read_external_EEPROM(j);
        j++;
    }
    
    array[i]='\0';
    
    i=0,j=0;
    
    while(array[i]!='\0')
    {
        if(i==8 || i==10)
        {
            clcd_putch(' ',LINE2(j));
            j++;
  
        }
        
        clcd_putch(array[i],LINE2(j));
        i++;
        j++;
    }
        
}
//displaying main menu
void display_main_menu(void)
{
    //star in first line
    if(star == 0)
    { 
         clcd_putch('*',LINE1(0));
        
    }
    else
    {
        clcd_putch('*',LINE2(0));
        
    }
    clcd_print(opt[scrollcount],LINE1(1));
    clcd_print(opt[scrollcount+1],LINE2(1));
    
    check_matrix_keypad();

//scrolling up
if (key == MK_SW1 && (scrollcount > 0 || star == 1)) 
    {
        
        if (star== 1) 
        {
           star = 0;
        } 
        else if (star == 0) 
        {
            scrollcount--;
        }

        CLEAR_DISP_SCREEN;

    }
    //scrolling down
    else if (key == MK_SW2 && (scrollcount < 2 || star ==0))
    {
        if (star == 0)
        {
            star = 1;
        }
        else if (star == 1)
        {
            scrollcount++;
        }
        CLEAR_DISP_SCREEN;
    }
    //exiting
    else if(key == MK_SW12)
    {
        CLEAR_DISP_SCREEN;
        state = e_dashboard;
        return;
    }
    //selecting fields from main menu
    else if(key == MK_SW11)
    {
        CLEAR_DISP_SCREEN;
        
        if(scrollcount == 0 && star==0)
        { 
            state= e_view_log; 
        }  
        else if(scrollcount == 2 && star == 0)
        { 
            state=e_download_log;    
        }
        else if(scrollcount == 1 && star == 0)
        {
            state=e_clear_log;
        } 
        else if(scrollcount == 0 && star == 1)
        {
            state=e_clear_log;
        }
        else if(scrollcount == 1 && star == 1)
        { 
            state=e_download_log;
        }
        else if(scrollcount == 2 && star == 1)
        {
            state=e_set_time;
        }
        
        return;
    }
        
}

//view log
void view_log(void)
{
    check_matrix_keypad();
    int i;
  //no logs are present
    if(event_count==0)
    {
        clcd_print("NO LOGS PRESENT",LINE1(0));
        
        for(unsigned long int i=0;i<500000;i++);
        CLEAR_DISP_SCREEN;
        state=e_main_menu;
        return;
            
    }
    //displaying logs
    clcd_print("# TIME     EV SP", LINE1(0)); 
    
    if(key == MK_SW12)
    {
        CLEAR_DISP_SCREEN;
        state=e_main_menu;
        readflag=0;
        return;
    }
    else if(key == MK_SW1 && event_count > 1 && viewcount > -1)
    {
        CLEAR_DISP_SCREEN;
        viewcount--;
        
        if(viewcount == -1)
        {
            viewcount=0;
        }
    }
    else if(key == MK_SW2 && (event_count-1) > viewcount )
    {
        CLEAR_DISP_SCREEN;
        viewcount++;
    }
    
    if(readflag==0)
    {
        read_event();
        readflag++;
    }
    
    clcd_putch('0' + viewcount,LINE2(0));
    
    clcd_print(read_ev[viewcount], LINE2(2)); 
       
             
}

//read events
void read_event(void)
{
    
    int i,j;
    int address=0;
    
    for(i=0;i < event_count;i++)
    {
        for(j=0;j<15;j++)
        {
            if(j==8 || j==11)
            {
                read_ev[i][j]=' ';
            }
            else if(j==14)
            {
                read_ev[i][j]='\0';
            }
            else
            {
                read_ev[i][j]=read_external_EEPROM(address);
                address++;
                
            }
        }
    }
       
}
//clear logs
void clear_log(void)
{
    
    event_count=0;
    address=0;
    
    clcd_print("LOGS CLEARED",LINE1(0));

    for(unsigned long int i=0;i<500000;i++);
    CLEAR_DISP_SCREEN;
    state=e_main_menu;
    return;
    
}
//downloading logs
void download_log(void)
{
    
    read_event();
    
    char ch;
    //No logs are present
    if(event_count==0)
    {
        puts("\n\nNo logs are available\n"); 
    }
    else
    {
        puts("\n\n# TIME     EV SP\n\r"); 

        for(i=0;i < event_count;i++)
        {
            ch='0'+ i;
            putch(ch);
            puts(" ");
            puts(read_ev[i]);
            puts("\n\r");         
        } 
    }
    clcd_print("Downloading",LINE1(0));
    for(unsigned long int i=0;i<500000;i++);
    CLEAR_DISP_SCREEN;
    
     state= e_main_menu;
   return;
}
//set time
void set_time(void)
{
    //display time
    clcd_print("HH:MM:SS",LINE1(0));
    static int flag;
    if(flag ==0)
    {
        hour = ((time[0]-48)*10)+(time[1]-48);
        min = ((time[3]-48)*10)+(time[4]-48);
        sec = ((time[6]-48)*10)+(time[7]-48);
        flag++;

    }
    
    clcd_putch((hour/10)+48,LINE2(0));
    clcd_putch((hour%10)+48,LINE2(1));
    clcd_putch(':',LINE2(2));
    clcd_putch((min/10)+48,LINE2(3));
    clcd_putch((min%10)+48,LINE2(4));
    clcd_putch(':',LINE2(5));
    clcd_putch((sec/10)+48,LINE2(6));
    clcd_putch((sec%10)+48,LINE2(7));
    
    
    check_matrix_keypad();
    
    if(key==MK_SW12)
    {
        state = e_dashboard;
        return;
    }
    else if(key == MK_SW2)
    {
        blink_flag++; 
        
        if(blink_flag==3)
        {
            blink_flag = 0;
        }
        
        
    }
    //incrementing the value of fields
    else if(key==MK_SW1)
    {
        if(blink_flag==0)
        {
            hour++;
            if(hour>=24)
            {
                hour = 0;
            }
        }
        else if(blink_flag==1)
        {
            min++;
            if(min>=60)
            {
                min=0;
            }
        }
        else if(blink_flag==2)
        {
            sec++;
            if(sec>=60)
            {
                sec=0;
            }
        }
    }
    //write time to rtc
    else if(key==MK_SW11)
    {
        CLEAR_DISP_SCREEN;
        write_ds1307(HOUR_ADDR,(((hour/10)<<4)|(hour%10) & 0x07f));
        write_ds1307(MIN_ADDR,((min/10)<<4)|(min%10));
        write_ds1307(SEC_ADDR,((sec/10)<<4)|(sec%10));
        CLEAR_DISP_SCREEN;
        state = e_dashboard;
        return;
    }
    //blinking the change fields
    if(delay++>=400)
    {
        if(delay==800)
        {
            delay=0;
        }
        if(blink_flag==0)
        {
            clcd_putch(0XFF,LINE2(0));
            clcd_putch(0XFF,LINE2(1));
        }
        else if(blink_flag==1)
        {
            clcd_putch(0XFF,LINE2(3));
            clcd_putch(0XFF,LINE2(4));
        }
        else if(blink_flag==2)
        {
            clcd_putch(0XFF,LINE2(6));
            clcd_putch(0XFF,LINE2(7));
        }
     }
    
    
}

