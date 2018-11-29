#ifndef DBLOCAL_H
#define BDLOCAL_H

#include "FS.h"
#include "SPIFFS.h"

class BDLocal{

private: 
  const char delimitador[2] = "\n";

public:
  BDLocal();
  
  void writeFile(fs::FS &fs, String pathAux, String datosAux);
  void readFile(fs::FS &fs, String pathAux);
  void deleteFile(fs::FS &fs,  String pathAux);
  void renameFile(fs::FS &fs, String pathAux1,  String pathAux2);
  void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
  void createDir(fs::FS &fs, const char * path);
  void removeDir(fs::FS &fs, const char * path);
  
  String getUsuario(fs::FS &fs, String dirnameAux, String uuidTarjeta);
  String getEstado(fs::FS &fs, String dirnameAux, String uuidTarjeta, String pin);
  String actualizarBD(fs::FS &fs, String datosUsuario);
  void setNumUsuarios(fs::FS &fs, String pathAux, String num);
  int getNumUsuarios(fs::FS &fs, String pathAux);
  void eliminarUsuarios(fs::FS &fs, String Usuario);
  
};


#endif
