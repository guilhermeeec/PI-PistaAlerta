// Transmissor de RF
// Link: (https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-transmissor-e-receptor-rf-315mhz-433mhz)
#include <RH_ASK.h> 
#include <SPI.h>

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>
#include <Adafruit_LiquidCrystal.h>

// -------- MACROS --------

#define RUIM  1
#define MEDIO 2
#define BOM   3
#define INIT  0
#define NULL  -1
#define USB_BAUD_RATE         9600
#define JANELA_MEDIA          4
#define JANELA_PICO           5

// -------- VARIÁVEIS GLOBAIS --------

// Objeto para abstrair driver do radio-receptor
RH_ASK rxDriver;

// Pinos
const int rxDigital = 11; // SPI SIMO (Slave-Input-Master-Output)

float qualidadePista = -1.0; // Valor inicial simbólico
unsigned contador = 0;
unsigned contadorPico = 0;
unsigned contadorPicoMedia = 0;

float pico = 0;
float picoMedia = 0;

float qualidades[JANELA_MEDIA];

//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(12, 10, 5, 4, 3, 2);

// -------- FUNÇÕES AUXILIARES --------

int recebeDado(float* qualidadeRecebida, RH_ASK& rxDriverLocal) {
  
  // cria buffer para guardar dado recebido
  uint8_t buffer[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t comprimentoBuffer = sizeof(buffer);  
  memset(buffer, 0, comprimentoBuffer);
  
  // Aguarda mensagem e preenche o buffer
  if(rxDriverLocal.recv(buffer, &comprimentoBuffer))  {
    
    // Formato da mensagem
    // const char idCarro[8+1] = "ffffffff";
    // sprintf(buffer, "%s:%.2f", idCarro, qualidadePista);
    // ffffffff:2.00
    
    // Filtra a mesnagem para garantir que está no formato certo
    bool mensagemValida = buffer[8] == ':';
    if(mensagemValida == false)
      return -1;

    // Separa partes da mensagem (tamanho não inclui \n)
    size_t tamanho = strlen((char*)buffer);
    char qualidadePistaTexto[ (tamanho-9) + 1];
    memcpy(qualidadePistaTexto, buffer+9, tamanho-9);
    qualidadePistaTexto[tamanho-9] = '\0';

    // Converte texto com a qualidade da pista para variável numérica
    // Problema: não vai ser possível tratar erro se não for um número
    String qualidadePistaString(qualidadePistaTexto);           
    *qualidadeRecebida = qualidadePistaString.toFloat();
    return 0;
  }

  // Se não recebeu nada
  return 1;
}

float calculaMediaMovel(float* qualidades) {
  float soma = 0;
  for(unsigned i = 0; i < JANELA_MEDIA; i++)
    soma += qualidades[i];
  return soma/JANELA_MEDIA;        
}

void calculaPico(float qualidade) {
  if(qualidade > pico) {
    pico = qualidade;
    contadorPico = 0;
  }
  else
    contadorPico++;
  
  if(contadorPico == JANELA_PICO) {
    pico = 0;
    contadorPico = 0;
  } 
}


void calculaHibrido(float media) {
  if(media > picoMedia) {
    picoMedia = media;
    contadorPicoMedia = 0;
  }
  else
    contadorPicoMedia++;
  
  if(contadorPicoMedia == JANELA_PICO) {
    picoMedia = 0;
    contadorPicoMedia = 0;
  }
}

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

// -------- SETUP --------

void setup(){

  // Inicializa monitor serial USB
  Serial.begin(USB_BAUD_RATE);
  
  // Inicializa driver do Rx RF (objeto global)
  rxDriver.init();

  memset(qualidades,0,JANELA_MEDIA);
  
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);
  displayInit();
}

// -------- LOOP --------

void loop(){
  
  float qualidadeRecebida = -1.0;
  int erro = 0;  
  
  // Protótipo: int, [float*, RH_ASK&]
  erro = recebeDado(&qualidadeRecebida, rxDriver);
  //Serial.println(erro);
  
  /*
  Serial.print(contador);
  Serial.print(": ");
  if(erro == 1)
    Serial.println("Mensagem nao recebida");
  if(erro == -1)
    Serial.println("Mensagem invalida");
  if(erro == 0)
    Serial.println(qualidadeRecebida);
  contador++;
  */
  
  if(erro == 0) {
    qualidades[contador] = qualidadeRecebida;

    // Calcular média móvel
    float media = calculaMediaMovel(qualidades);

    // Pega valor de pico e segura por um tempo
    calculaPico(qualidadeRecebida);   
    
    // Pega valor de pico da média móvel e segura por um tempo
    calculaHibrido(media);    
        
    Serial.print("Qualidade: ");
    Serial.print(qualidadeRecebida);
    Serial.print(",");
    Serial.print("Media: ");
    Serial.print(media);
    Serial.print(",");
    Serial.print("Pico: ");
    Serial.print(pico);
    Serial.print(",");
    Serial.print("Hibrido: ");
    Serial.println(picoMedia);
    contador++;
  }

  // Com Janela = 7
  // media < 50 -> Boa
  // 50 < media < 1200 -> Média
  // media > 1200 -> ruim

  if(contador == JANELA_MEDIA) contador = 0;

  if(picoMedia < 200) 
    displayBom();
  else if(picoMedia < 2000) 
    displayMedio();
  else 
    displayRuim();
  
  // Velocidade limite
  delay(5);
}

