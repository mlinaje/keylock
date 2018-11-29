#include "BDLocal.h"

BDLocal::BDLocal(){
  
}

void BDLocal::listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void BDLocal::readFile(fs::FS &fs, String pathAux){
    Serial.print("\n Reading file: ");
    Serial.println(pathAux);

    char path[50];
    pathAux.toCharArray(path, 50);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    while(file.available()){
        Serial.write(file.read());
    }
}

void BDLocal::writeFile(fs::FS &fs, String pathAux, String datosAux){
    Serial.print("Writing file: ");
    Serial.println(pathAux);

    char path[50];
    pathAux.toCharArray(path, 50);
    char datos[5000];
    datosAux.toCharArray(datos, 500);
    
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(datos)){
        Serial.println("- file written");
    } else {
        Serial.println("- frite failed");
    }
}

void BDLocal::renameFile(fs::FS &fs, String pathAux1, String pathAux2){
    Serial.printf("Renaming file %s to %s\r\n", pathAux1, pathAux2);

    char path1[50];
    pathAux1.toCharArray(path1, 50);
    char path2[50];
    pathAux2.toCharArray(path2, 50);
    
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void BDLocal::deleteFile(fs::FS &fs, String pathAux){
    Serial.print("Deleting file: ");
    Serial.println(pathAux);

    char path[50];
    pathAux.toCharArray(path, 50);
    
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void BDLocal::createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void BDLocal::removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

String BDLocal::getUsuario(fs::FS &fs, String dirnameAux, String uuidTarjeta){
  String user, uuid;
  char dirname[50];
  dirnameAux.toCharArray(dirname, 50);
  File root = fs.open(dirname);
  if(!root){
    return "Error: Al abrir el directorio";
  }
  if(!root.isDirectory()){
    return "Error: No es un directorio";
  }
  File file = root.openNextFile();
  while(file){
     String pathAux = file.name();     
     char path[50];
     pathAux.toCharArray(path, 50);
     File fileUser = fs.open(path);
     if(!fileUser || fileUser.isDirectory()){
        return "Error: Al abrir el fichero";
     }
    
  char res[500];
  char *pch; int j = 0;
  while(fileUser.available()){
     String datosUser = fileUser.readStringUntil('\r');
     datosUser.toCharArray(res, 500);
     pch = strtok(res, delimitador);
        while(pch != NULL){
          if(j == 0){
           user = String(pch);
          }
          if(j == 2){
           uuid = String(pch);
          }
          j++;
          pch = strtok(NULL, delimitador);
        }
     }
  if(uuid == uuidTarjeta){
    return user;
  }
  file = root.openNextFile();    
 }  
 
 return "Usuario no encontrado";
}

String BDLocal::getEstado(fs::FS &fs, String dirnameAux, String uuidTarjeta, String pinUser){
  String user, uuid, pin, estado;
  char dirname[50];
  dirnameAux.toCharArray(dirname, 50);
  File root = fs.open(dirname);
  if(!root){
    return "Error: Al abrir el directorio";
  }
  if(!root.isDirectory()){
    return "Error: No es un directorio";
  }
  File file = root.openNextFile();
  while(file){
     String pathAux = file.name();     
     char path[50];
     pathAux.toCharArray(path, 50);
     File fileUser = fs.open(path);
     if(!fileUser || fileUser.isDirectory()){
        return "Error: Al abrir el fichero";
     }
    
  char res[500];
  char *pch; int j = 0;
  while(fileUser.available()){
     String datosUser = fileUser.readStringUntil('\r');
     datosUser.toCharArray(res, 500);
     pch = strtok(res, delimitador);
        while(pch != NULL){
          if(j == 2){
           uuid = String(pch);
          }
          if(j == 3){
            pin = String(pch);
          }
          if(j == 4){
           estado = String(pch);
          }
          j++;
          pch = strtok(NULL, delimitador);
        }
     }
  if(uuid == uuidTarjeta && pin == pinUser){
    return estado;
  }
  file = root.openNextFile();    
 }  
 
 return "Usuario no encontrado";
}

String BDLocal::actualizarBD(fs::FS &fs, String datosUsuario){
  String user;
  char res[datosUsuario.length()];
  char *pch; int j = 0;  

  if(datosUsuario.length() == 0 ){
    return "Usuarios no encontrado";
  }

  Serial.println("Datos para actualizar: ");
  Serial.println(datosUsuario);
  
  datosUsuario.toCharArray(res, datosUsuario.length());
  pch = strtok(res, delimitador);
  
  while(pch != NULL){
      if(j == 0){
         user = String(pch);
         Serial.print("\nActualizando usuario: ");
         Serial.println(user);
      }
      j++;
      pch = strtok(NULL, delimitador);
  }

   String ruta = "/usuarios/"+user;
   Serial.println(ruta);
   writeFile(SPIFFS, ruta, datosUsuario);
  
  return "Usuario registrado en la BD Local!";
}

void BDLocal::setNumUsuarios(fs::FS &fs, String pathAux, String num){
  writeFile(SPIFFS, pathAux, num); 
}

int BDLocal::getNumUsuarios(fs::FS &fs, String pathAux){
  String numAux;
  char path[50];
  int res;
  pathAux.toCharArray(path, 50);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    return 0;
  }
  while(file.available()){
    String numAux = file.readString();
    res = numAux.toInt();
  }
  return res;
}

void BDLocal::eliminarUsuarios(fs::FS &fs, String Usuario){
  String user;
  char res[500];
  char *pch; int j = 0; 
  
  if(Usuario.length() == 0 ){
    return;
  }
  
  Usuario.toCharArray(res, 500);
  pch = strtok(res, delimitador);
  while(pch != NULL){
         user = String(pch);
         Serial.print("Eliminando usuario: ");
         Serial.println(user);
         String ruta = "/usuarios/"+user;
         deleteFile(SPIFFS, ruta);      
      pch = strtok(NULL, delimitador);
  }
}
  
  











  
