/** @file benchmark.cpp

    A simple benchmark of the all-pairs loop operating on an unsorted
    ParticleStorage.
 */

#define LOG4ESPP_LEVEL_WARN

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "espresso_common.hpp"
#include "types.hpp"
#include "particles/Storage.hpp"
#include "particles/Computer.hpp"
#include "pairs/All.hpp"
#include "pairs/ForceComputer.hpp"
#include "bc/PeriodicBC.hpp"
#include "potential/LennardJones.hpp"
#include "esutil/Timer.hpp"

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

using namespace espresso;
using namespace espresso::particles;
using namespace boost;

/// number of particles in each dimension
const int N = 20;
/// dimension of the cubic simulation box
const real size = 5.0;

static inline real dround(real x) { return floor(x + 0.5); }

/***********************************************************************************
 * Testing the Espresso way of calculating things
 ***********************************************************************************/

class TestEspresso {
public:
  Storage::SelfPtr storage;
  Property< Real3D >::SelfPtr position, force;
  size_t npart;
  
  TestEspresso(size_t nparticles)
    : npart(nparticles) {
    storage = make_shared< Storage >();
    position = make_shared< Property< Real3D > >(storage);
    force = make_shared< Property< Real3D > >(storage);
  }

    void addParticle(const Real3D &pos);

    void calculateForces(real epsilon, real sigma, real cutoff) NOINLINE;

    void runEmptyPairLoop() NOINLINE;

    real calculateMinDist() NOINLINE;

    void runEmptyLoop() NOINLINE;

    real calculateAverage() NOINLINE;

  Real3D getForce(size_t i) {
    return (*force).at(ParticleId(i));
  }
};

void TestEspresso::addParticle(const Real3D &pos)
{
  static size_t cnt = 0;
  ParticleId id = ParticleId(cnt++);
  storage->addParticle(id);
  (*position)[id] = pos;
  (*force)[id] = 0.0;
}

void TestEspresso::calculateForces(real epsilon, real sigma, real cutoff) {
  bc::PeriodicBC::SelfPtr pbc = make_shared< bc::PeriodicBC >(size);
  pairs::All allpairs(pbc, storage, position);
  potential::LennardJones ljint;
  ljint.set(epsilon, sigma, cutoff);
  pairs::ForceComputer *forceCompute =
    ljint.createForceComputer(pairs::ForceComputer(*force));
  allpairs.foreach(*forceCompute);
  delete forceCompute;
}

class EmptyPairComputer: public pairs::ConstComputer {
public:
    EmptyPairComputer() {}

    virtual void operator()(const Real3D &dist,
			    ConstParticleHandle ref1,
			    ConstParticleHandle ref2) {
    }
};

void TestEspresso::runEmptyPairLoop() {
  bc::PeriodicBC::SelfPtr pbc = make_shared< bc::PeriodicBC >(size);
  pairs::All::SelfPtr allpairs = make_shared< pairs::All >(pbc, storage, position);
  shared_ptr< EmptyPairComputer > ljc = make_shared< EmptyPairComputer >();
  allpairs->foreach(ljc);
}

class MinDistComputer: public pairs::ConstComputer {
public:
    real min;

    MinDistComputer(): min(1.0e10) {}

    virtual void operator()(const Real3D &dist,
			    ConstParticleHandle ref1,
			    ConstParticleHandle ref2) {
	real d = dist.sqr();
	if (min > d) {
	    min = d;
	}
    }
};

real TestEspresso::calculateMinDist() {
  bc::PeriodicBC::SelfPtr pbc = make_shared< bc::PeriodicBC >(size);
  pairs::All::SelfPtr allpairs = make_shared< pairs::All >(pbc, storage, position);
  MinDistComputer mincomp;
  allpairs->foreach(mincomp);
  return sqrt(mincomp.min);
}

class AverageComputer: public particles::ConstComputer {
public:

  ConstPropertyHandle<Real3D> property;
  real average;

