#ifndef WCTE_MPMT_PPS_H
#define WCTE_MPMT_PPS_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

class WCTEMPMTPPS{

public:
  WCTEMPMTPPS(){std::fill(std::begin(data), std::end(data), 0);}
  WCTEMPMTPPS(unsigned short& in_card_id, unsigned char* in_data){ card_id=in_card_id; memcpy(&data[0], in_data, sizeof(data));}
  
  unsigned short GetCardID(){return card_id;}
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetReserved(){return (data[0] & 0b00000011); }
  unsigned long GetPreviousPPSCoarseCounter(){ return (((unsigned long)data[1]) << 56) | (((unsigned long)data[2]) << 48) | (((unsigned long)data[3]) << 40) | (((unsigned long)data[4]) << 32) | (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]; }
  unsigned long GetCurrentPPSCoarseCounter(){return (((unsigned long)data[9]) << 56) | (((unsigned long)data[10]) << 48) | (((unsigned long)data[11]) << 40) | (((unsigned long)data[12]) << 32) | (data[13] << 24) | (data[14] << 16) | (data[15] << 8) | data[16];  }
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}  

  void SetCardID(unsigned short in){ card_id=in;}
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetReserved(unsigned short in){ data[0] = (data[0] & 0b11111100) | (in & 0b00000011);}
  void SetPreviousPPSCoarseCounter(unsigned long in){
    data[1] = in >> 56;
    data[2] = in >> 48;
    data[3] = in >> 40;
    data[4] = in >> 32;
    data[5] = in >> 24;
    data[6] = in >> 16;
    data[7] = in >> 8;
    data[8] = in;
  }
  void SetCurrentPPSCoarseCounter(unsigned long in){
    data[9] = in >> 56;
    data[10] = in >> 48;
    data[11] = in >> 40;
    data[12] = in >> 32;
    data[13] = in >> 24;
    data[14] = in >> 16;
    data[15] = in >> 8;
    data[16] = in;
  }
  
  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
    std::cout<<" previous_PPS_coarse_counter = "<<GetPreviousPPSCoarseCounter()<<std::endl;
    std::cout<<" current_PPS_coarse_counter = "<<GetCurrentPPSCoarseCounter()<<std::endl;
    
  }

  
private:

  unsigned short card_id;
  unsigned char data[17];

  
};


union UWCTEMPMTPPS{

  UWCTEMPMTPPS(){bits.reset();}
  WCTEMPMTPPS pps;
  std::bitset<136> bits;
  

};


#endif
