#include "Funciones.h"

Funciones::Funciones(){
  
}

boolean Funciones::getEstadoConx(){
   String response;
    
   Serial.println(F("\n---> Conectando con el servidor... "));
   HTTPClient http; 
   http.begin("https://"+host+":443/getestadoConexion");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
     Serial.println(response);
   }

   if(response.equals("OK")){
      Serial.println(F("Conectado al servidor central de cerraduras!"));
      return true;
   } else {
      Serial.println(F("No hay conexion con el servidor central!"));
      return false;
   }

}

String Funciones::getEstado(String uuid, String pin, String chipid) {

   String key = "?idCerradura=";
   String p1 = "&uuidTarjeta=";
   String p2 = "&pin=";
   String response;
   
   HTTPClient http;   
   String iv = getiv();
   http.begin("https://"+host+":443/getestado" + key + encryptAES(sha256(chipid)) + p1 + encryptAES(sha256(uuid)) + p2 + encryptAES(sha256(pin)) + "&iv=" + iv);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   Serial.println(F("Peticion getEstado enviada!"));
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
     
   }
   return response;
}

String Funciones::getUsuario(String uuid, String chipid) {

   String key = "?idCerradura=";
   String p1 = "&uuidTarjeta=";
   String response;
   
   HTTPClient http;   
   String iv = getiv();
   http.begin("https://"+host+":443/getusuario" + key + encryptAES(sha256(chipid)) + p1 + encryptAES(sha256(uuid)) + "&iv=" + iv);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   Serial.println(F("Peticion getUsuario enviada!"));
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
     
   }
   return response;
}

String Funciones::getActualizacion(String chipid, int cont) {

   String key = "?idCerradura=";
   String p1 = "&cont=";
   String response;

   HTTPClient http;   
   String iv = getiv();
   http.begin("https://"+host+":443/actualizacion" + key + encryptAES(sha256(chipid)) + p1 + cont + "&iv=" + iv);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   Serial.println(F("Peticion getActualizacion enviada!"));
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
   }
   return response;
}


int Funciones::getNumUsuarios(String chipid) {
   
   String key = "?idCerradura=";
   String response;
   
   HTTPClient http;   
   String iv = getiv();
   http.begin("https://"+host+":443/getnumusuarios" + key + encryptAES(sha256(chipid)) + "&iv=" + iv);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   Serial.println(F("Peticion getNumUsuarios enviada!"));
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
    
   }
  
   int r = response.toInt();
   return r;
}

int Funciones::getNumUsuariosModificados(String chipid) {
   
   String key = "?idCerradura=";
   String response;
   
   HTTPClient http;   
   String iv = getiv();
   http.begin("https://"+host+":443/getnumusuariosmod" + key + encryptAES(sha256(chipid)) + "&iv=" + iv);  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   Serial.println(F("Peticion getNumUsuariosModificados enviada!"));
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
     
   }
  
   int r = response.toInt();
   return r;
}

String Funciones::getUsuariosEliminados(){

   String response;
   
   HTTPClient http;   
   String iv = getiv();
   http.begin("https://"+host+":443/getusuarioseliminados");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET(); 
   Serial.println(F("Peticion getUsuariosEliminados enviada!"));
   if(httpResponseCode > 0){
     response = http.getString(); // Get the response to the request
     
   }
   return response;
}



void Funciones::notificarAcceso(String chipid, String tipo, String uuid){
    
   HTTPClient http;   
    String iv = getiv();
   http.begin("https://"+host+":443/notificacion?idDispositivo=" + encryptAES(sha256(chipid)) + "&tipo=" + tipo + "&uuidTarjeta=" + encryptAES(sha256(uuid)) + "&iv=" + iv);
   http.addHeader("Content-Type", "text/plain"); //Specify content-type header

   int httpResponseCode = http.GET();   //Send the actual POST request
   
   Serial.println(F("Peticion notificacion de acceso enviada!"));
   if(httpResponseCode > 0){
     String response = http.getString(); // Get the response to the request
     Serial.println(response);
   }
}



String Funciones::sha256(String dato) {
  String resultado;
  char payload[dato.length()+1];
  dato.toCharArray(payload, dato.length()+1);
  byte hmacResult[32];
 
  const size_t payloadLength = strlen(payload);  
  mbedtls_sha256_context ctx;
  mbedtls_sha256_init(&ctx);
  mbedtls_sha256_starts(&ctx, 0);
  mbedtls_sha256_update(&ctx, (const unsigned char *) payload, payloadLength);
  mbedtls_sha256_finish(&ctx, hmacResult);
  mbedtls_sha256_free(&ctx);
    
  for(int i= 0; i< sizeof(hmacResult); i++){
      char str[3];
      sprintf(str, "%02x", (int)hmacResult[i]);
      resultado += str;
  }
  return resultado;
}

String Funciones::sha512(String dato) {
  String resultado;
  char payload[dato.length()+1];
  dato.toCharArray(payload, dato.length()+1);
  byte hmacResult[64];
 
  const size_t payloadLength = strlen(payload);  
  mbedtls_sha512_context ctx;
  mbedtls_sha512_init(&ctx);
  mbedtls_sha512_starts(&ctx, 0);
  mbedtls_sha512_update(&ctx, (const unsigned char *) payload, payloadLength);
  mbedtls_sha512_finish(&ctx, hmacResult);
  mbedtls_sha512_free(&ctx);
    
  for(int i= 0; i< sizeof(hmacResult); i++){
      char str[3];
      sprintf(str, "%02x", (int)hmacResult[i]);
      resultado += str;
  }  
  return resultado;
}

String Funciones::encryptAES(String msg){
    char b64data[200];
    char b64data2[200];
    byte cipher[1000];

    aes.set_key( key , sizeof(key));  // Get the globally defined key
    getiv();
    
    base64_encode( b64data2, (char *)my_iv, N_BLOCK);
    
    int b64len = base64_encode(b64data, (char *)msg.c_str(),msg.length());
    aes.do_aes_encrypt((byte *)b64data, b64len , cipher, key, 128, my_iv);
    base64_encode(b64data, (char *)cipher, aes.get_size() );
    String encrypted = String(b64data);
    encrypted.replace("+","%2B");
    
    return encrypted;
}

String Funciones::decryptAES(String msg) {
  char data_decoded[200];
  byte out[200];
  char temp[200];
  byte iv[N_BLOCK];

  aes.set_key(key, sizeof(key)); // Get the globally defined key
  getiv();
  
  msg.toCharArray(temp, 200);
  int len = base64_decode(data_decoded, temp, msg.length());
  aes.do_aes_decrypt((byte *)data_decoded, len, out, key, 128, my_iv);
  
  char message[64];
  base64_decode(message, (char *)out, 88);
  return message;
}

String Funciones::getiv(){
    char b64data[200];
    gen_iv(my_iv);                  // Generate a random IV
    base64_encode(b64data, (char *)my_iv, N_BLOCK);
    String iv =  String(b64data);

    return iv;
}

uint8_t Funciones::getrnd() {
    uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
    return really_random;
}

// Generate a random initialization vector
void Funciones::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte) getrnd();
    }
}




