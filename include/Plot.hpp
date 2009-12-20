#ifndef PLOT_HPP
#define PLOT_HPP

////////////////////////////////////////////////////////////////////////////////
///UsrpInterface.h
///
///Primary Display window for USRP configuration GUI interface.
///
///Author: Ryan Seal
///Modified: 08/06/08
////////////////////////////////////////////////////////////////////////////////
#include "config.h"
#include <fltk/run.h>
#include <fltk/Window.h>
#include <fltk/draw.h>
#include <fltk/damage.h>
#include <fltk/layout.h>
#include <fltk/events.h>
#include <fltk/Group.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

#include <boost/multi_array.hpp>
#include <boost/multi_array/index_gen.hpp>
#include <boost/multi_array/index_range.hpp>
#include <boost/multi_array/types.hpp>

#include "Axis.hpp"

namespace splot{

   enum {XAXIS=0,YAXIS=1};

   ///Provides a container and display interface for the 
   ///USRP data collection system.
   class Plot : public fltk::Window 
   {

      fltk::Color gridColor_, bgColor_, fgColor_, xTickColor_,
         yTickColor_, pbgColor_, axisColor_;

      //typedef boost::multi_array<float,2> Array2D;
      typedef boost::multi_array_ref<float,2> Array2DRef;

      int x_,y_,w_,h_;
      int mx_,my_;
      int size_;

      //plot area
      int px_,py_,pw_,ph_;
      int textBorder_;

      Array2DRef data_;
      float dMid_;
      float dRatio_;
      float hShift_;
      float vShift_;
      float vScale_;
      float hScale_;
      bool  ticksEnabled_;
      bool  gridEnabled_;
      bool  labelsEnabled_;
      bool  validData_;

      std::string xLabel_;
      std::string yLabel_;
      std::string tLabel_;

      Axis xAxis_;
      Axis yAxis_;

      void FillBackground();
      void FillPlotArea();
      void DrawGrid();
      void DrawTicks();
      void DrawLabels();
      void CalculateTicks();
      void ScaleData();

      public:

      ///Constructor
      Plot(const int& x, const int& y, const int& w, const int& h, 
            Array2DRef data, const std::string& name);

      void Load(Array2DRef data);

      void UpdateCoordinates(){
         x_ = x();
         y_ = y();
         w_ = w();
         h_ = h();
         mx_ = w_/2.0;
         my_ = h_/2.0;
         px_ = textBorder_;
         py_ = h_ - textBorder_;
         pw_ = w_ - 2*textBorder_;
         ph_ = h_ - 2*textBorder_;
      }

      void XTick(const int& type, const int& length, const int& width=1){
         if(type == splot::MAJOR)
            xAxis_.MajorTick(length,width);
         else
            xAxis_.MinorTick(length,width);
      }

      void YTick(const int& type, const int& length, const int& width=1){
         if(type == splot::MAJOR)
            yAxis_.MajorTick(length,width);
         else
            yAxis_.MinorTick(length,width);
      }

      void Scale(float scale, const int& type);
      void Shift(float shift, const int& type);
      void EnableTicks(const bool& enable) { ticksEnabled_ = enable;}
      void EnableGrid(const bool& enable) { gridEnabled_ = enable;}
      void EnableLabels(const bool& enable) { labelsEnabled_ = enable;}
      void XLabel(const std::string& label) { xLabel_ = label;}
      void YLabel(const std::string& label) { yLabel_ = label;}
      void Title (const std::string& label) { tLabel_ = label;}

      void draw();
      ~Plot(){exit(0);};
   };
};
#endif
