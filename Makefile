#DATAMODEL:=/root/WCTEDAQ/DataModel
DATAMODEL:=$(CURDIR)
#FIXME update to your install of ToolFrameworkCore
TOOLFDIR:=$(DATAMODEL)/../Dependencies/ToolFrameworkCore

SILENCERS= -Wno-shift-op-parentheses -Wno-bitwise-op-parentheses -Wno-shift-count-overflow -Wno-shift-negative-value -Wno-shift-count-negative
CXXFLAGS= -g -std=c++11

all: test

WCTE_RootDict.cxx: $(DATAMODEL)/DAQInfo.h $(DATAMODEL)/ReadoutWindow.h $(DATAMODEL)/MPMTMessages.h $(DATAMODEL)/MPMTWaveformSamples.h $(DATAMODEL)/TriggerType.h SerialisableObject.h WCTE_Linkdef.h
	rootcling $(SILENCERS) -f $@ -I. -I$(DATAMODEL) -I$(TOOLFDIR)/include -c $^

libWCTE_RootDict.so: WCTE_RootDict.cxx
	g++ $(CXXFLAGS) -D__CLING__ -shared -fPIC -Wl,--no-undefined -o $@ $^ -I. -I$(DATAMODEL) -I`root-config --incdir` -I$(TOOLFDIR)/include `root-config --libs`

test: test_daq.cxx libWCTE_RootDict.so
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_daq $< -I. -I$(DATAMODEL) -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`

clean:
	rm -f libWCTE_RootDict.so  WCTE_RootDict.cxx  WCTE_RootDict_rdict.pcm test_daq
