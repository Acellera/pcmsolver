/* pcmsolver_copyright_start */
/*
 *     PCMSolver, an API for the Polarizable Continuum Model
 *     Copyright (C) 2013 Roberto Di Remigio, Luca Frediani and contributors
 *
 *     This file is part of PCMSolver.
 *
 *     PCMSolver is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     PCMSolver is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with PCMSolver.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     For information on the complete list of contributors to the
 *     PCMSolver API, see: <http://pcmsolver.github.io/pcmsolver-doc>
 */
/* pcmsolver_copyright_end */

#include "pcmsolver.h"
#include "Meddle.hpp"

#include <string>

#include "Config.hpp"

#include <Eigen/Core>

#include <boost/foreach.hpp>

#include "Cxx11Workarounds.hpp"
// Core classes
// This list all header files that need to be included here.
// It is automatically generated by CMake during configuration
#include "Includer.hpp"

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

PCMSOLVER_API
pcmsolver_context_t * pcmsolver_new(collect_nctot f_1, collect_atoms f_2, host_writer f_3,
                                    set_point_group f_4)
{
    return AS_TYPE(pcmsolver_context_t, new pcm::Meddle(f_1, f_2, f_3, f_4));
}

PCMSOLVER_API
void pcmsolver_delete(pcmsolver_context_t * context)
{
    if (!context) return;
    delete AS_TYPE(pcm::Meddle, context);
}

PCMSOLVER_API
void pcmsolver_print(pcmsolver_context_t * context)
{
    AS_TYPE(pcm::Meddle, context)->printInfo();
}

PCMSOLVER_API
unsigned int pcmsolver_get_cavity_size(pcmsolver_context_t * context)
{
    return (AS_TYPE(pcm::Meddle, context)->getCavitySize());
}

PCMSOLVER_API
unsigned int pcmsolver_get_irreducible_cavity_size(pcmsolver_context_t * context)
{
    return (AS_TYPE(pcm::Meddle, context)->getIrreducibleCavitySize());
}

PCMSOLVER_API
void pcmsolver_get_centers(pcmsolver_context_t * context, double centers[])
{
    AS_TYPE(pcm::Meddle, context)->getCenters(centers);
}

PCMSOLVER_API
void pcmsolver_compute_asc(pcmsolver_context_t * context,
                           const char * mep_name,
                           const char * asc_name,
                           int irrep)
{
    AS_TYPE(pcm::Meddle, context)->computeASC(mep_name, asc_name, irrep);
}

PCMSOLVER_API
void pcmsolver_compute_response_asc(pcmsolver_context_t * context,
                           const char * mep_name,
                           const char * asc_name,
                           int irrep)
{
    AS_TYPE(pcm::Meddle, context)->computeResponseASC(mep_name, asc_name, irrep);
}

PCMSOLVER_API
double pcmsolver_compute_polarization_energy(pcmsolver_context_t * context,
                                             const char * mep_name,
                                             const char * asc_name)
{
    return (AS_TYPE(pcm::Meddle, context)->computePolarizationEnergy(mep_name, asc_name));
}

PCMSOLVER_API
void pcmsolver_get_surface_function(pcmsolver_context_t * context,
                                    unsigned int size, double values[], const char * name)
{
    AS_TYPE(pcm::Meddle, context)->getSurfaceFunction(size, values, name);
}

PCMSOLVER_API
void pcmsolver_set_surface_function(pcmsolver_context_t * context,
                                    unsigned int size, double values[], const char * name)
{
    AS_TYPE(pcm::Meddle, context)->setSurfaceFunction(size, values, name);
}

PCMSOLVER_API
void pcmsolver_save_surface_functions(pcmsolver_context_t * context)
{
    AS_TYPE(pcm::Meddle, context)->saveSurfaceFunctions();
}

PCMSOLVER_API
void pcmsolver_save_surface_function(pcmsolver_context_t * context, const char * name)
{
    AS_TYPE(pcm::Meddle, context)->saveSurfaceFunction(name);
}

