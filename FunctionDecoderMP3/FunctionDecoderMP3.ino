// DCC Smile Function Decoder MP3
// DFPlayer miniと組み合わせて使える、MP3 Sound Decoder
// 
// mp3フォルダに、0001.mp3 0002.mp3 の様に4桁の数字ファイル名で格納
// ファンクションは F0:0001.mp3 〜 F10:0011.mp3 に対応
// 音量はCV58
//
// http://dcc.client.jp
// http://ayabu.blog.shinobi.jp

#include <arduino.h>
#include "DccCV.h"
#include "NmraDcc.h"

#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

#define DEBUG      //リリースのときはコメントアウトすること

//各種設定、宣言
unsigned int cvtbleNow = 0;
unsigned char cvtblePrev[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//使用クラスの宣言
NmraDcc   Dcc;
DCC_MSG  Packet;

SoftwareSerial mySoftwareSerial(A5, A4); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

struct CVPair {
  uint16_t  CV;
  uint8_t Value;
};

CVPair FactoryDefaultCVs [] = {
  {CV_MULTIFUNCTION_PRIMARY_ADDRESS, DECODER_ADDRESS}, // CV01
  {CV_ACCESSORY_DECODER_ADDRESS_MSB, 0},               // CV09 The LSB is set CV 1 in the libraries .h file, which is the regular address location, so by setting the MSB to 0 we tell the library to use the same address as the primary address. 0 DECODER_ADDRESS
  {CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB, 0},          // CV17 XX in the XXYY address
  {CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB, 0},          // CV18 YY in the XXYY address
  {CV_MP3_Vol,20},                                     // DFPlayer mini MP3 Volum
  {CV_dummy,0},
};

void(* resetFunc) (void) = 0;  //declare reset function at address 0
uint8_t FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs) / sizeof(CVPair);
void notifyDccReset(uint8_t hardReset );
void Dccinit(void);



//------------------------------------------------------------------
// Arduino固有の関数 setup() :初期設定
//------------------------------------------------------------------
void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Hello,Smile Function Decoder");
#endif

  // ソフトウエアシリアル通信レートセット:
  mySoftwareSerial.begin(9600);

  pinMode(A3, INPUT);  //Mp3 Busy pin 設定
  digitalWrite(A3, HIGH); //Internal pull up enabled

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(gCV58_MP3_Vol);  //Set volume value. From 0 to 30

  //ファンクションの割り当てピン初期化
  Dccinit();

  //Reset task
  gPreviousL5 = millis();
}

//---------------------------------------------------------------------
// Arduino main loop
//---------------------------------------------------------------------
void loop() {
  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();

  if ( (millis() - gPreviousL5) >= 100) // 100:100msec  10:10msec  Function decoder は 10msecにしてみる。
  {
    FunctionProcess();
    gPreviousL5 = millis();
  }
}

