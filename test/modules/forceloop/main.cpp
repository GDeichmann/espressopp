// Example C++ program doing the same as the force loop Python script

#include <types.hpp>
#include <logging.hpp>

#include <espresso_common.hpp>
#include <particles/Storage.hpp>
#include <bc/PeriodicBC.hpp>
#include <potential/LennardJones.hpp>
#include <potential/FENE.hpp>
#include <pairs/All.hpp>
#include <pairs/ForceComputer.hpp>
#include <thermostat/Langevin.hpp>

#include "ParticleWriter.hpp"

// #include "PairWriteComputer.hpp"

#include "integrator/VelocityVerlet.hpp"

#include <boost/make_shared.hpp>
#include <cstdio>
#include <vector>

using namespace espresso;
using namespace espresso::bc;
using namespace espresso::particles;
using namespace espresso::pairs;
using namespace espresso::potential;
using namespace espresso::integrator;
using namespace boost;

/** N stands for number particles in each dimensions.

    N   SIZE 
  10    15.0

*/

#define N 3
#define SIZE 4.0

/** Main routine of a test program:

    - generate N * N * N particle in a box of size SIZE * SIZE * SIZE 
    - print out particle data
    - define periodic boundary conditions
    - define Lennard Jones potential and apply it to all pairs
    - print out particle data
*/

void forceloop() {

  // Create a new particle storage

  Storage::SelfPtr particleStorage = make_shared< Storage >();

  Real3DProperty::SelfPtr position = make_shared< Real3DProperty >(particleStorage);
  Real3DProperty::SelfPtr velocity = make_shared< Real3DProperty >(particleStorage);
  Real3DProperty::SelfPtr force = make_shared< Real3DProperty >(particleStorage);

  // generate particles in the particle storage
  
  size_t cnt = 0;
  for (int i = 0; i < N; i++) 
  for (int j = 0; j < N; j++) 
  for (int k = 0; k < N; k++) {
      
       real r;
       r = 0.4 + 0.2 * rand() / RAND_MAX;
       real x = (i + r) / N * SIZE;
       real y = (j + r) / N * SIZE; 
       real z = (k + r) / N * SIZE;

       ParticleId id = ParticleId(cnt++);
       particleStorage->addParticle(id);

       (*position.get())[id] = Real3D(x, y, z);
       (*velocity.get())[id] = Real3D(x, y, z);
       (*force.get())[id] = 0.0;
  }

  // For test only: ParticleWriter prints each particle

  ParticleWriter pWriter(*particleStorage.get(), *position.get(), *force.get());

  // call pWriter(id) for each particle reference ref of particle storage

  particleStorage->foreach(pWriter);

  // define periodic boundary conditions

  PeriodicBC::SelfPtr pbc = make_shared< PeriodicBC >(SIZE);

  // define allpairs with (x, y) for all x, y in allSet

  pairs::All::SelfPtr allpairs = make_shared< pairs::All >(pbc, particleStorage, position);
  
  // For test only: PairWriter prints each particle pair 
  
  // PairWriteComputer pairWriter(&particleStorage, position);
  
  // call pairWriter(ref1, ref2) for each particle ref pair of allSet

  // allpairs.foreach(pairWriter);

  // define LennardJones potential

  LennardJones::SelfPtr ljint = make_shared< LennardJones >();
  
  ljint->set(1.0, 1.0, 2.5);
  
  // make a FENE potential

  FENE fene;

  fene.set(1.5, 1.0, 2.5);

  // force will be the vector of all forces in the particle storage
  // and force[ref] returns the force (as RealArrayRef) of particle reference ref

  PropertyHandle<Real3D> forceRef = *force.get();

  // Define a pair computer that computes the forces for particle pairs
  // ljint provides the routine computeForce for a particle pair
  // force (pointer to all forces of particles) tells us where the computed forces are added

  // ForceComputer *forcecompute = ljint->createForceComputer(ForceComputer(forceRef));

  // call forcecompute(ref1, ref2) for each particle ref pair of allSet

  // allpairs->foreach(*forcecompute);

  // delete forcecompute;

  // print out all particle data to see that it calculates some forces

  // particleStorage->foreach(pWriter);

  // create integrator and set properties

  VelocityVerlet::SelfPtr integrator = 
    make_shared< VelocityVerlet >(particleStorage, position, velocity, force);
  
  integrator->setTimeStep(0.005);
  /*
  integrator->addForce(ljint, allpairs);

  // make a shared pointer to a Lanevin thermostat with T=298.15 and gamma=0.5
  boost::shared_ptr<thermostat::Langevin> lv =
    boost::shared_ptr<thermostat::Langevin>
      (new thermostat::Langevin(allSet, 1.0, 0.5));
  // write out the gamma value
  std::cout << lv->getGamma() << "\t" << lv->getTemperature() << std::endl;
  // set the thermostat for the integrator
  integrator->setThermostat(lv);
  // get the integrator and call its thermalizeA method virtually
  integrator->getThermostat()->thermalizeA();

  // integrate for a certain number of timesteps
  integrator->run(100);

  // check to see that particles have new positions
  */
  particleStorage->foreach(pWriter);
}

int main() 

{

  printf("configure forceloop\n");

  LOG4ESPP_CONFIGURE();

  forceloop();

}
