#include <mega32.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>

#define ROW_PORT PORTD
#define ROW_DDR  DDRD
#define ROW_PIN  PIND
#define COL_PORT PORTD
#define COL_DDR  DDRD
#define COL_PIN  PIND

int row, col;
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
void LCD();
void Lights();
void LCD_menu();
void Lights_menu();
void On_off_ret(int status, int And, int or);

void main(void) {
    keypad_init();  
    lcd_init(16);
    
    ADCSRA = 0x83;
    DDRA = 0x00;
    DDRB = 0xff;

    lcd_puts("Welcome to the smart house");
    while (1) {
        logOut = 0;
        ADC_Air();
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
                            LCD_menu();
                            while(logOut != 1){
                                LCD();
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
void LCD(){
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
                On_off_ret( 0, 0b11111101, 0b00000010);
                LCD_menu();
                break;
            case '3':
                //lights
                Lights_menu();
                while(logOut != 1){
                    Lights();
                    }
                logOut = 0;
                LCD_menu();
                break;
            case '4':
                On_off_ret( 4, 0b10111111, 0b01000000);
                LCD_menu();
                break;
            default:
                lcd_clear();
                lcd_puts("Enter valid button!!");
                delay_ms(100);
                LCD_menu();
                break;
        }
    }
}

void Lights(){
    ADC_Air();
    key = keypad_scan();
    if (key){
            switch (key)
            {
            case '1':
                On_off_ret( 1, 0b11111011, 0b00000100);
                Lights_menu();
                break;
            case '2':
                On_off_ret( 2, 0b11110111, 0b00001000);
                Lights_menu();
                break;
            case '3':
                On_off_ret( 3, 0b11001111, 0b00110000);
                Lights_menu();
                break;
            case '4':
                logOut = 1;
                break;
            default:
                lcd_clear();
                lcd_puts("Enter valid button!!");
                delay_ms(100);
                Lights_menu();
                break;
            }
        }
}
void LCD_menu(){
    lcd_clear();
    lcd_puts("1-LogOut  2-TV");
    lcd_gotoxy(0,1);
    lcd_puts("3-Lights  4-Doors");
}
void Lights_menu(){
    lcd_clear();
    lcd_puts("1-room1  2-room2");
    lcd_gotoxy(0,1);
    lcd_puts("3-main  4-back");
}

void On_off_ret(int status, int And, int or){
    lcd_clear();
    if (status==0){
        lcd_puts("TV:");
    }else if(status==1){
        lcd_puts("Room one light");
    }else if (status==2){
        lcd_puts("Room 2 light");
    }else if (status==3){
        lcd_puts("Main room lights");
    }else if (status==4){
        lcd_puts("Doors lock:");
    }
    lcd_gotoxy(0, 1);
    lcd_puts("1-on 2-off 3-ret");
    while(logOut != 1){
        ADC_Air();
        key = keypad_scan();
        if (key){
            switch (key){
                case '1':
                    PORTB |= or;
                    break;
                case '2':
                    PORTB &= And;
                    break;
                case '3':
                    logOut = 1; 
                    break;
                default:
                    lcd_clear();
                    lcd_puts("Enter valid button!!");
                    delay_ms(100);
                    lcd_clear();
                    if (status==0){
                        lcd_puts("TV:");
                    }else if(status==1){
                        lcd_puts("Room one light");
                    }else if (status==2){
                        lcd_puts("Room 2 light");
                    }else if (status==3){
                        lcd_puts("Main room lights");
                    }else if (status==4){
                        lcd_puts("Doors:");
                    }
                    lcd_gotoxy(0, 1);
                    lcd_puts("1-on 2-off 3-ret");
            }
        }
    }
    logOut = 0; 
}