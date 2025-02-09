#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <cassert>
#include "SerialisableObject.h"
#include "DAQInfo.h"
#include "TriggerType.h"
#include "MPMTMessages.h"
#include "ReadoutWindow.h"
#include "MPMTWaveformSamples.h"

int main(int argc, const char** argv){
	if(argc<2){
		std::cout<<"usage: "<<argv[0]<<" <file> <num_events=-1> <verbosity=1>"<<std::endl;
		return 0;
	}
	
	std::string input_file = argv[1];
	long long int num_events=-1;
	int verbosity=1;
	if(argc>2) num_events = atoi(argv[2]);
	if(argc>3) verbosity = atoi(argv[3]); // TODO implement variable verbosity
	
	std::cout<<"opening input file "<<input_file<<std::endl;
	TFile f(input_file.c_str(),"READ");
	
	std::cout<<"getting daqinfo tree"<<std::endl;
	TTree* t_daq_info = (TTree*)f.Get("daq_info");
	if(!t_daq_info){
		std::cerr<<"no daqinfo tree!"<<std::endl;
		return -1;
	} else if(t_daq_info->GetEntries()!=1){
		std::cerr<<"Error: bad num daq_info tree entries "<<t_daq_info->GetEntries()<<std::endl;
		return -2;
	}
	DAQInfo daq_info;
	DAQInfo* daq_info_p = &daq_info;
	t_daq_info->SetBranchAddress("daq_info", &daq_info_p);
	t_daq_info->GetEntry(0);
	
	std::cout<<"DAQ_info:"<<std::endl;
	daq_info.Print();
	
	std::cout<<"getting data tree"<<std::endl;
	TTree* t_data = (TTree*)f.Get("data");
	if(!t_data){
		std::cerr<<"no data tree!"<<std::endl;
		return -3;
	} else {
		std::cout<<"This file contains "<<t_data->GetEntries()<<" readout windows"<<std::endl;
	}
	
	// setup branches
	std::vector<P_MPMTHit*> mpmt_hits;
	std::vector<P_MPMTHit*> trigger_hits;
	std::vector<TriggerInfo*> trigger_infos;
	std::vector<P_MPMTWaveformHeader*> mpmt_waveforms;
	std::vector<MPMTWaveformSamples> waveform_samples;
	
	std::vector<P_MPMTHit*>* mpmt_hits_p = &mpmt_hits;
	std::vector<P_MPMTHit*>* trigger_hits_p = &trigger_hits;
	std::vector<TriggerInfo*>* trigger_infos_p = &trigger_infos;
	std::vector<P_MPMTWaveformHeader*>* mpmt_waveforms_p = &mpmt_waveforms;
	std::vector<MPMTWaveformSamples>* waveform_samples_p = &waveform_samples;
	
	t_data->SetBranchAddress("mpmt_hits",&mpmt_hits_p);
	t_data->SetBranchAddress("trigger_hits",&trigger_hits_p);
	t_data->SetBranchAddress("trigger_infos",&trigger_infos_p);
	t_data->SetBranchAddress("waveform_headers",&mpmt_waveforms_p);
	t_data->SetBranchAddress("waveform_samples",&waveform_samples_p);
	
	/*
	t_data->SetBranchStatus("*",0);
	t_data->SetBranchStatus("mpmt_hits",1);
	t_data->SetBranchStatus("trigger_hits",1);
	t_data->SetBranchStatus("trigger_infos",1);
	t_data->SetBranchStatus("waveform_samples",1);
	t_data->SetBranchStatus("waveform_headers",1);
	*/
	
	for(size_t i=0; i<std::min(num_events,t_data->GetEntries()); ++i){
		std::cout<<"Getting entry "<<i<<std::endl;
		t_data->GetEntry(i);
	
		std::cout<<mpmt_hits.size()<<" mpmt_hits"<<std::endl;
		std::cout<<trigger_hits.size()<<" trigger_hits"<<std::endl;
		std::cout<<trigger_infos.size()<<" trigger_infos"<<std::endl;
		std::cout<<mpmt_waveforms.size()<<" waveform_headers"<<std::endl;
		std::cout<<waveform_samples.size()<<" waveform_samples"<<std::endl;
		if(mpmt_waveforms.size()!=waveform_samples.size()){
			std::cerr<<"ERROR! Mismatched waveform header and sample vectors!"<<std::endl;
		}
		
		if(mpmt_hits.size()){
			std::cout<<"first mpmt_hit:"<<std::endl;
			std::cout<<"spill: "<<mpmt_hits.front()->spill_num<<std::endl;
			std::cout<<"card: "<<mpmt_hits.front()->card_id<<std::endl;
			std::cout<<"hit at "<<mpmt_hits.front()->hit<<"\nHit details:"<<std::endl;
			mpmt_hits.front()->hit->Print();
		}
		
		/*
		// getters
		mpmt_hits.front()->hit->GetHeader();
		mpmt_hits.front()->hit->GetEventType();
		mpmt_hits.front()->hit->GetChannel();
		mpmt_hits.front()->hit->GetCoarseCounter();
		mpmt_hits.front()->hit->GetFineTime();
		mpmt_hits.front()->hit->GetCharge();
		mpmt_hits.front()->hit->GetQualityFactor();
		mpmt_hits.front()->hit->GetFlags();
		*/
		
		if(trigger_hits.size()){
			std::cout<<"first trigger_hit:"<<std::endl;
			std::cout<<"spill: "<<trigger_hits.front()->spill_num<<std::endl;
			std::cout<<"card: "<<trigger_hits.front()->card_id<<std::endl;
			std::cout<<"hit at "<<trigger_hits.front()->hit<<"\nHit details:"<<std::endl;
			trigger_hits.front()->hit->Print();
		}
		
		/*                                                                                                                     
		trigger_hits is of same type as mpmt_hits, so getters for trigger_hits.front()->hit
		are same as for mpmt_hits.front()->hit above.
		*/
		
		if(trigger_infos.size()){
			std::cout<<"first trigger_info"<<std::endl;
			std::cout<<"spill: "<<trigger_infos.front()->spill_num<<std::endl;
			std::cout<<"card: "<<trigger_infos.front()->card_id<<std::endl;
			std::cout<<"time: "<<trigger_infos.front()->time<<std::endl;
			std::cout<<"type: "<<static_cast<int>(trigger_infos.front()->type)<<std::endl; // FIXME use underlying_type
			std::cout<<"vme: "<<trigger_infos.front()->vme_event_num<<std::endl;
			std::cout<<"#leds: "<<trigger_infos.front()->mpmt_LEDs.size()<<std::endl;
			
			if(trigger_infos.front()->mpmt_LEDs.size()){
				std::cout<<"first mpmt_led at "<<trigger_infos.front()->mpmt_LEDs.front()<<"\nmpmt_led details:"<<std::endl;
				trigger_infos.front()->mpmt_LEDs.front()->Print();
				
				std::cout<<"first trigger info's first mpmt_led"<<std::endl;
				std::cout<<"spill: "<<trigger_infos.front()->mpmt_LEDs.front()->spill_num<<std::endl;
				std::cout<<"card: "<<trigger_infos.front()->mpmt_LEDs.front()->card_id<<std::endl;
				std::cout<<"led at: "<<trigger_infos.front()->mpmt_LEDs.front()->led<<"\nled details:"<<std::endl;
				trigger_infos.front()->mpmt_LEDs.front()->led->Print();
			}
		}
		
		/*
		// getters
		trigger_infos.front()->mpmt_LEDs.front()->led->GetHeader();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetEventType();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetLED();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetGain();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetDACSetting();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetType();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetSequenceNumber();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetCoarseCounter();
		trigger_infos.front()->mpmt_LEDs.front()->led->GetReserved();
		*/
		
		if(mpmt_waveforms.size()){
			std::cout<<"first mpmt_wavefroms' header at "<<mpmt_waveforms.front()->waveform_header<<"\nheader details: "<<std::endl;
			mpmt_waveforms.front()->waveform_header->Print();
		}
		
		/*
		// getters
		mpmt_waveforms.front()->waveform_header->GetHeader();
		mpmt_waveforms.front()->waveform_header->GetFlags();
		mpmt_waveforms.front()->waveform_header->GetCoarseCounter();
		mpmt_waveforms.front()->waveform_header->GetChannel();
		mpmt_waveforms.front()->waveform_header->GetNumSamples();
		mpmt_waveforms.front()->waveform_header->GetLength();
		mpmt_waveforms.front()->waveform_header->GetReserved();
		*/
		
		if(waveform_samples.size()){
			std::cout<<"first waveform had "<<waveform_samples.front().nbytes
			         <<" sample bytes at "<<&waveform_samples.front().bytes<<"\nPrint: "<<std::endl;
			waveform_samples.front().Print();
		}
		
	}
	
	std::cout<<"closing file"<<std::endl;
	f.Close();
	
	// TODO cleanup
	std::cout<<"done"<<std::endl;
	
	return 0;
	
}
