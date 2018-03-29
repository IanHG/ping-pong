#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include <iostream>
#include <string>
#include "keyboard.h"

class screen
{
   private:
      int m_height_x = 40;
      int m_height_y = 40;
      int m_pos_x = 1;
      int m_pos_y = 1;

      void move_up(const keyboard_state& state)
      {
         if(state.keystate_current(KEY_W))
            if(m_pos_y < m_height_y)
               ++m_pos_y;
      }
      
      void move_down(const keyboard_state& state)
      {
         if(state.keystate_current(KEY_S))
            if(m_pos_y > 0)
               --m_pos_y;
      }
      
      void move_left(const keyboard_state& state)
      {
         if(state.keystate_current(KEY_A))
            if(m_pos_x < m_height_x)
               ++m_pos_x;
      }
      
      void move_right(const keyboard_state& state)
      {
         if(state.keystate_current(KEY_D))
            if(m_pos_x > 0)
               --m_pos_x;
      }

   public:
      screen()
      {
         keyboard_queue& queue = keyboard_queue::instance();
         queue.register_function(KEY_W
                               , std::bind(&screen::move_up,this,std::placeholders::_1)
                               );
         queue.register_function(KEY_S
                               , std::bind(&screen::move_down,this,std::placeholders::_1)
                               );
         queue.register_function(KEY_A
                               , std::bind(&screen::move_left,this,std::placeholders::_1)
                               );
         queue.register_function(KEY_D
                               , std::bind(&screen::move_right,this,std::placeholders::_1)
                               );
      }

      void clear()
      {
         std::cout << std::string( 100, '\n' );
      }

      void print()
      {
         for(int y = m_height_y; y >= 0; --y)
         {
            for(int x = m_height_x; x >= 0; --x)
            {
               if(x == m_pos_x && y == m_pos_y)
                  std::cout << "X";
               else
                  std::cout << " ";
            }
            std::cout << "\n";
         }
         std::cout << std::flush;
      }
};

#endif /* SCREEN_H_INCLUDED */
