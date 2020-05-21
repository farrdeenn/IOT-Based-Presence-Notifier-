#include <SoftwareSerial.h>
#include<GSMSim.h>
GSMSim gsm;
SoftwareSerial GPRS(7, 8); // TX,RX

enum _parseState {
  PS_DETECT_MSG_TYPE,

  PS_IGNORING_COMMAND_ECHO,

  PS_READ_CMTI_STORAGE_TYPE,
  PS_READ_CMTI_ID,

  PS_READ_CMGR_STATUS,
  PS_READ_CMGR_NUMBER,
  PS_READ_CMGR_SOMETHING,
  PS_READ_CMGR_DATE,
  PS_READ_CMGR_CONTENT
};

byte state = PS_DETECT_MSG_TYPE;

char buffer[80];
byte pos = 0;

int lastReceivedSMSId = 0;
boolean validSender = false;

void resetBuffer() {
  memset(buffer, 0, sizeof(buffer));
  pos = 0;
}

void setup()
{
  GPRS.begin(9600);
  Serial.begin(9600);

  // Set as appropriate for your case


  for (int i = 1; i <= 15; i++) {
    GPRS.print("AT+CMGD=");
    GPRS.println(i);
    delay(200);

    // Not really necessary but prevents the serial monitor from dropping any input
    while(GPRS.available()) 
      Serial.write(GPRS.read());
  }
}

void loop()
{
  while(GPRS.available()) {
    parseATText(GPRS.read());
  }
}

void parseATText(byte b) {

  buffer[pos++] = b;

  if ( pos >= sizeof(buffer) )
    resetBuffer(); // just to be safe

  /*
   // Detailed debugging
   Serial.println();
   Serial.print("state = ");
   Serial.println(state);
   Serial.print("b = ");
   Serial.println(b);
   Serial.print("pos = ");
   Serial.println(pos);
   Serial.print("buffer = ");
   Serial.println(buffer);*/

  switch (state) {
  case PS_DETECT_MSG_TYPE: 
    {
      if ( b == '\n' )    
        resetBuffer();
      else {        
        if ( pos == 3 && strcmp(buffer, "AT+") == 0 ) {
          state = PS_IGNORING_COMMAND_ECHO;
        }
        else if ( pos == 6 ) {
          //Serial.print("Checking message type: ");
          //Serial.println(buffer);

          if ( strcmp(buffer, "+CMTI:") == 0 ) {
            Serial.println("Received CMTI");
            state = PS_READ_CMTI_STORAGE_TYPE;
          }
          else if ( strcmp(buffer, "+CMGR:") == 0 ) {
            Serial.println("Received CMGR");            
            state = PS_READ_CMGR_STATUS;
          }
          resetBuffer();
        }
      }
    }
    break;

  case PS_IGNORING_COMMAND_ECHO:
    {
      if ( b == '\n' ) {
        //Serial.print("Ignoring echo: ");
        //Serial.println(buffer);
        state = PS_DETECT_MSG_TYPE;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMTI_STORAGE_TYPE:
    {
      if ( b == ',' ) {
        Serial.print("SMS storage is ");
        Serial.println(buffer);
        state = PS_READ_CMTI_ID;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMTI_ID:
    {
      if ( b == '\n' ) {
        lastReceivedSMSId = atoi(buffer);
        Serial.print("SMS id is ");
        Serial.println(lastReceivedSMSId);

        GPRS.print("AT+CMGR=");
        GPRS.println(lastReceivedSMSId);
        //delay(500); don't do this!

        state = PS_DETECT_MSG_TYPE;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMGR_STATUS:
    {
      if ( b == ',' ) {
        Serial.print("CMGR status: ");
        Serial.println(buffer);
        state = PS_READ_CMGR_NUMBER;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMGR_NUMBER:
    {
      if ( b == ',' ) {
        Serial.print("CMGR number: ");
        Serial.println(buffer);
                parseSMSContent();

        // Uncomment these two lines to check the sender's cell number
        //validSender = false;
        //if ( strcmp(buffer, "\"+0123456789\",") == 0 )
        validSender = true;

        state = PS_READ_CMGR_SOMETHING;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMGR_SOMETHING:
    {
      if ( b == ',' ) {

        state = PS_READ_CMGR_DATE;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMGR_DATE:
    {
      if ( b == '\n' ) {
        Serial.print("CMGR date: ");
        Serial.println(buffer);
        state = PS_READ_CMGR_CONTENT;
        resetBuffer();
      }
    }
    break;

  case PS_READ_CMGR_CONTENT:
    {
      if ( b == '\n' ) {
        Serial.print("CMGR content: ");
        Serial.print(buffer);


        GPRS.print("AT+CMGD=");
        GPRS.println(lastReceivedSMSId);
        //delay(500); don't do this!

        state = PS_DETECT_MSG_TYPE;
        resetBuffer();
      }
    }
    break;
  }
}

void parseSMSContent() {
  char number[14];
  memcpy(number,buffer,sizeof(buffer)-66);
  number[0]=number[1];
  number[1]=number[2];
  number[2]=number[3];
  number[3]=number[4];
  number[4]=number[5];
  number[5]=number[6];
  number[6]=number[7];
  number[7]=number[8];
  number[8]=number[9];
  number[9]=number[10];
  number[10]=number[11];
  number[11]=number[12];
  number[12]=number[13];
  number[13]=number[14];
char number1[12];
memcpy(number1,number,sizeof(number)-1);
int buttonstate =0;
  pinMode(2, INPUT);
  buttonstate=digitalRead(2); 
   Serial.print("number is");
   Serial.print(number1);
delay(10000);
 if (buttonstate == LOW)
 
{
GPRS.begin(9600);
  GPRS.println("AT+CMGF=1");
  delay(2000);
  GPRS.print("AT+CMGS=\""); //replace x by your number
  GPRS.print(number1);
  GPRS.println("\"");
  delay(2000);
  GPRS.println("Arif Sir Is In His Office");
  delay(100);
  GPRS.println((char)26);
  delay(2000);
 /* 
  gsm.start();
  Serial.println(gsm.call(number1));
  delay(10000);
  gsm.callHangoff(); */
}
else
{
    GPRS.println("AT+CMGF=1");
  delay(2000);
  GPRS.print("AT+CMGS=\""); //replace x by your number
  GPRS.print(number1);
  GPRS.println("\"");
  delay(2000);
  GPRS.println("Arif Sir Is Not In His Ofiice");
  delay(100);
  GPRS.println((char)26);
  delay(2000);
}
}


