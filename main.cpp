#include <chrono>

#include "keyboard.h"
#include "keyboard_logger.h"
#include "screen.h"

int main()
{
   std::chrono::milliseconds timespan(10);
   keyboard k;
   keyboard_queue& queue = keyboard_queue::instance();
   //keyboard_logger logger;
   screen scr;

   while(true)
   {
      queue.handle_events();
      scr.clear();
      scr.print();
      std::this_thread::sleep_for(timespan);
   }

   return 0;
}
