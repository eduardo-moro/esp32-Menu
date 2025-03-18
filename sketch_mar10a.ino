/*
 * ESP32 Menu System with MQTT Control
 */ 

// Import libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <qrcode_st7789.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>

#define BAUD_RATE 115200

// TFT display pin definition
#define TFT_DC  2
#define TFT_RST 4
#define TFT_CS  5

// TFT display configuration
#define TFT_WIDTH 240
#define TFT_HEIGHT 280
#define TFT_ROTATION 2

// Button pin definition
#define UP_BTN 13
#define DOWN_BTN 12
#define LEFT_BTN 14
#define RIGHT_BTN 27
#define MID_BTN 26
#define SET_BTN 25
#define RESET_BTN 33

// Brightness pin definition
#define BRIGHTNESS_PIN 22


// Color definitions
#define BACKGROUND 0xFFFF
#define PRIMARY_COLOR 0x0000
#define SECONDARY_COLOR 0x4BF3

/*
 * MQTT CONNECTION
 */
 const char* ca_cert= \
"-----BEGIN CERTIFICATE-----\n"
"MIIFBjCCAu6gAwIBAgIRAIp9PhPWLzDvI4a9KQdrNPgwDQYJKoZIhvcNAQELBQAw" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw" \
"WhcNMjcwMzEyMjM1OTU5WjAzMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg" \
"RW5jcnlwdDEMMAoGA1UEAxMDUjExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB" \
"CgKCAQEAuoe8XBsAOcvKCs3UZxD5ATylTqVhyybKUvsVAbe5KPUoHu0nsyQYOWcJ" \
"DAjs4DqwO3cOvfPlOVRBDE6uQdaZdN5R2+97/1i9qLcT9t4x1fJyyXJqC4N0lZxG" \
"AGQUmfOx2SLZzaiSqhwmej/+71gFewiVgdtxD4774zEJuwm+UE1fj5F2PVqdnoPy" \
"6cRms+EGZkNIGIBloDcYmpuEMpexsr3E+BUAnSeI++JjF5ZsmydnS8TbKF5pwnnw" \
"SVzgJFDhxLyhBax7QG0AtMJBP6dYuC/FXJuluwme8f7rsIU5/agK70XEeOtlKsLP" \
"Xzze41xNG/cLJyuqC0J3U095ah2H2QIDAQABo4H4MIH1MA4GA1UdDwEB/wQEAwIB" \
"hjAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwEgYDVR0TAQH/BAgwBgEB" \
"/wIBADAdBgNVHQ4EFgQUxc9GpOr0w8B6bJXELbBeki8m47kwHwYDVR0jBBgwFoAU" \
"ebRZ5nu25eQBc4AIiMgaWPbpm24wMgYIKwYBBQUHAQEEJjAkMCIGCCsGAQUFBzAC" \
"hhZodHRwOi8veDEuaS5sZW5jci5vcmcvMBMGA1UdIAQMMAowCAYGZ4EMAQIBMCcG" \
"A1UdHwQgMB4wHKAaoBiGFmh0dHA6Ly94MS5jLmxlbmNyLm9yZy8wDQYJKoZIhvcN" \
"AQELBQADggIBAE7iiV0KAxyQOND1H/lxXPjDj7I3iHpvsCUf7b632IYGjukJhM1y" \
"v4Hz/MrPU0jtvfZpQtSlET41yBOykh0FX+ou1Nj4ScOt9ZmWnO8m2OG0JAtIIE38" \
"01S0qcYhyOE2G/93ZCkXufBL713qzXnQv5C/viOykNpKqUgxdKlEC+Hi9i2DcaR1" \
"e9KUwQUZRhy5j/PEdEglKg3l9dtD4tuTm7kZtB8v32oOjzHTYw+7KdzdZiw/sBtn" \
"UfhBPORNuay4pJxmY/WrhSMdzFO2q3Gu3MUBcdo27goYKjL9CTF8j/Zz55yctUoV" \
"aneCWs/ajUX+HypkBTA+c8LGDLnWO2NKq0YD/pnARkAnYGPfUDoHR9gVSp/qRx+Z" \
"WghiDLZsMwhN1zjtSC0uBWiugF3vTNzYIEFfaPG7Ws3jDrAMMYebQ95JQ+HIBD/R" \
"PBuHRTBpqKlyDnkSHDHYPiNX3adPoPAcgdF3H2/W0rmoswMWgTlLn1Wu0mrks7/q" \
"pdWfS6PJ1jty80r2VKsM/Dj3YIDfbjXKdaFU5C+8bhfJGqU3taKauuz0wHVGT3eo" \
"6FlWkWYtbt4pgdamlwVeZEW+LM7qZEJEsMNPrfC03APKmZsJgpWCDWOKZvkZcvjV" \
"uYkQ4omYCTX5ohy+knMjdOmdH9c7SpqEWBDC86fiNex+O0XOMEZSa8DA" \
"-----END CERTIFICATE-----\n";
const char *mqtt_broker = "a58bfb515e3d44e4bbec101bf408bb32.s1.eu.hivemq.cloud";
const char *topic = "esp/control";
const char *mqtt_username = "esp01";
const char *mqtt_password = "Esp32_01";
const int mqtt_port = 8883;
unsigned long lastMqttAttempt = 0;
const unsigned long mqttReconnectInterval = 2000;
#define MQTT_VERSION MQTT_VERSION_3_1


