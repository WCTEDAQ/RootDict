#ifndef MPMT_MESSAGES_H
#define MPMT_MESSAGES_H

#include <vector>
#include <mutex>
#include <TriggerType.h>

#ifndef __CLING__
#include <zmq.hpp>
#include <BinaryStream.h>
#endif

#include <SerialisableObject.h>

class TriggerInfo;

struct MPMTWaveformHeader{
  
  unsigned short GetHeader(){return ((data[0] & 0b11000000) >> 6 );}
  unsigned short GetFlags(){return ((data[0] & 0b00111100) >> 2 );}
  unsigned int GetCoarseCounter(){ return ((data[0] & 0b00000011) << 30 ) | (data[1] << 22) |  (data[2] << 14) | (data[3] << 6) | ((data[4] & 0b11111100) >> 2); }
  unsigned short GetChannel(){return  ((data[4] & 0b00000011) << 3) | ((data[5] & 0b11100000) >> 5 );}
  unsigned short GetNumSamples(){return (( data[5] & 0b00011111) << 11 ) | ( data[6] << 3) |  (( data[7] & 0b11100000) >> 5);}
  unsigned int GetLength(){ return ((data[7] & 0b00011111) << 12 ) | (data[8] << 4) |  ((data[9] & 0b11110000) >> 4); }
  unsigned short GetReserved(){return (data[9] & 0b00001111);}
  static unsigned int GetSize(){return sizeof(data);};
  unsigned char* GetData(){return data;}  
  
  void SetHeader(unsigned short in){ data[0] = (data[0] & 0b00111111) | ((in & 0b00000011) << 6);}
  void SetFlags(unsigned short in){ data[0] = (data[0] & 0b11000011) | ((in & 0b00001111) << 2);}
  void SetCoarseCounter(unsigned int in){
    data[0] = (data[0] & 0b11111100) | (in >> 30);
    data[1] = in >> 22;
    data[2] = in >> 14;
    data[3] = in >> 6;
    data[4] = (data[4] & 0b00000011) | ((in & 0b00111111) << 2);
  }
  void SetChannel(unsigned short in){
    data[4] = (data[4] & 0b11111100) | ((in & 0b00011000) >> 3);
    data[5] = (data[5] & 0b00011111) | ((in & 0b00000111) << 5);
    
  }
  void SetNumSamples(unsigned short in){
    data[5] = (data[5] & 0b11100000) | ((in >> 11) & 0b00011111 );
    data[6] = in >> 3;
    data[7] = (data[7] & 0b00011111) | ((in & 0b00000111) << 5);
  }
  void SetLength(unsigned int in){
    data[7] = (data[7] & 0b11100000) | ((in >> 12) & 0b00011111 );
    data[8] = in >> 4;
    data[9] = (data[9] & 0b00001111) | ((in & 0b00001111) << 4);
  }
  void SetReserved(unsigned short in){data[9] = (data[9] & 0b11110000) | (in & 0b00001111) ; }
  bool Print(){
        printf("header = %u\n",GetHeader());
	printf("flags = %u\n",GetFlags());
	printf("coarse_counter = %u\n",GetCoarseCounter());
	printf("channel = %u\n",GetChannel());
	printf("num_samples = %u\n",GetNumSamples());
	printf("length = %u\n",GetLength());
	printf("reserved = %u\n",GetReserved());
    std::cout<<"header = "<<GetHeader()<<std::endl;
    std::cout<<"flags = "<<GetFlags()<<std::endl;
    std::cout<<"coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<"channel = "<<GetChannel()<<std::endl;
    std::cout<<"num_samples = "<<GetNumSamples()<<std::endl;
    std::cout<<"length = "<<GetLength()<<std::endl;
    std::cout<<"reserved = "<<GetReserved()<<std::endl;

    return true;
  }
  
  unsigned char data[10];
  
};

struct P_MPMTWaveformHeader :SerialisableObject {

  MPMTWaveformHeader* waveform_header=0;
  unsigned char* bytes; //! << load bearing comment, don't serialise it ROOT
  unsigned long spill_num;
  unsigned short card_id;
  bool Print(){return waveform_header->Print();}
  P_MPMTWaveformHeader(){};
  
  P_MPMTWaveformHeader(MPMTWaveformHeader* in_waveform_header, unsigned long in_spill_num, unsigned short in_card_id){
    waveform_header=in_waveform_header;
    spill_num=in_spill_num;
    card_id=in_card_id;
    bytes = in_waveform_header->GetData() + in_waveform_header->GetLength();
  }

