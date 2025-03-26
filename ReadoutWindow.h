#ifndef READOUT_WINDOW_H
#define READOUT_WINDOW_H

#include <vector>
#include <string>
#include <SerialisableObject.h>
#ifndef __CLING__
#include <BinaryStream.h>
#endif

#include <WCTEMPMTHit.h>
#include <WCTEMPMTWaveform.h>
#include <WCTEMPMTLED.h>
#include <HKMPMTHit.h>
#include <TriggerType.h>
#include <TDCHit.h>
#include <QDCHit.h>
#include <MPMTMessages.h>
#ifndef __CLING__
#include <zmq.hpp>
#endif

class ReadoutWindow : SerialisableObject{

public:
  
  std::vector<TriggerInfo> triggers_info;
  std::vector<WCTEMPMTHit> mpmt_hits;  
  std::vector<WCTEMPMTWaveform> mpmt_waveforms;
  std::vector<HKMPMTHit> hk_mpmt_hits;
  std::vector<WCTEMPMTHit> extra_hits;
  std::vector<WCTEMPMTWaveform> extra_waveforms;
  std::vector<TDCHit> tdc_hits;
  std::vector<QDCHit> qdc_hits;
  unsigned long start_counter;
  unsigned long readout_num;

  bool Print(){

    std::cout<<"/////////////////////////trigger info//////////////////"<<std::endl;
    for(int i=0; i<triggers_info.size(); i++){ std::cout<<"///["<<i<<"]///"<<std::endl; triggers_info.at(i).Print();}
    std::cout<<"/////////////////////////mpmt hits//////////////////"<<std::endl;
    for(int i=0; i<mpmt_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; mpmt_hits.at(i).Print();}
    std::cout<<"/////////////////////////mpmt waveforms//////////////////"<<std::endl;
    for(int i=0; i<mpmt_waveforms.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; mpmt_waveforms.at(i).Print();}
    std::cout<<"/////////////////////////hk mpmt hits//////////////////"<<std::endl;
    for(int i=0; i<hk_mpmt_hits.size(); i++){ std::cout<<"///["<<i<<"]///"<<std::endl; hk_mpmt_hits.at(i).Print();}
    std::cout<<"/////////////////////////extra hits//////////////////"<<std::endl;
    for(int i=0; i<extra_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; extra_hits.at(i).Print();}
    std::cout<<"/////////////////////////extra waveforms//////////////////"<<std::endl;
    for(int i=0; i<extra_waveforms.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; extra_waveforms.at(i).Print();}
    std::cout<<"/////////////////////////tdc data//////////////////"<<std::endl;
    for(int i=0; i<tdc_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; tdc_hits.at(i).Print();}
    std::cout<<"/////////////////////////qdc data//////////////////"<<std::endl;
    for(int i=0; i<qdc_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; qdc_hits.at(i).Print();}   
    std::cout<<"///////////////////////////start counter ="<<start_counter<<"////////////////"<<std::endl;
   std::cout<<"///////////////////////////readout numebr ="<<readout_num<<"////////////////"<<std::endl;
   std::cout<<"///////////////////////////////////////////"<<std::endl<<std::endl;
    return true;

  }
  std::string GetVersion(){return "1.0";};
#ifndef __CLING__
  bool Serialise(BinaryStream &bs){
    
    bs & triggers_info;
    bs & mpmt_hits;
    bs & mpmt_waveforms;
    bs & hk_mpmt_hits;
    bs & extra_hits;
    bs & extra_waveforms;
    bs & tdc_hits;
    bs & qdc_hits;

    return true;
  }
#endif
  
};


class PReadoutWindow : SerialisableObject{

public:
  
