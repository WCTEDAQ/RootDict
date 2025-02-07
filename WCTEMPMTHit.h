#ifndef WCTE_MPMT_HIT_H
#define WCTE_MPMT_HIT_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

class WCTEMPMTHit{

public:
  WCTEMPMTHit(){std::fill(std::begin(data), std::end(data), 0);}
  WCTEMPMTHit(unsigned short& in_card_id, unsigned char* in_data){ card_id=in_card_id; memcpy(&data[0], in_data, sizeof(data));}
  
  unsigned short GetCardID(){return card_id;}
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetChannel(){return ((data[0] & 0b00000011) << 3) | ((data[1] & 0b11100000) >> 5); }
  unsigned short GetFlags(){return (data[1] & 0b00011111);}
  unsigned int GetCoarseCounter(){ return (data[2] << 24) | (data[3] << 16) | (data[4] << 8 ) | (data[5]) ;}
  unsigned short GetFineTime(){return (data[6] << 8) | (data[7]);}
  unsigned short GetCharge(){return (data[8] << 8 ) | data[9] ;}
  unsigned short GetQualityFactor(){return data[10];}
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}

  void SetCardID(unsigned short in){ card_id=in;}
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetChannel(unsigned short in){
    data[0] = (data[0] & 0b11111100) | ((in & 0b00011000) >> 3);
    data[1] = (data[1] & 0b00011111)  | ((in & 0b00000111) << 5);
   }
  void SetFlags(unsigned short in){ data[1] = (data[1] & 0b11100000) | (in & 0b00011111);}
  void SetCoarseCounter(unsigned int in){
    data[2] = in >> 24;
    data[3] = in >> 16;
    data[4] = in >> 8;
    data[5] = in;
  }
  void SetFineTime(unsigned short in){
    data[6] = in >> 8;
    data[7] = in;
  }
  void SetCharge(unsigned short in){
    data[8] = in >> 8;
    data[9] = in;
  }
  void SetQualityFactor(unsigned short in){ data[10] = in;}
  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" channel = "<<GetChannel()<<std::endl;
    std::cout<<" flags = "<<GetFlags()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" fine_time = "<<GetFineTime()<<std::endl;
    std::cout<<" charge = "<<GetCharge()<<std::endl;
    std::cout<<" quality_factor = "<<GetQualityFactor()<<std::endl;
  }


private:

  unsigned short card_id;
  unsigned char data[11];
  
};


union UWCTEMPMTHit{

  UWCTEMPMTHit(){bits.reset();}
  WCTEMPMTHit hit;
  std::bitset<88> bits;
  

};


#endif
