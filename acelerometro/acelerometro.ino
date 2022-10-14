// INCLUDES
#include <Wire.h> // Pinos: A4 (SDA) e A5 (SCL) 

// MACROS
#define JANELA_COLETA_MS      200 // segundos coletando antes de calcular
#define TAXA_COLETA_POR_SEG   100 // amostras/segundo
#define USB_BAUD_RATE         9600
#define QUANTIDADE_AMOSTRAS   2

// VARIÁVEIS GLOBAIS
const char idCarro[8+1] = "ffffffff";
const int enderecoAcelerometro = 0x68;

// FUNÇÕES AUXILIARES
void inicializaAcelerometro() {
  
  // Inicializa interface I2C pro acelerômetro/giroscópio
  // Link: https://mundoprojetado.com.br/acelerometro-mpu6050-arduino/
  Wire.begin();
  Wire.beginTransmission(enderecoAcelerometro);
  Wire.write(0x6B); // Reg: PWR_MGMT_1
  Wire.write(0); // Wake-up code
  Wire.endTransmission(true);
  
}

float coletaDadosAcelerometro (float matrizDados[QUANTIDADE_AMOSTRAS][6]) {
  // TALVEZ USAR "double"
  float qualidadeTotal = 0.0;
  for (int i = 0; i < sizeof(matrizDados); i++) {
    // Avaliar a qualidade de uma mostra (az^2 + gx^2 + gy^2 + gz^2)
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

  float matrizDados[QUANTIDADE_AMOSTRAS][6] = {
        {1, 2, 3, 4, 5, 6},
        {5, 4, 3, 2, 1, 0}
  };

  float valor = coletaDadosAcelerometro (matrizDados);
  Serial.println(valor);

}