PCMSOLVER_API
void pcmsolver_load_surface_function(pcmsolver_context_t * context, const char * name)
{
    AS_TYPE(pcm::Meddle, context)->loadSurfaceFunction(name);
}

namespace pcm {
    Meddle::Meddle(const NrNucleiGetter & f_1, const CoordinatesGetter & f_2, const HostWriter & f_3,
            const PointGroupSetter & f_4) //, const HostInput & f_5)
        : nrNuclei_(f_1), chargesAndCoordinates_(f_2), hostWriter_(f_3),
          pointGroup_(f_4) //, hostInputReader_(f_5)
    {
        initInput();
        initCavity();
        initSolver();
        // Reserve space for Tot-MEP/ASC, Nuc-MEP/ASC and Ele-MEP/ASC
        functions_.reserve(6);
    }

    Meddle::~Meddle()
    {
        delete cavity_;
        delete K_0_;
        if (hasDynamic_) delete K_d_;
    }

    unsigned int Meddle::getCavitySize() const
    {
        return cavity_->size();
    }

    unsigned int Meddle::getIrreducibleCavitySize() const
    {
        return cavity_->irreducible_size();
    }

    void Meddle::getCenters(double centers[]) const
    {
        centers = cavity_->elementCenter().data();
    }

    double Meddle::computePolarizationEnergy(const char * mep_name, const char * asc_name) const
    {
        double energy = functions_[std::string(mep_name)] * functions_[std::string(asc_name)];
        return (energy / 2.0);
    }

    void Meddle::computeASC(const char * mep_name, const char * asc_name, int irrep) const
    {
        std::string MEP(mep_name);
        std::string ASC(asc_name);

        // Get the proper iterators
        SurfaceFunctionMap::const_iterator iter_pot = functions_.find(MEP);
        SurfaceFunction asc(ASC, cavity_->size());
        asc.vector() = K_0_->computeCharge(iter_pot->second.vector(), irrep);
        asc /= double(cavity_->pointGroup().nrIrrep());
        // Insert it into the map
        if (functions_.count(ASC) == 1) { // Key in map already
            functions_[ASC] = asc;
        } else { // Create key-value pair
            functions_.insert(std::make_pair(ASC, asc));
        }
    }

    void Meddle::computeResponseASC(const char * mep_name, const char * asc_name, int irrep) const
    {
        std::string MEP(mep_name);
        std::string ASC(asc_name);

        // Get the proper iterators
        SurfaceFunctionMap::const_iterator iter_pot = functions_.find(MEP);
        SurfaceFunction asc(ASC, cavity_->size());
        if (hasDynamic_) {
            asc.vector() = K_d_->computeCharge(iter_pot->second.vector(), irrep);
        } else {
            asc.vector() = K_0_->computeCharge(iter_pot->second.vector(), irrep);
        }
        asc /= double(cavity_->pointGroup().nrIrrep());
        if (functions_.count(ASC) == 1) { // Key in map already
            functions_[ASC] = asc;
        } else { // Create key-value pair
            functions_.insert(std::make_pair(ASC, asc));
        }
    }

    void Meddle::getSurfaceFunction(unsigned int size, double values[], const char * name) const
    {
        if (cavity_->size() != size)
            PCMSOLVER_ERROR("You are trying to access a SurfaceFunction bigger than the cavity!");

        std::string functionName(name);

        SurfaceFunctionMap::const_iterator iter = functions_.find(functionName);
        if (iter == functions_.end())
            PCMSOLVER_ERROR("You are trying to access a non-existing SurfaceFunction.");

        for (size_t i = 0; i < size; ++i) {
            values[i] = iter->second.value(i);
        }
    }

    void Meddle::setSurfaceFunction(unsigned int size, double values[], const char * name) const
    {
        if (cavity_->size() != size)
            PCMSOLVER_ERROR("You are trying to allocate a SurfaceFunction bigger than the cavity!");

        std::string functionName(name);
        if (functions_.count(functionName) == 1) { // Key in map already
            functions_[functionName].setValues(values);
        } else {
            SurfaceFunction func(functionName, size, values);
            functions_.insert(std::make_pair(functionName, func));
        }
    }

