// bibliotech
#include <SoftwareSerial.h>     //серийный порт
#include <DFPlayer_Mini_Mp3.h>  //плеер
#include <EEPROM.h>             //EEPROM редактор
#include <EncButton.h>          //кнопка
#include <FastLED.h>            //эффекты

// Настройки ленты
#define LED_PIN 5  //пин управления
#define COLOR_ORDER GRB
#define CHIPSET WS2812        //тип ленты
#define NUM_LEDS 12           //колличество светодиодов
#define BRIGHTNESS 150        //яркость
#define FRAMES_PER_SECOND 50  //частота анимации
bool gReverseDirection = false;
CRGB leds[NUM_LEDS];

EncButton<EB_TICK, 2> enc;        //Пин кнопки
SoftwareSerial mySerial(10, 11);  // RX, TX Цифровой серийный порт.
int buttonCounter;

//Переменные аналогово чтения напряжения платы
const int analogPin = A0;  //Аналоговый пин замера напряжения
const int res1 = 10000;    //Резистор делителя напряжения 10кОм
const int res2 = 4700;     //Резистор делителя напряжения 4.7кОм
//переменные EEPROM
int CTrack = EEPROM.read(1000);  //текущий трек
int Evol = EEPROM.read(900);     //громкость в запись
int vol;                         //                     текущая громкость

// настройки
void setup() {
  pinMode(analogPin, INPUT);  //Аналоговый пин на вход

  //настройки ленты
  delay(1000);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  //настройки кнопки
  enc.setHoldTimeout(500);
  enc.setButtonLevel(LOW);

  //стартовый код
  Serial.begin(9600);
  mySerial.begin(9600);
  mp3_set_serial(mySerial);  //Выбор порта для плеера
  mp3_set_volume(Evol);
  delay(100);
  for (int i = 0; i < 12; i++) leds[i] = CRGB::Red;
  FastLED.show();
  float voltage = (float)analogRead(analogPin) * 5.0 * ((res1 + res2) / res2) / 1024;
  if (voltage > 12.4 || voltage < 6) {
    mp3_play(CTrack);
    delay(15000);
    //EEPROM.update(1000, 1);  //Если в адресе нули - включить
    //EEPROM.update(900, 1);    //Если в адресе нули - включить
  }
}


void loop() {
  // Громкость
  vol = analogRead(6);
  vol = map(vol, 0, 1023, 0, 30);
  vol = constrain(vol, 0, 30);
  mp3_set_volume(vol);
  Evol = vol;
  delay(30);
  //
  //Выбор трека
  enc.tick();
  if (enc.hasClicks(1)) {
    CTrack = 1;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(2)) {
    CTrack = 2;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(3)) {
    CTrack = 3;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(4)) {
    CTrack = 4;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(5)) {
    CTrack = 5;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(6)) {
    CTrack = 6;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(7)) {
    CTrack = 7;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(8)) {
    CTrack = 8;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(9)) {
    CTrack = 9;
    mp3_play(CTrack);
  }
  if (enc.hasClicks(10)) {
    CTrack = 10;
    mp3_play(CTrack);
  }
  if (enc.held()) {  //                                        Сохранение позиции
    EEPROM.update(1000, CTrack);
    EEPROM.update(900, Evol);
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Red;
    leds[2] = CRGB::Red;
    leds[3] = CRGB::Red;
    FastLED.show();
    delay(2000);
  }

  //запуск фоновой анимации
  pride();
  FastLED.show();
}

void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;  //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
}
