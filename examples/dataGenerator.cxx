////////////////////////////////////////////////////////////////////////////////
///dataGenerator.cxx
///
/// Main loop that loads UsrpInterface
///
///Author: Ryan Seal
///Modified: 08/06/08
////////////////////////////////////////////////////////////////////////////////
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/multi_array.hpp>
#include <iostream>
#include <random>
#include <fstream>
#include "../include/SThread.h"

using namespace std;

//threaded class to create random data sequences for real-time display test
class DataGen: public SThread{
   int size_;
   typedef boost::interprocess::file_lock FileLock;
   typedef boost::multi_array<float,2> Array;
   Array data_;
   std::uniform_real<> ud_;
   std::minstd_rand eng_;
   bool quit_;
   public:
   //typedef void(fptr)(*Array2DRef data) FunPtr;

   DataGen(const int& size): 
      size_(size), data_(boost::extents[size][2]), quit_(false){}

   void Run(){
      int i,j;
      j=0;

      while(!quit_){
         cout << "iter = " << j++ << endl;
	 ofstream out("/dev/shm/splot.buf",ios::out);
         FileLock fLock("/dev/shm/splot.buf");
         fLock.lock();
	 for(i=0; i<size_; ++i) {
	    data_[i][1]=   ud_(eng_)*1e-9 +  ((i>0.015*size_ && i<0.150*size_) ? 25*sin(60*M_PI*i/size_): 0);
	    data_[i][0]=i;
	    out.write(reinterpret_cast<char*>(&(data_[i][0])),sizeof(float));
	    out.write(reinterpret_cast<char*>(&(data_[i][1])),sizeof(float));
	 }
	 out.close();
         fLock.unlock();
	 Sleep(ST::ms,100);
      }
      cout << "exiting thread" << endl;
   }

   void Quit(){quit_ = true; Wait();}
   const int& Size() {return size_;}
};

///Main loop for program execution
int main()
{
   bool quit=false;
   DataGen dgen(800);
   string str;
   //start data generation thread
   dgen.Start();
   while(!quit){ 
      cin >> str;
      if(str.find("q") != string::npos){
	 dgen.Quit();
	 break;
      }
   }
   return 0;
};
