#include "python.hpp"

#include "FixedPairListAdress.hpp"

//#include <vector>
//#include <utility>
//#include <algorithm>
#include <boost/bind.hpp>
#include "storage/Storage.hpp"
#include "Buffer.hpp"

//using namespace std;

namespace espresso {

  LOG4ESPP_LOGGER(FixedPairListAdress::theLogger, "FixedPairListAdress");


  FixedPairListAdress::
  FixedPairListAdress(shared_ptr< storage::Storage > _storage)
    : FixedPairList(_storage)
  {
    LOG4ESPP_INFO(theLogger, "construct FixedPairListAdress");

    con1 = storage->beforeSendParticles.connect
      (boost::bind(&FixedPairList::beforeSendParticles, this, _1, _2));
    con2 = storage->afterRecvParticles.connect
      (boost::bind(&FixedPairList::afterRecvParticles, this, _1, _2));
    con3 = storage->onParticlesChanged.connect
      (boost::bind(&FixedPairListAdress::onParticlesChanged, this));

  }

  FixedPairListAdress::~FixedPairListAdress() {

    LOG4ESPP_INFO(theLogger, "~FixedPairListAdress");

    con1.disconnect();
    con2.disconnect();
    con3.disconnect();
  }


  bool FixedPairListAdress::add(longint pid1, longint pid2) {

    if (pid1 > pid2)
      std::swap(pid1, pid2);

    // ADD THE LOCAL PAIR
    Particle *p1 = storage->lookupAdrATParticle(pid1);
    Particle *p2 = storage->lookupAdrATParticle(pid2);
    if (!p1)
      // Particle does not exist here, return false
      return false;
    if (!p2)
      // TODO: Second particle does not exist here, throw exception!
      return false;
    // add the pair locally
    this->add(p1, p2);

    // ADD THE GLOBAL PAIR
    // see whether the particle already has pairs
    std::pair<GlobalPairs::const_iterator, 
      GlobalPairs::const_iterator> equalRange 
      = globalPairs.equal_range(pid1);
    if (equalRange.first == globalPairs.end()) {
      // if it hasn't, insert the new pair
      globalPairs.insert(std::make_pair(pid1, pid2));
    }
    else {
      // otherwise test whether the pair already exists
      for (GlobalPairs::const_iterator it = equalRange.first;
	   it != equalRange.second; ++it)
	if (it->second == pid2)
	  // TODO: Pair already exists, generate error!
	  ;
      // if not, insert the new pair
      globalPairs.insert(equalRange.first, std::make_pair(pid1, pid2));
    }
    LOG4ESPP_INFO(theLogger, "added fixed pair to global pair list");
    return true;
  }


  void FixedPairListAdress::onParticlesChanged() {

    LOG4ESPP_INFO(theLogger, "rebuild local bond list from global\n");

    this->clear();
    longint lastpid1 = -1;
    Particle *p1;
    Particle *p2;

    for (GlobalPairs::const_iterator it = globalPairs.begin();
	 it != globalPairs.end(); ++it) {

        if (it->first != lastpid1) {
            p1 = storage->lookupAdrATParticle(it->first);
            if (p1 == NULL) {
                printf("SERIOUS ERROR: AT particle %d not available\n", it->first);
            }
            lastpid1 = it->first;
        }

        p2 = storage->lookupAdrATParticle(it->second);
        if (p2 == NULL) {
            printf("SERIOUS ERROR: 2nd AT particle %d not available\n", it->second);
        }

        //std::cout << " adding (" << p1->getId() << ", " << p2->getId() << ")\n";
        this->add(p1, p2);
    }
    LOG4ESPP_INFO(theLogger, "regenerated local fixed pair list from global list");
  }

  /****************************************************
  ** REGISTRATION WITH PYTHON
  ****************************************************/

  void FixedPairListAdress::registerPython() {

    using namespace espresso::python;

    bool (FixedPairListAdress::*pyAdd)(longint pid1, longint pid2)
      = &FixedPairListAdress::add;

    class_<FixedPairListAdress, shared_ptr<FixedPairListAdress> >
      ("FixedPairListAdress", init <shared_ptr<storage::Storage> >())
      .def("add", pyAdd)
      ;
  }

}
