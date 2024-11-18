#pragma once

#include <LiquidCrystal.h>

#include "modules.h"

#define LCD_RS_PIN 12
#define LCD_EN_PIN 11
#define LCD_D4_PIN 5
#define LCD_D5_PIN 4
#define LCD_D6_PIN 3
#define LCD_D7_PIN 7

void lcd_setup(int cols = 16, int rows = 2);

void lcd_toggle_display();
void lcd_toggle_cursor();

void lcd_set_cursor(int col = 0, int row = 0);

void lcd_clear();

void lcd_display_right_to_left();
void lcd_display_left_to_right();

void lcd_print(arduino::Printable& data);
void lcd_print(char data);
void lcd_print(const char data[]);
void lcd_print(arduino::String data);
void lcd_print(double data, int precision = 2);
void lcd_print(unsigned long long data, int precision);
void lcd_print(long long data, int precision);
void lcd_print(unsigned long data, int precision);
void lcd_print(long data, int precision);
void lcd_print(unsigned int data);
void lcd_print(int data);
void lcd_print(unsigned char data);
void lcd_print(const arduino::__FlashStringHelper * data);

void lcd_println();
void lcd_println(arduino::Printable& data);
void lcd_println(char data);
void lcd_println(const char data[]);
void lcd_println(arduino::String data);
void lcd_println(double data, int precision = 2);
void lcd_println(unsigned long long data, int precision);
void lcd_println(long long data, int precision);
void lcd_println(unsigned long data, int precision);
void lcd_println(long data, int precision);
void lcd_println(unsigned int data);
void lcd_println(int data);
void lcd_println(unsigned char data);
void lcd_println(const arduino::__FlashStringHelper * data);
