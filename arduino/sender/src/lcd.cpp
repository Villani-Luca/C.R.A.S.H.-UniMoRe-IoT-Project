#include "lcd.h"

#if LCD_ENABLED
LiquidCrystal_I2C lc(LCD_ADDR, LCD_COLS, LCD_ROWS);
#endif

void lcd_setup(){
    #if LCD_ENABLED
    lc.init();
    lc.backlight();
    #endif
}

void lcd_toggle_display(){
    #if LCD_ENABLED
    lc.noDisplay();
    #endif
}

void lcd_toggle_cursor(){
    #if LCD_ENABLED
    lc.noCursor();
    #endif
}

void lcd_set_cursor(int col, int row){
    #if LCD_ENABLED
    lc.setCursor(col, row);
    #endif
}

void lcd_clear(){
    #if LCD_ENABLED
    lc.clear();
    #endif
}

void lcd_display_right_to_left(){
    #if LCD_ENABLED
    lc.rightToLeft();
    #endif
}

void lcd_display_left_to_right(){
    #if LCD_ENABLED
    lc.leftToRight();
    #endif
}

#pragma region PRINT

void lcd_print(arduino::Printable& data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(char data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(const char data[]){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(arduino::String data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(double data, int precision){
    #if LCD_ENABLED
    lc.print(data, precision);
    #endif
}

void lcd_print(unsigned long long data, int precision){
    #if LCD_ENABLED
    lc.print(data, precision);
    #endif
}

void lcd_print(long long data, int precision){
    #if LCD_ENABLED
    lc.print(data, precision);
    #endif
}

void lcd_print(unsigned long data, int precision){
    #if LCD_ENABLED
    lc.print(data, precision);
    #endif
}

void lcd_print(long data, int precision){
    #if LCD_ENABLED
    lc.print(data, precision);
    #endif
}

void lcd_print(unsigned int data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(int data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(unsigned char data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_print(const arduino::__FlashStringHelper * data){
    #if LCD_ENABLED
    lc.print(data);
    #endif
}

void lcd_println(){
    #if LCD_ENABLED
    lc.println();
    #endif
}

void lcd_println(arduino::Printable& data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(char data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(const char data[]){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(arduino::String data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(double data, int precision){
    #if LCD_ENABLED
    lc.println(data, precision);
    #endif
}

void lcd_println(unsigned long long data, int precision){
    #if LCD_ENABLED
    lc.println(data, precision);
    #endif
}

void lcd_println(long long data, int precision){
    #if LCD_ENABLED
    lc.println(data, precision);
    #endif
}

void lcd_println(unsigned long data, int precision){
    #if LCD_ENABLED
    lc.println(data, precision);
    #endif
}

void lcd_println(long data, int precision){
    #if LCD_ENABLED
    lc.println(data, precision);
    #endif
}

void lcd_println(unsigned int data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(int data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(unsigned char data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

void lcd_println(const arduino::__FlashStringHelper * data){
    #if LCD_ENABLED
    lc.println(data);
    #endif
}

#pragma endregion
