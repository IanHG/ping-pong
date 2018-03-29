#ifndef COLLIDEABLE_H_INCLUDED
#define COLLIDEABLE_H_INCLUDED

class shape
{
   public:
      bool intersect(const shape&) const = 0;
};

class point: public shape
{
   int m_x;
   int m_y;

   public:
      bool intersect(const shape& other) const
      {
         return other.intersect(*this);
      }

      bool intersect(const point& other) const
      {
         return (m_x == other.m_x) && (m_y && m_y.other);
      }
};

class collideable
{
   private:
      std::unique_ptr<shape> m_shape;

   public:
      bool collision(const collideable& other)
      {
         return m_shape->intersect(other.m_shape);
      }
};

#endif /* COLLIDEABLE_H_INCLUDED */
