#ifndef WCTE_MPMT_LED_H
#define WCTE_MPMT_LED_H

#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

class WCTEMPMTLED{

public:
  WCTEMPMTLED(){std::fill(std::begin(data), std::end(data), 0);}
  WCTEMPMTLED(unsigned short& in_card_id, unsigned char* in_data){ card_id=in_card_id; memcpy(&data[0], in_data, sizeof(data));}

  unsigned short GetCardID(){return card_id;}
  unsigned short GetHeader(){return (data[0] & 0b11000000) >> 6; }
  unsigned short GetEventType(){return (data[0] & 0b00111100) >> 2;}
  unsigned short GetLED(){return ((data[0] & 0b00000011) << 1) | ((data[1] & 0b10000000) >> 7); }
  bool GetGain(){return ((data[1] & 0b01000000) >> 6);}
  unsigned short GetDACSetting(){return ((data[1] & 0b00111111)  << 4 ) | ((data[2] & 0b11110000) >>4 ) ;}
  unsigned short GetType(){return ((data[2] & 0b00001100)  >> 2 );}
  unsigned short GetSequenceNumber(){return ((data[2] & 0b00000011)  << 12 ) | (data[3] << 4 ) | ((data[4] & 0b11110000 ) >> 4 ) ;}
  unsigned int GetCoarseCounter(){ return ((data[4] & 0b00001111) << 28) | (data[5] << 20) | (data[6] << 12 ) | (data[7] << 4) | ((data[8] & 0b11110000 ) >> 4 ) ;}
  unsigned short GetReserved(){return (data[8] & 0b00001111);}
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}  


  
  void SetCardID(unsigned short in){ card_id=in;}
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetEventType(unsigned short in){ data[0] = (data[0] & 0b11000011) | (( in & 0b00001111) << 2) ;}
  void SetLED(unsigned short in){
    data[0] = (data[0] & 0b11111100) | ((in & 0b00000110) >> 1);
    data[1] = (data[1] & 0b01111111)  | ((in & 0b00000001) << 7);
   }
  void SetGain(bool in){ data[1] = (data[1] & 0b10111111) | ((in & 0b00000001) << 6) ;}
  void SetDACSetting(unsigned short in){
    data[1] = (data[1] & 0b11000000) | ((in >> 4 ) & 0b00111111) ;
    data[2] = (data[2] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetType(unsigned short in){ data[2] = (data[2] & 0b11110011) | ((in & 0b00000011) << 2) ;}
  void SetSequenceNumber(unsigned short in){
    data[2] = (data[2] & 0b11111100) | ((in >> 12 ) & 0b00000011) ;
    data[3] = (in >> 4);
    data[4] = (data[4] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetCoarseCounter(unsigned int in){
    data[4] = (data[4] & 0b11110000)  | ((in >> 28 ) & 0b00001111);
    data[5] = in >> 20;
    data[6] = in >> 12;
    data[7] = in >> 4;
    data[8] = (data[8] & 0b00001111)  | ((in & 0b00001111) << 4 );
  }
  void SetReserved(unsigned short in){ data[8] = (data[8] & 0b11110000) | (in & 0b00001111) ;}

  void Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" led = "<<GetLED()<<std::endl;
    std::cout<<" gain = "<<GetGain()<<std::endl;
    std::cout<<" dac_setting = "<<GetDACSetting()<<std::endl;
    std::cout<<" type = "<<GetType()<<std::endl;
    std::cout<<" sequence_number = "<<GetSequenceNumber()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
  }


private:

  unsigned short card_id;
  unsigned char data[9];

};


union UWCTEMPMTLED{

  UWCTEMPMTLED(){bits.reset();}
  WCTEMPMTLED led;
  std::bitset<73> bits;
  

};


#endif
