#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include "BDLocal.h"
#include "Funciones.h"
#include <WiFi.h>


BDLocal bd;
Funciones f;

const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {13,12,14,27}; 
byte colPins[COLS] = {15,33,32}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define RST_PIN         22         
#define SS_PIN          21        
MFRC522 mfrc522(SS_PIN, RST_PIN);  

const char* ssid = "HUAWEI P20";
const char* password =  "julianBasketguarena";

const int ledred = 26;
const int ledgreen = 25;
const int ledblue = 5;

String chipid;
int cont = 0;
boolean conn = false;

void setup() {
  Serial.begin(115200);
 
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(ledblue, OUTPUT);
  
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(F(">>> Conectando a la red WiFi: "));
    Serial.println(ssid);
  }
  
  if(!SPIFFS.begin()){
     Serial.println(F(">>> Error al iniciar SPIFFS!"));
     return;
  }
  Serial.println(F(">>> Base de datos Local iniciada. Creando tabla de usuarios...\n"));
  bd.createDir(SPIFFS, "/usuarios");
 
  char tmp_h[9]; char tmp_l[9];
  uint64_t id = ESP.getEfuseMac(); 
  sprintf(tmp_h, "%04X", (uint32_t)(id>>32));
  sprintf(tmp_l, "%08X", (uint32_t)(id));
  chipid = String(tmp_h); chipid += String(tmp_l);
 
  SPI.begin();      
  mfrc522.PCD_Init();   
  mfrc522.PCD_DumpVersionToSerial();  
  Serial.println(F(">>> Módulo RFID activado! \n"));

  parpadeoLED(1);
}

void loop() {

  byte cardUID[4]; 
  String uid, estado, user;
  int pinAux;

  if(mfrc522.PICC_IsNewCardPresent()){ // Look for new cards
    if(mfrc522.PICC_ReadCardSerial()){ // Select one of the cards

    Serial.println(F("---> Comprobando conexión con el servidor de cerraduras..."));
    if(!f.getEstadoConx()){
      conn = false;
      parpadeoLED(1);
    } else {
      conn = true;
      parpadeoLED(1);
    }

    Serial.print(F("\n---> Tarjeta detectada con uuID:  "));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
       cardUID[i] = mfrc522.uid.uidByte[i]; 
       uid += String(cardUID[i],HEX);
    }
    Serial.println(uid);
     
    if(WiFi.status() == WL_CONNECTED && conn){
       user = f.getUsuario(uid, chipid);
       Serial.print(F("---> Bienvenid@ ")); 

       if(user.equals("Usuario no encontrado")){ // Error usuario no encontrado en la BD.
          parpadeoLED(0);
       } else {
          Serial.println(user);
          Serial.println(F("\n---> Introduzca el PIN para su autenticación: "));

          pinAux = getPin();
          String pin = String(pinAux);
          Serial.println(F("---> Comprobando el estado de su disipositivo... \n"));
          estado = f.getEstado(uid, pin, chipid);
          Serial.print(F("Respuesta: "));Serial.println(estado);

          if(estado.equals("Usuario no encontrado")){ // Error al introducir el pin de acceso.
            parpadeoLED(0);
          }
       }
    }                
    
    if(WiFi.status() != WL_CONNECTED || !conn){
       Serial.println(F("---> No hay conexion WiFi o el servidor está caido! Accediendo a la base de datos local..."));
       user = bd.getUsuario(SPIFFS, "/usuarios", f.sha256(uid));

       if(user.equals("Usuario no encontrado")){
          parpadeoLED(0);
       } else {
          Serial.print(F("---> Bienvenid@ ")); Serial.println(user);
          Serial.println(F("\n---> Introduzca el PIN para su autenticación: "));
       
          pinAux = getPin();
          String pin = String(pinAux);
          Serial.println(F("---> Comprobando el estado de su disipositivo... \n"));
          estado = bd.getEstado(SPIFFS, "/usuarios", f.sha256(uid), f.sha256(pin));
          Serial.print(F("Respuesta: ")); Serial.println(estado);
         
          if(estado.equals("Usuario no encontrado")){ // Error al introducir el pin de acceso.
             parpadeoLED(0);
          }
       }
    }

    if(estado.equals("off")){
      Serial.println(F("--- DISPOSITIVO BLOQUEADO ---\n"));
      digitalWrite(ledred, HIGH);delay(5000);digitalWrite(ledred, LOW); 
      f.notificarAcceso(chipid, "bloqueado", uid);
      if(conn){ // Si no hay conexion con el servidor de cerraduras, no se realiza la sincronización
         sincronizacion();
      }
    }else if(estado.equals("on")){
      Serial.println(F("--- DISPOSITIVO DESBLOQUEADO ---\n"));
      digitalWrite(ledgreen, HIGH);delay(5000);digitalWrite(ledgreen, LOW);
      f.notificarAcceso(chipid, "desbloqueado", uid);
      if(conn){ // Si no hay conexion con el servidor de cerraduras, no se realiza la sincronización
         sincronizacion();
      }
    }
   }
  }
}

