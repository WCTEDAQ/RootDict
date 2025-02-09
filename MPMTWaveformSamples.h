#ifndef MPMTWaveformSamples_H
#define MPMTWaveformSamples_H
//#include "Rtypes.h" // for classdef, efficient streaming

struct MPMTWaveformSamples {
	MPMTWaveformSamples(){};
	~MPMTWaveformSamples(){};
	int nbytes;
	unsigned char* bytes; //[nbytes]  << load bearing comment used by ROOT do not change
	void Print(bool printbytes=false){
		std::cout<<"nbytes = "<<nbytes<<std::endl;
		std::cout<<"bytes at = "<<(void*)bytes<<std::endl;
		if(printbytes){
			std::cout<<"bytes = [";
			for(int i=0; i<nbytes; ++i){
				if(i>0) std::cout<<",";
				std::cout<<"0x"<<std::hex<<(int)bytes[i];
			}
			std::cout<<"]"<<std::endl;
		}
	}
	//ClassDef(MPMTWaveformSamples, 1);
};
#endif
