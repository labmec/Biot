#ifndef SOURCES_H
#define SOURCES_H

#include <string>
#include <iostream>
#include "pzlog.h"
#include <Elasticity/TPZElasticity2D.h>
#include <Elasticity/TPZHybridElasticity2D.h>
#include <TPZLinearAnalysis.h>
#include "TPZSSpStructMatrix.h"
#include "pzstepsolver.h"
#include "TPZMultiphysicsCompMesh.h"
#include "TPZApproxCreator.h"
#include "TPZH1ApproxCreator.h"
#include "TPZSimpleTimer.h"
#include "pzbuildmultiphysicsmesh.h"
#include "TPZVTKGenerator.h"
#include "TPZStructMatrixOMPorTBB.h"
#include "pzskylstrmatrix.h"
#include "TPZVTKGeoMesh.h"
#include "TPZGmshReader.h"
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

TPZGeoMesh* GenerateGeoMesh(std::string filename, json inputFile, std::string meshName = "GeoMesh");

TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh, json inputFile, int HybridType, std::set<int> &matIDpostProcess);

void SetAnalysis(TPZLinearAnalysis* an, TPZCompMesh* cmesh);

void GetCompEls(TPZGeoMesh* gmesh, TPZCompMesh *cmeshH1, TPZCompMesh *cmeshHyb, TPZVec<TPZCompEl*> &celVecH1, TPZVec<TPZCompEl*> &celVecHyb, std::set<int> matId);

TPZVec<REAL> ComputeError(TPZVec<TPZCompEl*> &celVecH1, TPZVec<TPZCompEl*> &celVecHyb, TPZFMatrix<STATE> &elSolMat);

TPZVec<REAL> CalcElementError(TPZCompEl* celH1, TPZCompEl* celHyb);

TPZVec<REAL> CalcEnergy(TPZCompEl* celH1, TPZCompEl* celHyb);

void PrintGeoMesh(TPZGeoMesh *gmesh);

void PrintCompMesh(TPZCompMesh *cmesh);

void gravityLoad(const TPZVec<REAL> &loc, TPZVec<REAL> &result);

void inSituStress(const TPZVec<REAL> &loc, TPZVec<REAL> &result);


#endif