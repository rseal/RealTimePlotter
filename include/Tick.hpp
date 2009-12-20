#ifndef TICK_HPP
       
#define TICK_HPP
#include "Point.hpp"
#include <iostream>

namespace splot{

  struct Tick{
    Point p0_,p1_;

  public:
    Tick(const Point& p0, const Point& p1){
      p0_ = p0;
      p1_ = p1;
    }

    void Print(std::ostream& stream){
      stream << "P0<" << p0_.X() << "," << p0_.Y() << "\n"
	     << "P1<" << p1_.X() << "," << p1_.Y() << ">" << std::endl;
    }

    Point& P0() {return p0_;} 
    Point& P1() {return p1_;} 
    void P0(const Point& p0){p0_ = p0;}
    void P1(const Point& p1){p1_ = p1;}
  };

};

#endif
