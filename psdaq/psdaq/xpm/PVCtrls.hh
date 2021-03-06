#ifndef Xpm_PVCtrls_hh
#define Xpm_PVCtrls_hh

#include "psdaq/epicstools/EpicsPVA.hh"

#include <string>
#include <vector>

namespace Pds {

  class Semaphore;

  namespace Xpm {
    
    class Module;
    class XpmSequenceEngine;
    class PVSeq;

    class PVCtrls
    {
    public:
      PVCtrls(Module&, Semaphore& sem);
      ~PVCtrls();
    public:
      static void* notify_thread(void*);
    public:
      void allocate(const std::string& title);
      void update();
      void dump() const;
      void checkPoint(unsigned iseq, unsigned addr);
      void resetMmcm(unsigned);
    public:
      Module& module();
      Semaphore& sem();
      XpmSequenceEngine* seq();
    private:
      std::vector<Pds_Epics::EpicsPVA*> _pv;
      Module&              _m;
      Semaphore&           _sem;
      XpmSequenceEngine*   _seq;
      std::vector<PVSeq*>  _seq_pv;
      Pds_Epics::EpicsPVA* _mmcmPV[4];
      unsigned             _nmmcm;
    };
  };
};

#endif
