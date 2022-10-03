// PROJETO INTEGRADO 2022/2 - Código do receptor na pista (Arduino Uno)

// -------- INCLUDES --------

// Receptor de RF
// Link: (https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-transmissor-e-receptor-rf-315mhz-433mhz)
#include <RH_ASK.h>
#include <SPI.h>

// Display LCD

// -------- MACROS --------

#define USB_BAUD_RATE         9600

// -------- VARIÁVEIS GLOBAIS --------

// Objeto para abstrair driver do radio-receptor
RH_ASK rxDriver;

// Pinos
const int rxDigital = 11; // SPI SIMO (Slave-Input-Master-Output)

float qualidadePista = -1.0; // Valor inicial simbólico

// -------- FUNÇÕES AUXILIARES --------

// Talvez usar arquivos separados

int recebeDado(float* qualidadedeRecebida, RH_ASK& rxDriverLocal) {
  
  // cria buffer para guardar dado recebido
  uint8_t buffer[RH_ASK_MAX_MESSAGE_LEN];
  uint32_t comprimentoBuffer = sizeof(buffer);  
  memset(buffer, 0, comprimentoBuffer);
  
  // Aguarda mensagem e preenche o buffer
  if(rxDriverLocal.recv(buffer, &comprimentoBuffer))  {
    
    // Formato da mensagem
  }  

  // ...

}

// -------- SETUP --------

// Primeira função a ser executada
void setup() {

  // Inicializa monitor serial USB
  Serial.begin(USB_BAUD_RATE);

  // Inicializa Display LCD
  inicializaLcd();    

  // Inicializa driver do Rx RF (objeto global)
  rxDriver.init();
  
} // Fim do setup() -> Chama a função loop()

// -------- LOOP --------

void loop() {

  float qualidadeRecebida = -1.0;
  int erro = 0;  

  // Protótipo: int, [float*, RH_ASK&]
  erro = recebeDado(&qualidadeRecebida, rxDriver);

  // Atualiza qualidade
  // qualidadePista += qualidadeRecebida

  // Exibe no display

} // Fim do loop() -> Chama loop() denovo
