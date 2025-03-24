#ifndef TRIGGER_TYPE_H
#define TRIGGER_TYPE_H

#include <vector>
#include <string>

#ifndef __CLING__
#include <BinaryStream.h>
#endif

#include <SerialisableObject.h>
#include <MPMTData.h>


class DataModel;


struct MPMTLED{

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

  bool Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" led = "<<GetLED()<<std::endl;
    std::cout<<" gain = "<<GetGain()<<std::endl;
    std::cout<<" dac_setting = "<<GetDACSetting()<<std::endl;
    std::cout<<" type = "<<GetType()<<std::endl;
    std::cout<<" sequence_number = "<<GetSequenceNumber()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" reserved = "<<GetReserved()<<std::endl;
    return true;
  }


  unsigned char data[9];

};

struct P_MPMTLED : SerialisableObject {

  MPMTLED* led;
  
  unsigned char card_id;

  P_MPMTLED(){
    // spill_num=0;
    card_id=0;
    led=0;
  }
  P_MPMTLED(MPMTLED* in_led, unsigned char in_card_id){
    led=in_led;
    card_id=in_card_id;
  }
  ~P_MPMTLED(){
    //delete led;
    led=0;
  }
  bool Print(){return led->Print();}

  std::string GetVersion(){return "1.0";}
#ifndef __CLING__
  bool Serialise(BinaryStream &bs){

    if(led==0) led= new MPMTLED();
    bs & led->data;
    //    bs & spill_num;
    bs & card_id;
    return true;
  }
#endif
};


enum class TriggerType{ LASER, NHITS, LED, MAIN, MBEAM, EBEAM, NONE, HARD6 };

class TriggerInfo : public SerialisableObject {

public:

  ~TriggerInfo(){

    for(unsigned int i=0; i< mpmt_LEDs.size(); i++){
      delete mpmt_LEDs.at(i);
      mpmt_LEDs.at(i)=0;

    }
    mpmt_LEDs.clear();
    
  }
  TriggerType type;
  unsigned long time;
  std::vector<P_MPMTLED*> mpmt_LEDs;
  unsigned int card_id=0;
  unsigned long spill_num=0;
  unsigned vme_event_num=0; 
  
  bool Print(){
    std::cout<<"Trigger time = "<<time<<std::endl;
    
    return true;
  }
  std::string GetVersion(){return "1.0";}
#ifndef __CLING__
  bool Serialise(BinaryStream &bs){
    
    bs & type;
    bs & time;
    bs & mpmt_LEDs;
    bs & card_id;
    bs & spill_num;
    bs & vme_event_num;

    return true;
  }
#endif
  
};

#ifndef __CLING__
struct Trigger_algo_args : Thread_args {
  Trigger_algo_args(){
    m_data=0;
    sorted_data=0;
    trigger_vars=0;
  };
  ~Trigger_algo_args(){
    m_data=0;
    trigger_vars=0;
    //    delete sorted_data;
    sorted_data=0;
  };
  MPMTData* sorted_data;
  DataModel* m_data;
  Store* trigger_vars;
};
#endif

#endif
