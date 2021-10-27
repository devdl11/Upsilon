#include "isr.h"
#include <stdint.h>
#include <string.h>
#include <ion.h>
#include "../drivers/board.h"
#include "../drivers/rtc.h"
#include "../drivers/reset.h"
#include "../drivers/timing.h"
#include <drivers/led.h>
#include <kandinsky.h>
#include <drivers/cache.h>
#include "../drivers/timing.h"


typedef void (*cxx_constructor)();

extern "C" {
  extern char _data_section_start_flash;
  extern char _data_section_start_ram;
  extern char _data_section_end_ram;
  extern char _bss_section_start_ram;
  extern char _bss_section_end_ram;
  extern cxx_constructor _init_array_start;
  extern cxx_constructor _init_array_end;
}

/* In order to ensure that this method is execute from the external flash, we
 * forbid inlining it.*/

static void __attribute__((noinline)) external_flash_start() {
  /* Init the peripherals. We do not initialize the backlight in case there is
   * an on boarding app: indeed, we don't want the user to see the LCD tests
   * happening during the on boarding app. The backlight will be initialized
   * after the Power-On Self-Test if there is one or before switching to the
   * home app otherwise. */
  Ion::Device::Board::initPeripherals(false);
  return ion_main(0, nullptr);
}

/* This additional function call 'jump_to_external_flash' serves two purposes:
 * - By default, the compiler is free to inline any function call he wants. If
 *   the compiler decides to inline some functions that make use of VFP
 *   registers, it will need to push VFP them onto the stack in calling
 *   function's prologue.
 *   Problem: in start()'s prologue, we would never had a chance to enable the
 *   FPU since this function is the first thing called after reset.
 *   We can safely assume that neither memcpy, memset, nor any Ion::Device::init*
 *   method will use floating-point numbers, but ion_main very well can.
 *   To make sure ion_main's potential usage of VFP registers doesn't bubble-up to
 *   start(), we isolate it in its very own non-inlined function call.
 * - To avoid jumping on the external flash when it is shut down, we ensure
 *   there is no symbol references from the internal flash to the external
 *   flash except this jump. In order to do that, we isolate this
 *   jump in a symbol that we link in a special section separated from the
 *   internal flash section. We can than forbid cross references from the
 *   internal flash to the external flash. */

static void __attribute__((noinline)) jump_to_external_flash() {
  external_flash_start();
}


