#ifndef EVENT_HANDLER_H_INCLUDED
#define EVENT_HANDLER_H_INCLUDED

#include <string>
#include <map> // for multimap
#include <functional>

// EV := event type    F := function type
template<class EV, class F> 
class event_handler;

// EV := event type    T := function return type     Ts... := function argument types
template<class EV, class T, class... Ts> 
class event_handler<EV,T(Ts...)>
{
   protected:
      using event_t = EV;
      using function_t = std::function<T(Ts...)>;
      using event_map_t = std::multimap<event_t, function_t>;
   
   private:
      event_map_t m_function_map;

   public:
      event_handler() = default;
      event_handler(const event_handler&) = delete;
      event_handler& operator=(const event_handler&) = delete;
      
      /**
       * register function with event handler
       **/
      void register_function(const event_t& event, const function_t& f)
      {
         m_function_map.insert( {event,f} );
      }
      
      /**
       * handle an event
       **/
      void handle_event(const event_t& event, Ts&&... ts)
      {
         auto range = m_function_map.equal_range(event);
         for(auto iter = range.first; iter != range.second; ++iter)
         {
            iter->second(std::forward<Ts>(ts)...);
         }
      }
};

#endif /* EVENT_HANDLER_H_INCLUDED */
