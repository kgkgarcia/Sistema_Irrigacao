#include <WiFi.h>
#include <HTTPClient.h>

#define PIN_Sensor 35
#define LED_RED 4
#define LED_GREEN 5

const char* ssid = "CyberPhysicalSystems";
const char* password = "0123456789";
const char* serverUrlSensor = "http://192.168.1.100:3000/sensor";    // Rota para enviar dados do sensor
const char* serverUrlControl = "http://192.168.1.100:3000/control";  // Rota para buscar estado de controle

unsigned long previousMillisSensor = 0;
unsigned long previousMillisControl = 0;
const long intervalSensor = 5000;   // Intervalo de envio de dados do sensor (5s)
const long intervalControl = 2000;  // Intervalo de verificação de controle (2s)

void conectarWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi!");
}

int ler_dados() {
  int valorBruto = analogRead(PIN_Sensor);
  int umidade = map(valorBruto, 4095, 0, 0, 100);
  return umidade;
}

void enviarDadosSensor() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrlSensor);  // Conecta à rota de envio de sensor
    http.addHeader("Content-Type", "application/json");

    int umidade = ler_dados();
    String jsonData = "{\"humidity\": " + String(umidade) + "}";

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      Serial.print("Dados enviados com sucesso. Código de resposta: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Erro ao enviar dados do sensor.");
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Dados do sensor não enviados.");
  }
}

void verificarControle() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrlControl);  // Conecta à rota de controle
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      response.trim();  // Remove espaços em branco e quebras de linha
      Serial.print("Resposta do servidor: ");
      Serial.println(response);

      if (response == "on") {
        controlarLEDs("on");
      } else if (response == "off") {
        controlarLEDs("off");
      } else {
        Serial.println("Resposta desconhecida do servidor.");
      }
    } else {
      Serial.print("Erro ao verificar controle. Código de resposta: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Não foi possível verificar controle.");
  }
}


void controlarLEDs(String action) {
  Serial.print("Ação recebida para LEDs: ");
  Serial.println(action);

  if (action == "on") {
    digitalWrite(LED_GREEN, HIGH);  // Liga LED verde
    digitalWrite(LED_RED, LOW);     // Apaga LED vermelho
    Serial.println("Água ligada: LED verde ativado.");
  } else if (action == "off") {
    digitalWrite(LED_GREEN, LOW);  // Apaga LED verde
    digitalWrite(LED_RED, HIGH);   // Liga LED vermelho
    Serial.println("Água desligada: LED vermelho ativado.");
  }
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  conectarWiFi();
}

void loop() {
  unsigned long currentMillis = millis();

  // Enviar dados do sensor a cada 5 segundos
  if (currentMillis - previousMillisSensor >= intervalSensor) {
    previousMillisSensor = currentMillis;
    enviarDadosSensor();
  }

  // Verificar estado de controle a cada 2 segundos
  if (currentMillis - previousMillisControl >= intervalControl) {
    previousMillisControl = currentMillis;
    verificarControle();
  }
}
