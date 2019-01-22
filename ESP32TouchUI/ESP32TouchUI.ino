// Based on Code from Mudassar Tamboli: https://github.com/mudassar-tamboli/ESP32-OV7670-WebSocket-Camera

#include "OV7670.h"

#include <WebSocketsServer.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "canvas_htm.h"
#include <ArduinoJson.h> // tested for version 5.13.3
WiFiMulti WiFiMulti;
#include <Ticker.h>
Ticker temperTick, voltTick;
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;        
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

volatile uint8_t websock_num = 0, info_available = 0, output_ready = 0;
char info_buffer[80];


const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 26;
const int D2 = 25;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

boolean APmode = true;

#define ssid        "xxxxx"
#define password    "xxxxx"


OV7670 *camera;


WiFiServer server(80);

unsigned char pix = 0;

//unsigned char bmpHeader[BMP::headerSize];

unsigned char start_flag = 0xAA;
unsigned char end_flag = 0xFF;
unsigned char ip_flag = 0x11;

WebSocketsServer webSocket(81);    // create a websocket server on port 81

// motor outputs
const int MA1 = 18;
const int MA2 = 19;
const int MB1 = 16;
const int MB2 = 17;

// setting PWM properties
const int A1Ch = 4;
const int A2Ch = 5;
const int B1Ch = 6;
const int B2Ch = 7;

const int ADCbat = 36; // VP pin

void sendTemperature() {
  //Serial.println(temperatureRead());
  sprintf(info_buffer, "{\"temp\":\"%f\"}", temperatureRead());
  info_available = 1;
}

void sendVoltage() {
  double reading = analogRead(ADCbat); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  //if(reading < 1 || reading > 4095) return 0;
  reading = -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
  float batVoltage = reading*2;//analogRead(ADCbat)/4095.0*3.3*2; //12bit ADC, 50:50 voltage divider
  sprintf(info_buffer, "{\"volt\":\"%f.2\"}", batVoltage);
  info_available = 1;
}

