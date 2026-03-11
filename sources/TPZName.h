#ifndef TPZNAME_H
#define TPZNAME_H

#include <string>
#include <iostream>
#include <TPZGenGrid3D.h>
#include <TPZGenGrid2D.h>
#include <DarcyFlow/TPZDarcyFlow.h>
#include <DarcyFlow/TPZMixedDarcyFlow.h>
#include <TPZLinearAnalysis.h>
#include "TPZSSpStructMatrix.h"
#include "pzstepsolver.h"
#include "TPZMultiphysicsCompMesh.h"
#include "TPZSimpleTimer.h"
#include "pzbuildmultiphysicsmesh.h"
#include "TPZVTKGenerator.h"
#include "TPZStructMatrixOMPorTBB.h"
#include "pzskylstrmatrix.h"
#include "TPZVTKGeoMesh.h"

void CreateMaterial();

#endif 