    void Meddle::saveSurfaceFunctions() const
    {
        typedef SurfaceFunctionMap::value_type SurfaceFunctionPair;
        printer("\nDumping surface functions to .npy files");
        BOOST_FOREACH(SurfaceFunctionPair pair, functions_) {
            unsigned int dim = static_cast<unsigned int>(pair.second.nPoints());
            const unsigned int shape[] = {dim};
            std::string fname = pair.second.name() + ".npy";
            cnpy::npy_save(fname, pair.second.vector().data(), shape, 1, "w", true);
        }
    }

    void Meddle::saveSurfaceFunction(const char * name) const
    {
        typedef SurfaceFunctionMap::const_iterator surfMap_iter;
        std::string functionName(name);
        std::string fname = functionName + ".npy";

        surfMap_iter it = functions_.find(functionName);
        unsigned int dim = static_cast<unsigned int>(it->second.nPoints());
        const unsigned int shape[] = {dim};
        cnpy::npy_save(fname, it->second.vector().data(), shape, 1, "w", true);
    }

    void Meddle::loadSurfaceFunction(const char * name) const
    {
        std::string functionName(name);
        printer("\nLoading surface function " + functionName + " from .npy file");
        std::string fname = functionName + ".npy";
        cnpy::NpyArray raw_surfFunc = cnpy::npy_load(fname);
        unsigned int dim = raw_surfFunc.shape[0];
        if (dim != cavity_->size()) {
            PCMSOLVER_ERROR("Inconsistent dimension of loaded surface function!");
        } else {
            Eigen::VectorXd values = getFromRawBuffer<double>(dim, 1, raw_surfFunc.data);
            SurfaceFunction func(functionName, dim, values);
            // Append to global map
            if (functions_.count(functionName) == 1) { // Key in map already
                functions_[functionName] = func;
            } else {
                functions_.insert(std::make_pair(functionName, func));
            }
        }
    }

    void Meddle::printer(const std::string & message) const
    {
        // Extract C-style string from C++-style string and get its length
        const char * message_C = message.c_str();
        size_t message_length = strlen(message_C);
        // Call the host_writer
        hostWriter_(message_C, message_length);
    }

    void Meddle::printer(const std::ostringstream & stream) const
    {
        // Extract C++-style string from stream
        std::string message = stream.str();
        // Extract C-style string from C++-style string and get its length
        const char * message_C = message.c_str();
        size_t message_length = strlen(message_C);
        // Call the host_writer
        hostWriter_(message_C, message_length);
    }

    void Meddle::initInput()
    {
        input_ = Input("@pcmsolver.inp");

        // 1. number of atomic centers
        int nuclei = nrNuclei_();
        // 2. position and charges of atomic centers
        Eigen::Matrix3Xd centers;
        centers.resize(Eigen::NoChange, nuclei);
        Eigen::VectorXd charges  = Eigen::VectorXd::Zero(nuclei);
        double * chg = charges.data();
        double * pos = centers.data();
        chargesAndCoordinates_(chg, pos);

        if (input_.mode() != "EXPLICIT") {
            Molecule molec;
            initMolecule(input_, pointGroup_, nuclei, charges, centers, molec);
            input_.molecule(molec);
        }
    }

    void Meddle::initCavity()
    {
        cavity_ = Factory<Cavity, cavityData>::TheFactory().create(input_.cavityType(),
                                                                   input_.cavityParams());
        cavity_->saveCavity();
    }

    void Meddle::initSolver()
    {
        IGreensFunction * gf_i = Factory<IGreensFunction, greenData>::TheFactory().create(input_.greenInsideType(),
          input_.insideGreenParams());
        IGreensFunction * gf_o = Factory<IGreensFunction, greenData>::TheFactory().create(input_.greenOutsideType(),
                input_.outsideStaticGreenParams());
        std::string modelType = input_.solverType();
        K_0_ = Factory<PCMSolver, solverData, IGreensFunction *, IGreensFunction *>::TheFactory().create(modelType, input_.solverParams(), gf_i, gf_o);
        K_0_->buildSystemMatrix(*cavity_);
        if (input_.isDynamic()) {
            IGreensFunction * gf_o_dyn = Factory<IGreensFunction, greenData>::TheFactory().create(input_.greenOutsideType(),
                    input_.outsideDynamicGreenParams());
            K_d_ = Factory<PCMSolver, solverData, IGreensFunction *, IGreensFunction *>::TheFactory().create(modelType, input_.solverParams(), gf_i, gf_o_dyn);
            K_d_->buildSystemMatrix(*cavity_);
            hasDynamic_ = true;
        }
    }

