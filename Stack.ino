#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int BUTTON_PIN = 8;

int level = 1;
const int totalNivele = 4;
int lastButtonState = LOW;

void select_level() {
    int cursorPositions[] = {0, 3, 6, 10}; // pozitiile unde pune caracterul pentru a alege nivelul
    int currentPosition = cursorPositions[level - 1]; // incepe cu cursorul de la lvl 1

    lcd.setCursor(0, 0); 
    lcd.print("                "); 

    lcd.setCursor(0, 1);
    lcd.print("1  2  3  inf");
    
    lcd.setCursor(currentPosition, 0); 
    lcd.print(">"); 
    
}


void setup() {
    lcd.begin(16, 2);
    pinMode(BUTTON_PIN, INPUT); 
    select_level();
}

void loop() {
    int currentButtonState = digitalRead(BUTTON_PIN);

    if (currentButtonState == HIGH && lastButtonState == LOW) {
        level++;
        if (level > totalNivele) {
            level = 1;
        }
        select_level();
        delay(100); 
    }
    lastButtonState = currentButtonState;
}