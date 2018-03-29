#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

template<class A>
class singleton
{
   private:
   
   protected:
      singleton() = default;

   public:
      singleton(const singleton&) = delete;
      singleton(singleton&&) = delete;
      singleton& operator=(const singleton&) = delete;
      singleton& operator=(singleton&&) = delete;
      
      static A& instance()
      {
         static A a; 
         return a;
      }
};

#endif /* SINGLETON_H_INCLUDED */
