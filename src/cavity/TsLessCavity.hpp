#ifndef TSLESSCAVITY_HPP
#define TSLESSCAVITY_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include "Config.hpp"

#include "Cavity.hpp"
#include "CavityFactory.hpp"

/*! \file TsLessCavity.hpp
 *  \class TsLessCavity
 *  \brief A class for TsLess cavity. 
 *  \author Roberto Di Remigio
 *  \date 2013
 */

class TsLessCavity : public Cavity 
{
	public:
		TsLessCavity(){}
                TsLessCavity(const std::vector<Sphere> & _spheres, double _area, double _probeRadius = 0.0, bool _addSpheres = false) :  
                    Cavity(_spheres), averageArea(_area), probeRadius(_probeRadius), addSpheres(_addSpheres), maxAddedSpheres(100) 
                       {
                    	   makeCavity(10000, 10000000);
                       }
                virtual ~TsLessCavity() {}
                void makeCavity(int maxts, int lwork);
                void makeCavity();
                double getProbeRadius() { return probeRadius; }
                void setProbeRadius( double probeRadius );
                friend std::ostream & operator<<(std::ostream & os, TsLessCavity & cavity)
		{
			return cavity.printCavity(os);
		}
	private:
                double averageArea;  
                double probeRadius;
                bool addSpheres;
                int maxAddedSpheres;
                int addedSpheres;
                virtual std::ostream & printCavity(std::ostream & os);  
};

namespace
{
	Cavity* createTsLessCavity(const std::vector<Sphere> & _spheres, double _area, double _probeRadius = 0.0, 
			bool _addSpheres = false, int _patchLevel = 2, double _coarsity = 0.5)
	{
		return new TsLessCavity(_spheres, _area, _probeRadius, _addSpheres);
	}
	const std::string TSLESS("TsLess");
	const bool registeredTsLess = CavityFactory::TheCavityFactory().registerCavity(TSLESS, createTsLessCavity);
}

#endif // TSLESSCAVITY_HPP