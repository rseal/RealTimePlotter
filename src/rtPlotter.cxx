////////////////////////////////////////////////////////////////////////////////
///
/// Main loop that loads the data to the plotter
///
///Author: Ryan Seal
///Modified: 08/06/08
////////////////////////////////////////////////////////////////////////////////
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <fstream>
#include "../include/Plot.hpp"
#include <fltk/Window.h>
#include <fltk/run.h>
#include <fltk/damage.h>
#include <boost/multi_array.hpp>
#include <iostream>
#include <random>
#include <gnuradar/SThread.h>


//#include <fltk/Threads.h>
using namespace std;
using namespace splot;
using namespace fltk;

//threaded class to create random data sequences for real-time display test
class DataSource: public SThread{
   int size_;
   typedef boost::multi_array<float,2> Array;
   typedef boost::interprocess::file_lock FileLock;
   typedef boost::interprocess::scoped_lock<FileLock> ScopedLock;
   Array data_;
   Plot* plotPtr_;
   Window* win_;
   std::uniform_real<> ud_;
   std::minstd_rand eng_;
   bool quit_;
   string dataFile_;
   float *dPtr_;
   FileLock fLock_;
   public:

   DataSource(const string& dataFile):quit_(false), dataFile_(dataFile), fLock_(dataFile.c_str()){
      size_ = PlotSize();
      data_.resize(boost::extents[size_/2][2]);
   }
   
   const int PlotSize(){
      ScopedLock sl(fLock_);
      ifstream in(dataFile_.c_str(), ios::in);
      in.seekg(0,ios::end);
      size_ = in.tellg()/sizeof(float);
      in.close();
      cout << "size = " << size_ << endl;
      return size_;
   }

   void Run(){
      
      while(!quit_){
         fLock_.lock();
         ifstream in(dataFile_.c_str(),ios::in);
         in.read(reinterpret_cast<char*>(&data_[0][0]),size_*sizeof(float));
         in.close();
         fLock_.unlock();

         //these thread locks share a mutex with the main thread and are required
         //for proper serialization of FLTK graphical function calls.
         fltk::lock();
         plotPtr_->Load(data_);
         fltk::awake();

         fltk::flush();
         fltk::unlock();
         Sleep(ST::ms,100);
      }
      cout << "exiting thread" << endl;
   }

   void Register(Plot* plotPtr){
      plotPtr_ = plotPtr;
   }

   void Quit(){quit_ = true; Wait();}
   const Array& ArrayRef() {return data_;}

   const int& Size() {return size_;}
};
//create callback to exit thread properly and shutdown program
void Exit(Widget* widget, void *data){
   DataSource* dg = reinterpret_cast<DataSource*>(data);
   dg->Quit();
   widget->hide();
};

///Main loop for program execution
int main()
{
   string dataFile("/dev/shm/splot.buf");
   DataSource dSrc(dataFile);
   Plot *ui = new Plot(0,0,750,500,dSrc.ArrayRef(),"Channel 1");
   dSrc.Register(ui);

   //setup plot parameters
   fltk::lock();
   ui->label("Channel 1");
   ui->user_data(reinterpret_cast<void*>(&dSrc));
   ui->callback(Exit);
   ui->set_double_buffer();
   ui->resizable(ui);
   ui->EnableTicks(true);
   ui->EnableGrid(true);
   ui->EnableLabels(true);
   ui->XTick(splot::MAJOR, 15, 3);
   ui->XTick(splot::MINOR, 9, 2);
   ui->YTick(splot::MAJOR, 15, 3);
   ui->YTick(splot::MINOR, 9, 2);
   ui->show();
   fltk::awake();
   fltk::check();
   fltk::unlock();

   //start data generation thread
   dSrc.Start();

   return run();
};