//---------------------------------------------------------------------
//ファンクション受信によるイベント
//---------------------------------------------------------------------
void FunctionProcess(void){

#ifdef DEBUG
  gStateView();     // gState変数を表示
#endif
  
// F0 受信時の処理
    if(gState_F0 == 0 && cvtblePrev[0] == ON){
      myDFPlayer.stop();
      cvtblePrev[0] = OFF;    
    } else if(gState_F0 == 1 && cvtblePrev[0] == OFF){
      myDFPlayer.play(1);
      cvtblePrev[0] = ON;      
    }

// F1 受信時の処理
    if(gState_F1 == 0 && cvtblePrev[1] == ON){
      myDFPlayer.stop();
      cvtblePrev[1] = OFF;    
    } else if(gState_F1 == 1 && cvtblePrev[1] == OFF){
      myDFPlayer.play(2);
      cvtblePrev[1] = ON;      
    }
    
// F2 受信時の処理 gState_F2 に 2が入る
    if(gState_F2 == 0 && cvtblePrev[2] == ON){
      myDFPlayer.stop();
      cvtblePrev[2] = OFF;  
    } else if(gState_F2 == 1 && cvtblePrev[2]== OFF){
      myDFPlayer.play(3);
      cvtblePrev[2] = ON;  
    }

// F3 受信時の処理
    if(gState_F3 == 0 && cvtblePrev[3] == ON){
      myDFPlayer.stop();
      cvtblePrev[3] = OFF;  
    } else if(gState_F3 == 1 && cvtblePrev[3]== OFF){
      myDFPlayer.play(4);
      cvtblePrev[3] = ON;  
    }
    
// F4 受信時の処理
    if(gState_F4 == 0 && cvtblePrev[4] == ON){
      myDFPlayer.stop();
      cvtblePrev[4] = OFF;  
    } else if(gState_F4 == 1 && cvtblePrev[4]== OFF){
      myDFPlayer.play(5);
      cvtblePrev[4] = ON;  
    }
    
// F5 受信時の処理
    if(gState_F5 == 0 && cvtblePrev[5] == ON){
      myDFPlayer.stop();
      cvtblePrev[5] = OFF;  
    } else if(gState_F5 == 1 && cvtblePrev[5]== OFF){
      myDFPlayer.play(6);
      cvtblePrev[5] = ON;  
    }
    
// F6 受信時の処理
    if(gState_F6 == 0 && cvtblePrev[6] == ON){
      myDFPlayer.stop();
      cvtblePrev[6] = OFF;  
    } else if(gState_F6 == 1 && cvtblePrev[6]== OFF){
      myDFPlayer.play(7);
      cvtblePrev[6] = ON;  
    }

// F7 受信時の処理
    if(gState_F7 == 0 && cvtblePrev[7] == ON){
      myDFPlayer.stop();
      cvtblePrev[7] = OFF;  
    } else if(gState_F7 == 1 && cvtblePrev[7]== OFF){
      myDFPlayer.play(8);
      cvtblePrev[7] = ON;  
    }

// F8 受信時の処理
    if(gState_F8 == 0 && cvtblePrev[8] == ON){
      myDFPlayer.stop();
      cvtblePrev[8] = OFF;  
    } else if(gState_F8 == 1 && cvtblePrev[8]== OFF){
      myDFPlayer.play(9);
      cvtblePrev[8] = ON;  
    }

// F9 受信時の処理
    if(gState_F9 == 0 && cvtblePrev[9] == ON){
      myDFPlayer.stop();
      cvtblePrev[9] = OFF;  
    } else if(gState_F9 == 1 && cvtblePrev[9]== OFF){
      myDFPlayer.play(10);
      cvtblePrev[9] = ON;  
    }

// F10 受信時の処理
    if(gState_F10 == 0 && cvtblePrev[10] == ON){
      myDFPlayer.stop();
      cvtblePrev[10] = OFF;  
    } else if(gState_F10 == 1 && cvtblePrev[10]== OFF){
      myDFPlayer.play(11);
      cvtblePrev[10] = ON;  
    }
}

//---------------------------------------------------------------------
//DCC速度信号の受信によるイベント
//extern void notifyDccSpeed( uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t MaxSpeed )
//---------------------------------------------------------------------
extern void notifyDccSpeed( uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps )
{
    uint16_t aSpeedRef = 0;
  //速度値の正規化(255を100%とする処理)
  if( Speed >= 1)
  {
    aSpeedRef = ((Speed - 1) * 255) / SpeedSteps;
  }
  else
  {
    //緊急停止信号受信時の処理 //Nagoden comment 2016/06/11
    #ifdef DEBUG
        Serial.println("***** Emagency STOP **** ");
    #endif
    aSpeedRef = 0;
  
  }

  #ifdef DEBUG
    // デバッグメッセージ
    Serial.print("Speed - ADR: ");
    Serial.print(Addr);
    Serial.print(", SPD: ");
    Serial.print(Speed);
    Serial.print(", DIR: ");
    Serial.print(Dir);
    Serial.print(", aSpeedRef: ");
    Serial.println(aSpeedRef);
  #endif

}


