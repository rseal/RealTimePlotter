#ifndef POINT_HPP
#define POINT_HPP

namespace splot{

  struct Point{
    int x_;
    int y_;

  public:
    Point(){};
    Point(const int& x, const int& y): x_(x), y_(y){}
    void X(const int& x) { x_ = x;}
    void Y(const int& y) { y_ = y;}
    int& X() {return x_;}
    int& Y() {return y_;}
  };
};
  

#endif
