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
	// TODO add new branches, readout num, window num, tdc info, qdc info, hk_mpmt hits....
	
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
	
	for(size_t i=0; i<t_data->GetEntries(); ++i){
		std::cout<<"Getting entry "<<i<<std::endl;
		t_data->GetEntry(i);
	
		std::cout<<"\t"<<mpmt_hits.size()<<" mpmt_hits"<<std::endl;
		std::cout<<"\t"<<trigger_hits.size()<<" trigger_hits"<<std::endl;
		std::cout<<"\t"<<trigger_infos.size()<<" trigger_infos"<<std::endl;
		std::cout<<"\t"<<mpmt_waveforms.size()<<" waveform_headers"<<std::endl;
		std::cout<<"\t"<<waveform_samples.size()<<" waveform_samples"<<std::endl;
		if(mpmt_waveforms.size()!=waveform_samples.size()){
			std::cerr<<"ERROR! Mismatched waveform header and sample vectors!"<<std::endl;
		}
		
		std::cout<<"\thits"<<std::endl;
		for(size_t k=0; k<std::min(size_t(3),mpmt_hits.size()); ++k){
			std::cout<<"\t\thit: "<<k<<std::endl;
			std::cout<<"\t\tcard: "<<short(mpmt_hits.at(k)->card_id)<<", channel: "<<mpmt_hits.at(k)->hit->GetChannel()<<std::endl;
			std::cout<<"\t\tcc: "<<mpmt_hits.at(k)->hit->GetCoarseCounter()<<", fc: "<<mpmt_hits.at(k)->hit->GetFineTime()<<std::endl;
			//std::cout<<"\t\thit at "<<mpmt_hits.front()->hit<<std::endl;
			//std::cout<<"\t\tHit details:"<<std::endl;
			//mpmt_hits.front()->hit->Print();
			std::cout<<"\t\t----"<<std::endl;
		}
		//continue;
		
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
		
		for(size_t k=0; k<std::min(size_t(3),trigger_hits.size()); ++k){
			std::cout<<"\ttrigger_hit "<<k<<":"<<std::endl;
			std::cout<<"\t\tcard: "<<short(trigger_hits.at(k)->card_id)<<", channel: "<<trigger_hits.at(k)->hit->GetChannel()<<std::endl;
			//std::cout<<"\t\thit at "<<trigger_hits.at(k)->hit<<std::endl;
			//std::cout<<"\t\tHit details:"<<std::endl;
			//trigger_hits.at(k)->hit->Print();
			std::cout<<"\t\t----"<<std::endl;
		}
		
		/*
		trigger_hits is of same type as mpmt_hits, so getters for trigger_hits.front()->hit
		are same as for mpmt_hits.front()->hit above.
		*/
		
		std::cout<<"\ttriggers"<<std::endl;
		for(size_t k=0; k<std::min(size_t(3),trigger_infos.size()); ++k){
			std::cout<<"\t\ttrigger_info: "<<k<<std::endl;
			std::cout<<"\t\ttime: "<< trigger_infos.at(k)->time;
			if(trigger_infos.at(k)->mpmt_LEDs.size()){
				std::cout<<", first mpmt_led at "<<trigger_infos.at(k)->mpmt_LEDs.front()->led->GetCoarseCounter();
			}
			std::cout<<std::endl;
			if(k==0){
				std::cout<<"\t\tspill: "<<trigger_infos.at(k)->spill_num<<std::endl;
				std::cout<<"\t\tcard: "<<short(trigger_infos.at(k)->card_id)<<std::endl;
				std::cout<<"\t\ttype: "<<static_cast<int>(trigger_infos.at(k)->type)<<std::endl;
				std::cout<<"\t\tvme: "<<trigger_infos.at(k)->vme_event_num<<std::endl;
				std::cout<<"\t\tnum leds: "<<trigger_infos.at(k)->mpmt_LEDs.size()<<std::endl;
				
				if(trigger_infos.at(k)->mpmt_LEDs.size()){
					//std::cout<<"\t\tfirst mpmt_led at "<<trigger_infos.front()->mpmt_LEDs.front()<<std::endl;
					//std::cout<<"\t\tmpmt_led details:"<<std::endl;
					//trigger_infos.front()->mpmt_LEDs.front()->Print();
					
					// explicitly
					std::cout<<"\t\tfirst mpmt_led:"<<std::endl;
					std::cout<<"\t\t\tcard: "<<short(trigger_infos.at(k)->mpmt_LEDs.front()->card_id)<<std::endl;
					std::cout<<"\t\t\tcc: "<<trigger_infos.at(k)->mpmt_LEDs.front()->led->GetCoarseCounter()<<std::endl;
					
					//std::cout<<"\t\t\tled at: "<<trigger_infos.front()->mpmt_LEDs.front()->led<<std::endl;
					//std::cout<<"\t\t\tled details:"<<std::endl;
					//trigger_infos.front()->mpmt_LEDs.front()->led->Print();
					
				}
			}
			std::cout<<"\t\t-----"<<std::endl;
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
			std::cout<<"\tfirst mpmt_wavefroms' header at "<<mpmt_waveforms.front()->waveform_header<<std::endl;
			std::cout<<"\twaveform header details:"<<std::endl;
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
			std::cout<<"\tfirst waveform had "<<waveform_samples.front().nbytes
			         <<" sample bytes at "<<&waveform_samples.front().bytes<<std::endl;
			std::cout<<"\twaveform samples details: "<<std::endl;
			waveform_samples.front().Print(true);
		}
		
	}
	
	std::cout<<"closing file"<<std::endl;
	f.Close();
	
	// TODO cleanup
	std::cout<<"done"<<std::endl;
	
	return 0;
	
}
