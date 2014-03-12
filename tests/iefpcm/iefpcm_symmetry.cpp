#include <iostream>

#include "Config.hpp"

// Disable obnoxious warnings from Eigen headers
#if defined (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall" 
#pragma GCC diagnostic ignored "-Weffc++" 
#pragma GCC diagnostic ignored "-Wextra"
#include <Eigen/Dense>
#pragma GCC diagnostic pop
#elif (__INTEL_COMPILER)
#pragma warning push
#pragma warning disable "-Wall"
#include <Eigen/Dense>
#pragma warning pop
#endif

#include <boost/filesystem.hpp>

#include "GePolCavity.hpp"
#include "Vacuum.hpp"
#include "UniformDielectric.hpp"
#include "IEFSolver.hpp"

// Disable obnoxious warnings from Google Test headers
#if defined (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall" 
#pragma GCC diagnostic ignored "-Weffc++" 
#pragma GCC diagnostic ignored "-Wextra"
#include "gtest/gtest.h"
#pragma GCC diagnostic pop
#endif

namespace fs = boost::filesystem;

TEST(IEFSolver, pointChargeGePolC1) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 0; // C1
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.c1");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int size = cavity.size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(size);
	for (int i = 0; i < size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum();
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolCs) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 1; // Cs
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.cs");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolC2) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 2; // C2
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.c2");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolCi) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 3; // Ci
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.ci");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolC2h) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 4; // C2h
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.c2h");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolD2) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 5; // D2
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.d2");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolC2v) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;      		
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 6; // C2v
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.c2v");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}

TEST(IEFSolver, pointChargeGePolD2h) 
{
	// Set up cavity
	Eigen::Vector3d N(0.0, 0.0, 0.0); 		
	std::vector<Sphere> spheres;
	Sphere sph1(N, 1.0);      		
	spheres.push_back(sph1);          		
	double area = 0.4;
	double probeRadius = 0.0;
	double minRadius = 100.0;
	int pgroup = 7; // D2h
	GePolCavity cavity(spheres, area, probeRadius, minRadius, pgroup);
	fs::rename("PEDRA.OUT", "PEDRA.OUT.d2h");
	
	double permittivity = 78.39;
	Vacuum * gfInside = new Vacuum(2); // Automatic directional derivative
	UniformDielectric * gfOutside = new UniformDielectric(2, permittivity);
	bool symm = true;
	IEFSolver solver(gfInside, gfOutside, symm);
	solver.buildSystemMatrix(cavity);

	double charge = 1.0;
	int irr_size = cavity.irreducible_size();
	Eigen::VectorXd fake_mep = Eigen::VectorXd::Zero(irr_size);
	// Calculate it only on the irreducible portion of the cavity
	// then replicate it according to the point group
	for (int i = 0; i < irr_size; ++i)
	{
		Eigen::Vector3d center = cavity.elementCenter(i);
		double distance = center.norm();
		fake_mep(i) = charge / distance; 
	}
	int nr_irrep = cavity.pointGroup().nrIrrep();
	// The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
	Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(irr_size);
	solver.compCharge(fake_mep, fake_asc);
	double totalASC = - charge * (permittivity - 1) / permittivity;
	double totalFakeASC = fake_asc.sum() * nr_irrep;
	std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
	EXPECT_NEAR(totalASC, totalFakeASC, 3e-3);
}
