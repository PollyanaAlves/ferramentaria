#include <Pn532NfcReader.h>
#include <PN532_HSU.h>
#include <NfcAdapter.h>

//Definição das variáveis utilizadas
int greenpin = 6;
int redpin = 7;
int yellowpin = 8;
int whitepin = 9;
int botao = 10;
int estado;

uint16_t tv[9];
String tag;
String tagID;
String lugar;
String dados;
String conf;
String conf2;
String cond;
String cond2;

byte modo;
byte local;
bool laco;
bool laco2;
bool laco3;

//Definição das informações da etiqueta
int8_t mIncidenceMatrix[] = { -1, -1, -1, 1, 1, 0, 0, 0, 0, 1, 0,
                              1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
                              0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0,
                              0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, -1, 0, 0, 0, -1,
                              0, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

uint16_t mStartingTokenVector[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };

//Definição da comunicação com o leitor RFID PN532
PN532_HSU pn532hsu(Serial1);
NfcAdapter nfc = NfcAdapter(pn532hsu);

//Criação dos objetos de leitor e PNRD
Pn532NfcReader* reader = new Pn532NfcReader(&nfc);
Pnrd pnrd = Pnrd(reader, 9, 11);

void setup() {

pinMode(greenpin, OUTPUT);
pinMode(redpin, OUTPUT);
pinMode(yellowpin, OUTPUT);
pinMode(whitepin, OUTPUT);
pinMode(botao, INPUT);

//Iniciação do leitor e da UART
Serial.begin(9600);
reader->initialize();
nfc.begin();

pnrd.setIncidenceMatrix(mIncidenceMatrix);
pnrd.setTokenVector(mStartingTokenVector);

//Configuração para a abordagem PNRD clássica
pnrd.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
pnrd.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);

}

