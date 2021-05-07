#include <M5StickC.h>
#include <Ticker.h>

// put your own walking interval.
static const uint32_t walking_interval = 3;

// type definitions
enum WalkingMode {
  Fast,
  Slow
};
struct WalkingStatus {
  uint32_t minutes;
  uint32_t seconds;
  WalkingMode mode;
};

// static variables
static const uint32_t tick_interval_ms = 10;
static Ticker ticker;
static uint32_t elapsed_ms = 0;
static uint32_t elapsed_sec = 0;
static bool has_started = false;
static WalkingStatus walking_status;

void reset_status() {
  elapsed_ms = 0;
  elapsed_sec = 0;
  update_status();
}

void update_status() {
  uint32_t tmp_m = (elapsed_sec / 60);
  walking_status.minutes = tmp_m % walking_interval;
  walking_status.seconds = elapsed_sec % 60;
  walking_status.mode = ((tmp_m / walking_interval) % 2 == 0) ? Fast : Slow;
}

void tick() {
  elapsed_ms += tick_interval_ms;
  if (elapsed_ms >= 1000) {
    elapsed_ms -= 1000;
    elapsed_sec += 1;
    update_status();
    print_status();
  }
}

void print_status() {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextFont(4);
  if (walking_status.mode == Fast) {
    M5.Lcd.println("Fast");  // workaround for afterimage
  } else {
    M5.Lcd.println("Slow");
  }
  M5.Lcd.setTextFont(7);
  M5.Lcd.printf("%02d:%02d", walking_status.minutes, walking_status.seconds);
}

void setup() {
  M5.begin();
  setCpuFrequencyMhz(10);
  M5.Lcd.setRotation(3);
  print_status();
}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    if (!has_started) {
      ticker.attach_ms(tick_interval_ms, tick);
      has_started = true;
    } else {
      ticker.detach();
      has_started = false;
    }
  }
  if (M5.BtnB.wasPressed()) {
    ticker.detach();
    has_started = false;
    reset_status();
    print_status();
  }
}