//---------------------------------------------------------------------------
//ファンクション信号受信のイベント
//FN_0_4とFN_5_8は常時イベント発生（DCS50KはF8まで）
//FN_9_12以降はFUNCTIONボタンが押されたときにイベント発生
//前値と比較して変化あったら処理するような作り。
//---------------------------------------------------------------------------
//extern void notifyDccFunc( uint16_t Addr, FN_GROUP FuncGrp, uint8_t FuncState)
extern void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState)
{
  if( FuncGrp == FN_0_4){  // F0〜F4の解析
    gState_F0 = (FuncState & FN_BIT_00) ? 1 : 0;
    gState_F1 = (FuncState & FN_BIT_01) ? 1 : 0;
    gState_F2 = (FuncState & FN_BIT_02) ? 1 : 0;
    gState_F3 = (FuncState & FN_BIT_03) ? 1 : 0;
    gState_F4 = (FuncState & FN_BIT_04) ? 1 : 0;
  }

  if( FuncGrp == FN_5_8){  // F5〜F8の解析
    gState_F5 = (FuncState & FN_BIT_05) ? 1 : 0;
    gState_F6 = (FuncState & FN_BIT_06) ? 1 : 0;
    gState_F7 = (FuncState & FN_BIT_07) ? 1 : 0;
    gState_F8 = (FuncState & FN_BIT_08) ? 1 : 0;
  }
  
  if( FuncGrp == FN_9_12) { // F9〜F12の解析
    gState_F9 = (FuncState & FN_BIT_09) ? 1 : 0;
    gState_F10 = (FuncState & FN_BIT_10) ? 1 : 0;
    gState_F11 = (FuncState & FN_BIT_11) ? 1 : 0;
    gState_F12 = (FuncState & FN_BIT_12) ? 1 : 0;
  }

  if( FuncGrp == FN_13_20){   // F13〜F20の解析
    gState_F13 = (FuncState & FN_BIT_13) ? 1 : 0;
    gState_F14 = (FuncState & FN_BIT_14) ? 1 : 0;
    gState_F15 = (FuncState & FN_BIT_15) ? 1 : 0;
    gState_F16 = (FuncState & FN_BIT_16) ? 1 : 0;
    gState_F17 = (FuncState & FN_BIT_17) ? 1 : 0;
    gState_F18 = (FuncState & FN_BIT_18) ? 1 : 0;
    gState_F19 = (FuncState & FN_BIT_19) ? 1 : 0;
    gState_F20 = (FuncState & FN_BIT_20) ? 1 : 0;
  }
  
}

//------------------------------------------------------------------
// CVをデフォルトにリセット(Initialize cv value)
// Serial.println("CVs being reset to factory defaults");
//------------------------------------------------------------------
void resetCVToDefault()
{
  for (int j = 0; j < FactoryDefaultCVIndex; j++ ) {
    Dcc.setCV( FactoryDefaultCVs[j].CV, FactoryDefaultCVs[j].Value);
  }
};

//------------------------------------------------------------------
// CV8 によるリセットコマンド受信処理
//------------------------------------------------------------------
void notifyCVResetFactoryDefault()
{
  //When anything is writen to CV8 reset to defaults.

  resetCVToDefault();
  delay(1000);  //typical CV programming sends the same command multiple times - specially since we dont ACK. so ignore them by delaying
  resetFunc();
};

//------------------------------------------------------------------
// CV Ackの処理
// そこそこ電流を流さないといけない
//------------------------------------------------------------------
void notifyCVAck(void)
{
//サーボモータを動かすとギミックを壊しかねないのでコメントアウト
//Serial.println("notifyCVAck");
#if 0 
  digitalWrite(O1,HIGH);
  digitalWrite(O2,HIGH);
  digitalWrite(O3,HIGH);
  digitalWrite(O4,HIGH);
  delay( 6 );
  digitalWrite(O4,LOW);
  digitalWrite(O3,LOW);
  digitalWrite(O2,LOW);
  digitalWrite(O1,LOW);
#endif
//MOTOR_Ack();
}