// Button state
bool lastStateUp = HIGH;
bool lastStateDown = HIGH;
bool lastStateLeft = HIGH;
bool lastStateRight = HIGH;
bool lastStateMid = HIGH;
bool lastStateSet = HIGH;
bool lastStateReset = HIGH;

// Function prototypes
void drawCentreString(const char *buf, int y, int fontSize);
void writeMenuItems();
void reverseMainItem();
void redrawMainItem();
void initializeMenuLayout();
void eraseMenuLabels();
void toggleSelection();
void setBrightness(int level);
void doNothing();
void adjustBrightnessRight();
void adjustBrightnessLeft();
void wifiOnMid();
void callback(char *topic, byte *payload, unsigned int length);
void checkButton(int button, bool &lastState, const char *name);
void handleMqttTest();
void setupMqtt();
void reconnectMqtt();
void startAP();
void stopAP();
void displayQRCode();
void handleWiFiPage();
void handleConnect();

/*
* Menu UI structure
*/

typedef struct {
  const char* label;
  void (*onMid)();
  void (*onLeft)();
  void (*onRight)();
} MenuItem;

// menu items
MenuItem menuItems[] = {
  {"Wifi", wifiOnMid, doNothing, doNothing},
  {"< Bright >", doNothing, adjustBrightnessLeft, adjustBrightnessRight},
  {"MQTT", handleMqttTest, doNothing, doNothing},
};

// Menu layout
int radius = 12;
int item_space = 4;

// main item size
int main_item_position_x = 12;
int main_item_width = TFT_WIDTH - main_item_position_x * 2;
int main_item_height = TFT_HEIGHT / 5;
int main_item_position_y = TFT_HEIGHT / 2 - main_item_height / 2;

// sub item size
int sub_item_width = TFT_WIDTH - main_item_position_x * 4;
int sub_item_height = TFT_HEIGHT / 6;
int sub_items_position_x =  main_item_position_x * 2;

int height_difference = main_item_height - sub_item_height;

int sub_item_top_position_y = main_item_position_y - main_item_height + height_difference - item_space;
int sub_item_bottom_position_y = main_item_position_y + main_item_height + item_space;

// menu variables
const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);
int currentMenuIndex = 0;
bool isMainItemReversed = false;
int lastStationCount = -1;

int brightness = 255;

String savedSSID = "";
String savedPass = "";

// TFT display object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// QR code object
QRcode_ST7789 qrcode (&tft);

// Web server object
WebServer server(80);

// MQTT client object
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Preferences object
Preferences preferences;

// AP mode variables
bool apMode = false;
String wifiSSID;
String wifiPassword;

/*
 * PROJECT SETUP 
 */
