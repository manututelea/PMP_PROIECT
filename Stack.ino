#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <LedControl.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); 
LedControl lc = LedControl(12, 11, 10, 4); 

const int BTN_SCHIMBA = 8; 
const int BTN_START = 9;
const int BUZZER_PIN = 7; 

int level = 1;
const int totalNivele = 4;

int lastStateSchimba = HIGH; 
int lastStateStart = HIGH;

bool jocPornit = false; 
int viteza = 100;

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

void sunetPlasare() 
{
    digitalWrite(BUZZER_PIN, LOW); 
    delay(50); 
    digitalWrite(BUZZER_PIN, HIGH);
}

void sunetCastig() 
{
    for(int i=0; i<3; i++)
    {
        digitalWrite(BUZZER_PIN, LOW);  
        delay(100); 
        digitalWrite(BUZZER_PIN, HIGH); 
        delay(100); 
    }
    digitalWrite(BUZZER_PIN, LOW);  
    delay(500);
    digitalWrite(BUZZER_PIN, HIGH);
}

void joaca_cu_bucle() 
{
    lcd.clear();
    lcd.print("Joc Pornit!");
    unsigned long timpStart = millis();
    
    for(int i=0; i<4; i++) 
      lc.clearDisplay(i);

    for (int modul = 0; modul < 4; modul++) 
    {
        for (int rand = 7; rand >= 0; rand--) 
        {
            int blocPozitie = 0;
            int blocDirectie = 1;
            int blocLatime = 4; 
            bool blocPlasat = false; 
            unsigned long timpMiscare = 0;

            while (blocPlasat == false) 
            {   
                if (millis() - timpMiscare > viteza) 
                {
                    timpMiscare = millis();
                    for(int i=0; i<blocLatime; i++) 
                    {
                         if(blocPozitie + i < 8) 
                            lc.setLed(modul, blocPozitie + i, rand, false);
                    }

                    blocPozitie += blocDirectie;
                    if (blocPozitie >= (8 - blocLatime) || blocPozitie <= 0) 
                    {
                        blocDirectie *= -1;
                    }

                    for(int i=0; i<blocLatime; i++) 
                    {
                        if(blocPozitie + i < 8)
                            lc.setLed(modul, blocPozitie + i, rand, true);
                    }
                }
                
                int stareBtn = digitalRead(BTN_START);
                if (stareBtn == LOW) // trecem mai sus
                {
                    blocPlasat = true; 
                    sunetPlasare(); 
                    delay(300); 
                }
            } 
        } 
    } 

    unsigned long timpFinal = millis();
    float secundeJucate = (timpFinal - timpStart) / 1000.0;

    lcd.clear();
    lcd.print("CASTIGATOR!");
    lcd.setCursor(0, 1);
    lcd.print("Timp: ");
    lcd.print(secundeJucate);
    lcd.print("s");
    
    sunetCastig(); 
    
    delay(4000);
    jocPornit = false; 
    select_level();
}

void setup() 
{
    lcd.init();        
    lcd.backlight();    
    for(int i=0; i<4; i++) 
    {
        lc.shutdown(i, false); 
        lc.setIntensity(i, 8); 
        lc.clearDisplay(i); 
    }
    
    pinMode(BTN_SCHIMBA, INPUT_PULLUP); 
    pinMode(BTN_START, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, HIGH); //initializam high, canta pe low
    select_level();      
}

void loop() 
{
    if (jocPornit == false) 
    {
        int stareSchimba = digitalRead(BTN_SCHIMBA);
        int stareStart = digitalRead(BTN_START);

        if (stareSchimba == LOW && lastStateSchimba == HIGH) 
        {
            level++;
            if (level > totalNivele) level = 1;
            select_level(); 
            delay(200);
        }
        lastStateSchimba = stareSchimba;

        if (stareStart == LOW && lastStateStart == HIGH) 
        {
            jocPornit = true;
        }
        lastStateStart = stareStart;
    } 
    else 
    {
        joaca_cu_bucle();
    }
}