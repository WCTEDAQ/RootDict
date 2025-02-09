// 
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include "SerialisableObject.h"
#include "DAQInfo.h"
#include "TriggerType.h"
#include "MPMTMessages.h"
#include "ReadoutWindow.h"
#include "MPMTWaveformSamples.h"

int main(){
	std::cout<<"making output file"<<std::endl;
	TFile fout("testfile.root","RECREATE");
	
	std::cout<<"making daqinfo tree"<<std::endl;
	TTree* t_daq_info = new TTree("daq_info","Run Information");
	DAQInfo daq_info;
	t_daq_info->Branch("daq_info", &daq_info);
	daq_info.run_number = 444;
	daq_info.sub_run_number = 44;
	t_daq_info->Fill();
	
	std::cout<<"making data tree"<<std::endl;
	TTree* t_data = new TTree("data","data");
	std::vector<P_MPMTHit*> mpmt_hits;
	std::vector<P_MPMTHit*> trigger_hits;
	std::vector<TriggerInfo*> trigger_infos;
	std::vector<P_MPMTWaveformHeader*> mpmt_waveforms;
	std::vector<MPMTWaveformSamples> waveform_samples;
	t_data->Branch("mpmt_hits",&mpmt_hits);
	t_data->Branch("trigger_hits",&trigger_hits);
	t_data->Branch("trigger_infos",&trigger_infos);
	t_data->Branch("waveform_headers",&mpmt_waveforms);
	t_data->Branch("waveform_samples",&waveform_samples);
	
	std::cout<<"making base pointers"<<std::endl;
	mpmt_hits.push_back(new P_MPMTHit);
	trigger_hits.push_back(new P_MPMTHit);
	trigger_infos.push_back(new TriggerInfo);
	mpmt_waveforms.push_back(new P_MPMTWaveformHeader);
	waveform_samples.resize(1);
	
	std::cout<<"populating mpmt_hits"<<std::endl;
	mpmt_hits.front()->spill_num = 777;
	mpmt_hits.front()->card_id = 100;
	mpmt_hits.front()->hit = new MPMTHit;
	mpmt_hits.front()->hit->SetHeader(1);
	mpmt_hits.front()->hit->SetEventType(0);
	mpmt_hits.front()->hit->SetChannel(1);
	mpmt_hits.front()->hit->SetCoarseCounter(12345);
	mpmt_hits.front()->hit->SetFineTime(678);
	mpmt_hits.front()->hit->SetCharge(100);
	mpmt_hits.front()->hit->SetQualityFactor(0);
	mpmt_hits.front()->hit->SetFlags(0);
	
	std::cout<<"populating trigger_hits"<<std::endl;
	trigger_hits.front()->spill_num = 777;
	trigger_hits.front()->card_id = 100;
	trigger_hits.front()->hit = new MPMTHit;
	trigger_hits.front()->hit->SetHeader(1);
	trigger_hits.front()->hit->SetEventType(0);
	trigger_hits.front()->hit->SetChannel(1);
	trigger_hits.front()->hit->SetCoarseCounter(12345);
	trigger_hits.front()->hit->SetFineTime(678);
	trigger_hits.front()->hit->SetCharge(100);
	trigger_hits.front()->hit->SetQualityFactor(0);
	trigger_hits.front()->hit->SetFlags(0);
	
	std::cout<<"populating trigger_infos"<<std::endl;
	trigger_infos.front()->spill_num = 777;
	trigger_infos.front()->card_id = 100;
	trigger_infos.front()->time = 12345;
	trigger_infos.front()->type = TriggerType::MAIN;
	trigger_infos.front()->vme_event_num = 888;
	trigger_infos.front()->mpmt_LEDs.push_back(new P_MPMTLED);
	
	std::cout<<"populating trigger infos MPMTLEDs"<<std::endl;
	trigger_infos.front()->mpmt_LEDs.front()->spill_num = 777;
	trigger_infos.front()->mpmt_LEDs.front()->card_id = 100;
	trigger_infos.front()->mpmt_LEDs.front()->led = new MPMTLED;
	
	std::cout<<"populating trigger infos MPMTLEDs front"<<std::endl;
	trigger_infos.front()->mpmt_LEDs.front()->led->SetHeader(1);
	trigger_infos.front()->mpmt_LEDs.front()->led->SetEventType(2); // 2 = led
	trigger_infos.front()->mpmt_LEDs.front()->led->SetLED(4);
	trigger_infos.front()->mpmt_LEDs.front()->led->SetGain(1); // 0 or 1
	trigger_infos.front()->mpmt_LEDs.front()->led->SetDACSetting(66);
	trigger_infos.front()->mpmt_LEDs.front()->led->SetType(2);
	trigger_infos.front()->mpmt_LEDs.front()->led->SetSequenceNumber(3);
	trigger_infos.front()->mpmt_LEDs.front()->led->SetCoarseCounter(12345);
	trigger_infos.front()->mpmt_LEDs.front()->led->SetReserved(0);
	
	std::cout<<"populating waveforms"<<std::endl;
	mpmt_waveforms.front()->waveform_header = new MPMTWaveformHeader;
	mpmt_waveforms.front()->waveform_header->SetHeader(2);
	mpmt_waveforms.front()->waveform_header->SetFlags(0);
	mpmt_waveforms.front()->waveform_header->SetCoarseCounter(12345);
	mpmt_waveforms.front()->waveform_header->SetChannel(1);
	mpmt_waveforms.front()->waveform_header->SetNumSamples(32);
	mpmt_waveforms.front()->waveform_header->SetLength(48);
	mpmt_waveforms.front()->waveform_header->SetReserved(0);
	
	std::cout<<"populating waveform samples"<<std::endl;
	waveform_samples.front().nbytes = mpmt_waveforms.front()->waveform_header->GetLength();
	waveform_samples.front().bytes = new unsigned char[waveform_samples.front().nbytes];
	for(size_t i=0; i<waveform_samples.front().nbytes; ++i){
		waveform_samples.front().bytes[i] = i;
	}
	
	std::cout<<"filling tree"<<std::endl;
	t_data->Fill();
	
	std::cout<<"writing file"<<std::endl;
	fout.Write();
	fout.Close();
	
	// cleanup
	std::cout<<"Cleanup..."<<std::endl;
	delete mpmt_hits.front()->hit; // MPMTHit
	delete trigger_hits.front()->hit; // MPMTHit
	delete trigger_infos.front()->mpmt_LEDs.front()->led; // MPMTLED
	//delete trigger_infos.front()->mpmt_LEDs.front(); // P_MPMTLED. ~TriggerInfo deletes its mpmt_LEDs
	delete mpmt_waveforms.front()->waveform_header; // MPMTWaveformHeader
	delete[] waveform_samples.front().bytes;
	
	delete mpmt_hits.front(); // P_MPMTHit;
	delete trigger_hits.front(); // P_MPMTHit;
	delete trigger_infos.front(); // TriggerInfo;
	delete mpmt_waveforms.front(); // P_MPMTWaveformHeader;
	
	std::cout<<"done"<<std::endl;
	
	return 0;
	
}
