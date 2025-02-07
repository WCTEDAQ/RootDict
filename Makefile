SELF_DIR:= $(dir $(lastword $(MAKEFILE_LIST)))
BASEDIR:=$(shell readlink -f $(SELF_DIR)/..)
TOOLFDIR:=$(BASEDIR)/Dependencies/ToolFrameworkCore

SILENCERS= -Wno-shift-op-parentheses -Wno-bitwise-op-parentheses -Wno-shift-count-overflow -Wno-shift-negative-value -Wno-shift-count-negative
CXXFLAGS= -g -std=c++11

all: libWCTE_RootDict.so

$(BASEDIR)/RootDict/WCTE_RootDict.cxx: $(BASEDIR)/DataModel/DAQInfo.h $(BASEDIR)/DataModel/ReadoutWindow.h $(BASEDIR)/DataModel/MPMTMessages.h $(BASEDIR)/DataModel/TriggerType.h $(BASEDIR)/RootDict/SerialisableObject.h $(BASEDIR)/RootDict/WCTE_Linkdef.h
	rootcling $(SILENCERS) -f $@ -I. -I$(BASEDIR)/DataModel -I$(TOOLFDIR)/include -c $^

libWCTE_RootDict.so: $(BASEDIR)/RootDict/WCTE_RootDict.cxx
	g++ $(SILENCERS) -std=c++11 -D__CLING__ -shared -fPIC -Wl,--no-undefined -o $@ $^ -I. -I$(BASEDIR)/DataModel -I`root-config --incdir` -I$(TOOLFDIR)/include `root-config --libs`

clean:
	rm -f libWCTE_RootDict.so  WCTE_RootDict*  WCTE_RootDict

test: test_w.cxx test_r.cxx test_read_daq.cxx libWCTE_RootDict.so
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_w test_w.cxx -I. -I$(BASEDIR)/DataModel -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_r test_r.cxx -I. -I$(BASEDIR)/DataModel -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`
	g++ $(CXXFLAGS) -D__CLING__ -Wl,--no-undefined -o test_daq test_read_daq.cxx -I. -I$(BASEDIR)/DataModel -I`root-config --incdir` -I$(TOOLFDIR)/include -L. -lWCTE_RootDict `root-config --libs`
