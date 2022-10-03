// PROJETO INTEGRADO 2022/2 - Código do Transmissor do carro (Arduino Nano)

// -------- INCLUDES --------

// Transmissor de RF
// Link: (https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-transmissor-e-receptor-rf-315mhz-433mhz)
#include <RH_ASK.h>
#include <SPI.h>

// Acelerometro (interface I2C)
#include <Wire.h> // Pinos: A4 (SDA) e A5 (SCL) 

// -------- MACROS --------

#define JANELA_COLETA_MS      200 // segundos coletando antes de calcular
#define TAXA_COLETA_POR_SEG   100 // amostras/segundo
#define TAMANHO_MAX_BUFFER    16
#define USB_BAUD_RATE         9600

// -------- VARIÁVEIS GLOBAIS --------

const char idCarro[8+1] = "ffffffff";
const int enderecoAcelerometro = 0x68;

// Objeto para abstrair driver do radio-transmissor
RH_ASK txDriver;

// Pinos
const int txDigital = 12; // SPI MISO (Master-Input-Slave-Output)

// -------- FUNÇÕES AUXILIARES --------

// Talvez usar arquivos separados

void inicializaAcelerometro() {
  
  // Inicializa interface I2C pro acelerômetro/giroscópio
  // Link: https://mundoprojetado.com.br/acelerometro-mpu6050-arduino/
  Wire.begin();
  Wire.beginTransmission(enderecoAcelerometro);
  Wire.write(0x6B); // Reg: PWR_MGMT_1
  Wire.write(0); // Wake-up code
  Wire.endTransmission(true);
  
}

void enivaDado(float qualidadePista, const char* idCarro, RH_ASK& txDriverLocal) {
  
  // Carrega buffer e envia seu conteúdo (uint8_t -> byte padrão)
  char buffer[TAMANHO_MAX_BUFFER];
  sprintf(buffer, "%s:%.2f", idCarro, qualidadePista);

  txDriverLocal.send((uint8_t*)buffer, strlen(buffer));
  txDriverLocal.waitPacketSent();    
}

// -------- SETUP --------

// Primeira função a ser executada
void setup() {

  // Inicializa monitor serial USB
  Serial.begin(USB_BAUD_RATE);

  // Inicializa acelerômetro/girscópio
  inicializaAcelerometro();    

  // Inicializa driver do Tx RF (objeto global)
  txDriver.init();
  
} // Fim do setup() -> Chama a função loop()

// -------- LOOP --------

void loop() {

  // Tamanho da linha de medidas
  // Exemplo:
  // JANELA_COLETA_MS = 200 ms coletando dados
  // TAXA_COLETA_POR_SEG = 100 dados/s
  // Quantidade de amostras = (200/1000)s * 100dados/s = 20 dados
  unsigned int quantidadeAmostras = JANELA_COLETA_MS * TAXA_COLETA_POR_SEG / 1000;

  // Cada linha é uma medida (ax, ay, az, gx, gy, gz). Passada por referência
  // OBS: sizeof(unsigned int) = 2 bytes Comp. 2 (-32768 a +32767)
  unsigned int matrizDados[6][quantidadeAmostras];

  // Fica T segundos coletando dados (protótipo a definir)
  coletaDadosAcelerometro(matrizDados, JANELA_COLETA_MS, TAXA_COLETA_POR_SEG); // Protótipo a definir

  // Calcula norma do vetor (ou alguma outra transformação) (protótipo a definir)  
  float qualidadePista = calculaQualidadePista(matrizDados);  

  // Protótipo: [float, const char*, RH_ASK&]
  enivaDado(qualidadePista, idCarro, txDriver);

} // Fim do loop() -> Chama loop() denovo
