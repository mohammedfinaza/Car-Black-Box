#include "black_box.h"
#include "ds1307.h"

int event_count;
//unsigned char time[9] = "00:00:00";
extern unsigned char time[9];
unsigned char EV[9][3] = {"ON", "GN", "G1", "G2", "G3", "G4", "G5", "GR", "C_"};
unsigned char opt[4][16] = {"View Log       ", "Clear Log      ", "Download Log   ", "Set Time       "};
int starflag = 0;
unsigned int speed = 0;
unsigned int index = 0;
unsigned char key;
int address = 0;
int scrollcount = 0;
int viewcount = 0;
int readflag = 0;
unsigned char read_ev[10][15];
int colflag = 0;
int revflag = 0;
int neuflag = 0;
int min, hr, sec;

void check_matrix_keypad(void) 
{

    key = read_switches(STATE_CHANGE);

}

//function for viewing the dashboard in clcd

void view_dashboard(void) 
{
    clcd_print("TIME     EV SP ", LINE1(0));
    check_matrix_keypad();
    clcd_print(time, LINE2(0));
    clcd_print(EV[index], LINE2(9));
    clcd_putch('0' + (speed / 10), LINE2(12));
    clcd_putch('0' + (speed % 10), LINE2(13));
    speed = read_adc(CHANNEL4) / 10.33;

    if (key == MK_SW1)//collision
    {
        index = 8;

        if (colflag == 0)
        {
            event_store();
            colflag = 1;

        }
        revflag = 0;
        neuflag = 0;

    } else if (key == MK_SW2)//gear incrementing
    {
        if (index < 7)
        {
            index++;
        }
        if (index == 8)
        {
            index = 1;
        }

        if (index == 7 && revflag == 0) 
        {
            event_store();
            revflag = 1;

        }

        if (revflag == 0)
        {
            event_store();

        }

        colflag = 0;
        neuflag = 0;

    } else if (key == MK_SW3)//gear decrementing
    {
        if (index > 1 && index < 8) 
        {
            index--;
        }

        if (index == 8) 
        {
            index = 1;
        }

        if (index == 1 && neuflag == 0) 
        {
            event_store();
            neuflag = 1;

        }

        if (neuflag == 0)
        {
            event_store();
        }


        colflag = 0;
        revflag = 0;

    } else if (key == MK_SW11)//main menu
    {
        state = e_main_menu;
        CLEAR_DISP_SCREEN;

    }

}


//storing events

void event_store(void) 
{
   // CLEAR_DISP_SCREEN;

    char array[13];
    int i, j, k;

    event_count++;

    if (event_count > 10) 
    {


        event_count--;

        char ch;

        int add1 = 0;
        int add2 = 12;

        for (i = 1; i < 10; i++) 
        {
            if (i == 9) 
            {
                address = add2;
            }

            for (k = 0; k < 12; k++)
            {
                ch = read_external_EEPROM(add2++);
                write_external_EEPROM(add1++, ch);

            }
        }
    }

    j = address;

    for (i = 0; i < 8; i++)
    {
        write_external_EEPROM(address++, time[i]);
    }

    for (i = 0; i < 2; i++)
    {
        write_external_EEPROM(address++, EV[index][i]);

    }

    write_external_EEPROM(address++, '0' + speed / 10);
    write_external_EEPROM(address++, '0' + speed % 10);


    for (i = 0; i < 12; i++) {

        array[i] = read_external_EEPROM(j);
        j++;
    }

    array[i] = '\0';

    i = 0, j = 0;

    while (array[i] != '\0') {
        if (i == 8 || i == 10) {
            clcd_putch(' ', LINE2(j));
            j++;

        }

        clcd_putch(array[i], LINE2(j));
        i++;
        j++;
    }

}

//displaying the main menu

