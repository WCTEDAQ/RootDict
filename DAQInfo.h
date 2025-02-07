#ifndef DAQ_INFO_H
#define DAQ_INFO_H

#include <string>
#include <iostream>
#include <SerialisableObject.h>
#ifndef __CLING__
#include <BinaryStream.h>
#endif

class DAQInfo : public SerialisableObject {

 public:
  
  unsigned long run_number;
  unsigned long sub_run_number;

  bool Print(){
    std::cout<<"run_numner="<<run_number<<std::endl;
    std::cout<<"sub_rub_number="<<sub_run_number<<std::endl;

    return true;
  }
  
  std::string GetVersion(){ return "1.0";}

#ifndef __CLING__
   bool Serialise(BinaryStream &bs){

    bs & run_number;
    bs & sub_run_number;

    return true;
  }
#endif
  
};



#endif
