#ifndef MPMTWaveformSamples_H
#define MPMTWaveformSamples_H
//#include "Rtypes.h" // for classdef, efficient streaming

struct MPMTWaveformSamples {
	MPMTWaveformSamples(){};
	~MPMTWaveformSamples(){};
	unsigned int nsamples=0;
	int nbytes=0; // must be type int for root
	unsigned char* bytes=0; //[nbytes]  << load bearing comment used by ROOT do not change
	std::vector<int16_t> GetSamples(){
		std::vector<int16_t> samples;
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
			
			// convert to signed
			int16_t nextsample_s = nextsample & 0xfff; // probably redundant
			nextsample_s = nextsample | (-(nextsample & 0x800));
			/*
			// alternative conversion
			if(nextsample & 0x800){
				// 0x800 is 12th bit, i.e. sign bit for 12-bit signed integer
				// if this is set, then num is negative; set upper 4 bits to 1's for 2's complement,
				// then reinterpret as signed
				nextsample = nextsample | 0b1111000000000000;
				nextsample_s = reinterpret_cast<int16_t&>(nextsample);
			} else {
				// if positive a simple implicit cast will do fine
				nextsample_s = nextsample;
			}
         		*/
			//std::cout<<"pushing back sample "<<nextsample<<std::endl;
			samples.push_back(nextsample_s);
			toggle=!toggle;
		}
		return samples;
	}
	void Print(bool printsamples=false, bool printbytes=false){
		std::cout<<"nbytes = "<<nbytes<<std::endl;
		std::cout<<"bytes at = "<<(void*)bytes<<std::endl;
		if(printsamples){
			std::vector<int16_t> samples = GetSamples();
			std::cout<<"samples = [";
			for(int i=0; i<samples.size(); ++i){
				if(i>0) std::cout<<",";
				std::cout<<samples[i];
			}
			std::cout<<"]"<<std::endl;
		}
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