void display_main_menu(void) 
{
    if (starflag == 0)
    {
        clcd_putch('*', LINE1(0));

    } else 
    {
        clcd_putch('*', LINE2(0));

    }
    clcd_print(opt[scrollcount], LINE1(1));
    clcd_print(opt[scrollcount + 1], LINE2(1));

    check_matrix_keypad();


    if (key == MK_SW1 && (scrollcount > 0 || starflag == 1)) 
    {
        
        if (starflag == 1) 
        {
           starflag = 0;
        } else if (starflag == 0) 
        {
            scrollcount--;
        }

        CLEAR_DISP_SCREEN;

    } 
    else if (key == MK_SW2 && (scrollcount < 2 || starflag ==0))
    {
        if (starflag == 0)
            starflag = 1;
        else if (starflag == 1)
            scrollcount++;
        CLEAR_DISP_SCREEN;
    }
    else if (key == MK_SW12)
    {
        CLEAR_DISP_SCREEN;
        scrollcount = 0;
        starflag = 0;
        state = e_dashboard;
        return;
    } 
    else if (key == MK_SW11)
    {
        CLEAR_DISP_SCREEN;

        if (scrollcount == 0 && starflag == 0) 
        {
            state = e_view_log;
        }
        else if (scrollcount == 0 && starflag == 1) 
        {
            state = e_clear_log;
        }
        else if (scrollcount == 1 && starflag == 1) 
        {
            state = e_download_log;
        } 
        else if (scrollcount == 2 && starflag == 1)
        {
            state = e_set_time;
        } 
        else if (scrollcount == 2 && starflag == 0) 
        {
            state = e_download_log;
        } 
        else if (scrollcount == 1 && starflag == 0) 
        {
            state = e_clear_log;
        }
        
        scrollcount = 0;
        starflag = 0;
        
        return;
    }

}

//viewing the event log in clcd

void view_log(void) 
{
    check_matrix_keypad();
    int i;

    if (event_count == 0) 
    {
        clcd_print("NO LOGS PRESENT", LINE1(0));

        for (unsigned long int i = 0; i < 500000; i++);
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
        return;

    }

    clcd_print("# TIME     EV SP", LINE1(0));

    if (key == MK_SW12)
    {
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
        readflag = 0;
        return;
    } 
    else if (key == MK_SW1 && event_count > 1 && viewcount > -1) 
    {
        CLEAR_DISP_SCREEN;
        viewcount--;

        if (viewcount == -1) {
            viewcount = 0;
        }
    } 
    else if (key == MK_SW2 && (event_count - 1) > viewcount) 
    {
        CLEAR_DISP_SCREEN;
        viewcount++;
    }

    if (readflag == 0)
    {
        read_event();
        readflag++;
    }

    clcd_putch('0' + viewcount, LINE2(0));

    clcd_print(read_ev[viewcount], LINE2(2));


}

//reading the event from EEPROM and storing to a 2d array

void read_event(void) {

    int i, j;
    int address = 0;

    for (i = 0; i < event_count; i++)
    {
        for (j = 0; j < 15; j++) 
        {
            if (j == 8 || j == 11)
            {
                read_ev[i][j] = ' ';
            } 
            else if (j == 14) 
            {
                read_ev[i][j] = '\0';
            } else 
            {
                read_ev[i][j] = read_external_EEPROM(address);
                address++;

            }
        }
    }

}

//clearing the event log

void clear_log(void) 
{

    event_count = 0;
    address = 0;

    clcd_print("LOGS CLEARED", LINE1(0));

    for (unsigned long int i = 0; i < 500000; i++);
    CLEAR_DISP_SCREEN;
    state = e_main_menu;
    return;

}

//download the event to external device

void download_log(void) 
{
    read_event();

    int i;
    char ch;

    if (event_count == 0) 
    {
        puts("\n\nNO LOGS ARE PRESENT\r\n\n");

    } else 
    {
        puts("\n\n# TIME     EV SP\r\n\n");

        for (i = 0; i < event_count; i++) 
        {
            ch = '0' + i;
            putch(ch);
            puts(" ");
            puts(read_ev[i]);
            puts("\r\n");
        }

    }

    clcd_print("DOWNLOADING", LINE1(0));
    for (unsigned long int i = 0; i < 90000; i++);
    CLEAR_DISP_SCREEN;

    state = e_main_menu;

    return;
}

