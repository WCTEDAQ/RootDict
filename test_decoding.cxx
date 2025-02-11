#include <iostream>
#include <math.h>
#include <vector>
#include <bitset>
#include "MPMTWaveformSamples.h"

int main(){
	
	int nsamples=13;
	int nbytes = std::ceil(double(nsamples)*12./8.);
	std::vector<uint8_t> vec(nbytes, 1);
	
	MPMTWaveformSamples wav;
	wav.nsamples = nsamples;
	wav.nbytes = nbytes;
	wav.bytes=vec.data();
	std::vector<uint16_t> decoded = wav.GetSamples();
	std::cout<<"nsamples: "<<nsamples<<", nbytes: "<<wav.nbytes<<", decoded samples: "<<decoded.size()<<std::endl;
	
	/* this ought to be:
	in bytes:
	[0000  0001][0000  0001][0000  0001][0000  0001][0000  0001]...
	in samples:
	[0000  0001  0000][0001  0000  0001][0000  0001  0000]...
	*/
	uint16_t check_even = 0b000000010000;
	uint16_t check_odd  = 0b000100000001;
	std::cout<<"expect even samples to be "<<check_even<<", odd sampls to be "<<check_odd<<std::endl;
	
	std::cout<<"got "<<decoded.size()<<" samples, values: [";
	for(int i=0; i<decoded.size(); ++i){
		if(i!=0) std::cout<<", ";
		std::cout<<decoded[i];
		uint16_t check=0;
		if((i%2)==0) check = check_even;
		else         check = check_odd;
		if(decoded[i]!=check){
			std::cerr<<"DECODING ERROR AT SAMPLE "<<i<<std::endl;
			std::bitset<16> bits(decoded[i]);
			std::cout<<"sample bits: "<<bits<<std::endl;
			break;
		}
	}
	std::cout<<"]"<<std::endl;
	
	std::cout<<"All data OK!"<<std::endl;
	
	return 0;
}
