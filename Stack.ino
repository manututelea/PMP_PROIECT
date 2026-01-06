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

void sunetGameOver() 
{
    digitalWrite(BUZZER_PIN, LOW); 
    delay(1000);
    digitalWrite(BUZZER_PIN, HIGH);
}

void joaca_cu_bucle() 
{
    lcd.clear();
    lcd.print("Joc Pornit!");
    unsigned long timpStart = millis();
    
    for(int i=0; i<4; i++) //stergem tot de pe matricea de led uri
      lc.clearDisplay(i);

    int prevBlocPozitie = 0; 
    int prevBlocLatime = 4;  
    bool gameOver = false;   
    
    long scor = 0; 
    int cicluInfinit = 0; 

    while (gameOver == false) 
    {
        for (int modul = 0; modul < 4; modul++) //trecem prin cele 4 module de cate 8 randuri
        {
            if (gameOver == true) 
                break;

            for (int rand = 7; rand >= 0; rand--) //trecem pe randuri (sunt numerotate de la 7 jos si 0 sus)
            {
                if (gameOver) 
                    break;
                if (cicluInfinit > 0 && modul == 0 && rand == 7) //daca am terminat un ciclu pana sus resetam pe primul nivel cu ceea ce a ramas initial
                {
                    for(int i = 0; i < prevBlocLatime; i++) 
                        lc.setLed(modul, prevBlocPozitie + i, rand, true);
                    continue; 
                }

                int blocPozitie = 0;
                int blocDirectie = 1;
                int blocLatime = prevBlocLatime; 
                bool blocPlasat = false; 
                unsigned long timpMiscare = 0;

                while (blocPlasat == false) //logica pentru plasarea blocurilor
                {   
                    if (millis() - timpMiscare > viteza) //cu cat viteza e mai mare cu atat plasam mai incet blocurile
                    {
                        timpMiscare = millis();
                        for(int i=0; i<blocLatime; i++) 
                        {
                             if(blocPozitie + i < 8) 
                                lc.setLed(modul, blocPozitie + i, rand, false);//stergem punctele trecute
                        }

                        blocPozitie += blocDirectie;
                        if (blocPozitie >= (8 - blocLatime) || blocPozitie <= 0) //verificam in ce parte mergem
                        {
                            blocDirectie *= -1;
                        }

                        for(int i=0; i<blocLatime; i++) 
                        {
                            if(blocPozitie + i < 8)
                                lc.setLed(modul, blocPozitie + i, rand, true);//aprindem punctele actuale
                        }
                    }
                    
                    int stareBtn = digitalRead(BTN_START);
                    if (stareBtn == LOW) 
                    {
                        blocPlasat = true; 
                        sunetPlasare(); 
                        delay(300); 
                    }
                } 

                if (!(modul == 0 && rand == 7 && cicluInfinit == 0)) // daca nu suntem la baza verificam blocurile puse
                {
                    int curentStart = blocPozitie;
                    int curentEnd = blocPozitie + blocLatime;
                    int prevStart = prevBlocPozitie;
                    int prevEnd = prevBlocPozitie + prevBlocLatime;

                    int suprapunereStart = max(curentStart, prevStart); // verificam daca se suprapun blocurile
                    int suprapunereEnd = min(curentEnd, prevEnd);
                    int suprapunereLatime = suprapunereEnd - suprapunereStart;

                    if (suprapunereLatime <= 0) //daca nu avem suprapuse pierdem
                    {
                        gameOver = true;
                        lcd.clear();
                        lcd.print("GAME OVER!");
                        lcd.setCursor(0, 1);
                        lcd.print("Score: ");
                        lcd.print(scor);
                        sunetGameOver();
                        delay(4000);
                    }
                        else //daca nu continuam jocul si plasam blocurile in continuare
                        {
                            for(int i=0; i<8; i++) 
                                lc.setLed(modul, i, rand, false);//stergem blocurile vechi
                            for(int i=0; i<suprapunereLatime; i++) 
                            {
                                lc.setLed(modul, suprapunereStart + i, rand, true);//le punem pe cele ramase
                            }
                            prevBlocLatime = suprapunereLatime;
                            prevBlocPozitie = suprapunereStart;
                            scor += suprapunereLatime; 
                        }
                }
                else 
                {
                    prevBlocLatime = blocLatime;
                    prevBlocPozitie = blocPozitie;
                    scor += prevBlocLatime; 
                }
            } 
        } 

        if (gameOver) 
            break;

        if (level == 4) //daca suntem la nivelul infinit crestem viteza si reinitializam matricea de led uri in cazul in care am ajuns la finalul ei
        {
            cicluInfinit++;
            if (viteza >= 50)
                viteza = viteza - 10;
            for(int i = 0; i < 4; i++) lc.clearDisplay(i);
        }
        else //daca nu terminam jocul si afisam pe lcd
        {
            unsigned long timpFinal = millis();
            float secundeJucate = (timpFinal - timpStart) / 1000.0;
            lcd.clear();
            lcd.print("WIN! Score:");
            lcd.print(scor);
            lcd.setCursor(0, 1);
            lcd.print("Timp: "); lcd.print(secundeJucate); lcd.print("s");
            sunetCastig(); 
            delay(4000);
            break; 
        }
    } 

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
    digitalWrite(BUZZER_PIN, HIGH); 
    
    select_level();      
}





void loop() 
{
    if (jocPornit == false) //daca nu e pornit jocul avem logica pentru alegerea nivelului
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

        if (stareStart == LOW && lastStateStart == HIGH) //incepem jocul
        {
            if(level == 1) viteza = 200;
            if(level == 2) viteza = 70;
            if(level == 3) viteza = 40;
            if(level == 4) viteza = 200;

            jocPornit = true;
        }
        lastStateStart = stareStart;
    } 
    else //incepem jocul 
    {
        joaca_cu_bucle();
    }
}