void setup () {
  //brightness
  pinMode(BRIGHTNESS_PIN, OUTPUT);
  
  //buttons
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  pinMode(MID_BTN, INPUT_PULLUP);
  pinMode(SET_BTN, INPUT_PULLUP);
  pinMode(RESET_BTN, INPUT_PULLUP);

  Serial.begin(BAUD_RATE);

  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(TFT_ROTATION);

  tft.fillScreen(BACKGROUND);
  tft.setTextColor(PRIMARY_COLOR);
  tft.setTextWrap(true);

  espClient.setCACert(ca_cert);

  preferences.begin("brightness", true);
  brightness = preferences.getUInt("level", 255);
  preferences.end();
  
  setBrightness(brightness);
  initializeMenuLayout();

  preferences.begin("wifiCreds", true);  // open in read-only mode
  savedSSID = preferences.getString("ssid", "");
  savedPass = preferences.getString("pass", "");
  preferences.end();

  if (savedSSID != "") {
    Serial.println("Found saved WiFi credentials. Connecting to " + savedSSID);
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to WiFi!");
      setupMqtt();
    }
  }
}

void loop() {
  client.loop();
    
  if (apMode) {
    server.handleClient();
    int currentStationCount = WiFi.softAPgetStationNum();

    // Only update the QR code if the number of connected clients changes
    if (currentStationCount != lastStationCount) {
      displayQRCode();
      lastStationCount = currentStationCount; // Update the last known station count
    }

    // If the ESP has connected to a WiFi network as a station, stop AP mode.
    if (WiFi.status() == WL_CONNECTED) {
      //mqtt config
      if (!client.connected() && (millis() - lastMqttAttempt > mqttReconnectInterval)) {
        lastMqttAttempt = millis();
        reconnectMqtt();
      }

      stopAP();
      initializeMenuLayout();
      
    }
  }

  checkButton(UP_BTN, lastStateUp, "Up");
  checkButton(DOWN_BTN, lastStateDown, "Down");
  checkButton(LEFT_BTN, lastStateLeft, "Left");
  checkButton(RIGHT_BTN, lastStateRight, "Right");
  checkButton(MID_BTN, lastStateMid, "Mid");
  checkButton(SET_BTN, lastStateSet, "Set");
  checkButton(RESET_BTN, lastStateReset, "Reset");

  delay(50);
}

void setupMqtt() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  if (!client.connected()) {
    reconnectMqtt();  
  }
}

void reconnectMqtt() {
  String client_id = "esp32-client-" + String(WiFi.macAddress());
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("MQTT reconnected!");
    client.publish(topic, "Hi, I'm ESP32 ^^");
    client.subscribe(topic);
    client.subscribe("esp/control/bright");
  } else {
    Serial.print("MQTT reconnect failed with state ");
    Serial.println(client.state());
  }
}

void callback(char *receivedTopic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(receivedTopic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char) payload[i];
  }

  // Check if the topic is for brightness control
  if (String(receivedTopic) == "esp/control/bright") {
    int newBrightness = message.toInt();
    // Constrain brightness between 0 and 255
    newBrightness = constrain(newBrightness, 0, 255);
    brightness = newBrightness;
    analogWrite(BRIGHTNESS_PIN, brightness);
    Serial.print("Brightness set to: ");
    Serial.println(brightness);
  } else {
    // Handle other topics if needed
    Serial.print("Message: ");
    Serial.println(message);
    Serial.println("-----------------------");
  }
}

void handleMqttTest() {
  client.publish(topic, "Hi, I'm ESP32 ^^");
}

void handleConnect() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    
    // Save credentials in non-volatile storage
    preferences.begin("wifiCreds", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", pass);
    preferences.end();
    
    // Attempt to connect to the selected WiFi network
    WiFi.begin(ssid.c_str(), pass.c_str());
    server.send(200, "text/html", "<html><body><h1>Connecting to " + ssid + "...</h1></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Bad Request</h1></body></html>");
  }
}

void wifiOnMid() {
  if (!apMode) {
    // In menu mode: start the AP and display the QR code.
    startAP();
  } else {
    // In QR mode: stop AP and return to the menu.
    stopAP();
    initializeMenuLayout();
  }
}

