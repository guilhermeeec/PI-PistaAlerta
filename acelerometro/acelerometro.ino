// INCLUDES
#include <Wire.h> // Pinos: A4 (SDA) e A5 (SCL) 

// MACROS
//#define JANELA_COLETA_MS      200 // segundos coletando antes de calcular
#define TAXA_COLETA_POR_SEG   100 // amostras/segundo
#define USB_BAUD_RATE         9600
#define QUANTIDADE_AMOSTRAS   100

// VARIÁVEIS GLOBAIS
const int enderecoAcelerometro = 0x68;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; //Variaveis para pegar os valores medidos
int16_t offsetAcx = -16750;
int16_t offsetGyy = 50;
int16_t offsetGyz = 50;

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

double calcularQualidadePista (int16_t matrizDados[QUANTIDADE_AMOSTRAS][6]) {
  double qualidadeTotal = 0.0;
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
      
    double qualidadeAmostra = ( pow(acx, 2) + pow(gyy, 2) + pow(gyz, 2) ); 
    qualidadeTotal += qualidadeAmostra;
  }

  return qualidadeTotal/1e6;
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
  int16_t matrizDados[QUANTIDADE_AMOSTRAS][6];
  coletaDadosAcelerometro(matrizDados);
  double qualidade = calcularQualidadePista (matrizDados);
  Serial.println(qualidade);

}
