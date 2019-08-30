#include "psdaq/hsd/PV126Ctrls.hh"
#include "psdaq/hsd/Module126.hh"
#include "psdaq/hsd/FexCfg.hh"
#include "psdaq/hsd/QABase.hh"
#include "psdaq/hsd/Pgp.hh"
#include "psdaq/epicstools/EpicsPVA.hh"

#include <algorithm>
#include <sstream>
#include <cctype>
#include <stdio.h>
#include <unistd.h>

using Pds_Epics::EpicsPVA;

static bool _interleave = true;
static const int SYNC_WIDTH = (1<<16)/12;

namespace Pds {
  namespace HSD {

    PV126Ctrls::PV126Ctrls(Module126& m, Pds::Task& t) : 
      PVCtrlsBase(t),
      _m(m)
    {}

    void PV126Ctrls::_allocate()
    {
      _m.stop();
    }

    void PV126Ctrls::configure(unsigned fmc) {

#define PVGET(name) pv.getScalarAs<unsigned>(#name)
#define PVGETF(name) pv.getScalarAs<float>(#name)

      Pds_Epics::EpicsPVA& pv = *_pv[fmc];

      _m.stop();

      _m.dumpPgp();

      _m.trig_shift(PVGET(trig_shift));

      // don't know how often this is necessary
      _m.train_io(0);

      unsigned enable = PVGET(enable);
      if (enable) {
        unsigned channelMask = 0;
        for(unsigned i=0; i<4; i++) {
          if (enable & (1<<i)) {
            channelMask |= (1<<i);
            if (_interleave) break;
          }
        }
        printf("channelMask 0x%x: ilv %c\n",
               channelMask, _interleave?'T':'F');
        _m.setAdcMux( _interleave, channelMask );

        // set testpattern
        int pattern = PVGET(test_pattern);
        printf("Pattern: %d\n",pattern);
        _m.disable_test_pattern();
        if (pattern>=0)
          _m.enable_test_pattern((Module126::TestPattern)pattern);
        
        int ephlo = PVGET(sync_ph_even) - SYNC_WIDTH/2;
        int ephhi = ephlo + SYNC_WIDTH;
        int ophlo = PVGET(sync_ph_odd) - SYNC_WIDTH/2;
        int ophhi = ophlo + SYNC_WIDTH;
        int eph, oph;
        while(1) {
          eph = _m.trgPhase()[0];
          oph = _m.trgPhase()[1];
          printf("trig phase %05d [%05d/%05d] %05d [%05d/%05d]\n",
                 eph,ephlo,ephhi,
                 oph,ophlo,ophhi);
          if (eph > ephlo && eph < ephhi &&
              oph > ophlo && oph < ophhi)
            break;
          _m.sync();
          usleep(100000);  // Wait for relock
        }

        // zero the testpattern error counts
        _m.clear_test_pattern_errors();
        
        // _m.sample_init(32+48*length, 0, 0);
        QABase& base = *reinterpret_cast<QABase*>((char*)_m.reg()+0x80000);
        base.init();
        base.resetCounts();

        { std::vector<Pgp*> pgp = _m.pgp();
          for(unsigned i=0; i<pgp.size(); i++)
            pgp[i]->resetCounts(); }

        //  These aren't used...
        //      base.samples = ;
        //      base.prescale = ;

        unsigned group = PVGET(readoutGroup);
        _m.trig_daq(group);

        FexCfg& fex = _m.fex()[0];
        _configure_fex(0,fex);

        base.dump();

        printf("Configure done\n");

        _m.start();
      }

      _ready[fmc]->putFrom<unsigned>(1);
    }

    void PV126Ctrls::reset() {
      QABase& base = *reinterpret_cast<QABase*>((char*)_m.reg()+0x80000);
      base.resetFbPLL();
      usleep(1000000);
      base.resetFb ();
      base.resetDma();
      usleep(1000000);
    }

    void PV126Ctrls::loopback(bool v) {
      std::vector<Pgp*> pgp = _m.pgp();
      for(unsigned i=0; i<4; i++)
        pgp[i]->loopback(v);

      // for(unsigned i=0; i<4; i++)
      //   pgp[i]._rxReset = 1;
      // usleep(10);
      // for(unsigned i=0; i<4; i++)
      //   pgp[i]._rxReset = 0;
      // usleep(100);
    }
  };
};
