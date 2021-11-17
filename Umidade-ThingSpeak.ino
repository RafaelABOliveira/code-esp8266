#include <ESP8266WiFi.h>

#define SSID_REDE "Rede"  
#define SENHA_REDE "senha-wifi"  
#define INTERVALO_ENVIO_THINGSPEAK 500 

char EnderecoAPIThingSpeak[] = "api.thingspeak.com";
String ChaveEscritaThingSpeak = "77YF2AYTLJJF0J5C";
long lastConnectionTime; 
WiFiClient client;

void EnviaInformacoesThingspeak(String StringDados);
void FazConexaoWiFi(void);
float FazLeituraUmidade(void);

void EnviaInformacoesThingspeak(String StringDados)
{
    if (client.connect(EnderecoAPIThingSpeak, 80))
    {         
        //faz a requisição HTTP ao ThingSpeak
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+ChaveEscritaThingSpeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(StringDados.length());
        client.print("\n\n");
        client.print(StringDados);
   
        lastConnectionTime = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
     }   
}

void FazConexaoWiFi(void)
{
    client.stop();
    Serial.println("Conectando-se à rede WiFi...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");  
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
 
    delay(1000);
}


float FazLeituraUmidade(void)
{
    int ValorADC;
    float UmidadePercentual;
 
     ValorADC = analogRead(A0);   
     Serial.print("[Leitura ADC] ");
     Serial.println(ValorADC);
      
     UmidadePercentual = 100 * ((1024-(float)ValorADC) / 624);
     Serial.print("[Umidade Percentual] ");
     Serial.print(UmidadePercentual);
     Serial.println("%");
 
     return UmidadePercentual;
}

void setup()
{  
    Serial.begin(9600);
    lastConnectionTime = 0; 
    FazConexaoWiFi();
    Serial.println("Planta IoT com ESP8266 NodeMCU");
}

void loop()
{
    float UmidadePercentualLida;
    int UmidadePercentualTruncada;
    char FieldUmidade[11];
     
    //Força desconexão ao ThingSpeak (se ainda estiver desconectado)
    if (client.connected())
    {
        client.stop();
        Serial.println("- Desconectado do ThingSpeak");
        Serial.println();
    }
 
    UmidadePercentualLida = FazLeituraUmidade();
    UmidadePercentualTruncada = (int)UmidadePercentualLida; //trunca umidade como número inteiro
     
    //verifica se está conectado no WiFi e se é o momento de enviar dados ao ThingSpeak
    if(!client.connected() && 
      (millis() - lastConnectionTime > INTERVALO_ENVIO_THINGSPEAK))
    {
        sprintf(FieldUmidade,"field1=%d",UmidadePercentualTruncada);
        EnviaInformacoesThingspeak(FieldUmidade);
    }
 
     delay(1000);
}