int getPin() {
 String value;
 int intvalue = 0;
 char pin[5];
 digitalWrite(ledblue, HIGH);
 
 while(cont < 5){
  char key = keypad.getKey(); 
   if (key){
     pin[cont] = key;
     cont++;
   }  
 }
  if(cont == 5){
    for(int j=0; j<5; j++){
      value += pin[j]; 
    }
    intvalue = value.toInt();
    digitalWrite(ledblue, LOW); 
    cont = 0;
  }
  return intvalue;
}

void sincronizacion(){
  int cont = f.getNumUsuarios(chipid);
  int usersMod = f.getNumUsuariosModificados(chipid);
  int j=0;
  
  if(cont == -1){
    Serial.println("Conexión fallida con el servidor!");
    return;
  } else {
    int contAux = bd.getNumUsuarios(SPIFFS, "/numUsuarios");
  
    if(cont < contAux){
      Serial.println("Usuarios/s eliminado/s detectado!");
      String users = f.getUsuariosEliminados();
      bd.eliminarUsuarios(SPIFFS, users);
    }
    bd.setNumUsuarios(SPIFFS, "/numUsuarios", String(cont));

    if(usersMod == NULL){
      Serial.println("No hay usuarios modificados!");
      return;
    }

    for(int i=1; i<usersMod+1; i++){
      String res = f.getActualizacion(chipid, i);

      String resDec;
      char *pch; 
      char resultado[res.length()];
      res.toCharArray(resultado, res.length());
      pch = strtok(resultado, "\n");
      while(pch != NULL){
          if(j==4){
            resDec += String(pch) + "\n";
          } else {
             resDec += f.decryptAES(String(pch)) + "\n";
          }
          j++;
          pch = strtok(NULL, "\n");
      } 
      bd.actualizarBD(SPIFFS, resDec);
    }
  }
}

void parpadeoLED(int led){
 if(led == 0){
     digitalWrite(ledred, HIGH);   
     delay(200);
     digitalWrite(ledred, LOW);   
     delay(200);
     digitalWrite(ledred, HIGH);   
     delay(200);
     digitalWrite(ledred, LOW);    
     delay(200);
     digitalWrite(ledred, HIGH);   
     delay(200);
     digitalWrite(ledred, LOW);   
     delay(200);
     digitalWrite(ledred, HIGH);   
     delay(200);
     digitalWrite(ledred, LOW);  
     delay(200);
     digitalWrite(ledred, HIGH);   
     delay(200);
     digitalWrite(ledred, LOW);   
 } else if(led == 1){
     digitalWrite(ledgreen, HIGH);  
     delay(200);
     digitalWrite(ledgreen, LOW);   
     delay(200);
     digitalWrite(ledgreen, HIGH);  
     delay(200);
     digitalWrite(ledgreen, LOW);   
     delay(200);
     digitalWrite(ledgreen, HIGH);  
     delay(200);
     digitalWrite(ledgreen, LOW);  
     delay(200);
     digitalWrite(ledgreen, HIGH);  
     delay(200);
     digitalWrite(ledgreen, LOW);        
     delay(200);
     digitalWrite(ledgreen, HIGH);  
     delay(200);
     digitalWrite(ledgreen, LOW);  
 }
}


