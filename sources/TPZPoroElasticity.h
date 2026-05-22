#ifndef TPZPOROELASTICITY_H
#define TPZPOROELASTICITY_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <math.h>
#include "pzcmesh.h"
#include "TPZElementMatrixT.h"
#include "pzmanvector.h"
#include "TPZGeoMeshTools.h"
#include "TPZCompMeshTools.h"
#include "TPZRefPattern.h"
#include "TPZGenGrid2D.h"
#include "TPZVTKGeoMesh.h"
#include "pzvec.h"
#include "TPZVTKGenerator.h"
#include <fstream>
#include "DarcyFlow/TPZDarcyFlow.h"
#include "DarcyFlow/TPZMixedDarcyFlow.h"
#include "TPZSkylineNSymStructMatrix.h"
#include "TPZNullMaterialCS.h"
#include "TPZNullMaterial.h"
#include "TPZAnalysis.h"
#include "pzstepsolver.h"
#include "TPZLinearAnalysis.h"
#include "TPZSSpStructMatrix.h"
#include "TPZGmshReader.h"
#include <set>
#include "TPZAnalyticSolution.h"
#include "TPZMultiphysicsCompMesh.h"
#include "TPZHDivApproxCreator.h"
#include "TPZLagrangeMultiplierCS.h"
#include "pzintel.h"

/**
 * @brief This class implements a approximation for the poro-elasticity problem based on Biot's consolidation theory.
 * Deformation of a porous (elastic) matrix together with the flow of the fluid.
 */

class TPZPoroElasApproxCreator{

public:
    /**
     * @brief Default constructor
     */
    TPZPoroElasApproxCreator();

    /**
	 * @brief Class constructor
	 * @param [in] id material id
	 * @param [in] dim problem dimension
	 */
    TPZPoroElasApproxCreator(int id, int dim);


    TPZPoroElasApproxCreator(const TPZPoroElasApproxCreator &copy);


    int Dimension() const { return this->fDim; }


protected:
    /**
     * @brief Problem dimension
     */
    int fDim;

};

#endif 
