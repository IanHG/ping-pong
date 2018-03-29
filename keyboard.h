#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <fcntl.h> // open
#include <unistd.h> // close
#include <linux/input.h> // struct input_event
#include <deque>

#include "singleton.h"
#include "event_handler.h"
#include "blocking_queue.h"

///////
// 
// input_event from file
// /usr/src/linux-headers-3.16.0-30/include/uapi/linux/input.h
// has following structure:
//
//    struct input_event {
//       struct timeval time;
//       __u16 type;
//       __u16 code;
//       __s32 value;
//    };
///////

// find keyboard fd either by command: 
// $cat /proc/bus/input/devices
// or viewing the var/log/Xorg.0.log searching for "keyboard"
#define KEYBOARD_DEV "/dev/input/event3"

// define some constants
#define EV_RELEASED 0
#define EV_PRESSED 1
#define EV_REPEAT 2

using ev_type = __u16;

// key constants can be found in e.g.
// /usr/src/linux-headers-3.16.0-30/include/uapi/linux/input.h

class keyboard_queue;

/*
 * struct keyboard_state - represents keyboards current state
 */
class keyboard_state
   : public singleton<keyboard_state>
{
   private:
      mutable std::mutex m_keystate_mutex;
      signed short m_keystate_current[KEY_CNT];
      signed short m_keystate_prev[KEY_CNT];
      
      /**
       * update keyboard state
       **/
      void update(input_event* keyboard_event)
      {
         std::lock_guard<std::mutex> keystate_lock(m_keystate_mutex);
         m_keystate_prev[keyboard_event->code] = m_keystate_current[keyboard_event->code];
         m_keystate_current[keyboard_event->code] = keyboard_event->value;
      }

      friend keyboard_queue;
   
   public:
      short signed keystate_current(unsigned short key) const 
      {
         std::lock_guard<std::mutex> keystate_lock(m_keystate_mutex);
         return m_keystate_current[key];
      }
};

std::ostream& operator<<(std::ostream& os, const keyboard_state& state)
{
   for(int i = 0; i < KEY_CNT; ++i)
   {
      os << state.keystate_current(i);
   }
   return os;
}

/*
 * struct keyboard_queue - represents keyboard event queue
 */
class keyboard_queue
   : public singleton<keyboard_queue>
   , private event_handler<ev_type, void(const keyboard_state&)>
   , private blocking_queue<input_event>
{
   using event_handler_t = event_handler<ev_type, void(const keyboard_state&)>;
   using queue_t = blocking_queue<input_event>;
   using event_t = typename event_handler_t::event_t;
   using function_t = typename event_handler_t::function_t;

   private:
      keyboard_state& m_keyboard_state = keyboard_state::instance();

   public:
      void update(input_event* keyboard_event)
      {
         queue_t::push(*keyboard_event);
      }

      void register_function(const event_t& event, const function_t& f)
      {
         event_handler_t::register_function(event,f);
      }

      void handle_events()
      {
         while(!queue_t::empty())
         {
            // get an event
            input_event event;
            queue_t::pop_try_wait(event); // 
            m_keyboard_state.update(&event); // first update keyboard state
            
            // then handle event
            event_handler_t::handle_event(event.code, m_keyboard_state);
         }
      }
};

/*
 *
 */
class keyboard
{
   private:
      bool m_active = false;
      int m_keyboard_fd = 0;
      char m_name[256];
      std::unique_ptr<input_event> m_keyboard_event = std::unique_ptr<input_event>(new input_event);
      keyboard_queue& m_keyboard_queue = keyboard_queue::instance();
      std::future<void> m_return_from_thread;
      
      bool is_active() const
      {
         return m_active;
      }
      
      void read_event()
      {
         int bytes = read(m_keyboard_fd, m_keyboard_event.get(), sizeof(*m_keyboard_event));
         if(bytes)
         {
            //std::cout << "code : " << m_keyboard_event->code << std::endl;
            //std::cout << "value: " << m_keyboard_event->value << std::endl;
            //std::cout << std::endl;
            if(m_keyboard_event->type & EV_KEY) // check that it is actally a keyboard event
            {
               //m_keyboard_state.update(m_keyboard_event.get());
               m_keyboard_queue.update(m_keyboard_event.get());
            }
         }
         //else
         //{
         //   std::cout << " read nothing " << std::endl;
         //}
      }
      
      void loop()
      {
         while(is_active())
         {
            read_event();
         }
      }

   public:
      keyboard()
      {
         /**
          * NB: need to execute as root (su) to open keyboard fd
          **/
         //m_keyboard_fd = open(KEYBOARD_DEV, O_RDONLY | O_NONBLOCK);
         m_keyboard_fd = open(KEYBOARD_DEV, O_RDONLY); // we spawn a thread to do the read-in, so we do not have to do it non-blocking
         if(m_keyboard_fd > 0)
         {
            ioctl(m_keyboard_fd, EVIOCGNAME(256), m_name);
            std::cout << " Keyboard name: " << m_name << std::endl;
            m_active = true;
            m_return_from_thread = std::async(std::launch::async,std::bind(&keyboard::loop,this));
         }
         else
         {
            std::cout << " could not open keyboard fd " << std::endl;
            exit(2);
         }
      }

      keyboard(const keyboard&) = delete;
      keyboard& operator=(const keyboard&) = delete;
      
      ~keyboard()
      {
         if(m_keyboard_fd > 0)
         {
            m_active = false;
            m_return_from_thread.wait(); // wait for thread to finish
            close(m_keyboard_fd);
         }
         m_keyboard_fd = 0;
      }
};

#endif /* KEYBOARD_H_INCLUDED */
