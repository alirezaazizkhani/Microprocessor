#include <mega32.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
// Define the row and column pins
#define ROW_PORT PORTD
#define ROW_DDR  DDRD
#define ROW_PIN  PIND
#define COL_PORT PORTD
#define COL_DDR  DDRD
#define COL_PIN  PIND


int row, col;
unsigned char First_time = 1;
char correct_password[] = "1234";
char entered_password[5];  
int index = 0; 
unsigned char temp;
int logOut = 0;
int i = 0;  
char key;
unsigned char buffer[16];
char keys[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

void keypad_init(); 
char keypad_scan(); 
int check_password();
void reset_password();
void ADC_Air();

void main(void) {
    keypad_init();  
    lcd_init(16);
    
    ADCSRA = 0x83;
    DDRA = 0x00;
    DDRB = 0xff;

    while (1) {
        ADC_Air();

        logOut = 0;
        if (First_time == 1){
            lcd_puts("Welcome to the smart house");
            First_time = 0;
        }

        key = keypad_scan();
        if (key == '*') {
            lcd_clear();
            lcd_puts("Enter pass :");

            while( logOut != 1){
                ADC_Air();

                key = keypad_scan();
                if (key){
                    if (key == '#'){
                        if (check_password()) {
                            lcd_clear();
                            lcd_puts("Password is correct!!");
                            delay_ms(100);

                            lcd_clear();
                            lcd_puts("1-LogOut  2-TV");
                            lcd_gotoxy(0,1);
                            lcd_puts("3-Lights  4-Doors");
                            
                            while(logOut != 1){
                                ADC_Air();

                                key = keypad_scan();
                                if (key)
                                {
                                    switch (key)
                                    {
                                        case '1':
                                            lcd_clear();
                                            PORTB &= 0b00000001;
                                            lcd_puts("Goodbye");
                                            delay_ms(200);
                                            lcd_clear();
                                            lcd_puts("Welcome to the smart house");
                                            delay_ms(5);
                                            reset_password(); 
                                            logOut = 1;                                            
                                            break;
                                        case '2':
                                            PINB.1 ^= (1 << 1);
                                            break;
                                        case '3':
                                            //lights
                                            lcd_clear();
                                            lcd_puts("1-room1  2-room2");
                                            lcd_gotoxy(0,1);
                                            lcd_puts("3-main  4-back");

                                            while(logOut != 1){
                                                ADC_Air();
                                                key = keypad_scan();
                                                if (key){
                                                        switch (key)
                                                        {
                                                        case '1':
                                                            PINB.2 ^= (1 << 2);
                                                            break;
                                                        case '2':
                                                            PINB.3 ^= (1 << 3);
                                                            break;
                                                        case '3':
                                                            PINB.4 ^= (1 << 4);
                                                            PINB.5 ^= (1 << 5);
                                                            break;
                                                        case '4':
                                                            logOut = 1;
                                                            break;
                                                        default:
                                                            lcd_clear();
                                                            lcd_puts("Enter valid value!!");
                                                            delay_ms(300);
                                                            break;
                                                        }
                                                    }
                                                }
                                            logOut = 0;
                                            lcd_clear();
                                            lcd_puts("1-LogOut  2-TV");
                                            lcd_gotoxy(0,1);
                                            lcd_puts("3-Lights  4-Doors");
                                            break;
                                        case '4':
                                            PINB.6 ^= (1 << 6);
                                            break;
                                        default:
                                            lcd_clear();
                                            lcd_puts("Enter valid value!!");
                                            delay_ms(300);
                                            lcd_clear();
                                            lcd_puts("1-LogOut  2-TV");
                                            lcd_gotoxy(0,1);
                                            lcd_puts("3-Lights  4-Doors");
                                            break;
                                    }
                                }
                            }

                        } else {
                            lcd_clear();
                            lcd_puts("Password Incorrect!!");
                        }
                    }else if (key == '*')
                    {  // reset 
                        reset_password(); 
                        lcd_clear();
                        lcd_puts("Enter pass :");
                    }else{  
                        lcd_gotoxy(i,1);
                        sprintf(buffer, "%c", key);
                        lcd_puts(buffer);
                        if (index < 4) {  
                            entered_password[index] = key;
                            index++;
                            entered_password[index] = '\0';  
                        }
                        i++;
                    }
                }
            }
        }
    }
}
void keypad_init(){
    ROW_DDR = 0b00001111; 
    ROW_PORT = 0xFF; 
}
char keypad_scan(){
    for (row = 0; row < 4; row++) {
        ROW_PORT = ~(1 << row);  
        delay_us(5);            
        for (col = 0; col < 3; col++) {
            if (!(COL_PIN & (1 << (col + 4)))) { 
                while (!(COL_PIN & (1 << (col + 4)))); 
                return keys[row][col]; 
            }
        }
    }
    return 0; 
}
int check_password(){
    if (strcmp(entered_password, correct_password) == 0) {
        return 1;  
    } else {
        return 0;  
    }
}
void reset_password(){
    index = 0;
    i=0;
    entered_password[0] = '\0';  
}

void ADC_Air(){
    ADMUX = 0x40 ;
    delay_us(10);
    ADCSRA |= (1<<6);
    while((ADCSRA & (1<<4))==0);
    ADCSRA = 0x83;
    delay_us(10);
    temp = ADCW; 
    temp = temp/2;
    if (temp > 30){
        PORTB |= 0b00000001;
    }else {
        PORTB &= 0b11111110; 
    }
}