  AverageComputer(const ConstPropertyHandle<Real3D> &_property)
    : property(_property), average(0) {}

  virtual void operator()(ConstParticleHandle ref) {
    const Real3D p = property[ref];
    average += sqrt(p.sqr());
  }
};

real TestEspresso::calculateAverage() {
  AverageComputer avgcompute(*force);
  storage->foreach(avgcompute);
  return avgcompute.average;
}

class EmptyComputer: public particles::ConstComputer {
public:
    EmptyComputer() {}

    virtual void operator()(ConstParticleHandle ref) {
    }
};

void TestEspresso::runEmptyLoop() {
  EmptyComputer avgcompute;
  storage->foreach(avgcompute);
}

/***********************************************************************************
 * Testing the hardcoded way
 ***********************************************************************************/

class TestBasic {
public:
    std::vector<Real3D> position, force;
    size_t npart;

    TestBasic(size_t nparticles): npart(nparticles) {
        position.reserve(nparticles);
        force.reserve(nparticles);
    }

    void addParticle(const Real3D &pos);

    void calculateForces(real epsilon, real sigma, real cutoff) NOINLINE;

    real calculateMinDist() NOINLINE;

    real calculateAverage() NOINLINE;

    Real3D getForce(size_t i) {
        return force[i];
    }
};

void TestBasic::addParticle(const Real3D &pos) {
    position.push_back(pos);
    force.push_back(0.0);
}

void TestBasic::calculateForces(real epsilon, real sigma, real cutoff) {
    real cutoffSqr = cutoff*cutoff;
    real sizeInverse = 1./size;

    for (size_t i = 0; i < npart; ++i) {
        for (size_t j = i+1; j < npart; ++j) {
            Real3D pos1 = position[i];
            Real3D pos2 = position[j];
            Real3D dist = pos1 - pos2;

            for(size_t c = 0; c < 3; ++c) {
                dist[c] -= dround(dist[c]*sizeInverse)*size;
            }

            {
                real   frac2;
                real   frac6;
                real distSqr = dist.sqr();
                
                if (distSqr < cutoffSqr) {
		    Real3D f(0.0, 0.0, 0.0);
                    frac2 = sigma / distSqr;
                    frac6 = frac2 * frac2 * frac2;
                    real ffactor = 48.0 * epsilon * (frac6*frac6 - 0.5 * frac6) * frac2;
                    f = dist * ffactor;
		    force[i] += f;
		    force[j] -= f;
                } 
            }
        }
    }
}

real TestBasic::calculateMinDist() {
    real min = 1e10;
    real sizeInverse = 1./size;

    for (size_t i = 0; i < npart; ++i) {
        for (size_t j = i+1; j < npart; ++j) {
            Real3D pos1 = position[i];
            Real3D pos2 = position[j];

            Real3D dist = pos1 - pos2;
            for(size_t c = 0; c < 3; ++c) {
                dist[c] -= dround(dist[c]*sizeInverse)*size;
            }
            real d = dist.sqr();

	    if (min > d) {
		min = d;
            }
        }
    }
    return sqrt(min);
}

real TestBasic::calculateAverage() {
    real average = 0;
    for (size_t i = 0; i < npart; ++i) {
        Real3D p = force[i];
        average += sqrt(p.sqr());
    }
    return average;
}

/***********************************************************************************
 * Testing environment
 ***********************************************************************************/

template<class Test>
void generateParticles(Test &test) {
    srand48(123);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) { 
            for (int k = 0; k < N; k++) {
      
                real r;
                r = 0.4 + 0.2 * drand48();
                Real3D pos = Real3D(
                    (i + r) / N * size,
                    (j + r) / N * size, 
                    (k + r) / N * size);

                test.addParticle(pos);
            }
        }
    }
}