    void Meddle::printInfo() const
    {
        std::ostringstream out_stream;
        out_stream << "\n" << std::endl;
        out_stream << "~~~~~~~~~~ PCMSolver ~~~~~~~~~~" << std::endl;
        out_stream << "Using CODATA " << input_.CODATAyear() << " set of constants." << std::endl;
        out_stream << "Input parsing done " << input_.providedBy() << std::endl;
        out_stream << "========== Cavity " << std::endl;
        out_stream << *cavity_ << std::endl;
        out_stream << "========== Solver " << std::endl;
        out_stream << "============= Static " << std::endl;
        out_stream << *K_0_ << std::endl;
        if (hasDynamic_) {
            out_stream << "============= Static " << std::endl;
            out_stream << *K_d_ << std::endl;
        }
        out_stream << "============ Medium " << std::endl;
        if (input_.fromSolvent()) {
            out_stream << "Medium initialized from solvent built-in data." << std::endl;
            Solvent solvent = input_.solvent();
            out_stream << solvent << std::endl;
        }
        out_stream << K_0_->printGreensFunctions() << std::endl;
        printer(out_stream);
    }

    void initMolecule(const Input & inp, const PointGroupSetter & set_group,
            int nuclei, const Eigen::VectorXd & charges, const Eigen::Matrix3Xd & centers,
            Molecule & molecule)
    {
        // 3. list of atoms and list of spheres
        bool scaling = inp.scaling();
        std::string set = inp.radiiSet();
        double factor = angstromToBohr(inp.CODATAyear());
        std::vector<Atom> radiiSet, atoms;
        if ( set == "UFF" ) {
            radiiSet = Atom::initUFF();
        } else {
            radiiSet = Atom::initBondi();
        }
        std::vector<Sphere> spheres;
        for (int i = 0; i < charges.size(); ++i) {
            int index = int(charges(i)) - 1;
            atoms.push_back(radiiSet[index]);
            double radius = radiiSet[index].atomRadius() * factor;
            if (scaling) {
                radius *= radiiSet[index].atomRadiusScaling();
            }
            spheres.push_back(Sphere(centers.col(i), radius));
        }
        // 4. masses
        Eigen::VectorXd masses = Eigen::VectorXd::Zero(nuclei);
        for (int i = 0; i < masses.size(); ++i) {
            masses(i) = atoms[i].atomMass();
        }
        // Based on the creation mode (Implicit or Atoms)
        // the spheres list might need postprocessing
        if (inp.mode() == "ATOMS") {
            initSpheresAtoms(inp, centers, spheres);
        }
        // 5. molecular point group
        int nr_gen;
        int gen1, gen2, gen3;
        set_group(&nr_gen, &gen1, &gen2, &gen3);
        Symmetry pg = buildGroup(nr_gen, gen1, gen2, gen3);

        // OK, now get molecule
        molecule = Molecule(nuclei, charges, masses, centers, atoms, spheres, pg);
    }

    void initSpheresAtoms(const Input & inp, const Eigen::Matrix3Xd & sphereCenter_,
            std::vector<Sphere> & spheres_)
    {
        // Loop over the atomsInput array to get which atoms will have a user-given radius
        for (size_t i = 0; i < inp.atoms().size(); ++i) {
            size_t index = inp.atoms(i) - 1; // -1 to go from human readable to machine readable
            // Put the new Sphere in place of the implicit-generated one
            spheres_[index] = Sphere(sphereCenter_.col(index), inp.radii(i));
        }
    }

} /* end namespace pcm */