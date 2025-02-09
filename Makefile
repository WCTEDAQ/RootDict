SELFDIR:= $(dir $(lastword $(MAKEFILE_LIST)))
BASEDIR:=$(shell readlink -f $(SELFDIR)/../WCTEDAQ)
DATAMODEL:=$(BASEDIR)/DataModel
TOOLFDIR:=$(BASEDIR)/Dependencies/ToolFrameworkCore

SILENCERS= -Wno-shift-op-parentheses -Wno-bitwise-op-parentheses -Wno-shift-count-overflow -Wno-shift-negative-value -Wno-shift-count-negative
CXXFLAGS= -g -std=c++11

all: libWCTE_RootDict.so

WCTE_RootDict.cxx: $(DATAMODEL)/DAQInfo.h $(DATAMODEL)/ReadoutWindow.h $(DATAMODEL)/MPMTMessages.h $(DATAMODEL)/MPMTWaveformSamples.h $(DATAMODEL)/TriggerType.h $(SELFDIR)/SerialisableObject.h WCTE_Linkdef.h
	rootcling $(SILENCERS) -f $@ -I. -I$(DATAMODEL) -I$(TOOLFDIR)/include -c $^

libWCTE_RootDict.so: WCTE_RootDict.cxx
	g++ $(CXXFLAGS) -D__CLING__ -shared -fPIC -Wl,--no-undefined -o $@ $^ -I. -I$(DATAMODEL) -I`root-config --incdir` -I$(TOOLFDIR)/include `root-config --libs`
	#cp libWCTE_RootDict.so WCTE_RootDict_rdict.pcm $(BASEDIR)/lib

test: test_daq.cxx test_w.cxx test_r.cxx libWCTE_RootDict.so
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_daq $< -I. -I$(DATAMODEL) -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_w test_w.cxx -I. -I$(DATAMODEL) -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_r test_r.cxx -I. -I$(DATAMODEL) -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`

clean:
	rm -f libWCTE_RootDict.so WCTE_RootDict* test_r test_w test_daq
