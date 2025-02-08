#ifndef MPMTWaveformSamples_H
#define MPMTWaveformSamples_H
//#include "Rtypes.h" // for classdef, efficient streaming

struct MPMTWaveformSamples {
	MPMTWaveformSamples(){};
	~MPMTWaveformSamples(){};
	int nsamples;
	unsigned char* samples; //[nsamples]  << load bearing comment used by ROOT do not change
	void Print(bool printsamples=false){
		std::cout<<"nsamples = "<<nsamples<<std::endl;
		std::cout<<"samples at = "<<(void*)samples<<std::endl;
		if(printsamples){
			std::cout<<"samples = [";
			for(int i=0; i<nsamples; ++i){
				if(i>0) std::cout<<",";
				std::cout<<"0x"<<std::hex<<(int)samples[i];
			}
			std::cout<<"]"<<std::endl;
		}
	}
	//ClassDef(MPMTWaveformSamples, 1);
};
#endif
