#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>


#define TFT_DC  2
#define TFT_RST 4
#define TFT_CS  5

#define BAUD_RATE 115200
#define UP_BTN 13
#define DOWN_BTN 12
#define LEFT_BTN 14
#define RIGHT_BTN 27
#define MID_BTN 26
#define SET_BTN 25
#define RESET_BTN 33

#define BRIGHTNESS_PIN 22

#define TFT_WIDTH 240
#define TFT_HEIGHT 280
#define TFT_ROTATION 2

#define BACKGROUND 0x0000
#define MENU_BACKGROUND 0xFFFF
#define SUB_MENU_BACKGROUND 0x4BF3

bool lastStateUp = HIGH;
bool lastStateDown = HIGH;
bool lastStateLeft = HIGH;
bool lastStateRight = HIGH;
bool lastStateMid = HIGH;
bool lastStateSet = HIGH;
bool lastStateReset = HIGH;


// functions interface
void initializeMenuLayout();
void eraseMenuLabels();
void toggleSelection();
void setBrightness(int level);
void doNothing();
void adjustBrightnessRight();
void adjustBrightnessLeft();

int brightness = 255;
bool isMainItemReversed = false;

/*
 * Menu interface style
 */
// border radius
int radius = 12;
int item_space = 4;

// main item size and position
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


typedef struct {
    const char* label;
    void (*onMid)();
    void (*onLeft)();
    void (*onRight)();
} MenuItem;

// menu items
MenuItem menuItems[] = {
    {"Wifi", doNothing, doNothing, doNothing},
    {"< Bright >", doNothing, adjustBrightnessLeft, adjustBrightnessRight},
};

const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);

int currentMenuIndex = 0;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

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
  tft.setTextColor(MENU_BACKGROUND);
  tft.setTextWrap(true);

  setBrightness(brightness);
  initializeMenuLayout();
}

void loop() {
  checkButton(UP_BTN, lastStateUp, "Up");
  checkButton(DOWN_BTN, lastStateDown, "Down");
  checkButton(LEFT_BTN, lastStateLeft, "Left");
  checkButton(RIGHT_BTN, lastStateRight, "Right");
  checkButton(MID_BTN, lastStateMid, "Mid");
  checkButton(SET_BTN, lastStateSet, "Set");
  checkButton(RESET_BTN, lastStateReset, "Reset");

  delay(50);
}

void setBrightness(int level) {
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
  // draw main item
  tft.drawRoundRect(main_item_position_x, main_item_position_y, main_item_width, main_item_height, radius, MENU_BACKGROUND);

  // draw sub items
  tft.drawRoundRect(sub_items_position_x, sub_item_top_position_y,    sub_item_width, sub_item_height, radius, SUB_MENU_BACKGROUND);  
  tft.drawRoundRect(sub_items_position_x, sub_item_bottom_position_y, sub_item_width, sub_item_height, radius, SUB_MENU_BACKGROUND);  
  
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
  
  tft.setTextColor(SUB_MENU_BACKGROUND);
  drawCentreString(menuItems[topIndex].label,    (TFT_HEIGHT / 2) - (main_item_height / 2) - item_space - (sub_item_height / 2), 2);
  drawCentreString(menuItems[bottomIndex].label, (TFT_HEIGHT / 2) + (main_item_height / 2) + item_space + (sub_item_height / 2), 2);
  tft.setTextColor(MENU_BACKGROUND);
}


/*
 * desenha o item principal com a cor do background, e o item preenchido pela cor principal
 */
void reverseMainItem() {
  tft.setTextColor(BACKGROUND);
  tft.fillRoundRect(main_item_position_x, main_item_position_y, main_item_width, main_item_height, radius, MENU_BACKGROUND);
  drawCentreString(menuItems[currentMenuIndex].label, TFT_HEIGHT / 2, 3);
  tft.setTextColor(MENU_BACKGROUND);
}

/*
 * desenha o item principal com a cor principal, limpa a área do botão, e então redesenha o botão
 * (não é chamada a função writeMenuItems para evitar piscar a tela.
 */
void redrawMainItem() {
  tft.fillRect(     main_item_position_x, main_item_position_y, main_item_width, main_item_height, BACKGROUND);
  tft.drawRoundRect(main_item_position_x, main_item_position_y, main_item_width, main_item_height, radius, MENU_BACKGROUND);
  
  tft.setTextColor(MENU_BACKGROUND);
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
