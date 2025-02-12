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

int main(){
	std::cout<<"opening input file"<<std::endl;
	TFile f("testfile.root","READ");
	
	std::cout<<"getting daqinfo tree"<<std::endl;
	TTree* t_daq_info = (TTree*)f.Get("daq_info");
	if(!t_daq_info){
		std::cerr<<"no daqinfo tree!"<<std::endl;
		return -1;
	}
	assert(t_daq_info->GetEntries()==1 && "bad # daqinfo tree entries");
	
	DAQInfo daq_info;
	DAQInfo* daq_info_p = &daq_info;
	t_daq_info->SetBranchAddress("daq_info", &daq_info_p);
	t_daq_info->GetEntry(0);
	assert(daq_info.run_number == 444 && "Bad run num!");
	assert(daq_info.sub_run_number == 44 && "Bad subrun num!");
	
	std::cout<<"getting data tree"<<std::endl;
	TTree* t_data = (TTree*)f.Get("data");
	if(!t_data){
		std::cerr<<"no data tree!"<<std::endl;
		return -2;
	}
	assert(t_data->GetEntries()==1 && "bad # data tree entries");
	
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
	
	t_data->GetEntry(0);
	
	std::cout<<"checking sizes"<<std::endl;
	std::cout<<mpmt_hits.size()<<" mpmt_hits"<<std::endl;
	std::cout<<trigger_hits.size()<<" trigger_hits"<<std::endl;
	std::cout<<trigger_infos.size()<<" trigger_infos"<<std::endl;
	std::cout<<mpmt_waveforms.size()<<" waveform_headers"<<std::endl;
	std::cout<<waveform_samples.size()<<" waveform_samples"<<std::endl;
	
	assert(mpmt_hits.size()==1 && "no mpmt_hits!");
	assert(trigger_hits.size()==1 && "no trigger_hits!");
	assert(trigger_infos.size()==1 && "no trigger_infos!");
	assert(mpmt_waveforms.size()==1 && "no waveform headers!");
	assert(waveform_samples.size()==1 && "no waveform samples!");
	
	std::cout<<"getting first elements"<<std::endl;
	std::cout<<"first mpmt_hits at "<<mpmt_hits.front()<<std::endl;
	std::cout<<"first trigger_hits at "<<trigger_hits.front()<<std::endl;
	std::cout<<"first trigger_infos at "<<trigger_infos.front()<<std::endl;
	std::cout<<"first mpmt_waveforms at "<<mpmt_waveforms.front()<<std::endl;
	// no such check for waveform samples as its not a pointer
	
	std::cout<<"checking first mpmt_hit"<<std::endl;
	assert(mpmt_hits.front()->spill_num == 777 && "bad mpmt_hits spillnum");
	assert(mpmt_hits.front()->card_id == 100 && "bad mpmt_hits cardid");
	
	std::cout<<"first mpmt_hits' hit at "<<mpmt_hits.front()->hit<<"\nPrint"<<std::endl;
	mpmt_hits.front()->hit->Print();
	
	std::cout<<"check vals"<<std::endl;
	assert(mpmt_hits.front()->hit->GetHeader()==1 && "bad header");
	assert(mpmt_hits.front()->hit->GetEventType()==0 && "bad event type");
	assert(mpmt_hits.front()->hit->GetChannel()==1 && "bad channel");
	assert(mpmt_hits.front()->hit->GetCoarseCounter()==12345 && "bad counter");
	assert(mpmt_hits.front()->hit->GetFineTime()==678 && "bad fine time");
	assert(mpmt_hits.front()->hit->GetCharge()==100 && "bad charge");
	assert(mpmt_hits.front()->hit->GetQualityFactor()==0 && "bad quality");
	assert(mpmt_hits.front()->hit->GetFlags()==0 && "bad flags");
	
	std::cout<<"checking first trigger_hits"<<std::endl;
	assert(trigger_hits.front()->spill_num == 777 && "bad spill");
	assert(trigger_hits.front()->card_id == 100 && "bad card");
	
	std::cout<<"first trigger_hits' hit at "<<trigger_hits.front()->hit<<"\nPrint"<<std::endl;
	trigger_hits.front()->hit->Print();
	
	std::cout<<"check vals"<<std::endl;
	assert(trigger_hits.front()->hit->GetHeader()==1 && "bad header");
	assert(trigger_hits.front()->hit->GetEventType()==0 && "bad event type");
	assert(trigger_hits.front()->hit->GetChannel()==1 && "bad channel");
	assert(trigger_hits.front()->hit->GetCoarseCounter()==12345 && "bad counter");
	assert(trigger_hits.front()->hit->GetFineTime()==678 && "bad fine time");
	assert(trigger_hits.front()->hit->GetCharge()==100 && "bad fine time");
	assert(trigger_hits.front()->hit->GetQualityFactor()==0 && "bad quality");
	assert(trigger_hits.front()->hit->GetFlags()==0 && "bad flags");
	
	std::cout<<"checking first trigger_infos"<<std::endl;
	assert(trigger_infos.front()->spill_num == 777 && "bad spill");
	assert(trigger_infos.front()->card_id == 100 && "bad card");
	assert(trigger_infos.front()->time == 12345 && "bad time");
	assert(trigger_infos.front()->type == TriggerType::MAIN && "bad type");
	assert(trigger_infos.front()->vme_event_num == 888 && "bad vme event");
	assert(trigger_infos.front()->mpmt_LEDs.size()==1 && "bad mpmt_LEDs size!");
	
	std::cout<<"first trigger infos' first mpmt_LED at "<<trigger_infos.front()->mpmt_LEDs.front()<<"\nPrint:"<<std::endl;
	trigger_infos.front()->mpmt_LEDs.front()->Print();
	
	std::cout<<"checking first trigger info's first mpmt_led"<<std::endl;
	assert(trigger_infos.front()->mpmt_LEDs.front()->spill_num == 777 && "bad spill");
	assert(trigger_infos.front()->mpmt_LEDs.front()->card_id == 100 && "bad card");
	
	std::cout<<"first trigger_info's first mpmt_LEDs' led at "<<trigger_infos.front()->mpmt_LEDs.front()->led<<"\nPrint:"<<std::endl;
	trigger_infos.front()->mpmt_LEDs.front()->led->Print();
	
	std::cout<<"checking first trigger_info's firstmpmt_LEDs' led"<<std::endl;
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetHeader()==1 && "bad header");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetEventType()==2 && "bad event type");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetLED()==4 && "bad led");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetGain()==1 && "bad gain");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetDACSetting()==66 && "bad dac");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetType()==2 && "bad type");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetSequenceNumber()==3 && "bad sequence");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetCoarseCounter()==12345 && "bad counter");
	assert(trigger_infos.front()->mpmt_LEDs.front()->led->GetReserved()==0 && "bad reserved");
	
	std::cout<<"first mpmt_wavefroms' waveform header at "<<mpmt_waveforms.front()->waveform_header<<"\nPrint: "<<std::endl;
	mpmt_waveforms.front()->waveform_header->Print();
	std::cout<<"checking vals"<<std::endl;
	assert(mpmt_waveforms.front()->waveform_header->GetHeader()==2 && "bad header");
	assert(mpmt_waveforms.front()->waveform_header->GetFlags()==0 && "bad flags");
	assert(mpmt_waveforms.front()->waveform_header->GetCoarseCounter()==12345 && "bad counter");
	assert(mpmt_waveforms.front()->waveform_header->GetChannel()==1 && "bad channel");
	assert(mpmt_waveforms.front()->waveform_header->GetNumSamples()==32 && "bad nsamples");
	assert(mpmt_waveforms.front()->waveform_header->GetLength()==48 && "bad length");
	assert(mpmt_waveforms.front()->waveform_header->GetReserved()==0 && "bad reserved");
	
	std::cout<<"first waveforms had "<<waveform_samples.front().nbytes<<" bytes at "
	         <<&waveform_samples.front().bytes<<"\nPrint: "<<std::endl;
	waveform_samples.front().Print(true);
	std::cout<<"checking vals"<<std::endl;
	assert(waveform_samples.front().nbytes==48 && "bad nsamples");
	for(size_t i=0; i<waveform_samples.front().nbytes; ++i){
		assert(waveform_samples.front().bytes[i]==i && "bad sample val");
	}
	
	std::cout<<"closing file"<<std::endl;
	f.Close();
	
	// TODO cleanup
	std::cout<<"done"<<std::endl;
	
	return 0;
	
}