  std::vector<TriggerInfo*>* triggers_info;
  std::vector<P_MPMTHit*> mpmt_hits;  
  std::vector<P_MPMTWaveformHeader*> mpmt_waveforms;
  std::vector<HKMPMTHit*> hk_mpmt_hits;
  std::vector<P_MPMTHit*>* trigger_hits;
  //std::vector<P_MPMTLED*>* leds;
  //std::vector<WCTEMPMTWaveform*> extra_waveforms;
  std::vector<TDCHit*> tdc_hits;
  std::vector<QDCHit*> qdc_hits;
  unsigned long start_counter;
  unsigned long readout_num;
  unsigned long spill_num;
  
#ifndef __CLING__
  MPMTCollection* mpmt_collection;
#else
  int* mpmt_collection; // dummy  
#endif
  PReadoutWindow(){
    triggers_info=0;
    trigger_hits=0;
    mpmt_collection=0;
  }
  ~PReadoutWindow(){
    ////printf("e1 %u\n", triggers_info->size());
    //    for(unsigned int i=0; i< triggers_info->size(); i++){
    // delete triggers_info->at(i);
    // triggers_info->at(i)=0;
    //}
    
    //triggers_info->clear();
    //printf("e1.5\n");
#ifndef __CLING__
    if(triggers_info != &(mpmt_collection->triggers_info)) delete triggers_info;
#endif
    triggers_info =0;
    //printf("e2\n");
    
    //for(unsigned int i=0; i< mpmt_hits.size(); i++){
      ////printf("e2/1\n");
      ////delete mpmt_hits.at(i);
      //mpmt_hits.at(i)=0;
      ////printf("e2.1\n");
    // }
    mpmt_hits.clear();
    //printf("e3\n");
    
    //for(unsigned int i=0; i< mpmt_waveforms.size(); i++){
      //delete mpmt_waveforms.at(i);
      //mpmt_waveforms.at(i)=0;
      
    //}
    mpmt_waveforms.clear();
    //printf("e4\n");
    
    //for(unsigned int i=0; i< trigger_hits->size(); i++){
    //delete trigger_hits->at(i);
    //trigger_hits->at(i)=0;
      
    //}
    //trigger_hits->clear();
    //printf("e5\n");
#ifndef __CLING__
    if(trigger_hits != &(mpmt_collection->triggers)) delete trigger_hits;
#endif
    trigger_hits =0;
    //printf("e6 %p\n", this);
    delete mpmt_collection;
    mpmt_collection=0;
    //printf("e7\n");
  }
  
