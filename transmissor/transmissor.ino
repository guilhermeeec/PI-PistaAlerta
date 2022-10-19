// PROJETO INTEGRADO 2022/2 - Código do Transmissor do carro (Arduino Nano)

// -------- INCLUDES --------

// Transmissor de RF
// Link: (https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-transmissor-e-receptor-rf-315mhz-433mhz)
#include <RH_ASK.h>
#include <SPI.h>

// Acelerometro (interface I2C)
#include <Wire.h> // Pinos: A4 (SDA) e A5 (SCL) 

// -------- MACROS --------

#define TAMANHO_MAX_BUFFER    16
#define USB_BAUD_RATE         9600
#define TAXA_COLETA_POR_SEG   100 // amostras/segundo
#define QUANTIDADE_AMOSTRAS   100

// -------- VARIÁVEIS GLOBAIS --------

const char idCarro[8+1] = "ffffffff";
const int enderecoAcelerometro = 0x68;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; //Variaveis para pegar os valores medidos
int16_t offsetAcx = -16750;
int16_t offsetGyy = 50;
int16_t offsetGyz = 50;

// Objeto para abstrair driver do radio-transmissor
RH_ASK txDriver;

// Pinos
const int txDigital = 12; // SPI MISO (Master-Input-Slave-Output)

// Botao de começar/encerrar sistema
const int pinoBotao = 3;
bool f = false;
bool a = false;

// -------- FUNÇÕES AUXILIARES --------

void inicializaAcelerometro() {
  
  Wire.begin(); //Inicia a comunicação I2C
  Wire.beginTransmission(enderecoAcelerometro); //Começa a transmissao de dados para o sensor
  Wire.write(0x6B); // registrador PWR_MGMT_1
  Wire.write(0); // Manda 0 e "acorda" o MPU 6050
  Wire.endTransmission(true);  

  Wire.beginTransmission(enderecoAcelerometro); //Começa a transmissao de dados para o sensor
  Wire.write(0x3B); // registrador dos dados medidos (ACCEL_XOUT_H)
  Wire.endTransmission(false);

}

void coletaDadosAcelerometro(int16_t matrizDados[QUANTIDADE_AMOSTRAS][6]) {
  
  for (int i = 0; i < QUANTIDADE_AMOSTRAS; i++) {
    inicializaAcelerometro();
    Wire.requestFrom(enderecoAcelerometro,14,true); // faz um "pedido" para ler 14 registradores, que serão os registrados com os dados medidos
    AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    
    matrizDados[i][0] = AcX+offsetAcx;
    matrizDados[i][1] = AcY;
    matrizDados[i][2] = AcZ;
    matrizDados[i][3] = GyX;
    matrizDados[i][4] = GyY+offsetGyy;
    matrizDados[i][5] = GyZ+offsetGyz;       
    delay(1000/TAXA_COLETA_POR_SEG);
  }
}

float calculaQualidadePista (int16_t matrizDados[QUANTIDADE_AMOSTRAS][6]) {
  float qualidadeTotal = 0.0;
  int16_t acx,gyy,gyz;
  for (int i = 0; i < QUANTIDADE_AMOSTRAS; i++) {
    if(matrizDados[i][0]<=150)
      acx = (matrizDados[i][0]>=-150 ? 0 : matrizDados[i][0]+150); 
    else if(matrizDados[i][0]>-150 )
      acx = (matrizDados[i][0]<150 ? 0 : matrizDados[i][0]-150);
    
    if(matrizDados[i][4]<=60)
      gyy = (matrizDados[i][4]>=-60 ? 0 : matrizDados[i][4]+60); 
    else if(matrizDados[i][4]>-60)
      gyy = (matrizDados[i][4]<60 ? 0 : matrizDados[i][4]-60);
      
    if(matrizDados[i][5]<=60)
      gyz = (matrizDados[i][5]>=-60 ? 0 : matrizDados[i][5]+60); 
    else if(matrizDados[i][5]>-60)
      gyz = (matrizDados[i][5]<60 ? 0 : matrizDados[i][5]-60);
      
    float qualidadeAmostra = (float) ( pow(acx, 2) + pow(gyy, 2) + pow(gyz, 2) ); 
    qualidadeTotal += qualidadeAmostra;
  }

  return qualidadeTotal/1e6;
}

void enviaDado(float qualidadePista, const char* idCarro, RH_ASK& txDriverLocal) {
  
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

// Primeira função a ser executada
void setup() {

  // Inicializa monitor serial USB
  Serial.begin(USB_BAUD_RATE);

  // Inicializa acelerômetro/girscópio
  inicializaAcelerometro();    

  // Inicializa driver do Tx RF (objeto global)
  txDriver.init();

  pinMode(pinoBotao, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
} // Fim do setup() -> Chama a função loop()

// -------- LOOP --------

void loop() {

  // Aperta botão para começar/encerrar sistema 
  startPause();

  // Cada linha é uma medida (ax, ay, az, gx, gy, gz). Passada por referência
  // OBS: sizeof(unsigned int) = 2 bytes Comp. 2 (-32768 a +32767)
  int16_t matrizDados[QUANTIDADE_AMOSTRAS][6];

  // Fica T segundos coletando dados (protótipo a definir)
  coletaDadosAcelerometro(matrizDados); // Protótipo a definir

  // Calcula norma do vetor (ou alguma outra transformação) (protótipo a definir)  
  float qualidadePista = calculaQualidadePista(matrizDados);  

  // Protótipo: [float, const char*, RH_ASK&]
  enviaDado(qualidadePista, idCarro, txDriver);

} // Fim do loop() -> Chama loop() denovo
