/*
     CÓDIGO:  	D0074
     AUTOR:   	BrincandoComIdeias
     ACOMPANHE: https://www.youtube.com/brincandocomideias ; https://www.instagram.com/canalbrincandocomideias/
     APRENDA:	  https://cursodearduino.net/ ; https://cursoderobotica.net
     COMPRE:  	https://www.arducore.com.br/
     SKETCH:    Menu com Rotary Encoder
     DATA:    	25/08/2020
*/

// INCLUSÃO DE BIBLIOTECAS
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

// DEFINIÇÕES DE PINOS
#define pinSW 5 // BOTAO DO ENCODER
#define pinDT 6
#define pinCLK 7

#define pinR 10 // PINO LED VERMELHO
#define pinG 11 // PINO LED VERDE
#define pinB 12 // PINO LED AZUL

// DEFINIÇÕES
#define menuMin 0 // VALOR MÍNIMO DE NAVEGAÇÃO DO MENU
#define menuMax 4 // VALOR MÁXIMO DE NAVEGAÇÃO DO MENU

// INSTANCIANDO OBJETOS
LiquidCrystal_I2C lcd(0x27, 16, 2);
RotaryEncoder encoder(pinDT, pinCLK);

// DECLARAÇÃO DE FUNÇÕES
bool botaoApertado(byte pin_sw, int tempoDebounce = 100);
void configuraInterrupcao();
void atualizaDisplay();

// DECLARAÇÃO DE VARIÁVEIS
int menu = 0;
long posicao = 0, posicaoAnt = 0;

unsigned long tempoDisplay;
bool girouEncoder = true;

byte Leds[3] = {pinR, pinG, pinB}; // ARRAY DAS PORTAS DOS LEDS

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < 3; i++) {
    pinMode(Leds[i], OUTPUT);
    digitalWrite(Leds[i], LOW);
  }

  Serial.println("Iniciando Display");
  
  lcd.init(); // INICIA O DISPLAY (CONFERIR VERSÃO DA BIBLIOTECA)
  lcd.backlight();

  lcd.print("Brincando ");
  lcd.setCursor(0, 1);
  lcd.print("      com Ideias");
  delay(2500);

  lcd.clear();
  lcd.noBacklight();

  lcd.print("| Cor do LED: |");
  lcd.setCursor(0, 1);

  configuraInterrupcao();

  posicaoAnt = encoder.getPosition(); //SALVA A POSIÇÃO ATUAL DO ENCODER PARA COMPARAR
  Serial.println("Fim Setup");
}

void loop() {
  posicao = encoder.getPosition();
  menu = constrain( (menu + (posicao - posicaoAnt) ), menuMin, menuMax);

  if ( posicao != posicaoAnt) {
    girouEncoder = true;
  }

  atualizaDisplay();

  switch (menu) {
    case 1: {
        if (girouEncoder) {
          lcd.setCursor(0, 1);
          lcd.print("1: Vermelho     ");
        }

        digitalWrite(pinR , HIGH);
        digitalWrite(pinG , LOW);
        digitalWrite(pinB , LOW);
        girouEncoder = false;

        break;
      }
    case 2: {
        if (girouEncoder) {
          lcd.setCursor(0, 1);
          lcd.print("2: Verde        ");
        }

        digitalWrite(pinR , LOW);
        digitalWrite(pinG , HIGH);
        digitalWrite(pinB , LOW);
        girouEncoder = false;
        
        break;
      }
    case 3: {
        if (girouEncoder) {
          lcd.setCursor(0, 1);
          lcd.print("3: Azul         ");
        }
        digitalWrite(pinR , LOW);
        digitalWrite(pinG , LOW);
        digitalWrite(pinB , HIGH);
        girouEncoder = false;
        
        break;
      }
    case 4: {
        if (girouEncoder) {
          lcd.setCursor(0, 1);
          lcd.print("4: Branco       ");
        }

        digitalWrite(pinR , HIGH);
        digitalWrite(pinG , HIGH);
        digitalWrite(pinB , HIGH);
        girouEncoder = false;

        break;
      }
    default: {
        if (girouEncoder) {
          lcd.setCursor(0, 1);
          lcd.print("Desligado       ");
        }
        
        digitalWrite(pinR , LOW);
        digitalWrite(pinG , LOW);
        digitalWrite(pinB , LOW);
        girouEncoder = false;

        break;
      }
  }

  posicaoAnt = posicao; //SALVA A POSIÇÃO ATUAL DO ENCODER PARA COMPARAR
}

// IMPLEMENTO DE FUNÇÕES
bool botaoApertado(byte pin_sw, int tempoDebounce = 100) {
  static unsigned long swDebounce;
  bool sw;
  static bool swAnt = 1;

  sw  = digitalRead(pin_sw);

  if ( millis() - swDebounce  > tempoDebounce) {

    if (!sw && swAnt) {
      swDebounce = millis();
      swAnt = sw;
      return true;

    } else if (sw && !swAnt) {
      swDebounce = millis();
      swAnt = sw;
    }
  }

  return false;
}

void configuraInterrupcao() {

  PCICR |= (1 << PCIE2); // HABILITA A INTERRUPÇÃO DAS PORTAS 0 A 7
  PCMSK2 |= (1 << PCINT22) | (1 << PCINT23); // CONFIGURA AS PORTAS 6 E 7 PARA INTERRUPÇÃO

}

void atualizaDisplay() {

  if (botaoApertado(pinSW) || (posicao != posicaoAnt) ) {
    lcd.backlight();
    tempoDisplay = millis();
  }

  if (millis() - tempoDisplay > 5000) lcd.noBacklight();

}

ISR(PCINT2_vect) { // FUNÇÃO DA INTERRUPÇÃO
  encoder.tick();
}
