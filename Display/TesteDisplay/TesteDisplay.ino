#include <Adafruit_LiquidCrystal.h>

//Programa: Display LCD 16x2
//Autor: FILIPEFLOP
 
//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>

// Macros para o nível de qualidade da pista
#define RUIM  1
#define MEDIO 2
#define BOM   3
#define INIT  0
#define NULL  -1

//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void displayBom(){
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("QUALIDADE:BOA");
  lcd.setCursor(2, 1);
  lcd.print("PISTA SEGURA");
  
  delay(2000);
}

void displayMedio(){
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("QUALIDADE:MEDIA");
  lcd.setCursor(1, 1);
  lcd.print("TENHA CUIDADO!");
  
  delay(2000);
}

void displayRuim(){
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("QUALIDADE:RUIM");
  lcd.setCursor(0, 1);
  lcd.print("PISTA PERIGOSA!");
  
  delay(2000);
}

void displayInit(){
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("ESPERANDO DADOS");
  lcd.setCursor(0, 1);
  lcd.print("################");
}


void idQualidade(int qualidadePista){
  switch (qualidadePista){
  	case BOM:
      displayBom();
      break;
    case MEDIO:
      displayMedio();
      break;
    case RUIM:
      displayRuim();
      break;
    case INIT:
      displayInit();
      break;
    default:
      lcd.clear();
  }
  
}


void setup()
{
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);
  idQualidade(INIT);
  //delay(5000);
}
 
void loop()
{
  int arr[3] = {BOM, MEDIO, RUIM};
  for (int idx = 0; idx < sizeof(arr); idx++) {
    idQualidade(arr[idx]);
    //delay(2000);
  }
}