void loop() {

delay(1000);

//Condição
pnrd.getTokenVector(tv);

//Aguarda o dado vindo do Python
if (Serial.available() > 0){
  modo = Serial.read();

  //Modo de cadastro
  if (modo == '1'){
    digitalWrite(yellowpin, HIGH);
    laco = true;

    while (laco == true){

      //Aguarda a leitura da tag
      if (nfc.tagPresent()){
         NfcTag tag = nfc.read();
         tagID = tag.getUidString();

         //Tentativa de escrita na tag
         if (pnrd.saveData() == WriteError::NO_ERROR){
           digitalWrite(greenpin, HIGH);
           delay(1000);
           digitalWrite(greenpin, LOW);
         }
         else {
           digitalWrite(redpin, HIGH);
           delay(1000);
           digitalWrite(redpin, LOW);
         }

         //Manda os dados de localização e tagID para o Python
         if(tv[0]==1){
           lugar = "1";
           dados = tagID + "|" + lugar;
           Serial.println(dados);
           laco = false;
         }
      }
    }
    digitalWrite(yellowpin, LOW);
  }

  //Modo de movimentação
  if(modo == '2'){
    digitalWrite(whitepin, HIGH);
    laco = true;
    laco2 = true;
    laco3 = true;

    while (laco == true){

      //Aguarda a leitura da tag
      if (nfc.tagPresent()){
         NfcTag tag = nfc.read();
         tagID = tag.getUidString();
         digitalWrite(greenpin, HIGH);
         delay(1000);
         digitalWrite(greenpin, LOW);

         if(tv[0]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "1";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           while (laco2 == true){

             //Espera a escolha do local de destino pelo usuário
             if (Serial.available() > 0){
               local = Serial.read();
  
               //Indo para a Oficina
               if (local == '3'){
                 delay(2000);
  
                 //Disparo da transição 0
                 FireError fireError = pnrd.fire(0);
  
                 switch(fireError){
  
                    case FireError::NO_ERROR:
                      while (laco3 == true){
                        //Salva a nova informação na tag
                        if(pnrd.saveData() == WriteError::NO_ERROR){
                          digitalWrite(greenpin, HIGH);
                          delay(1000);
                          digitalWrite(greenpin, LOW);
                          conf = "ok";
                          conf2 = conf + "|";
                          Serial.println(conf2);
                          laco3 = false;
                        }
                        else {
                          digitalWrite(redpin, HIGH);
                          delay(1000);
                          digitalWrite(redpin, LOW);
                        }
                      }
                      break;
  
                    case FireError::PRODUCE_EXCEPTION:
                      break;
  
                    case FireError::CONDITIONS_ARE_NOT_APPLIED:
                      break;
                 }
                 laco = false;
                 laco2 = false;
               }
  
               //Indo para o Serviço Externo
               if (local == '4'){
                 delay(2000);
  
                 //Disparo da transição 1
                 FireError fireError = pnrd.fire(1);
  
                 switch(fireError){
  
                    case FireError::NO_ERROR:
                      while (laco3 == true){
                        //Salva a nova informação na tag
                        if(pnrd.saveData() == WriteError::NO_ERROR){
                          digitalWrite(greenpin, HIGH);
                          delay(1000);
                          digitalWrite(greenpin, LOW);
                          conf = "ok";
                          conf2 = conf + "|";
                          Serial.println(conf2);
                          laco3 = false;
                        }
                        else {
                          digitalWrite(redpin, HIGH);
                          delay(1000);
                          digitalWrite(redpin, LOW);
                        }
                      }
                      break;
  
                    case FireError::PRODUCE_EXCEPTION:
                      break;
  
                    case FireError::CONDITIONS_ARE_NOT_APPLIED:
                      break;
                 }
                 laco = false;
                 laco2 = false;
               }
  
               //Indo para o Suprimento
               if (local == '5'){
                 delay(2000);
  
                 //Disparo da transição 2
                 FireError fireError = pnrd.fire(2);
  
                 switch(fireError){
  
                    case FireError::NO_ERROR:
                      while (laco3 == true){
                        //Salva a nova informação na tag
                        if(pnrd.saveData() == WriteError::NO_ERROR){
                          digitalWrite(greenpin, HIGH);
                          delay(1000);
                          digitalWrite(greenpin, LOW);
                          conf = "ok";
                          conf2 = conf + "|";
                          Serial.println(conf2);
                          laco3 = false;
                        }
                        else {
                          digitalWrite(redpin, HIGH);
                          delay(1000);
                          digitalWrite(redpin, LOW);
                        }
                      }
                      break;
  
                    case FireError::PRODUCE_EXCEPTION:
                      break;
  
                    case FireError::CONDITIONS_ARE_NOT_APPLIED:
                      break;
                 }
                 laco = false;
                 laco2 = false;
               }
             }
           }
         }

         if(tv[1]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "2";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           //Disparo da transição 3
           FireError fireError = pnrd.fire(3);
  
           switch(fireError){
  
              case FireError::NO_ERROR:
                while (laco3 == true){
                  //Salva a nova informação na tag
                  if(pnrd.saveData() == WriteError::NO_ERROR){
                    digitalWrite(greenpin, HIGH);
                    delay(1000);
                    digitalWrite(greenpin, LOW);
                    laco3 = false;
                  }
                  else {
                    digitalWrite(redpin, HIGH);
                    delay(1000);
                    digitalWrite(redpin, LOW);
                  }
                }
                break;
  
              case FireError::PRODUCE_EXCEPTION:
                break;
  
              case FireError::CONDITIONS_ARE_NOT_APPLIED:
                break;
           }
           laco = false;
         }

         if(tv[2]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "3";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           //Disparo da transição 4
           FireError fireError = pnrd.fire(4);
  
           switch(fireError){
  
              case FireError::NO_ERROR:
                while (laco3 == true){
                  //Salva a nova informação na tag
                  if(pnrd.saveData() == WriteError::NO_ERROR){
                    digitalWrite(greenpin, HIGH);
                    delay(1000);
                    digitalWrite(greenpin, LOW);
                    laco3 = false;
                  }
                  else {
                    digitalWrite(redpin, HIGH);
                    delay(1000);
                    digitalWrite(redpin, LOW);
                  }
                }
                break;
  
              case FireError::PRODUCE_EXCEPTION:
                break;
  
              case FireError::CONDITIONS_ARE_NOT_APPLIED:
                break;
           }
           laco = false;
         }

         if(tv[3]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "4";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           //Disparo da transição 5
           FireError fireError = pnrd.fire(5);

           switch(fireError){

              case FireError::NO_ERROR:
                while (laco3 == true){
                  //Salva a nova informação na tag
                  if(pnrd.saveData() == WriteError::NO_ERROR){
                    digitalWrite(greenpin, HIGH);
                    delay(1000);
                    digitalWrite(greenpin, LOW);
                    laco3 = false;
                  }
                  else {
                    digitalWrite(redpin, HIGH);
                    delay(1000);
                    digitalWrite(redpin, LOW);
                  }
                }
                break;

              case FireError::PRODUCE_EXCEPTION:
                break;

              case FireError::CONDITIONS_ARE_NOT_APPLIED:
                break;
           }
           laco = false;
         }

         if(tv[4]==1){
           estado = digitalRead(botao);
           
           //Manda os dados de localização e tagID para o Python
           lugar = "5";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           delay(3000);
           if (estado == HIGH){
             digitalWrite(yellowpin, HIGH);
             
             //Disparo da transição 10
             FireError fireError = pnrd.fire(10);
        
             switch(fireError){
        
                case FireError::NO_ERROR:
                  while (laco3 == true){
                    //Salva a nova informação na tag
                    if(pnrd.saveData() == WriteError::NO_ERROR){
                      digitalWrite(greenpin, HIGH);
                      delay(1000);
                      digitalWrite(greenpin, LOW);
                      cond = "quar";
                      cond2 = cond + "|";
                      Serial.println(cond2);
                      laco3 = false;
                    }
                    else {
                      digitalWrite(redpin, HIGH);
                      delay(1000);
                      digitalWrite(redpin, LOW);
                    }
                  }
                  break;
        
                case FireError::PRODUCE_EXCEPTION:
                  break;
        
                case FireError::CONDITIONS_ARE_NOT_APPLIED:
                  break;
             }
             digitalWrite(yellowpin, LOW);
             laco = false;
           }
        
           else{
             //Disparo da transição 6
             FireError fireError = pnrd.fire(6);
        
             switch(fireError){
        
                case FireError::NO_ERROR:
                  while (laco3 == true){
                    //Salva a nova informação na tag
                    if(pnrd.saveData() == WriteError::NO_ERROR){
                      digitalWrite(greenpin, HIGH);
                      delay(1000);
                      digitalWrite(greenpin, LOW);
                      cond = "cali";
                      cond2 = cond + "|";
                      Serial.println(cond2);
                      laco3 = false;
                    }
                    else {
                      digitalWrite(redpin, HIGH);
                      delay(1000);
                      digitalWrite(redpin, LOW);
                    }
                  }
                  break;
        
                case FireError::PRODUCE_EXCEPTION:
                  break;
        
                case FireError::CONDITIONS_ARE_NOT_APPLIED:
                  break;
             }
             laco = false;
           }
         }

         if(tv[5]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "6";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           //Disparo da transição 7
           FireError fireError = pnrd.fire(7);

           switch(fireError){

              case FireError::NO_ERROR:
                while (laco3 == true){
                  //Salva a nova informação na tag
                  if(pnrd.saveData() == WriteError::NO_ERROR){
                    digitalWrite(greenpin, HIGH);
                    delay(1000);
                    digitalWrite(greenpin, LOW);
                    laco3 = false;
                  }
                  else {
                    digitalWrite(redpin, HIGH);
                    delay(1000);
                    digitalWrite(redpin, LOW);
                  }
                }
                break;

              case FireError::PRODUCE_EXCEPTION:
                break;

              case FireError::CONDITIONS_ARE_NOT_APPLIED:
                break;
           }
           laco = false;
         }

         if(tv[6]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "5";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           //Disparo da transição 8
           FireError fireError = pnrd.fire(8);

           switch(fireError){

              case FireError::NO_ERROR:
                while (laco3 == true){
                  //Salva a nova informação na tag
                  if(pnrd.saveData() == WriteError::NO_ERROR){
                    digitalWrite(greenpin, HIGH);
                    delay(1000);
                    digitalWrite(greenpin, LOW);
                    laco3 = false;
                  }
                  else {
                    digitalWrite(redpin, HIGH);
                    delay(1000);
                    digitalWrite(redpin, LOW);
                  }
                }
                break;

              case FireError::PRODUCE_EXCEPTION:
                break;

              case FireError::CONDITIONS_ARE_NOT_APPLIED:
                break;
           }
           laco = false;
         }

         if(tv[7]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "4";
           dados = tagID + "|" + lugar;
           Serial.println(dados);

           //Disparo da transição 9
           FireError fireError = pnrd.fire(9);
  
           switch(fireError){
  
              case FireError::NO_ERROR:
                while (laco3 == true){
                  //Salva a nova informação na tag
                  if(pnrd.saveData() == WriteError::NO_ERROR){
                    digitalWrite(greenpin, HIGH);
                    delay(1000);
                    digitalWrite(greenpin, LOW);
                    laco3 = false;
                  }
                  else {
                    digitalWrite(redpin, HIGH);
                    delay(1000);
                    digitalWrite(redpin, LOW);
                  }
                }
                break;
  
              case FireError::PRODUCE_EXCEPTION:
                break;
  
              case FireError::CONDITIONS_ARE_NOT_APPLIED:
                break;
           }
           laco = false;
         }

         if(tv[8]==1){
           //Manda os dados de localização e tagID para o Python
           lugar = "7";
           dados = tagID + "|" + lugar;
           Serial.println(dados);
           
           digitalWrite(redpin, HIGH);
           delay(1000);
           digitalWrite(redpin, LOW);
           laco = false;
           modo = ' ';
         }
      }
    }
    digitalWrite(whitepin, LOW);
  }

}
}
