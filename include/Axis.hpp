#ifndef AXIS_HPP
#define AXIS_HPP

#include "Tick.hpp"
#include <vector>

namespace splot{

  typedef std::vector<Tick> TickVector;
  enum {MAJOR=0,MINOR=1};

class Axis{
    
    //use one vector for major and minor ticks
    TickVector tics_;
    int majorLength_;
    int minorLength_;
    int majorWidth_;
    int minorWidth_;
    int pixelsPerTick_;
    int ratio_;
    std::string label_;

  public:

    Axis(): majorLength_(10),minorLength_(5),
	    majorWidth_(3), minorWidth_(1),
	    pixelsPerTick_(10),ratio_(10),
	    label_("Axis"){}

    TickVector& GetRef(){return tics_;}
  
    void MajorTick(const int& length, const int& width){
      majorLength_ = length;
      majorWidth_ = width;
    }

    void MinorTick(const int& length, const int& width){
      minorLength_ = length;
      minorWidth_ = width;
    }
    
    const int& TickLength(const int& type) { 
      return type==MAJOR ? majorLength_ : minorLength_;
    }

    const int& TickWidth(const int& type) { 
      return type==MAJOR ? majorWidth_ : minorWidth_;
    }

    const int& PixelsPerTick() { return pixelsPerTick_;}
    void PixelsPerTick(const int& ppt){ pixelsPerTick_ = ppt;}
    const int& Ratio() { return ratio_;}
    void Ratio(const int& ratio){ ratio_ = ratio;}
  };
  
};

#endif
