#ifndef COLLISION_DETECTOR_H_INCLUDED
#define COLLISION_DETECTOR_H_INCLUDED

#include "collideable.h"

class collision_detector
{
   private:
      std::set<collideable> m_collideables;

   public:
      void handle_collisions()
      {
         for(auto iter1: m_collideables)
         {
            for(auto iter2: m_collideables)
            {
               if(iter1->collision(*iter2))
                  std::cout << " we have collision " << std::endl;
            }
         }
      }
};

#endif /* COLLISION_DETECTOR_H_INCLUDED */
