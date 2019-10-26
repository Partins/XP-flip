/*
  Simple test program demonstrating how to interface with a Mobitec flip dot display
  using RS-485.
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "SSID";
const char* password = "PASSWORD";

const char* PARAM_INPUT_1 = "input1";

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup(){
  Serial.begin(4800);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //Serial.println("WiFi Failed!");
    return;
  }
  //Serial.println();
  //Serial.print("IP Address: ");
  //Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  write_text("    XP-el");
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    else {
      inputMessage = " ";
      inputParam = "none";
    }
    
    write_text(inputMessage.c_str());
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
    
  });
  server.onNotFound(notFound);
  server.begin();
  
}
/*
  Calculates the checksum and adds it to data. Size is adjusted accordingly.
*/
static void add_checksum(void *data, int *size)
{
  int csum = 0;
  int i;
  unsigned char *p = (unsigned char *)data;

  for(i=1; i<*size; i++)
  {
    csum += p[i];
  }

  p[i] = csum & 0xff;

  (*size)++;
  if(p[i] == 0xfe)
  {
    p[i+1] = 0x00;
    (*size)++;
  } else if(p[i] == 0xff)
  {
    p[i] = 0xfe;
    p[i+1] = 0x01;
    (*size)++;
  }
}

static int fd;

/* Simple header with start byte 0xff, sign address 0x06, and 0xa2 for text mark */
static const unsigned char sign_hdr[] = {0xff, 0x06, 0xa2, 0xf1, 0xd0, 0x70, 0xd1, 0x10};

/*
  Writes the supplied ascii string to the display
*/
static void write_text(const char* txt)
{
   char msg[2048];
  int len;
  
  memcpy(msg, sign_hdr, sizeof(sign_hdr));
  len = sizeof(sign_hdr);
  strcpy((char *)msg + len, txt);
  len += strlen(txt);
  add_checksum(msg, &len);
  msg[len++] = 0xff;
  Serial.print(msg);
  

}

/*
  Sets all pixels except the first column
*/
static void write_all_white(void)
{
  char buffer[1024];
  int len;

  len = 0;
  buffer[len++] = 0xd2; // Set x coordinate to 1
  buffer[len++] = 1;
  buffer[len++] = 0xd3; // Set y coordinate to 4
  buffer[len++] = 4;
  buffer[len++] = 0xd4; // Select bitmap font
  buffer[len++] = 0x77;
  // 0x20 - 0x3f is used with the bitmap font
  // 0x20 is no pixels set and 0x3f is all pixels set
  memset(buffer+len, 0x3f, 112);
  len += 112;

  buffer[len++] = 0xd2;
  buffer[len++] = 1;
  buffer[len++] = 0xd3;
  buffer[len++] = 9;
  buffer[len++] = 0xd4;
  buffer[len++] = 0x77;
  memset(buffer+len, 0x3f, 112);
  len += 112;

  buffer[len++] = 0xd2;
  buffer[len++] = 1;
  buffer[len++] = 0xd3;
  buffer[len++] = 14;
  buffer[len++] = 0xd4;
  buffer[len++] = 0x77;
  memset(buffer+len, 0x3f, 112);
  len += 112;

  buffer[len++] = 0xd2;
  buffer[len++] = 1;
  buffer[len++] = 0xd3;
  buffer[len++] = 19;
  buffer[len++] = 0xd4;
  buffer[len++] = 0x77;
  memset(buffer+len, 0x3f, 112);
  len += 112;

  buffer[len] = '\0';
  write_text(buffer);
}

void loop()
{

delay(30000);
write_text("    XP-el");

  
 

  
}
