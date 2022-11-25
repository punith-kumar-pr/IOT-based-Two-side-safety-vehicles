const char simPIN[]   = "";
#define SMS_TARGET  "+916361776734"

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb
String smsMessage;

#include <TinyGsmClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 20, 4);

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23 
#define MODEM_TX             27
#define MODEM_RX             26

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT  Serial1

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

const int tiltPin = 18;
const int buzzerPin = 32;
const int impactPin = 34;
//ultrasonic
int trigPin = 14;
int echoPin = 27;

//PIR Motion sensor
int PIRPin = 25;
//motor
int in1 = 12;
int in2 = 13;
//led
int LEDPin = 15;


//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float distanceInch;

void setup() {
  
  //lat and long
    String lati ="12.9091635,77.5643664";
  //String longi ="77.573394";


  // Set console baud rate
  SerialMon.begin(115200);
  // LCD
  lcd. begin ();
  lcd. backlight ();
  lcd.print("BIT BY BIT");

  
  pinMode(impactPin,INPUT);
  pinMode(tiltPin,INPUT);

  pinMode(PIRPin, INPUT);
  //ultrasonic sensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  //motor driver pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(LEDPin, OUTPUT);
  
  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  
  smsMessage = "https://maps.google.com/maps?q=loc:"+lati;
  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.init();
  // use modem.init() if you don't need the complete restart

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
    
  }
  
}

void loop() {

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(LEDPin, HIGH);
  lcd.clear();
  lcd. setCursor (0, 0);
  lcd.print("TEAM BIT BY BIT");
  lcd. setCursor (0, 1);
  lcd.print("MOTOR RUNNING");
  Serial.println("motor Running");

// PREVENTION  --------------------------------------------------------
  // ultrasonic code
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = (duration * SOUND_SPEED)/2.0;
  distanceInch = distanceCm * CM_TO_INCH;
  
  //Prints the distance in the Serial Monitor
//  Serial.print("Distance (cm): ");
//  Serial.println(distanceCm);
//  Serial.print("Distance (inch): ");
//  Serial.println(distanceInch);

  // motion sensor and Ultra sonic
  if(digitalRead(PIRPin) == HIGH && distanceInch < 2.0){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(LEDPin, LOW);

    Serial.println("Obstacle detected , motor turning off");
    lcd.clear();
    lcd. setCursor (0, 0);
    lcd.print("TEAM BIT BY BIT");
    lcd. setCursor (0, 1);
    lcd.print("OBSTACLE PRESENT");
    delay(5000);
    }
//  else{
//    digitalWrite(in1, HIGH);
//    digitalWrite(in2, LOW);
//    lcd.clear();
//    lcd. setCursor (0, 0);
//    lcd.print("TEAM BIT BY BIT");
//    lcd. setCursor (0, 1);
//    lcd.print("MOTOR RUNNING");
//    Serial.println("motor Running");
//    delay(1000);
//  }
// ACCIDENT -----------------------------------------------------------
 if(digitalRead(tiltPin) == LOW || digitalRead(impactPin) == LOW){
    digitalWrite(buzzerPin,HIGH);
    if(modem.sendSMS(SMS_TARGET, smsMessage)){
      SerialMon.println(smsMessage);
    } 
    else{
      SerialMon.println("SMS failed to send due to network issue");
   }
   delay(5000);
  }//IF CLOSE
  else{ 
    digitalWrite(buzzerPin,LOW);
    SerialMon.println("Cool Bro");
  }
  //delay(5000);  
}
