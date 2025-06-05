#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include <vector>
#include <cassert>
#include <chrono>
#include <thread>
#include <signal.h>
#include "SerialisableObject.h"
#include "DAQInfo.h"
#include "TriggerType.h"
#include "MPMTMessages.h"
#include "ReadoutWindow.h"
#include "MPMTWaveformSamples.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"

const std::vector<std::string> trigger_types_vec{ "LASER", "NHITS", "LED", "MAIN", "MBEAM", "EBEAM", "NONE", "HARD6" };


/*
* This function is registered as the signal handler. It gets called when the program receives the signal, and sets 'gotStopSignal' member to true.
* @param[in] _ignored Not used here (normally the signal type) but required to match the signal handler function prototype.
*/
bool gotStopSignal = false;
void stopSignalHandler(int _ignored){
        // technically we could choose what to do based on the signal type passed, if we registered this function with multliple signals.
        gotStopSignal = true;
}


int main(int argc, const char** argv){
	
	if(argc<2){
		std::cout<<"usage: "<<argv[0]<<" <num_events=-1> <file1> <file2> ... "<<std::endl;
		return 0;
	}
	
	if(signal((int) SIGINT, stopSignalHandler) == SIG_ERR){
		std::cerr<<"Failed to setup signal handler!"<<std::endl;
		return -1;
	}
	
	int verbosity=1;   // unused, fixme
	int print_hits=1;
	int print_trigger_hits=60;
	int print_triggers=10;
	int print_waveforms=0;
	std::vector<int> prints{print_hits, print_trigger_hits, print_triggers, print_waveforms};
	int print_events = *std::max_element(prints.begin(),prints.end());
	
	long int num_events=-1;
	int first_file = 2;
	try{
		num_events = std::stoi(argv[1]);
	} catch(std::exception& e){
		num_events = -1;
		first_file = 1;
	}
	
	TFile f(argv[first_file],"READ");
	
	std::cout<<"getting first file daqinfo tree"<<std::endl;
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
	
	std::cout<<"getting first file pps tree"<<std::endl;
	TTree* t_pps = (TTree*)f.Get("pps");
	unsigned long n_pps = t_pps->GetEntries();
	std::cout<<"first file PPS tree has "<<n_pps<<" entries"<<std::endl;
	//std::vector<WCTEMPMTPPS> pps;
	//std::vector<WCTEMPMTPPS>* pps_p;  // TODO add pps validation
	
	// remainder of analysis will be on tchain to cover multiple files
	f.Close();
	
	TChain* t_data = new TChain("data", "data");
	for(int i=first_file; i<argc; ++i){
		std::cout<<"adding input file "<<argv[i]<<std::endl;
		t_data->Add(argv[i]);
	}
	
	std::cout<<"getting data tree"<<std::endl;
	//TTree* t_data = (TTree*)f.Get("data");
	if(!t_data){
		std::cerr<<"no data tree!"<<std::endl;
		return -3;
	} else {
		std::cout<<"This file contains "<<t_data->GetEntries()<<" readout windows"<<std::endl;
	}
	unsigned long entries_to_read = (num_events>0) ? std::min(num_events,(long int)t_data->GetEntries()) : t_data->GetEntries();
	
	// setup branches
	std::vector<P_MPMTHit*> mpmt_hits;
	std::vector<P_MPMTHit*> trigger_hits;
	std::vector<TriggerInfo*> trigger_infos;
	std::vector<P_MPMTWaveformHeader*> mpmt_waveforms;
	std::vector<MPMTWaveformSamples> waveform_samples;
	std::vector<HKMPMTHit*> hk_mpmt_hits;
	std::vector<TDCHit*> tdc_hits;
	std::vector<QDCHit*> qdc_hits;
	unsigned long readout_num;
	unsigned long spill_num;
	unsigned long start_counter;
	
	// TODO add new branches, readout num, window num, tdc info, qdc info, hk_mpmt hits....
	
	std::vector<P_MPMTHit*>* mpmt_hits_p = &mpmt_hits;
	std::vector<P_MPMTHit*>* trigger_hits_p = &trigger_hits;
	std::vector<TriggerInfo*>* trigger_infos_p = &trigger_infos;
	std::vector<P_MPMTWaveformHeader*>* mpmt_waveforms_p = &mpmt_waveforms;
	std::vector<MPMTWaveformSamples>* waveform_samples_p = &waveform_samples;
	std::vector<HKMPMTHit*>* hk_mpmt_hits_p = &hk_mpmt_hits;
	std::vector<TDCHit*>* tdc_hits_p = &tdc_hits;
	std::vector<QDCHit*>* qdc_hits_p = &qdc_hits;
	
	t_data->SetBranchAddress("mpmt_hits",&mpmt_hits_p);
	t_data->SetBranchAddress("trigger_hits",&trigger_hits_p);
	t_data->SetBranchAddress("trigger_infos",&trigger_infos_p);
	t_data->SetBranchAddress("waveform_headers",&mpmt_waveforms_p);
	t_data->SetBranchAddress("waveform_samples",&waveform_samples_p);
	t_data->SetBranchAddress("hk_mpmt_hits",&hk_mpmt_hits_p);
	t_data->SetBranchAddress("tdc_hits",&tdc_hits_p);
	t_data->SetBranchAddress("qdc_hits",&qdc_hits_p);
	t_data->SetBranchAddress("readout_num",&readout_num);
	t_data->SetBranchAddress("spill_num",&spill_num);
	t_data->SetBranchAddress("start_counter",&start_counter);
	
	TApplication tapp("app_name", &argc, (char**)argv);
	TFile fout("plots.root","RECREATE");
	TH1D num_hits("num_hits","num_hits",200,0,0);
	TH2D num_hits_by_card_vs_readout("num_hits_by_card_vs_readout","num_hits_by_card_vs_readout",entries_to_read,0,entries_to_read-1,130,0,129);
	TH1D num_trigs("num_trigs","num_trigs",50,0,0);
	TH1D num_trighits("num_trighits","num_trighits",200,0,0);
	TH1D hit_times("hit_times","hit_times",400,0,0);
	TH1D hit_times_wrt_startcounts("hit_times_wrt_startcounts","hit_times_wrt_startcounts",1000,0,0);
	TH1D hit_charges("hit_charges","hit_charges",1000,0,0);
	TH1D trighit_times_wrt_startcounts("trighit_times_wrt_startcounts","trighit_times_wrt_startcounts",1000,0,0);
	TH1D firstled_times_wrt_startcounts("firstled_times_wrt_startcounts","firstled_times_wrt_startcounts",400,0,0);
	TH1D firstled_times_wrt_trig("firstled_times_wrt_trig","firstled_times_wrt_trig",400,0,0);
	TH1I trigger_times("trigger_times","trigger_times",400,0,0);
	TH1I trigger_times_wrt_startcounts("trigger_times_wrt_startcounts","trigger_times_wrt_startcounts",1000,0,0);
	TH1I trigger_types("triggertypes","triggertypes", trigger_types_vec.size(),0,trigger_types_vec.size());
	THStack trig_tdiffs("trig_tdiffs","trig_tdiffs");
	THStack trig_times_by_type("trig_times_by_type","trig_times_by_type");
	std::vector<unsigned long> last_trig_ts(trigger_types_vec.size(),0);
	for(int i=0; i<trigger_types_vec.size(); ++i){
		trigger_types.GetXaxis()->SetBinLabel(i+1,trigger_types_vec.at(i).c_str());
		std::string title = "trig_tdiffs_"+trigger_types_vec.at(i);
		trig_tdiffs.Add(new TH1D(title.c_str(), title.c_str(),100,0,0));
		
		title = "trig_times_by_type_"+trigger_types_vec.at(i);
		trig_times_by_type.Add(new TH1D(title.c_str(), title.c_str(),100,0,0));
	}
	TGraph spill_num_vs_entry(t_data->GetEntries());
	spill_num_vs_entry.SetTitle("spill_num_vs_entry"); spill_num_vs_entry.SetName("spill_num_vs_entry");
	TGraph readout_num_vs_entry(t_data->GetEntries());
	readout_num_vs_entry.SetTitle("readout_num_vs_entry"); readout_num_vs_entry.SetName("readout_num_vs_entry");
	TGraph start_counter_vs_entry(t_data->GetEntries());
	start_counter_vs_entry.SetTitle("start_counter_vs_entry"); start_counter_vs_entry.SetName("start_counter_vs_entry");
	TGraph spill_num_vs_readout_num(t_data->GetEntries());
	spill_num_vs_readout_num.SetTitle("spill_num_vs_readout_num"); spill_num_vs_readout_num.SetName("spill_num_vs_readout_num");
	TGraph spill_num_vs_start_counter(t_data->GetEntries());
	spill_num_vs_start_counter.SetTitle("spill_num_vs_start_counter"); spill_num_vs_start_counter.SetName("spill_num_vs_start_counter");
	TGraph readout_num_vs_start_counter(t_data->GetEntries());
	readout_num_vs_start_counter.SetTitle("readout_num_vs_start_counter"); readout_num_vs_start_counter.SetName("readout_num_vs_start_counter");
	TGraph nhits_vs_entry(t_data->GetEntries());
	nhits_vs_entry.SetTitle("nhits_vs_entry"); nhits_vs_entry.SetName("nhits_vs_entry");
	TGraph ntrighits_vs_entry(t_data->GetEntries());
	ntrighits_vs_entry.SetTitle("ntrighits_vs_entry"); ntrighits_vs_entry.SetName("ntrighits_vs_entry");
	TGraph ntrigs_vs_entry(t_data->GetEntries());
	ntrigs_vs_entry.SetTitle("ntrigs_vs_entry"); ntrigs_vs_entry.SetName("ntrigs_vs_entry");
	
	TCanvas c1("c1","c1",800,600);
	c1.Divide(3,2);
	c1.cd(0);
	num_hits.Draw();
	c1.cd(1);
	num_trighits.Draw();
	c1.cd(2);
	num_trigs.Draw();
	c1.cd(3);
	hit_times_wrt_startcounts.Draw();
	c1.cd(4);
	trighit_times_wrt_startcounts.Draw();
	c1.cd(5);
	firstled_times_wrt_startcounts.Draw();
	//c1. cd(1,3);
	//firstled_times_wrt_trig.Draw();
	
	for(size_t i=0; i<entries_to_read; ++i){
		if(gotStopSignal) break;
		/*if(i<print_events)*/ std::cout<<"Getting entry "<<i<<std::endl;
		t_data->GetEntry(i);
		
		unsigned long start_counter_32 = (start_counter & ((1UL << 32)-1));
		
		//if(mpmt_hits.size()<500) continue;
		
		spill_num_vs_entry.SetPoint(i, i, spill_num);
		readout_num_vs_entry.SetPoint(i, i, readout_num);
		start_counter_vs_entry.SetPoint(i, i, start_counter);
		spill_num_vs_readout_num.SetPoint(i, spill_num, readout_num);
		spill_num_vs_start_counter.SetPoint(i, spill_num, start_counter);
		if(readout_num!=0) readout_num_vs_start_counter.SetPoint(i, readout_num, start_counter);
		nhits_vs_entry.SetPoint(i, i, mpmt_hits.size());
		ntrighits_vs_entry.SetPoint(i, i, trigger_hits.size());
		ntrigs_vs_entry.SetPoint(i, i, trigger_infos.size());
		
		std::cout<<"\t"<<mpmt_hits.size()<<" mpmt_hits"<<std::endl;
		std::cout<<"\t"<<hk_mpmt_hits.size()<<" HK mpmt hits"<<std::endl;
		std::cout<<"\t"<<tdc_hits.size()<<" TDC hits"<<std::endl;
		std::cout<<"\t"<<qdc_hits.size()<<" QDC hits"<<std::endl;
		std::cout<<"\t"<<trigger_hits.size()<<" trigger_hits"<<std::endl;
		std::cout<<"\t"<<trigger_infos.size()<<" trigger_infos"<<std::endl;
		std::cout<<"\t"<<mpmt_waveforms.size()<<" waveform_headers"<<std::endl;
		std::cout<<"\t"<<waveform_samples.size()<<" waveform_samples"<<std::endl;
		if(mpmt_waveforms.size()!=waveform_samples.size()){
			std::cerr<<"ERROR! Mismatched waveform header and sample vectors!"<<std::endl;
		}
		
		num_hits.Fill(mpmt_hits.size());
		num_trigs.Fill(trigger_infos.size());
		num_trighits.Fill(trigger_hits.size());
		
		// seems we can't reset binning to auto so need to remake it
		TH1D hit_times_wrt_startcounts_oneev("hit_times_wrt_startcounts_oneev","hit_times_wrt_startcounts_oneev",1000,0,0);
		
		std::cout<<"\thits"<<std::endl;
		//for(size_t k=0; k<std::min(size_t(3),mpmt_hits.size()); ++k){
		for(size_t k=0; k<mpmt_hits.size(); ++k){
			if(k<print_hits){
				std::cout<<"\t\thit: "<<k<<std::endl;
				std::cout<<"\t\tcard: "<<short(mpmt_hits.at(k)->card_id)<<", channel: "<<mpmt_hits.at(k)->hit->GetChannel()<<std::endl;
				std::cout<<"\t\tcc: "<<mpmt_hits.at(k)->hit->GetCoarseCounter()<<", fc: "<<mpmt_hits.at(k)->hit->GetFineTime()<<std::endl;
				std::cout<<"\t\tq: "<<(short)mpmt_hits.at(k)->hit->GetCharge()<<std::endl;
				//std::cout<<"\t\thit at "<<mpmt_hits.front()->hit<<std::endl;
				//std::cout<<"\t\tHit details:"<<std::endl;
				//mpmt_hits.front()->hit->Print();
				std::cout<<"\t\t----"<<std::endl;
			}
			hit_times.Fill(mpmt_hits.at(k)->hit->GetCoarseCounter());
			hit_charges.Fill((short)mpmt_hits.at(k)->hit->GetCharge());
			hit_times_wrt_startcounts.Fill(mpmt_hits.at(k)->hit->GetCoarseCounter() - start_counter_32); //trigger_infos.at(0)->time);
			hit_times_wrt_startcounts_oneev.Fill(mpmt_hits.at(k)->hit->GetCoarseCounter() - start_counter_32);
			num_hits_by_card_vs_readout.Fill(i,short(mpmt_hits.at(k)->card_id));
		}
		if(mpmt_hits.size()>2000){
			std::vector<double> hit_times_binedges(hit_times_wrt_startcounts_oneev.GetNbinsX());
			for(int k=0; k<hit_times_binedges.size(); ++k){
				hit_times_binedges.at(k) = hit_times_wrt_startcounts_oneev.GetXaxis()->GetBinLowEdge(k);
			}
			TGraph g_hittimes_thisev(hit_times_binedges.size(), hit_times_binedges.data(), hit_times_wrt_startcounts_oneev.fArray);
			std::string nexttitle="Event "+std::to_string(i);
			g_hittimes_thisev.SetName(nexttitle.c_str());
			g_hittimes_thisev.SetTitle(nexttitle.c_str());
			g_hittimes_thisev.Write();
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
		
		//for(size_t k=0; k<std::min(size_t(3),trigger_hits.size()); ++k){
		for(size_t k=0; k<trigger_hits.size(); ++k){
			if(k<print_trigger_hits){
				std::cout<<"\ttrigger_hit "<<k<<":"<<std::endl;
				std::cout<<"\t\tcard: "<<short(trigger_hits.at(k)->card_id)<<", channel: "<<trigger_hits.at(k)->hit->GetChannel()<<std::endl;
				std::cout<<"\t\thit time: "<<trigger_hits.at(k)->hit->GetCoarseCounter()<<std::endl;
				//std::cout<<"\t\thit at "<<trigger_hits.at(k)->hit<<std::endl;
				//std::cout<<"\t\tHit details:"<<std::endl;
				//trigger_hits.at(k)->hit->Print();
				std::cout<<"\t\t----"<<std::endl;
			}
			if(trigger_infos.size()) trighit_times_wrt_startcounts.Fill(trigger_hits.at(k)->hit->GetCoarseCounter() - start_counter_32); //trigger_infos.at(0)->time);
		}
		
		/*
		trigger_hits is of same type as mpmt_hits, so getters for trigger_hits.front()->hit
		are same as for mpmt_hits.front()->hit above.
		*/
		
		std::cout<<"\ttriggers"<<std::endl;
		for(size_t k=0; k<trigger_infos.size(); ++k){
			trigger_types.Fill((int)trigger_infos.at(k)->type);
			trigger_times.Fill(trigger_infos.at(k)->time);
			((TH1D*)(trig_times_by_type.GetHists()->At((int)trigger_infos.at(k)->type)))->Fill(trigger_infos.at(k)->time);
			unsigned long trigger_info_32 = trigger_infos.at(k)->time & (( 1UL << 32) -1);
			if(last_trig_ts.at((int)trigger_infos.at(k)->type) != 0){
				((TH1D*)(trig_tdiffs.GetHists()->At((int)trigger_infos.at(k)->type)))->Fill(trigger_infos.at(k)->time - last_trig_ts.at((int)trigger_infos.at(k)->type));
			}
			last_trig_ts.at((int)trigger_infos.at(k)->type) = trigger_infos.at(k)->time;
			trigger_times_wrt_startcounts.Fill(trigger_infos.at(k)->time - start_counter);
			if(k<print_triggers){
				std::cout<<"\t\ttrigger_info: "<<k<<std::endl;
				std::cout<<"\t\ttime: "<< trigger_infos.at(k)->time;
			}
			if(trigger_infos.at(k)->mpmt_LEDs.size()){
				if(k<print_triggers) std::cout<<", first mpmt_led at "<<trigger_infos.at(k)->mpmt_LEDs.front()->led->GetCoarseCounter();
				firstled_times_wrt_startcounts.Fill(trigger_infos.at(k)->mpmt_LEDs.front()->led->GetCoarseCounter() - start_counter_32); //trigger_infos.at(0)->time);
				firstled_times_wrt_trig.Fill(trigger_infos.at(k)->mpmt_LEDs.front()->led->GetCoarseCounter() - trigger_info_32);
			}
			if(k<print_triggers){
				std::cout<<std::endl;
				std::cout<<"\t\tspill: "<<trigger_infos.at(k)->spill_num<<std::endl;
				std::cout<<"\t\tcard: "<<short(trigger_infos.at(k)->card_id)<<std::endl;
				std::cout<<"\t\ttype: "<<trigger_types_vec.at(static_cast<int>(trigger_infos.at(k)->type))<<std::endl;
				std::cout<<"\t\tvme: "<<trigger_infos.at(k)->vme_event_num<<std::endl;
				std::cout<<"\t\tnum leds: "<<trigger_infos.at(k)->mpmt_LEDs.size()<<std::endl;
			}
			
			if(trigger_infos.at(k)->mpmt_LEDs.size()){
				if(k<print_triggers){
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
					std::cout<<"\t\t-----"<<std::endl;
				}
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
		
		for(int k=0; k<waveform_samples.size(); ++k){
			if(k<print_waveforms){
				std::cout<<"\tfirst mpmt_wavefroms' header at "<<mpmt_waveforms.at(k)->waveform_header<<std::endl;
				std::cout<<"\twaveform header details:"<<std::endl;
				mpmt_waveforms.at(k)->waveform_header->Print();
			}
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
		
		for(int k=0; k<waveform_samples.size(); ++k){
			if(k<print_waveforms){
				std::cout<<"\tfirst waveform had "<<waveform_samples.at(k).nbytes
				         <<" sample bytes at "<<&waveform_samples.at(k).bytes<<std::endl;
				std::cout<<"\twaveform samples details: "<<std::endl;
				waveform_samples.at(k).Print(true);
			}
		}
		
	}
	
	/*
	c1.Draw();
	while(gROOT->FindObject("c1")){
		c1.Modified();
		c1.Update();
		gSystem->ProcessEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	*/
	
	fout.Write();
	spill_num_vs_entry.Write();
	readout_num_vs_entry.Write();
	start_counter_vs_entry.Write();
	spill_num_vs_readout_num.Write();
	spill_num_vs_start_counter.Write();
	readout_num_vs_start_counter.Write();
	nhits_vs_entry.Write();
	ntrighits_vs_entry.Write();
	ntrigs_vs_entry.Write();
	fout.Close();
	
	// TODO cleanup
	std::cout<<"done"<<std::endl;
	
	return 0;
	
}