  ~P_MPMTWaveformHeader(){

    //delete waveform_header;
    waveform_header=0;
 
  }
  std::string GetVersion(){return "1.0";};
#ifndef __CLING__
  bool Serialise(BinaryStream &bs){

    if(waveform_header==0) waveform_header = new MPMTWaveformHeader();

    bs & waveform_header->data;

    if(bs.m_write) bs.Bwrite(&bytes[0], waveform_header->GetLength());
    else{
      bytes = new unsigned char[waveform_header->GetLength()];
      bs.Bread(&bytes[0], waveform_header->GetLength());
    }
    
    bs & spill_num;
    bs & card_id;

    return true;
  }
#endif
  
};



struct MPMTHit{
  
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
  bool Print(){
    std::cout<<" header = "<<GetHeader()<<std::endl;
    std::cout<<" event_type = "<<GetEventType()<<std::endl;
    std::cout<<" channel = "<<GetChannel()<<std::endl;
    std::cout<<" flags = "<<GetFlags()<<std::endl;
    std::cout<<" coarse_counter = "<<GetCoarseCounter()<<std::endl;
    std::cout<<" fine_time = "<<GetFineTime()<<std::endl;
    std::cout<<" charge = "<<GetCharge()<<std::endl;
    std::cout<<" quality_factor = "<<GetQualityFactor()<<std::endl;
    return true;
  }

  unsigned char data[11];
  
};

struct P_MPMTHit: SerialisableObject {

  MPMTHit* hit;
  bool Print(){ return hit->Print();}
  
  unsigned long spill_num;
  unsigned short card_id;
  std::string GetVersion(){return "1.0";};
  P_MPMTHit(){
    hit=nullptr;
    spill_num=0;
    card_id=0;
  };
  P_MPMTHit(MPMTHit* in_hit, unsigned long in_spill_num, unsigned short in_card_id){
    hit=in_hit;
    spill_num=in_spill_num;
    card_id = in_card_id;
  }
  ~P_MPMTHit(){
    //delete hit;
    hit=0;

  }
  
#ifndef __CLING__
  bool Serialise(BinaryStream &bs){

    if(hit==0) hit=new MPMTHit();
    
    bs & hit->data;
    bs & spill_num;
    bs & card_id;

    return true;
  }
#endif
  
};







 
#ifndef __CLING__   // TODO move classes to independent headers
class MPMTMessage : public SerialisableObject{

 public:
  
  zmq::message_t daq_header;
  zmq::message_t mpmt_data;
  std::vector<P_MPMTHit> hits;
  std::vector<P_MPMTWaveformHeader> waveforms;
  
  bool Print(){ return true;}
  std::string GetVersion(){return "1.0";}
  bool Serialise(BinaryStream &bs){

    unsigned int tmp = daq_header.size();
    bs & tmp;
    bs.Bwrite(daq_header.data(), daq_header.size());
    tmp = mpmt_data.size();
    bs & tmp;
    bs.Bwrite(mpmt_data.data(),  mpmt_data.size());

    return true;
  }


};

class MPMTCollection : public SerialisableObject{

 public:
  std::vector<MPMTMessage*> mpmt_output;
  
  std::vector<TriggerInfo*> triggers_info;
  std::vector<P_MPMTHit*> triggers;
  std::mutex mtx;

  ~MPMTCollection(){

    for(unsigned int i=0; i<mpmt_output.size(); i++){
      delete mpmt_output.at(i);
      mpmt_output.at(i)=0;
    }
    mpmt_output.clear();

     for(unsigned int i=0; i<triggers_info.size(); i++){
      delete triggers_info.at(i);
      triggers_info.at(i)=0;
    }
    triggers_info.clear();
    
     for(unsigned int i=0; i<triggers.size(); i++){
      delete triggers.at(i);
      triggers.at(i)=0;
    }
    triggers.clear();
   
  }
  
  bool Print(){ return true;}
  std::string GetVersion(){return "1.0";}
  bool Serialise(BinaryStream &bs){

    unsigned int tmp = mpmt_output.size();
    bs & tmp;
    for(int i=0; i<mpmt_output.size(); i++){
      bs & (*mpmt_output.at(i));
    }

    return true;
  }
  
};
#endif  // CLING ignore



#endif