void set_time(void) 
{
    check_matrix_keypad();

    static int delay = 0,blink=0, flag=0;

    if (flag == 0) 
    {
        flag = 1;
        hr = ((time[0] - 48)*10)+(time[1] - 48);
        min = ((time[3] - 48)*10)+(time[4] - 48);
        sec = ((time[6] - 48)*10)+(time[7] - 48);
    }
    
    clcd_print("HH:MM:SS", LINE1(1));

    if (key == MK_SW1)
    {
        if(blink == 0)
        {
            hr++;
            if(hr >= 24)
                hr=0;
        }
        else if(blink == 1)
        {
            min++;
            if(min >= 60)
                min=0;
        }
        else if(blink == 2)
        {
            sec++;
            if(sec >= 60)
                sec=0;
        }

    } else if (key == MK_SW2)
    {
        blink++;
        if(blink > 2)
            blink=0;
     
    } else if (key == MK_SW12) 
    {
         CLEAR_DISP_SCREEN;
        state = e_dashboard;
        blink=0;
        flag=0;
        return;
    }
    else if( key == MK_SW11)
    {
        write_ds1307(HOUR_ADDR,(((hr/10)<<4)) | (hr%10));
        write_ds1307(MIN_ADDR,(((min/10)<<4)) | (min%10));
        write_ds1307(SEC_ADDR,(((sec/10)<<4)) | (sec%10));
        state = e_dashboard;
        CLEAR_DISP_SCREEN;
        blink=0;
        flag=0;
        return;
        
    }

    

    if (delay++ >= 400)
    {
       if (delay == 800)
          delay = 0;

        if (blink == 0) 
        {
           clcd_putch(0xFF, LINE2(1));
           clcd_putch(0xFF, LINE2(2));
           clcd_putch(':', LINE2(3));
           clcd_putch((min / 10) + 48, LINE2(4));
           clcd_putch((min % 10) + 48, LINE2(5));
           clcd_putch(':', LINE2(6));
           clcd_putch((sec / 10) + 48, LINE2(7));
           clcd_putch((sec % 10) + 48, LINE2(8));

        } else if (blink == 1) 
        {  
            clcd_putch((hr / 10) + 48, LINE2(1));
            clcd_putch((hr % 10) + 48, LINE2(2));
            clcd_putch(':', LINE2(3));
            clcd_putch(0xFF, LINE2(4));
            clcd_putch(0xFF, LINE2(5));
            clcd_putch(':', LINE2(6));
            clcd_putch((sec / 10) + 48, LINE2(7));
            clcd_putch((sec % 10) + 48, LINE2(8));

       } else if (blink == 2) 
       {
            clcd_putch((hr / 10) + 48, LINE2(1));
            clcd_putch((hr % 10) + 48, LINE2(2));
            clcd_putch(':', LINE2(3));
            clcd_putch((min / 10) + 48, LINE2(4));
            clcd_putch((min % 10) + 48, LINE2(5));
            clcd_putch(':', LINE2(6));
            clcd_putch(0xFF, LINE2(7));
            clcd_putch(0xFF, LINE2(8));
         
       }
    }
    else
    {
        clcd_putch((hr / 10) + 48, LINE2(1));
        clcd_putch((hr % 10) + 48, LINE2(2));
        clcd_putch(':', LINE2(3));
        clcd_putch((min / 10) + 48, LINE2(4));
        clcd_putch((min % 10) + 48, LINE2(5));
        clcd_putch(':', LINE2(6));
        clcd_putch((sec / 10) + 48, LINE2(7));
        clcd_putch((sec % 10) + 48, LINE2(8));
    }


}