/// this routine runs the tests as defined above
int main()
{
    LOG4ESPP_CONFIGURE();

    esutil::WallTimer timer;
    TestEspresso espresso(N*N*N);
    std::cout << std::setw(20) << "setup Espresso: " << timer << std::endl;

    timer.reset();
    TestBasic basic(N*N*N);
    std::cout << std::setw(20) << "setup Basic: " << timer << std::endl;

    // generate particles in the particle storage

    timer.reset();
    generateParticles(espresso);
    std::cout << std::setw(20) << "generate Espresso: " << timer << std::endl;

    timer.reset();
    generateParticles(basic);
    std::cout << std::setw(20) << "generate Basic: " << timer << std::endl;

    std::cout << std::endl << "PARTICLE PAIR LOOPING TESTS" << std::endl;

    // check empty pair loop

    timer.reset();
    espresso.runEmptyPairLoop();
    std::cout << std::setw(30) << "empty pair loop: " << timer << std::endl << std::endl;

    // calculate forces

    timer.reset();
    basic.calculateForces(1.1, 1.2, 2.5);
    real basicCalcTime = timer.getElapsedTime();
    std::cout << std::setw(30) << "calc basic: " << timer << std::endl;

    timer.reset();
    espresso.calculateForces(1.1, 1.2, 2.5);
    std::cout << std::setw(30) << "calc Espresso: " << timer << std::endl;
    std::cout << "RATIO: " << (timer.getElapsedTime() / basicCalcTime) << std::endl;

    // calculate minimum distance

    timer.reset();
    real minb = basic.calculateMinDist();
    real basicMinTime = timer.getElapsedTime();
    std::cout << std::setw(30) << "min Basic: " << timer << std::endl; 

    timer.reset();
    real mine = espresso.calculateMinDist();
    std::cout << std::setw(30) << "min Espresso: " << timer << std::endl;
    std::cout << "RATIO: " << (timer.getElapsedTime() / basicMinTime) << std::endl;

    std::cout << std::endl << "PARTICLE LOOPING TESTS" << std::endl;

    // check empty loop

    timer.reset();
    for (size_t cnt = 0; cnt < 10000; ++cnt)
        espresso.runEmptyLoop();
    std::cout << std::setw(30) << "empty loop: " << timer << std::endl << std::endl;

    // calculate average

    timer.reset();
    real ave;
    for (size_t cnt = 0; cnt < 10000; ++cnt)
        ave = espresso.calculateAverage();
    real espressoAvgTime = timer.getElapsedTime();
    std::cout << std::setw(30) << "average Espresso: " << timer << std::endl;

    timer.reset();
    real avb;
    for (size_t cnt = 0; cnt < 10000; ++cnt)
        avb = basic.calculateAverage();
    std::cout << std::setw(30) << "average Basic: " << timer << std::endl;
    std::cout << "RATIO: " << (espressoAvgTime / timer.getElapsedTime()) << std::endl;

    // check consistency
    std::cout << "min dists: " << mine << " " << minb << std::endl;
    if (std::abs(mine-minb)/std::abs(mine) > 1e-5) {
        std::cerr << "ERROR: minima are different: " << mine << " != " << minb << std::endl;        
    }

    // take into account that Espresso calculates forces twice to test two algorithms
    std::cout << "average force: " << ave << " " << avb << std::endl;
    if (std::abs(ave-avb)/std::abs(ave) > 1e-5) {
        std::cerr << "ERROR: averages are different: " << ave << " != " << avb << std::endl;        
    }

    for (size_t i = 0; i < N*N*N; ++i) {
        Real3D f1 = espresso.getForce(i);
        Real3D f2 = basic.getForce(i);
        real diff = sqrt((f1-f2).sqr());
        if (diff/std::abs(f1.sqr()) > 1e-5) {
            std::cerr << "ERROR: difference " << diff << " too big for particle " << i << std::endl;
            std::cerr << "ERROR: " << f1[0] << " vs. " << f2[0] << std::endl;
            std::cerr << "ERROR: " << f1[1] << " vs. " << f2[1] << std::endl;
            std::cerr << "ERROR: " << f1[2] << " vs. " << f2[2] << std::endl;
        }
    }
}
