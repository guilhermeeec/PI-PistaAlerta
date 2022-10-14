// INCLUDES
#include <Wire.h> // Pinos: A4 (SDA) e A5 (SCL) 

// MACROS
#define JANELA_COLETA_MS      200 // segundos coletando antes de calcular
#define TAXA_COLETA_POR_SEG   100 // amostras/segundo
#define USB_BAUD_RATE         9600
#define QUANTIDADE_AMOSTRAS   2

// VARIÁVEIS GLOBAIS
const int enderecoAcelerometro = 0x68;

// FUNÇÕES AUXILIARES
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

void coletaDadosAcelerometro(float matrizDados[QUANTIDADE_AMOSTRAS][6]) {
  
  for (int i = 0; i < sizeof(matrizDados); i++) {
    Wire.requestFrom(enderecoAcelerometro,14,true); // faz um "pedido" para ler 14 registradores, que serão os registrados com os dados medidos
    AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    
    matrizDados[i][0] = AcX;
    matrizDados[i][1] = AcY;
    matrizDados[i][2] = AcZ;
    matrizDados[i][3] = GyX;
    matrizDados[i][4] = GyY;
    matrizDados[i][5] = GyZ;       
  }
  
}

float calcularQualidadePista (float matrizDados[QUANTIDADE_AMOSTRAS][6]) {
  // TALVEZ USAR "double"
  float qualidadeTotal = 0.0;
  for (int i = 0; i < sizeof(matrizDados); i++) {
    // Avaliar a qualidade de uma mostra (AcZ^2 + GyX^2 + GyY^2 + GyZ^2)
    // TALVEZ USAR "double"
    float qualidadeAmostra = (float) ( pow(matrizDados[i][2], 2) + pow(matrizDados[i][3], 2) + pow(matrizDados[i][4], 2) + pow(matrizDados[i][5], 2) ); 
    // Adicionar à qualidade total
    qualidadeTotal += qualidadeAmostra;
  }

  return qualidadeTotal;
}

void setup() {

  // Inicializa monitor serial USB
  Serial.begin(USB_BAUD_RATE);

  // Inicializa acelerômetro/girscópio
  inicializaAcelerometro();    

}

void loop() {
  // Tamanho da linha de medidas
  // Exemplo:
  // JANELA_COLETA_MS = 200 ms coletando dados
  // TAXA_COLETA_POR_SEG = 100 dados/s
  // Quantidade de amostras = (200/1000)s * 100dados/s = 20 dados

}
