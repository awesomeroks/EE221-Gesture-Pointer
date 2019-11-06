
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
String html_1 = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body     { font-size:120%;} 
    #main    { display: table; width: 300px; margin: auto;  padding: 10px 10px 10px 10px; border: 3px solid blue; border-radius: 10px; text-align:center;} 
    #BTN_LED { width:200px; height:40px; font-size: 110%;  }
    p        { font-size: 75%; }
  </style>
 
  <title>MPU6050</title>
</head>
<body>
  <div id='main'>
    <h3>MPU6050 Data</h3>
    <p>Ax = <span id='ax'>---</span> </p>
    <p>Ay = <span id='ay'>---</span> </p>
    <p>Az = <span id='az'>---</span> </p>
    <p>Gx = <span id='gx'>---</span> </p>
    <p>Gy = <span id='gy'>---</span> </p>
    <p>Gz = <span id='gz'>---</span> </p>
    <p>T = <span id='temp'>---</span> </p>
    <button id = "calibrate" type="button" onclick="buttonclick();">Calibrate</button>
    <br />
    
   </div>
</body>
 
<script>
  var Socket;
  function init() 
  {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    Socket.onmessage = function(event) { processReceivedCommand(event); };
  }
  
 function buttonclick() {
    Socket.send('1');
  }
 
function processReceivedCommand(evt) 
{
    
    
    var val = evt.data.split("\n");
    document.getElementById('ax').innerHTML = val[0];
    document.getElementById('ay').innerHTML = val[1];
    document.getElementById('az').innerHTML = val[2];
    document.getElementById('gx').innerHTML = val[3];
    document.getElementById('gy').innerHTML = val[4];
    document.getElementById('gz').innerHTML = val[5];
    document.getElementById('temp').innerHTML = val[6];
}
 


 
  window.onload = function(e)
  { 
    init();
  }
</script>
 
 
</html>
)=====";
 
#include <Wire.h>
#include <SimpleKalmanFilter.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* WIFI_SSID     = "redmi";
const char* WIFI_PASS = "abcdefgh";

int a = 30;
int b = 30;
float u = 0.01;
SimpleKalmanFilter kAx(a,b,u);
SimpleKalmanFilter kAy(a,b,u);
SimpleKalmanFilter kAz(a,b,u);
SimpleKalmanFilter kGx(a,b,u);
SimpleKalmanFilter kGy(a,b,u);
SimpleKalmanFilter kGz(a,b,u);

//String output5State = "off";
//String output4State = "off";

long currenttime = 0;
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D1;
const uint8_t sda = D2;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;
double Ax, Ay, Az, T, Gx, Gy, Gz, calAx,calAy,calAz,calGx,calGy,calGz;
int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

double kalAx,kalAy,kalAz,kalGz,kalGx,kalGy;



void setup() {
  Serial.begin(9600);
  Wire.begin(sda, scl);
  MPU6050_Init();
  WiFi.mode(WIFI_STA);

   // Connect
   Serial.println ();
   Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
   Serial.println();
   WiFi.begin(WIFI_SSID, WIFI_PASS);

   // Wait
   while (WiFi.status() != WL_CONNECTED) 
   {
      Serial.print(".");
      delay(100);
   }
   Serial.print(" ==> CONNECTED!" );
   Serial.println();

   // Connected!
   Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
   Serial.println();
  server.begin();
   webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
   
   
    
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
      //divide each with their sensitivity scale factor
      Ax = (double)AccelX/AccelScaleFactor + calAx;
      Ay = (double)AccelY/AccelScaleFactor + calAy;
      Az = (double)AccelZ/AccelScaleFactor + calAz;
      T = (double)Temperature/340+36.53; //temperature formula
      Gx = (double)GyroX/GyroScaleFactor   + calGx;
      Gy = (double)GyroY/GyroScaleFactor   + calGy;
      Gz = (double)GyroZ/GyroScaleFactor   + calGz;
      kalAx = kAx.updateEstimate(Ax*1000);
      kalAy = kAy.updateEstimate(Ay*1000);
      kalAz = kAz.updateEstimate(Az*1000);

      kalGx = Gx;
      kalGy = Gy;
      kalGz = Gz;
//      kalGx = kGx.updateEstimate(Gx);
//      kalGy = kGy.updateEstimate(Gy);
//      kalGz = kGz.updateEstimate(Gz);
      Serial.print(" "); Serial.print(kalAx);
      Serial.print(" "); Serial.print(kalAy);
      Serial.print(" "); Serial.print(kalAz);
      Serial.print(" "); Serial.print(kalGx);
      Serial.print(" "); Serial.print(kalGy);
      Serial.print(" "); Serial.println(kalGz);
      webSocket.broadcastTXT(String(kalAx) +"\n" + String(kalAy) +"\n" + String(kalAz) +"\n" + String(kalGx) +"\n" + String(kalGy) +"\n" + String(kalGz)+"\n" +String(T)+"\n"  );
      delay(10);  
      
  webSocket.loop();
  WiFiClient client = server.available();
    if (!client)  {  return;  }
 
    client.flush();
    client.print( header );
    client.print( html_1 ); 
    Serial.println("New page served");
 
    delay(5);
  
  
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

void calibrate(){
  calAx = - Ax;
  calAy = - Ay;
  calAz = - Az;
  calGx = - Gx;
  calGy = - Gy;
  calGz = - Gz;
  kalAx=kalAy=kalAz=kalGz=kalGx=kalGy=0;
  
  }

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  calibrate();
  Serial.println("PAYLOAD RECEIVED");
//  if(type == WStype_TEXT)
//  {
//      if((char)payload[0] == '1'){
//        calibrate();
//      }
//  }
// 
//  else 
//  {
//    Serial.print("WStype = ");   Serial.println(type);  
//    Serial.print("WS payload = ");
//    for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
//    Serial.println();
//    
// 
//  }
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
