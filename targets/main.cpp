#include <string>
#include <iostream>
#include <DarcyFlow/TPZMixedDarcyFlow.h>
#include <Elasticity/TPZElasticity2D.h>
#include <Elasticity/TPZHybridElasticity2D.h>
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
#include "TPZGmshReader.h"

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
    ELagrange = 10
};

TPZGeoMesh* GenerateGeoMesh(std::string filename, std::string meshName = "GeoMesh");
TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh);
void SetAnalysis(TPZLinearAnalysis* an, TPZCompMesh* cmesh);
void PrintGeoMesh(TPZGeoMesh *gmesh);
void PrintCompMesh(TPZCompMesh *cmesh);
void gravityLoad(const TPZVec<REAL> &loc, TPZVec<REAL> &result);

// TPZCompMesh* CreateCompMesh(TPZGeoMesh* gmesh);

// void Solve(TPZLinearAnalysis an);


int main() {

    std::string myMesh = "/home/marina/programming/Biot-Research/Biot/Meshes/simple-frac.msh";
    TPZGeoMesh *gmesh = GenerateGeoMesh(myMesh);
    PrintGeoMesh(gmesh);

    TPZCompMesh *cmesh = CreateCompMesh(gmesh);
    PrintCompMesh(cmesh);
    
    TPZLinearAnalysis *Analisys = new TPZLinearAnalysis(cmesh);

    SetAnalysis(Analisys, cmesh);
    {
        const std::string plotfile = "pure-stretch";
        constexpr int vtkRes{0};
        TPZManVector<std::string, 3> fields = {"Displacement", "Pressure", "SigmaX", "SigmaY", "TauXY"};
        auto vtk = TPZVTKGenerator(cmesh, fields, plotfile, vtkRes);
        vtk.Do();
    }

    return 0;
}


TPZGeoMesh* GenerateGeoMesh(std::string filename, std::string meshName) {
    
    TPZGmshReader GeometryFine;
    TPZGeoMesh *gmesh;
    REAL l = 1.0;
    GeometryFine.SetCharacteristiclength(l);

    TPZManVector<std::map<std::string,int>,4> dim_name_and_physical_tag(4);

    dim_name_and_physical_tag[2]["domain"] = EMatId;
    // dim_name_and_physical_tag[2]["far-field"] = EFarFieldId;
    // dim_name_and_physical_tag[2]["pre-stress"] = EReservoirId;
    dim_name_and_physical_tag[1]["Fault"] = EFault;
    dim_name_and_physical_tag[1]["left"] = EbcLeft;
    dim_name_and_physical_tag[1]["right"] = EbcRight;
    dim_name_and_physical_tag[1]["bottom"] = EbcBottom;
    dim_name_and_physical_tag[1]["top"] = EbcTop;
    // dim_name_and_physical_tag[1]["res-left"] = EbcReservoir;
    // dim_name_and_physical_tag[1]["res-right"] = EbcReservoir;
    // dim_name_and_physical_tag[1]["res-bottom"] = EbcReservoir;
    // dim_name_and_physical_tag[1]["res-top"] = EbcReservoir;

    GeometryFine.SetDimNamePhysical(dim_name_and_physical_tag);
    gmesh = GeometryFine.GeometricGmshMesh(filename,nullptr,false);
    gmesh->SetName(meshName);

    return gmesh;
}

TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh){

    TPZCompMesh *cmesh =  new TPZCompMesh(gmesh);
    cmesh->SetName("CompMesh");

    // Input data
    STATE grav = 9.81;
    STATE rho = 2.8e3;
    STATE Ef = 50e9;
    STATE nuf = 0.25;
    STATE fxf = 0; 
    STATE fyf = 0;
    TPZFMatrix<STATE> bcValue(4,2,0.0);
    bcValue(0,0) = 0; // top x
    bcValue(0,1) = 50e6; // top y
    bcValue(1,0) = 0; // bottom x
    bcValue(1,1) = 0; // bottom y
    bcValue(2,0) = 0; // left x
    bcValue(2,1) = 0; // left y
    bcValue(3,0) = 0; // right x
    bcValue(3,1) = 0; // right y
    STATE Er = 70e9;
    STATE nur = 0.25;
    STATE fxr = 0;
    STATE fyr = 0;


    int planestrain = 0;

    // Add materials
    //TPZElasticity2D *matElas = new TPZElasticity2D(EFarFieldId, Ef, nuf, fxf, fyf, planestrain);
    //TPZElasticity2D *matReservoir = new TPZElasticity2D(EReservoirId,  Er, nur, fxr, fyr, planestrain);
    TPZElasticity2D *matElas = new TPZElasticity2D(EMatId,  Er, nur, fxr, fyr, planestrain);
    TPZElasticity2D *matFault = new TPZElasticity2D(EFault,  Er, nur, fxr, fyr, planestrain);

    //matElas->SetForcingFunction(gravityLoad,1);
    //matReservoir->SetForcingFunction(gravityLoad,1);
    TPZFMatrix<STATE> pre_sigma(3,3,0.0);
    pre_sigma(0,0) = 20e6; //pore pressure
    pre_sigma(1,1) = 20e6;
    pre_sigma(2,2) = 20e6;

    //matReservoir->SetPreStress(pre_sigma(0,0), pre_sigma(1,1), pre_sigma(0,1), pre_sigma(2,2));

    cmesh->InsertMaterialObject(matElas);
    //cmesh->InsertMaterialObject(matFault);
    //cmesh->InsertMaterialObject(matReservoir);


    // Add boundary conditions
    TPZFMatrix<STATE> val1(2,2,0.0);
    TPZVec<STATE> val2(2,0.0);
    int DirType = 0;
    int NeuType = 1;
    
    val2[0] = bcValue(1,0);
    val2[1] = bcValue(1,1);
    TPZBndCond *bottom = matElas->CreateBC(matElas, EbcBottom, DirType, val1, val2);
    val2[0] = bcValue(0,0);
    val2[1] = bcValue(0,1);
    TPZBndCond *top = matElas->CreateBC(matElas, EbcTop, NeuType, val1, val2);
    val2[0] = bcValue(2,0);
    val2[1] = bcValue(2,1);
    TPZBndCond *left = matElas->CreateBC(matElas, EbcLeft, NeuType, val1, val2);
    val2[0] = bcValue(3,0);
    val2[1] = bcValue(3,1);
    TPZBndCond *right = matElas->CreateBC(matElas, EbcRight, NeuType, val1, val2);
    cmesh->InsertMaterialObject(bottom);
    cmesh->InsertMaterialObject(top);
    cmesh->InsertMaterialObject(left);
    cmesh->InsertMaterialObject(right);

    cmesh->AutoBuild();

    cmesh->CleanUpUnconnectedNodes();

    cmesh->ApproxSpace().SetAllCreateFunctionsContinuous();

    cmesh->ExpandSolution();

    return cmesh;
}


void SetAnalysis(TPZLinearAnalysis* an, TPZCompMesh* cmesh){

    #ifdef PZ_USING_MKL
    TPZSSpStructMatrix<STATE> matMixed(cmesh);
    #else
    TPZFStructMatrix<STATE> matMixed(cmesh);
    #endif
    matMixed.SetNumThreads(4);
    an->SetStructuralMatrix(matMixed);

    TPZStepSolver<STATE> step;
    step.SetDirect(ELDLt); // Cholesky?
    an->SetSolver(step);
    
    an->Assemble();
    an->Solve();
}


void gravityLoad(const TPZVec<REAL> &loc, TPZVec<REAL> &result){
    STATE grav = 9.81;
    STATE rho = 2.8e3; //! Ver
    result[0] = 0;
    result[1] = rho*grav*loc[1];
    result[2] = 0;
}


void PrintGeoMesh(TPZGeoMesh *gmesh){
    std::cout << "\nPrinting geo mesh in .txt and .vtk formats...\n";

    std::ofstream VTKGeoMeshFile(gmesh->Name() + ".vtk");
    std::ofstream TextGeoMeshFile(gmesh->Name() + ".txt");

    TPZVTKGeoMesh::PrintGMeshVTK(gmesh, VTKGeoMeshFile);
    gmesh->Print(TextGeoMeshFile);
}


void PrintCompMesh(TPZCompMesh *cmesh){
    std::cout << "\nPrinting comp mesh in .txt and .vtk formats...\n";

    std::ofstream VTKCompMeshFile(cmesh->Name() + ".vtk");
    std::ofstream TextCompMeshFile(cmesh->Name() + ".txt");

    TPZVTKGeoMesh::PrintCMeshVTK(cmesh, VTKCompMeshFile);
    cmesh->Print(TextCompMeshFile);
}