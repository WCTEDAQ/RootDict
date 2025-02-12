#ifndef MPMTWaveformSamples_H
#define MPMTWaveformSamples_H
//#include "Rtypes.h" // for classdef, efficient streaming

struct MPMTWaveformSamples {
	MPMTWaveformSamples(){};
	~MPMTWaveformSamples(){};
	unsigned int nsamples=0;
	int nbytes=0; // must be type int for root
	unsigned char* bytes=0; //[nbytes]  << load bearing comment used by ROOT do not change
	std::vector<uint16_t> GetSamples(){
		std::vector<uint16_t> samples;
		bool toggle=true;
		for(int i=0; i<(nbytes-1); ){
			//std::cout<<"building sample "<<samples.size()-1<<" from bytes "<<i<<" and "<<i+1<<std::endl;
			uint16_t nextsample=0;
			if(toggle){
				nextsample = (bytes[i] << 4) | (bytes[i+1] >> 4);
				++i;
			} else {
				nextsample = ( (bytes[i] << 8) | bytes[i+1] ) & 0b0000111111111111;
				i+=2;
			}
			//std::cout<<"pushing back sample "<<nextsample<<std::endl;
			samples.push_back(nextsample);
			toggle=!toggle;
		}
		return samples;
	}
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
