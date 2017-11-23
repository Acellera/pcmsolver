/*
 * PCMSolver, an API for the Polarizable Continuum Model
 * Copyright (C) 2017 Roberto Di Remigio, Luca Frediani and collaborators.
 *
 * This file is part of PCMSolver.
 *
 * PCMSolver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PCMSolver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PCMSolver.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For information on the complete list of contributors to the
 * PCMSolver API, see: <http://pcmsolver.readthedocs.io/>
 */

#pragma once

#include <string>

#include "Config.hpp"

/*! @struct SolverData
 *  @brief Contains all data defined from user input in the solver section.
 */

namespace pcm {
struct SolverData {
  /*! The type of solver */
  std::string solverType;
  /*! The correction factor to be use in a CPCM calculation */
  double correction;
  /*! Triggers hermitivitization of the PCM matrix obtained by collocation */
  bool hermitivitize;
  /*! Whether the structure was initialized with user input or not */
  bool empty;

  SolverData() { empty = true; }
  SolverData(const std::string & type, double corr, bool symm = true)
      : solverType(type), correction(corr), hermitivitize(symm) {
    empty = false;
  }
};
} // namespace pcm