double ReadVoltage(){
  }



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t payloadlength) { // When a WebSocket message is received

  int blk_count = 0;

  char canvas_VGA[] = "canvas-VGA";
  char canvas_Q_VGA[] = "canvas-Q-VGA";
  char canvas_QQ_VGA[] = "canvas-QQ-VGA";
  char canvas_QQQ_VGA[] = "canvas-QQQ-VGA";
  char ipaddr[26] ;
  IPAddress localip;
  //Serial.printf("Get: %s Char0: %c\r\n", payload, payload[0]);

  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendBIN(0, &ip_flag, 1);
        localip = WiFi.localIP();
        sprintf(ipaddr, "%d.%d.%d.%d", localip[0], localip[1], localip[2], localip[3]);
        webSocket.sendTXT(0, (const char *)ipaddr);

      }
      break;
    case WStype_TEXT:                     // if new text data is received

      if (payload[0] == '{') { // check if json command was received
        DynamicJsonBuffer json_input;
        JsonObject& root = json_input.parseObject((const char *)payload);
        const char *cmd = root["cmd"];
        const int val = root["val"];

        if (strstr(cmd, "power")) {
          Serial.println("power off command");
        }
        else if (strstr(cmd, "cam")) {
          Serial.println("camera command");
        }
        else if (strstr(cmd, "light")) {
          digitalWrite(2, val);
        }
        else if (strstr(cmd, "pos")) {
          const int val2 = root["val2"];
          if(val > 0){ //4xPWM motor control
            ledcWrite(A1Ch, val);
            ledcWrite(A2Ch, 0);
          } else {
            ledcWrite(A1Ch, 0);
            ledcWrite(A2Ch, abs(val));
          }      
          if(val2 > 0){
            ledcWrite(B1Ch, val2);
            ledcWrite(B2Ch, 0);
          } else {
            ledcWrite(B1Ch, 0);
            ledcWrite(B2Ch, abs(val2));
          } 
          Serial.printf("Motor input received: %dL %dR\r\n", val, val2);
        }
        else {
          Serial.println("Unknown command");
          //sendMSG("INFO","ESP32: Unknown command received");
        }
      }
      else {
        if (payloadlength == sizeof(canvas_QQQ_VGA) - 1) {
          if (memcmp(canvas_QQQ_VGA, payload, payloadlength) == 0) {
            Serial.printf("canvas_QQQ_VGA");
            webSocket.sendBIN(0, &end_flag, 1);
            camera = new OV7670(OV7670::Mode::QQQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
          }
        } else if (payloadlength == sizeof(canvas_QQ_VGA) - 1) {
          if (memcmp(canvas_QQ_VGA, payload, payloadlength) == 0) {
            Serial.printf("canvas_QQ_VGA");
            webSocket.sendBIN(0, &end_flag, 1);
            camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
          }
        } else if (payloadlength == sizeof(canvas_Q_VGA) - 1) {
          if (memcmp(canvas_Q_VGA, payload, payloadlength) == 0) {
            Serial.printf("canvas_Q_VGA");
            webSocket.sendBIN(0, &end_flag, 1);
            camera = new OV7670(OV7670::Mode::QVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
          }
        } else if (payloadlength == sizeof(canvas_VGA) - 1) {
          if (memcmp(canvas_VGA, payload, payloadlength) == 0) {
            Serial.printf("canvas_VGA");
            webSocket.sendBIN(0, &end_flag, 1);
            camera = new OV7670(OV7670::Mode::VGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
          }
        }
        //else if "next-frame" do camera stuff:

        blk_count = camera->yres / I2SCamera::blockSlice; //30, 60, 120
        for (int i = 0; i < blk_count; i++) {

          if (i == 0) {
            camera->startBlock = 1;
            camera->endBlock = I2SCamera::blockSlice;
            webSocket.sendBIN(0, &start_flag, 1);
          }

          if (i == blk_count - 1) {
            webSocket.sendBIN(0, &end_flag, 1);
          }

          camera->oneFrame();
          webSocket.sendBIN(0, camera->frame, camera->xres * I2SCamera::blockSlice * 2);
          camera->startBlock += I2SCamera::blockSlice;
          camera->endBlock   += I2SCamera::blockSlice;
        }
      }
      break;
    case WStype_ERROR:                     // if new text data is received
      Serial.printf("Error \n");
    default:
      Serial.printf("WStype %x not handled \n", type);

  }
}



void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); //headlights
  pinMode(5, OUTPUT); //User LED on ESP32 Robot Board
  delay(1000);
  if(APmode){
    WiFi.softAP("ESP32bot");
  }
  else{
    WiFiMulti.addAP(ssid, password);
    while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print("."); delay(500);
    }
    Serial.print("\nConnected to ");  Serial.print(ssid);
    Serial.print(" with IP address: "); Serial.println(WiFi.localIP());
  }
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  adcAttachPin(ADCbat);
  analogSetClockDiv(255); // 1338mS

  delay(1000);
  temperTick.attach_ms(2000, sendTemperature);
  voltTick.attach_ms(5000, sendVoltage);

  ledcAttachPin(MA1, A1Ch);
  ledcAttachPin(MA2, A2Ch);
  ledcAttachPin(MB1, B1Ch);
  ledcAttachPin(MB2, B2Ch);
  
  ledcSetup(A1Ch, 5000, 8);
  ledcSetup(A2Ch, 5000, 8);
  ledcSetup(B1Ch, 5000, 8);
  ledcSetup(B2Ch, 5000, 8);
  
  server.begin();
}

void loop(){
  
  webSocket.loop();
  
  if (info_available == 1) {
    info_available = 0;
    webSocket.sendTXT(websock_num, info_buffer, strlen(info_buffer));
  }

  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(canvas_htm);
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
  }

}
