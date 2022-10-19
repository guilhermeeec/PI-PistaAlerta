// Transmissor de RF
// Link: (https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-transmissor-e-receptor-rf-315mhz-433mhz)

#include <RH_ASK.h>
#include <SPI.h> //INCLUSÃO DE BIBLIOTECA

RH_ASK txDriver; //CRIA O DRIVER PARA COMUNICAÇÃO

// -------- MACROS --------

#define JANELA_COLETA_MS      200 // mili-segundos coletando antes de calcular
#define TAMANHO_MAX_BUFFER    16
#define USB_BAUD_RATE         9600

// -------- VARIÁVEIS GLOBAIS --------

const char idCarro[8+1] = "ffffffff";

// Botao de começar/encerrar sistema
const int pinoBotao = 3;
bool f = false;
bool a = false;

// -------- FUNÇÕES AUXILIARES --------

void enivaDado(float qualidadePista, const char* idCarro, RH_ASK& txDriverLocal) {
  
  // Carrega buffer e envia seu conteúdo (uint8_t -> byte padrão)
  char buffer[TAMANHO_MAX_BUFFER];
  char qualidadePistaString[5];
  dtostrf((double) qualidadePista, 4, 2, qualidadePistaString);
  sprintf(buffer, "%s:%s", idCarro, qualidadePistaString);

  txDriverLocal.send((uint8_t*)buffer, strlen(buffer));
  txDriverLocal.waitPacketSent();    
}

void startPause() {

  /* Máquina de Estados Finita com 4 estados e 1 entrada
   * A -> botão está apertado
   * E -> entrada nova do botão
   * F -> Funcionando
   */     
  do {
    bool e = (digitalRead(pinoBotao) == LOW);
    bool a_novo = e;
    bool f_novo = (!e && f) || (e && (a == f));  
    a = a_novo;
    f = f_novo;
    digitalWrite(LED_BUILTIN, f_novo ? HIGH : LOW);
  }
  while (f == false);  
}

// -------- SETUP --------

void setup(){
  // Inicializa driver do Tx RF (objeto global)
  txDriver.init();
  Serial.begin(9600);
  pinMode(pinoBotao, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

// -------- LOOP --------

void loop(){

  // Aperta botão para começar/encerrar sistema 
  startPause();
    
  // Simula tempo de coleta
  delay(JANELA_COLETA_MS);

  // Protótipo: [float, const char*, RH_ASK&]
  enivaDado(2.00, idCarro, txDriver);
}