void __attribute__((noinline)) abort() {
  Ion::Device::Board::shutdownPeripherals(true);
  Ion::Device::Board::initPeripherals(false);
  Ion::Timing::msleep(100);
  Ion::Backlight::init();
  Ion::LED::setColor(KDColorRed);
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0xffffff));
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  ctx->drawString("UPSILON CRASH", KDPoint(90, 10), KDFont::LargeFont, KDColorRed, KDColor::RGB24(0xffffff));
  ctx->drawString("An error occurred", KDPoint(10, 30), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("If you have some important data, please", KDPoint(10, 45), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("use getomega.dev/ide to backup them.", KDPoint(10, 60), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("YOU WILL LOSE ALL YOUR DATA", KDPoint(10, 85), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ You can try to reboot by presssing the", KDPoint(10, 110), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("reset button at the back of the calculator", KDPoint(10, 125), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ If Upsilon keeps crashing, you can connect", KDPoint(10, 140), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("the calculator to a computer or a phone", KDPoint(10, 160), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("and try to reinstall Upsilon", KDPoint(10, 175), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("HARDFAULT", KDPoint(220, 200), KDFont::SmallFont, KDColorRed, KDColor::RGB24(0xffffff));
  Ion::Timing::msleep(100);
  while(true){
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false, false, 0);
  }
  
}

void  __attribute__((noinline)) nmi_abort(){
  Ion::Device::Board::shutdownPeripherals(true);
  Ion::Device::Board::initPeripherals(false);
  Ion::Timing::msleep(100);
  Ion::Backlight::init();
  Ion::LED::setColor(KDColorRed);
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0xffffff));
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  ctx->drawString("UPSILON CRASH", KDPoint(90, 10), KDFont::LargeFont, KDColorRed, KDColor::RGB24(0xffffff));
  ctx->drawString("An error occurred", KDPoint(10, 30), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("If you have some important data, please", KDPoint(10, 45), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("use getomega.dev/ide to backup them.", KDPoint(10, 60), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("YOU WILL LOSE ALL YOUR DATA", KDPoint(10, 85), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ You can try to reboot by presssing the", KDPoint(10, 110), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("reset button at the back of the calculator", KDPoint(10, 125), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ If Upsilon keeps crashing, you can connect", KDPoint(10, 140), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("the calculator to a computer or a phone", KDPoint(10, 160), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("and try to reinstall Upsilon", KDPoint(10, 175), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("NMIFAULT", KDPoint(220, 200), KDFont::SmallFont, KDColorRed, KDColor::RGB24(0xffffff));
  Ion::Timing::msleep(100);
  while(true){
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false, false, 0);
  }
}

void  __attribute__((noinline)) bf_abort(){
  Ion::Device::Board::shutdownPeripherals(true);
  Ion::Device::Board::initPeripherals(false);
  Ion::Timing::msleep(100);
  Ion::Backlight::init();
  Ion::LED::setColor(KDColorRed);
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0xffffff));
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  ctx->drawString("UPSILON CRASH", KDPoint(90, 10), KDFont::LargeFont, KDColorRed, KDColor::RGB24(0xffffff));
  ctx->drawString("An error occurred", KDPoint(10, 30), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("If you have some important data, please", KDPoint(10, 45), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("use getomega.dev/ide to backup them.", KDPoint(10, 60), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("YOU WILL LOSE ALL YOUR DATA", KDPoint(10, 85), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ You can try to reboot by presssing the", KDPoint(10, 110), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("reset button at the back of the calculator", KDPoint(10, 125), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ If Upsilon keeps crashing, you can connect", KDPoint(10, 140), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("the calculator to a computer or a phone", KDPoint(10, 160), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("and try to reinstall Upsilon", KDPoint(10, 175), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("BUSFAULT", KDPoint(220, 200), KDFont::SmallFont, KDColorRed, KDColor::RGB24(0xffffff));
  Ion::Timing::msleep(100);
  while(true){
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false, false, 0);
  }
}
void  __attribute__((noinline)) uf_abort(){
  Ion::Device::Board::shutdownPeripherals(true);
  Ion::Device::Board::initPeripherals(false);
  Ion::Timing::msleep(100);
  Ion::Backlight::init();
  Ion::LED::setColor(KDColorRed);
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0xffffff));
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  ctx->drawString("UPSILON CRASH", KDPoint(90, 10), KDFont::LargeFont, KDColorRed, KDColor::RGB24(0xffffff));
  ctx->drawString("An error occurred", KDPoint(10, 30), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("If you have some important data, please", KDPoint(10, 45), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("use getomega.dev/ide to backup them.", KDPoint(10, 60), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("YOU WILL LOSE ALL YOUR DATA", KDPoint(10, 85), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ You can try to reboot by presssing the", KDPoint(10, 110), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("reset button at the back of the calculator", KDPoint(10, 125), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("→ If Upsilon keeps crashing, you can connect", KDPoint(10, 140), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("the calculator to a computer or a phone", KDPoint(10, 160), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("and try to reinstall Upsilon", KDPoint(10, 175), KDFont::SmallFont, KDColorBlack, KDColor::RGB24(0xffffff));
  ctx->drawString("USAGEFAULT", KDPoint(220, 200), KDFont::SmallFont, KDColorRed, KDColor::RGB24(0xffffff));
  Ion::Timing::msleep(100);
  while(true){
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false, false, 0);
  }
}

/* When 'start' is executed, the external flash is supposed to be shutdown. We
 * thus forbid inlining to prevent executing this code from external flash
 * (just in case 'start' was to be called from the external flash). */

void __attribute__((noinline)) start() {
  /* This is where execution starts after reset.
   * Many things are not initialized yet so the code here has to pay attention. */

  /* Copy data section to RAM
   * The data section is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data section (e.g. global variables) contains
   * garbage values and should not be used. */
  size_t dataSectionLength = (&_data_section_end_ram - &_data_section_start_ram);
  memcpy(&_data_section_start_ram, &_data_section_start_flash, dataSectionLength);

  /* Zero-out the bss section in RAM
   * Until we do, any uninitialized global variable will be unusable. */
  size_t bssSectionLength = (&_bss_section_end_ram - &_bss_section_start_ram);
  memset(&_bss_section_start_ram, 0, bssSectionLength);

  /* Initialize the FPU as early as possible.
   * For example, static C++ objects are very likely to manipulate float values */
  Ion::Device::Board::initFPU();

  /* Call static C++ object constructors
   * The C++ compiler creates an initialization function for each static object.
   * The linker then stores the address of each of those functions consecutively
   * between _init_array_start and _init_array_end. So to initialize all C++
   * static objects we just have to iterate between theses two addresses and
   * call the pointed function. */
#define SUPPORT_CPP_GLOBAL_CONSTRUCTORS 0
#if SUPPORT_CPP_GLOBAL_CONSTRUCTORS
  for (cxx_constructor * c = &_init_array_start; c<&_init_array_end; c++) {
    (*c)();
  }
#else
  /* In practice, static initialized objects are a terrible idea. Since the init
   * order is not specified, most often than not this yields the dreaded static
   * init order fiasco. How about bypassing the issue altogether? */
  if (&_init_array_start != &_init_array_end) {
    abort();
  }
#endif

  Ion::Device::Board::init();
  
  /* At this point, we initialized clocks and the external flash but no other
   * peripherals. */
  

  jump_to_external_flash();

  abort();
}

void __attribute__((interrupt, noinline)) isr_systick() {
  auto t = Ion::Device::Timing::MillisElapsed;
  t++;
  Ion::Device::Timing::MillisElapsed = t;
}
