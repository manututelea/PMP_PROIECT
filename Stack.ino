#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int BTN_SCHIMBA = 8; 
const int BTN_START = 9;

int level = 1;
const int totalNivele = 4;

int lastStateSchimba = HIGH; 
int lastStateStart = HIGH;

void select_level() 
{
    int cursorPositions[] = {0, 3, 6, 10}; 
    int currentPosition = cursorPositions[level - 1]; 

    lcd.setCursor(0, 0); 
    lcd.print("                "); 
    lcd.setCursor(0, 1);
    lcd.print("1  2  3  inf");
    lcd.setCursor(currentPosition, 0); 
    lcd.print(">"); 
}

void porneste_jocul() 
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("START JOC!");
    
    lcd.setCursor(0, 1);
    if (level == 4) 
    {
        lcd.print("Mod: Infinit");
    } 
    else 
    {
        lcd.print("Nivel: ");
        lcd.print(level);
    }
    
    delay(2000); 
    select_level(); 
}

void setup() 
{
    lcd.init();        
    lcd.backlight();    
    pinMode(BTN_SCHIMBA, INPUT_PULLUP); 
    pinMode(BTN_START, INPUT_PULLUP);
    select_level();     
}

void loop() 
{
    int stareSchimba = digitalRead(BTN_SCHIMBA);
    int stareStart = digitalRead(BTN_START);

    if (stareSchimba == LOW && lastStateSchimba == HIGH) 
    {
        level++;
        if (level > totalNivele) 
        {
            level = 1;
        }
        select_level(); 
        delay(200);
    }
    lastStateSchimba = stareSchimba;

    if (stareStart == LOW && lastStateStart == HIGH) 
    {
        porneste_jocul();
    }
    lastStateStart = stareStart;
}