void startAP() {
  // Generate a random SSID and password for the ESP AP
  wifiSSID = "MORO";
  wifiPassword = "PASS" + String(random(1000, 9999));

  Serial.println("Starting AP: " + wifiSSID);

  WiFi.softAP(wifiSSID.c_str(), wifiPassword.c_str());

  // Setup web server: register both the WiFi page and connect handlers
  server.on("/", handleWiFiPage);
  server.on("/connect", HTTP_POST, handleConnect);
  server.begin();

  apMode = true;

  displayQRCode();
}

void stopAP() {
  WiFi.softAPdisconnect(true);
  server.stop();
  apMode = false;
}

void displayQRCode() {
  tft.fillScreen(BACKGROUND);
  String qrContent;
  if (WiFi.softAPgetStationNum() > 0) {
    // When a client connects to the ESP AP, update the QR code to point to the root ("/") URL.
    qrContent = "http://" + WiFi.softAPIP().toString();
    qrcode.init(2, 180, 180);
    qrcode.create(qrContent.c_str());
  } else {
    // Otherwise, display the QR code with the WiFi credentials.
    String wifiConfig = "WIFI:T:WPA;S:" + wifiSSID + ";P:" + wifiPassword + ";;";
    qrContent = wifiConfig;
    
    qrcode.init(2, 180, 180);
    qrcode.create(qrContent.c_str());
    
    tft.setTextColor(PRIMARY_COLOR);
    drawCentreString(wifiSSID.c_str(), 32 , 3);
    drawCentreString(wifiPassword.c_str(), TFT_HEIGHT - 32 , 2);
  }
}

