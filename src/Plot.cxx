////////////////////////////////////////////////////////////////////////////////
///Plot.cxx
///Author: Ryan Seal
///Modified: 05/11/09
////////////////////////////////////////////////////////////////////////////////
#include "../include/Plot.hpp"
#include <cmath>

#include <boost/algorithm/minmax.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/lexical_cast.hpp>

using namespace fltk;
using namespace std;
using namespace splot;

Plot::Plot(const int& x, const int& y, const int& w, const int& h, 
      Array2DRef data, const std::string& name): 
   fltk::Window(w,h,name.c_str()), textBorder_(50), data_(data),
   ticksEnabled_(false), gridEnabled_(false), labelsEnabled_(false), 
   validData_(false), xLabel_("X Axis"), yLabel_("Y Axis"),
   tLabel_("Plot Title")
{
   gridColor_  = fltk::GRAY80;
   bgColor_    = 0x55555500;
   xTickColor_ = 0x22222200;
   yTickColor_ = 0x22222200;
   fgColor_    = fltk::WHITE;
   pbgColor_   = 0xBBBBEE00;
   axisColor_  = fltk::GRAY10;

   this->when(fltk::WHEN_NEVER);
   //int baseX      = 25;
   //int baseY      = 25;
   //int tab1Width  = 50;
   //int width2     = 80;
   //int tab1Height = 30;
   this->box(fltk::UP_BOX);
}

//load 2D plot data
void Plot::Load(Array2DRef data)
{
  if(data.size() != data_.size())
     UpdateCoordinates();
  validData_=true;
   data_ = data;
   ScaleData();
   redraw();
}

inline
void Plot::ScaleData(){ 
   //create 1D view of array (y-data)
   Array2DRef::index_gen indices;
   typedef boost::multi_array_types::index_range range;
   typedef Array2DRef::array_view<1>::type::iterator Iterator;
   Array2DRef::array_view<1>::type view1D = data_[indices[range()][1]];

   //use efficient boost minmax library for 2n performance
   std::pair<Iterator,Iterator> result = boost::minmax_element(view1D.begin(),view1D.end());
   float dMin(*(result.first)),dMax(*(result.second));

   dMid_  = (dMin + dMax)/2.0f;
   dRatio_= ph_/(abs(dMax)+abs(dMin));
} 