void MOTOR_Ack(void)
{
//  analogWrite(O4, 128);
//  delay( 6 );  
//  analogWrite(O4, 0);
}

//------------------------------------------------------------------
// DCC初期化処理）
//------------------------------------------------------------------
void Dccinit(void)
{

  //DCCの応答用負荷ピン
#if defined(DCCACKPIN)
  //Setup ACK Pin
  pinMode(DccAckPin, OUTPUT);
  digitalWrite(DccAckPin, 0);
#endif

#if !defined(DECODER_DONT_DEFAULT_CV_ON_POWERUP)
  if ( Dcc.getCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS) == 0xFF ) {   //if eeprom has 0xFF then assume it needs to be programmed
    notifyCVResetFactoryDefault();
  } else {
    Serial.println("CV Not Defaulting");
  }
#else
  Serial.println("CV Defaulting Always On Powerup");
  notifyCVResetFactoryDefault();
#endif

  // Setup which External Interrupt, the Pin it's associated with that we're using, disable pullup.
  Dcc.pin(0, 2, 0);   // ArduinoNANO D2(PD2)pinをDCC信号入力端子に設定

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_NUMBER, 100,   FLAGS_MY_ADDRESS_ONLY , 0 );

  //Init CVs
  gCV1_SAddr = Dcc.getCV( CV_MULTIFUNCTION_PRIMARY_ADDRESS ) ;
  gCV1_SAddr = Dcc.getCV( CV_MULTIFUNCTION_PRIMARY_ADDRESS ) ;
  gCVx_LAddr = (Dcc.getCV( CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB ) << 8) + Dcc.getCV( CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB );
  gCV58_MP3_Vol = Dcc.getCV( CV_MP3_Vol ) ;

}


//------------------------------------------------------------------
// CV値が変化した時の処理（特に何もしない）
//------------------------------------------------------------------
extern void     notifyCVChange( uint16_t CV, uint8_t Value) {
   //CVが変更されたときのメッセージ
  #ifdef DEBUG
    Serial.print("CV "); 
    Serial.print(CV); 
    Serial.print(" Changed to "); 
    Serial.println(Value, DEC);
  #endif
};


//------------------------------------------------------------------
// Resrt処理（特に何もしない）
//------------------------------------------------------------------
void notifyDccReset(uint8_t hardReset )
{
}


//------------------------------------------------------------------
// gState_F*の内容を表示（デバック用）
//------------------------------------------------------------------

void gStateView(){
  Serial.print(gState_F0);
  Serial.print(",");
  Serial.print(gState_F1);
  Serial.print(",");
  Serial.print(gState_F2);
  Serial.print(",");
  Serial.print(gState_F3);
  Serial.print(",");
  Serial.print(gState_F4);
  Serial.print(",");
  Serial.print(gState_F5);
  Serial.print(",");
  Serial.print(gState_F6);
  Serial.print(",");
  Serial.print(gState_F7);
  Serial.print(",");
  Serial.print(gState_F8);
  Serial.print(",");
  Serial.print(gState_F9);
  Serial.print(",");
  Serial.print(gState_F10);
  Serial.print(",");
  Serial.print(gState_F11);
  Serial.print(",");
  Serial.print(gState_F12);
  Serial.print(",");
  Serial.print(gState_F13);
  Serial.print(",");
  Serial.print(gState_F14);
  Serial.print(",");
  Serial.print(gState_F15);
  Serial.print(",");
  Serial.print(gState_F16);
  Serial.print(",");
  Serial.print(gState_F17);
  Serial.print(",");
  Serial.print(gState_F18);
  Serial.print(",");
  Serial.print(gState_F19);
  Serial.print(",");
  Serial.println(gState_F20);
}