void handleWiFiPage() {
  // Scan for available networks (this may take a moment)
  int n = WiFi.scanNetworks();
  String options = "";
  for (int i = 0; i < n; i++) {
    // Build an <option> element for each network; you can also display the RSSI if desired
    options += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
  }

  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<title>ESP WiFi Setup</title></head><body>";
  html += "<h1>Connect ESP to WiFi</h1>";
  html += "<form action='/connect' method='POST'>";
  html += "<label for='ssid'>Select WiFi Network:</label>";
  html += "<select name='ssid' id='ssid'>" + options + "</select><br><br>";
  html += "<label for='pass'>Password:</label>";
  html += "<input type='password' name='pass' id='pass' placeholder='Password'><br><br>";
  html += "<input type='submit' value='Connect'>";
  html += "</form>";
  // A separate form for re-scanning the available networks.
  html += "<br><form action='/' method='GET'>";
  html += "<input type='submit' value='Search Again'>";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setBrightness(int level) {
  client.publish("esp/control/bright", String(level).c_str());
  preferences.begin("brightness", false);
  preferences.putUInt("level", level);
  preferences.end();
  analogWrite(BRIGHTNESS_PIN, level);
}

void adjustBrightnessLeft() {
  reverseMainItem();
  brightness = max(0, brightness - 25);
  setBrightness(brightness);
}

void adjustBrightnessRight() {
  reverseMainItem();
  brightness = min(255, brightness + 25);
  setBrightness(brightness);
}

void doNothing() {
  reverseMainItem();
}

void initializeMenuLayout() {
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(BACKGROUND);
  tft.setTextColor(PRIMARY_COLOR);

  // draw main item
  tft.drawRoundRect(main_item_position_x, main_item_position_y, main_item_width, main_item_height, radius, PRIMARY_COLOR);

  // draw sub items
  tft.drawRoundRect(sub_items_position_x, sub_item_top_position_y,    sub_item_width, sub_item_height, radius, SECONDARY_COLOR);
  tft.drawRoundRect(sub_items_position_x, sub_item_bottom_position_y, sub_item_width, sub_item_height, radius, SECONDARY_COLOR);

  writeMenuItems();
}

void drawCentreString(const char *buf, int y, int fontSize) {
  tft.setTextSize(fontSize); // Set font size

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // Get text width & height

  int x = (tft.width() - w) / 2;  // Center X
  int yCentered = y - (h / 2);    // Adjust Y to center text properly

  tft.setCursor(x, yCentered);
  tft.print(buf);
}

void writeMenuItems() {
  int topIndex = (currentMenuIndex - 1 + menuItemCount) % menuItemCount;
  int bottomIndex = (currentMenuIndex + 1) % menuItemCount;

  eraseMenuLabels();

  drawCentreString(menuItems[currentMenuIndex].label, TFT_HEIGHT / 2, 3);

  tft.setTextColor(SECONDARY_COLOR);
  drawCentreString(menuItems[topIndex].label,    (TFT_HEIGHT / 2) - (main_item_height / 2) - item_space - (sub_item_height / 2), 2);
  drawCentreString(menuItems[bottomIndex].label, (TFT_HEIGHT / 2) + (main_item_height / 2) + item_space + (sub_item_height / 2), 2);
  tft.setTextColor(PRIMARY_COLOR);
}


/*
   desenha o item principal com a cor do background, e o item preenchido pela cor principal
*/
void reverseMainItem() {
  tft.setTextColor(BACKGROUND);
  tft.fillRoundRect(main_item_position_x, main_item_position_y, main_item_width, main_item_height, radius, PRIMARY_COLOR);
  drawCentreString(menuItems[currentMenuIndex].label, TFT_HEIGHT / 2, 3);
  tft.setTextColor(PRIMARY_COLOR);
}

/*
   desenha o item principal com a cor principal, limpa a área do botão, e então redesenha o botão
   (não é chamada a função writeMenuItems para evitar piscar a tela.
*/
void redrawMainItem() {
  tft.fillRect(     main_item_position_x, main_item_position_y, main_item_width, main_item_height, BACKGROUND);
  tft.drawRoundRect(main_item_position_x, main_item_position_y, main_item_width, main_item_height, radius, PRIMARY_COLOR);

  tft.setTextColor(PRIMARY_COLOR);
  drawCentreString(menuItems[currentMenuIndex].label, TFT_HEIGHT / 2, 3);
}

void eraseMenuLabels() {
  tft.fillRect(main_item_position_x + radius, main_item_position_y       + radius, main_item_width - radius * 2, main_item_height - radius * 2, BACKGROUND);
  tft.fillRect(sub_items_position_x + radius, sub_item_top_position_y    + radius, sub_item_width  - radius * 2, sub_item_height  - radius * 2, BACKGROUND);
  tft.fillRect(sub_items_position_x + radius, sub_item_bottom_position_y + radius, sub_item_width  - radius * 2, sub_item_height  - radius * 2, BACKGROUND);
}

void checkButton(int button, bool &lastState, const char *name) {
  bool currentState = digitalRead(button);

  if (currentState != lastState) {
    if (currentState == LOW) {
      Serial.print("Pressed ");
      Serial.println(name);

      if (apMode) {
        stopAP();
        initializeMenuLayout();
      }

      if (button == RESET_BTN) {
        initializeMenuLayout();
      } else if (button == DOWN_BTN) {
        currentMenuIndex = (currentMenuIndex + 1) % menuItemCount;
        writeMenuItems();
      } else if (button == UP_BTN) {
        currentMenuIndex = (currentMenuIndex - 1 + menuItemCount) % menuItemCount;
        writeMenuItems();
      } else if (button == MID_BTN) {
        menuItems[currentMenuIndex].onMid();
      } else if (button == LEFT_BTN) {
        menuItems[currentMenuIndex].onLeft();
      } else if (button == RIGHT_BTN) {
        menuItems[currentMenuIndex].onRight();
      }
    } else if (currentState == HIGH) {
      if (button == MID_BTN) {
        if (menuItems[currentMenuIndex].onMid == doNothing) {
          redrawMainItem();
        }
      } else if (button == LEFT_BTN) {
        if (menuItems[currentMenuIndex].onLeft == doNothing || menuItems[currentMenuIndex].onLeft == adjustBrightnessLeft) {
          redrawMainItem();
        }
      } else if (button == RIGHT_BTN) {
        if (menuItems[currentMenuIndex].onRight == doNothing || menuItems[currentMenuIndex].onRight == adjustBrightnessRight) {
          redrawMainItem();
        }
      }
    }
    lastState = currentState;
  }
}
