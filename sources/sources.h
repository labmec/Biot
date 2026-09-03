#ifndef SOURCES_H
#define SOURCES_H

#include <string>
#include <iostream>
#include "pzlog.h"
#include <Elasticity/TPZElasticity2D.h>
#include <Elasticity/TPZElasticity3D.h>
#include <Elasticity/TPZHybridElasticity2D.h>
#include <Elasticity/TPZHybridElasticity3D.h>
#include <TPZLinearAnalysis.h>
#include "TPZSSpStructMatrix.h"
#include "pzstepsolver.h"
#include "TPZMultiphysicsCompMesh.h"
#include "TPZApproxCreator.h"
#include "TPZH1ApproxCreator.h"
#include "TPZNullMaterialCS.h"
#include "TPZNullMaterialSol.h"
#include "TPZSimpleTimer.h"
#include "pzbuildmultiphysicsmesh.h"
#include "TPZVTKGenerator.h"
#include "TPZStructMatrixOMPorTBB.h"
#include "pzskylstrmatrix.h"
#include "TPZVTKGeoMesh.h"
#include "TPZGmshReader.h"
#include "TPZExtendGridDimension.h"
#include "pzintel.h"
#include "pzcondensedcompel.h"
#include <json.hpp>
using json = nlohmann::json;

#include "TPZRefPatternDataBase.h"
#include "TPZRefPatternTools.h"


enum MatID{
    EMatId = 1, 
    EFarFieldId = 2,
    EReservoirId = 3,
    EbcLeft = 4,
    EbcRight = 5,
    EbcTop = 6,
    EbcBottom = 7,
    EbcReservoir = 8,
    EFault = 9,
    EFracEnds = 10,
    ELagrange = 500,
    ELayer = 100
};

TPZGeoMesh* GenerateGeoMesh(std::string filename, json inputFile, std::set<int> &matIDvolEls, std::set<int> &matIDBCs, std::string meshName = "GeoMesh");

TPZGeoMesh* ExtrudeMesh(TPZGeoMesh* finegmesh, REAL thickness, int numLayers, std::set<int> &matIdBCs, std::string meshName = "GeoMesh3D");

TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh, json inputFile, int HybridType, std::set<int> &matIDpostProcess);

TPZCompMesh *CreateCompMesh3D(TPZGeoMesh* gmesh, json inputFile, int HybridType, std::set<int> &matIDpostProcess);

void ApplyPreStress(TPZCompMesh* cmesh, json inputFile, int step);

void DuplicateConnectFracture(TPZGeoMesh *gmesh, TPZCompMesh *cmesh);

void SetAnalysis(TPZLinearAnalysis* an, TPZCompMesh* cmesh);

void LinePlot(TPZGeoMesh *gmesh, TPZCompMesh* cmesh, std::set<int> lineMatId, std::set<int> &matIDvolEls, std::map<REAL, TPZVec<REAL>> &results);

void GetCompEls(TPZGeoMesh* gmesh, TPZCompMesh *cmeshH1, TPZCompMesh *cmeshHyb, TPZVec<TPZCompEl*> &celVecH1, TPZVec<TPZCompEl*> &celVecHyb, std::set<int> matId);

TPZVec<REAL> ComputeError(TPZVec<TPZCompEl*> &celVecH1, TPZVec<TPZCompEl*> &celVecHyb, TPZFMatrix<STATE> &elSolMat);

TPZVec<REAL> CalcElementError(TPZCompEl* celH1, TPZCompEl* celHyb);

TPZVec<REAL> CalcElementEnergy(TPZCompEl* celH1, TPZCompEl* celHyb);

void PrintGeoMesh(TPZGeoMesh *gmesh);

void PrintCompMesh(TPZCompMesh *cmesh);

void gravityLoad(const TPZVec<REAL> &loc, TPZVec<REAL> &result);

void inSituStress(const TPZVec<REAL> &loc, TPZVec<REAL> &result);

std::function<void (const TPZVec<REAL> &loc, TPZVec<REAL> &rhsVal, TPZFMatrix<REAL> &matVal)> inSituStressBC(int bcId);


#endif