void Plot::DrawLabels(){

   typedef boost::multi_array_types::index_range range;
   typedef Array2DRef::array_view<1>::type::iterator Iterator;

   Array2DRef::index_gen indices;
   Array2DRef::array_view<1>::type view1D = data_[indices[range()][0]];
   TickVector& xTicks = xAxis_.GetRef();
   const int numXTicks = xTicks.size();
   string label;
   int i;
   fltk::Flags fl = fltk::RAW_LABEL | fltk::ALIGN_CENTER;
   setfont(labelfont(),labelsize());

   //create x tick array
   for(i=0; i<numXTicks; ++i){
      const int type = (i%xAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      if(type == splot::MAJOR){
         const Rectangle rec(xTicks[i].P0().X()-10,xTicks[i].P0().Y()+7,20,10);
         label = boost::lexical_cast<string>(view1D[i]);
         drawtext(label.c_str(),rec, fl);
      }
   }
   drawtext(xLabel_.c_str(), Rectangle(w_/2,py_+textBorder_/2.0,50,10),fl);
   //fltk::push_matrix();
   //  drawtext(yLabel_.c_str(), 
   // 	     Rectangle(textBorder_/2.0,h_/2.0,50,10),
   // 	     Flags());
   //drawtext_transformed(yLabel_.c_str(), yLabel_.length(), 0.0,h_/2.0);
   //fltk::pop_matrix();
}
//anytime a resize is required, ticks are recalculated here
void Plot::CalculateTicks(){

   TickVector& xTicks = xAxis_.GetRef();
   TickVector& yTicks = yAxis_.GetRef();

   xTicks.clear();
   yTicks.clear();

   int numXTicks = pw_ / xAxis_.PixelsPerTick();
   int numYTicks = ph_ / yAxis_.PixelsPerTick();
   int i,x,y;

   //preallocate memory to speed up push_backs
   xTicks.reserve(numXTicks);
   yTicks.reserve(numYTicks);

   //create x tick array
   for(i=0; i<numXTicks; ++i){
      x = px_ + i*xAxis_.PixelsPerTick();
      y = py_;
      const int type = (i%xAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      const Point p0(x,y);
      const Point p1(x, y - yAxis_.TickLength(type));
      const Tick tick(p0,p1);
      xTicks.push_back(tick);
   }

   //create y tick array
   for(i=0; i<numYTicks; ++i){
      x = px_;
      y = py_ - i*yAxis_.PixelsPerTick();
      const int type = (i%yAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      const Point p0(x,y);
      const Point p1(x + yAxis_.TickLength(type),y);
      const Tick tick(p0,p1);
      yTicks.push_back(tick);
   }
}

//display ticks on plot
inline
void Plot::DrawTicks(){

   TickVector& xTicks = xAxis_.GetRef();
   TickVector& yTicks = yAxis_.GetRef();
   int type;
   const fltk::Color cColor = color();

   fltk::setcolor(xTickColor_);
   for(uint i=0; i<xTicks.size(); ++i){
      type = (i%xAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      line_style(fltk::SOLID, xAxis_.TickWidth(type));
      fltk::drawline(xTicks[i].P0().X(),
            xTicks[i].P0().Y(),
            xTicks[i].P1().X(),
            xTicks[i].P1().Y());
   }

   fltk::setcolor(yTickColor_);
   for(uint i=0; i<yTicks.size(); ++i){
      type = (i%yAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      line_style(fltk::SOLID, yAxis_.TickWidth(type));
      fltk::drawline(yTicks[i].P0().X(),
            yTicks[i].P0().Y(),
            yTicks[i].P1().X(),
            yTicks[i].P1().Y());
   }

   fltk::setcolor(cColor);
}

//display grid
inline
void Plot::DrawGrid(){
   TickVector& xTicks = xAxis_.GetRef();
   TickVector& yTicks = yAxis_.GetRef();
   int type;
   const fltk::Color cColor = color();

   //cout << "draw grid" << endl;
   line_style(fltk::DOT, 0);

   fltk::setcolor(gridColor_);
   for(uint i=0; i<xTicks.size(); ++i){
      type = (i%xAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      //if(type==splot::MAJOR) cout << "major" << endl;
      //line_style(fltk::SOLID, xAxis_.TickWidth(type));
      fltk::drawline(xTicks[i].P1().X(),
            xTicks[i].P1().Y(),
            xTicks[i].P1().X(),
            xTicks[i].P1().Y()-ph_+ xAxis_.TickLength(type));
   }

   for(uint i=0; i<yTicks.size(); ++i){
      type = (i%yAxis_.Ratio()) ? splot::MINOR : splot::MAJOR;
      //line_style(fltk::SOLID, yAxis_.TickWidth(type));
      fltk::drawline(yTicks[i].P1().X(),
            yTicks[i].P1().Y(),
            yTicks[i].P1().X()+pw_ - yAxis_.TickLength(type),
            yTicks[i].P1().Y());
   }

   fltk::setcolor(cColor);

}

inline
void Plot::FillBackground(){
   //fill total area
   fltk::setcolor(bgColor_);
   fltk::fillrect(0,0,w_,h_);
}

inline
void Plot::FillPlotArea(){
   //fill plot area
   fltk::setcolor(pbgColor_);
   fltk::fillrect(px_,py_-ph_,pw_,ph_);
}


void Plot::draw()
{
   UpdateCoordinates();
   CalculateTicks();
   if(validData_) ScaleData();

   hScale_ = 1.0f*pw_/data_.size();
   vScale_ = 0.75f;
   vShift_ = ph_/2.0f - vScale_*dRatio_*dMid_;

   FillBackground();
   FillPlotArea();

   if(gridEnabled_) DrawGrid();

   fltk::push_matrix();
   line_style(fltk::SOLID,2);
   fltk::concat(hScale_,0,0,vScale_*dRatio_,px_,textBorder_+vShift_);
   fltk::setcolor(0x0000FF00);
   fltk::addvertices(data_.size(),reinterpret_cast<float(*)[2]>(&data_[0][0]) );
   //setcolor(GREEN);
   fltk::strokepath();
   fltk::pop_matrix();

   if(ticksEnabled_){
      fltk::setcolor(xTickColor_);
      DrawTicks();
      fltk::setcolor(axisColor_);
      line_style(fltk::SOLID, 5);
      fltk::drawline(px_,py_,px_+pw_,py_);
      fltk::drawline(px_,py_,px_,py_-ph_);
   }

   if(labelsEnabled_){
      fltk::setcolor(WHITE);
      DrawLabels();
   }
}

