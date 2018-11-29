#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <WiFi.h>
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "AES.h"
#include "base64.h"
#include <HTTPClient.h>

class Funciones{

private:
  AES aes;
  const char aux[2] = "\n";
  const int ledblue = 5;
  String host = "192.168.43.142";
  byte key[16] = {0x98, 0x7E, 0x99, 0xD5, 0x00, 0xAE, 0xD2, 0xA6, 0xFA, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0xF9};
  byte my_iv[N_BLOCK] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
public:
  Funciones();

  String getUsuario(String uuid, String chipid);
  String getEstado(String uuid, String pin, String chipid);
  String getUsuariosEliminados();
  int getNumUsuarios(String chipid);
  int getNumUsuariosModificados(String chipid);
  String getActualizacion(String chipid, int cont);
  bool getEstadoConx();
  void notificarAcceso(String chipid, String tipo, String uuid);

  String sha256(String dato);
  String sha512(String dato);

  String encryptAES(String msg);
  String decryptAES(String msg);
  uint8_t getrnd();
  String getiv();
  void gen_iv(byte  *iv);
  
  
};

#endif