  bool Print(){
    
    std::cout<<"/////////////////////////trigger info//////////////////size="<<triggers_info->size()<<std::endl;
    for(int i=0; i<triggers_info->size(); i++){ std::cout<<"///["<<i<<"]///"<<std::endl; triggers_info->at(i)->Print();}
    std::cout<<"/////////////////////////mpmt hits//////////////////size="<<mpmt_hits.size()<<std::endl;
    for(int i=0; i<mpmt_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; mpmt_hits.at(i)->Print();}
    std::cout<<"/////////////////////////trigger hits//////////////////size="<<trigger_hits->size()<<std::endl;
        for(int i=0; i<trigger_hits->size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; trigger_hits->at(i)->Print();}
    std::cout<<"/////////////////////////mpmt waveforms//////////////////size="<<mpmt_waveforms.size()<<std::endl;
    for(int i=0; i<mpmt_waveforms.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; mpmt_waveforms.at(i)->Print();}
    /*std::cout<<"/////////////////////////hk mpmt hits//////////////////"<<std::endl;
      for(int i=0; i<hk_mpmt_hits.size(); i++){ std::cout<<"///["<<i<<"]///"<<std::endl; hk_mpmt_hits.at(i).Print();}
    std::cout<<"/////////////////////////extra hits//////////////////"<<std::endl;
    for(int i=0; i<extra_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; extra_hits.at(i).Print();}
    std::cout<<"/////////////////////////extra waveforms//////////////////"<<std::endl;
    for(int i=0; i<extra_waveforms.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; extra_waveforms.at(i).Print();}
    std::cout<<"/////////////////////////tdc data//////////////////"<<std::endl;
    for(int i=0; i<tdc_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; tdc_hits.at(i).Print();}
    std::cout<<"/////////////////////////qdc data//////////////////"<<std::endl;
    for(int i=0; i<qdc_hits.size(); i++){std::cout<<"///["<<i<<"]///"<<std::endl; qdc_hits.at(i).Print();}   
    std::cout<<"///////////////////////////start counter ="<<start_counter<<"////////////////"<<std::endl;
   std::cout<<"///////////////////////////readout numebr ="<<readout_num<<"////////////////"<<std::endl;
   std::cout<<"///////////////////////////////////////////"<<std::endl<<std::endl;
    */
    return true;

  }
  std::string GetVersion(){return "1.0";};
#ifndef __CLING__
  bool Serialise(BinaryStream &bs){
    // std::cout<<"readout window serialise"<<std::endl;

    bs & spill_num;
    
    unsigned int size = 0;
    
    if(bs.m_write){
      //std::cout<<"in write"<<std::endl;
      size =  triggers_info->size(); 
      bs & size;

      for(unsigned int i=0; i<size; i++){
	bs & (*triggers_info->at(i));
      }

      size =  trigger_hits->size(); 
      bs & size;
      
      for(unsigned int i=0; i<size; i++){
	bs & (*trigger_hits->at(i));
      }
      
       size =  mpmt_hits.size(); 
       bs & size;

       //       printf("hits size=%u\n", size);
       for(unsigned int i=0; i<size; i++){
	 bs & (*mpmt_hits.at(i));
       }

       size =  mpmt_waveforms.size(); 
       bs & size;
       
       for(unsigned int i=0; i<size; i++){
	 bs & (*mpmt_waveforms.at(i));
       }
    }
    else{
      std::cout<<"in read "<<std::endl;
            bs & size;
      triggers_info = new std::vector<TriggerInfo*>; 
      triggers_info->resize(size); 
      std::cout<<"triggers_info size="<<size<<std::endl;
      for(unsigned int i=0; i<size; i++){
	TriggerInfo* tmp = new TriggerInfo;
	bs & (*tmp);
	triggers_info->at(i)=tmp;
      }
 std::cout<<"read triggers_info"<<std::endl;
 
      bs & size;
      trigger_hits = new std::vector<P_MPMTHit*>;
      trigger_hits->resize(size);
      std::cout<<"trigger_hits size="<<size<<std::endl;
      for(unsigned int i=0; i<size; i++){
        P_MPMTHit* tmp = new P_MPMTHit;
	bs & (*tmp);
	trigger_hits->at(i) = tmp;
      }
      
std::cout<<"read trigger_hits"<<std::endl;
 
       bs & size;
       mpmt_hits.resize(size); 
       std::cout<<"mpmt_hits size="<<size<<std::endl;       
       for(unsigned int i=0; i<size; i++){
	 P_MPMTHit* tmp = new P_MPMTHit;
	 bs & (*tmp);
	 mpmt_hits.at(i)= tmp;
       }

std::cout<<"read mpmt_hits"<<std::endl;
       
       bs & size;
       mpmt_waveforms.resize(size); 
       std::cout<<"read waveforms size="<<size<<std::endl;
       for(unsigned int i=0; i<size; i++){
	 P_MPMTWaveformHeader* tmp = new P_MPMTWaveformHeader;
	 bs & (*tmp);
	 mpmt_waveforms.at(i) =tmp;
       }

std::cout<<"read waveforms"<<std::endl;

      
    }
    return true;
  }

 void Send(zmq::socket_t* sock, int flag=0){
   if(mpmt_hits.size() !=0){
     std::string identifier="RW";
     zmq::message_t identifier_msg(identifier.length()+1);
     memcpy(identifier_msg.data(), identifier.data(), identifier.length()+1);
     sock->send(identifier_msg, ZMQ_SNDMORE);
     
     unsigned int tmp_size = mpmt_hits.size();
     zmq::message_t size(tmp_size);
     memcpy(size.data(), &tmp_size, sizeof tmp_size);
     sock->send(size, ZMQ_SNDMORE);
     
     for(unsigned int i=0; i< mpmt_hits.size()-1; i++){
       
       mpmt_hits.at(i)->Send(sock, ZMQ_SNDMORE);
       
     }
     
     mpmt_hits.at(mpmt_hits.size()-1)->Send(sock, flag);
     
   }
 }

  void Receive(zmq::socket_t* sock){

    unsigned int tmp_size=0;
    zmq::message_t ms1;
    sock->recv(&ms1);
    memcpy(&tmp_size, ms1.data(), sizeof tmp_size);
    mpmt_hits.resize(tmp_size);
    bool more=false;

    for(unsigned int i=0; i < tmp_size; i++){
      mpmt_hits.at(i) = new P_MPMTHit;
      mpmt_hits.at(i)->Receive(sock, more);
    
    }
    
  }
  
#endif
  
};


#endif
