#include <string>
#include <iostream>
#include "pzlog.h"
#include <DarcyFlow/TPZMixedDarcyFlow.h>
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
#include <json.hpp>
using json = nlohmann::json;

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
TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh, json inputFile);
TPZCompMesh *CreateCMesh(TPZGeoMesh* gmesh, json inputFile); // H1 Approx Creator
TPZCompMesh *CreateMPCompMesh(TPZGeoMesh* gmesh, json inputFile);
void SetAnalysis(TPZLinearAnalysis* an, TPZCompMesh* cmesh);
void PrintGeoMesh(TPZGeoMesh *gmesh);
void PrintCompMesh(TPZCompMesh *cmesh);
void gravityLoad(const TPZVec<REAL> &loc, TPZVec<REAL> &result);
void inSituStress(const TPZVec<REAL> &loc, TPZVec<REAL> &result);

// void Solve(TPZLinearAnalysis an);


int main() {

#ifdef PZ_LOG
    TPZLogger::InitializePZLOG();
#endif

    std::ifstream filejson("/home/marina/programming/Biot-Research/Biot/Inputs/Test2.json");

    json fInputFile = json::parse(filejson,nullptr,true,true,true); 

    //---------------------------- Reading input file ---------------------------------------
    std::string fMeshName = fInputFile["MeshName"];
    std::string fMeshDirectory = fInputFile["MeshDirectory"];
    int fHybridType = fInputFile["Simulation"]["ApproximationType"];

    //---------------------------- Geometric Mesh -------------------------------------------
    TPZGeoMesh *gmesh = GenerateGeoMesh(fMeshDirectory, fInputFile);
    PrintGeoMesh(gmesh);

    //---------------------------- Computational Mesh ---------------------------------------
    TPZCompMesh *cmesh;
    if(fHybridType) cmesh = CreateMPCompMesh(gmesh, fInputFile);
    else cmesh = CreateCMesh(gmesh, fInputFile);
    PrintCompMesh(cmesh);
    
    //--------------------------------- Analysis  --------------------------------------------
    TPZLinearAnalysis *Analisys = new TPZLinearAnalysis(cmesh);

    SetAnalysis(Analisys, cmesh);
    PrintCompMesh(cmesh);
    {
        const std::string plotfile = fMeshName;
        constexpr int vtkRes{0};
        TPZManVector<std::string, 3> fields = {"Displacement", "Pressure", "SigmaX", "SigmaY", "TauXY"};
        auto vtk = TPZVTKGenerator(cmesh, fields, plotfile, vtkRes);
        vtk.Do();
    }

    return 0;
}


TPZGeoMesh* GenerateGeoMesh(std::string filename, json inputFile, std::string meshName) {
    
    TPZGmshReader GeometryFine;
    TPZGeoMesh *gmesh;
    REAL l = 1.0;
    GeometryFine.SetCharacteristiclength(l);


    TPZManVector<std::map<std::string,int>,4> dim_name_and_physical_tag(4);

    for(auto& domain : inputFile["DomainData"]){
        if(domain.find("Name") == domain.end()) DebugStop(); // check if the information exists
        dim_name_and_physical_tag[2][domain["Name"]] = domain["matId"];
    }

    for(auto& bc : inputFile["BCs"]){
        if(bc.find("Name") == bc.end()) DebugStop(); 
        dim_name_and_physical_tag[bc["Dim"]][bc["Name"]] = bc["matId"];
    }

    //dim_name_and_physical_tag[1]["Fault"] = EFault;
    //dim_name_and_physical_tag[0]["FracEnd"] = EFracEnds;

    GeometryFine.SetDimNamePhysical(dim_name_and_physical_tag);
    gmesh = GeometryFine.GeometricGmshMesh(filename,nullptr,false);
    gmesh->SetName(meshName);

    return gmesh;
}

TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh, json inputFile){

    TPZCompMesh *cmesh =  new TPZCompMesh(gmesh);
    cmesh->SetName("CompMesh");

    int nLayers = inputFile["nLayers"];
    std::set<int> layerIds;
    std::map<int,REAL> Elas; // matId --> Elas
    std::map<int,REAL> nu; // matId --> nu
    std::map<int,REAL> rho; // matId --> rho

    std::set<int> bcIds;
    std::map<int,int> bcTypes; // matId --> Type
    std::map<int,TPZManVector<double, 2>> bcValues; // matId --> Value
    TPZManVector<double, 2>  bcValueVec = {0.0, 0.0};

    for(auto& layer : inputFile["DomainData"]){
        if(layer.find("Name") == layer.end()) DebugStop(); // check if the information exists
        int layId = layer["matId"];
        layerIds.insert(layId);
        Elas[layer["matId"]] = layer["Young"];
        nu[layer["matId"]] = layer["Poisson"];
        rho[layer["matId"]] = layer["Density"];
    }

    for(auto& bc : inputFile["BCs"]){
        if(bc.find("Name") == bc.end()) DebugStop(); // check if the information exists
        int bcId = bc["matId"];
        bcIds.insert(bcId);
        bcTypes[bc["matId"]] = bc["Type"];
        bcValueVec[0] = bc["Value"][0];
        bcValueVec[1] = bc["Value"][1];
        bcValues[bc["matId"]] = bcValueVec;
    }

    // State Stress
    STATE fxf = 0; 
    STATE fyf = 0;
    STATE fxr = 0;
    STATE fyr = 0;

    TPZFMatrix<STATE> pre_sigma(3,3,0.0);
    STATE pp = inputFile["InSitu_Stress"]["Pore-Pressure"];
    pre_sigma(0,0) = inputFile["InSitu_Stress"]["Sigma_H"]; 
    pre_sigma(1,1) = inputFile["InSitu_Stress"]["Sigma_v"];
    pre_sigma(0,1) = 0; //! VERIFY
    pre_sigma(2,2) = 0; //! VERIFY

    int planestrain = 0; 


    // Add materials
    //TPZElasticity2D *matElas = new TPZElasticity2D(EMatId, Ef, nuf, fxf, fyf, planestrain);
    TPZElasticity2D *matLayer = nullptr;
    for(int layId : layerIds){
        matLayer = new TPZElasticity2D(layId, Elas[layId], nu[layId], fxf, fyf, planestrain);
        matLayer->SetPreStress(pre_sigma(0,0), pre_sigma(1,1), pre_sigma(0,1), pre_sigma(2,2));
        //matLayer->SetForcingFunction(gravityLoad,1);
        //matLayer->SetForcingFunction(inSituStress,1);
        cmesh->InsertMaterialObject(matLayer);
    }

    //cmesh->InsertMaterialObject(matElas);
    //cmesh->InsertMaterialObject(matFault);


    // Add boundary conditions
    TPZFMatrix<STATE> val1(2,2,0.0);
    TPZVec<STATE> val2(2,0.0);
    int DirType = 0;
    int NeuType = 1;

    for(int bcId : bcIds){
        val2[0] = bcValues[bcId][0];
        val2[1] = bcValues[bcId][1];
        TPZBndCond *bcDomain = matLayer->CreateBC(matLayer, bcId, bcTypes[bcId], val1, val2);
        cmesh->InsertMaterialObject(bcDomain);
    }

    cmesh->AutoBuild();

    cmesh->CleanUpUnconnectedNodes();

    cmesh->ApproxSpace().SetAllCreateFunctionsContinuous();

    cmesh->ExpandSolution();

    return cmesh;
}

TPZCompMesh *CreateCMesh(TPZGeoMesh* gmesh, json inputFile){

    int nLayers = inputFile["nLayers"];
    std::set<int> layerIds;
    std::map<int,REAL> Elas; // matId --> Elas
    std::map<int,REAL> nu; // matId --> nu
    std::map<int,REAL> rho; // matId --> rho
    std::map<int,TPZManVector<double, 3>> preStress; 
    TPZManVector<double, 3>  preStressVec = {0.0, 0.0, 0.0};

    std::set<int> bcIds;
    std::map<int,int> bcTypes; // matId --> Type
    std::map<int,TPZManVector<double, 2>> normal; // matId --> unit normal to the surface
    std::map<int,TPZFMatrix<STATE>> bcValues1; // matId --> Value1
    std::map<int,TPZManVector<STATE, 2>> bcValues2; // matId --> Value2
    TPZFMatrix<STATE> bcValueMat(2,2,0.0);
    TPZManVector<STATE, 2>  bcValueVec = {0.0, 0.0};

    for(auto& layer : inputFile["DomainData"]){
        if(layer.find("Name") == layer.end()) DebugStop(); // check if the information exists
        int layId = layer["matId"];
        layerIds.insert(layId);
        Elas[layer["matId"]] = layer["Young"];
        nu[layer["matId"]] = layer["Poisson"];
        preStressVec[0] = layer["PreStress"][0];
        preStressVec[1] = layer["PreStress"][1];
        preStressVec[2] = layer["PreStress"][2];
        preStress[layer["matId"]] = preStressVec;
    }

    for(auto& bc : inputFile["BCs"]){
        if(bc.find("Name") == bc.end()) DebugStop();
        int bcId = bc["matId"];
        bcIds.insert(bcId);
        bcTypes[bc["matId"]] = bc["Type"];
        bcValueVec[0] = bc["Value2"][0];
        bcValueVec[1] = bc["Value2"][1];
        bcValues2[bc["matId"]] = bcValueVec;
        // if(bc.find("normal") != bc.end()){
        //     normal[bc["matId"]] = bc["normal"];
        // }
        if(bc.find("Value1") != bc.end()){
            bcValueMat(0,0) = bc["Value1"][0][0];
            bcValueMat(0,1) = bc["Value1"][0][1];
            bcValueMat(1,0) = bc["Value1"][1][0];
            bcValueMat(1,1) = bc["Value1"][1][1];
            bcValues1[bc["matId"]] = bcValueMat;
        }
    }


    // Create and Set-Up Approximation Spaces
    TPZH1ApproxCreator approxCreator(gmesh);
    approxCreator.ProbType() = ProblemType::EElastic;
    approxCreator.HybridType() = HybridizationType::ENone;
    approxCreator.SetShouldCondense(false);
    approxCreator.SetDefaultOrder(1);
    approxCreator.SetExtraInternalOrder(1);


    // Body Forces
    STATE fxf = 0; 
    STATE fyf = 0;
    STATE fxr = 0;
    STATE fyr = 0;

    // In-Situ State Stress
    TPZFMatrix<STATE> farField(3,3,0.0);
    STATE pp = inputFile["InSitu_Stress"]["Pore-Pressure"];
    farField(0,0) = inputFile["InSitu_Stress"]["Sigma_H"]; 
    farField(1,1) = inputFile["InSitu_Stress"]["Sigma_v"];
    farField(0,1) = 0; 
    farField(2,2) = 0; 

    int planestrain = 0; 
    int planestress = 1;


    // Add materials
    //TPZElasticity2D *matElas = new TPZElasticity2D(EMatId, Ef, nuf, fxf, fyf, planestrain);
    TPZElasticity2D *matLayer = nullptr;
    for(int layId : layerIds){
        matLayer = new TPZElasticity2D(layId, Elas[layId], nu[layId], fxf, fyf, planestress);
        matLayer->SetPreStress(preStress[layId][0], preStress[layId][1], preStress[layId][2], 0.0);
        //matLayer->SetForcingFunction(gravityLoad,1);
        approxCreator.InsertMaterialObject(matLayer);
    }


    // Add boundary conditions
    TPZFMatrix<STATE> val1(2,2,0.0);
    TPZVec<STATE> val2(2,0.0);
    int DirType = 0;
    int NeuType = 1;

    for(int bcId : bcIds){
        if(bcValues1.find(bcId) != bcValues1.end()) val1 = bcValues1[bcId];
        if(bcValues2.find(bcId) != bcValues2.end()) val2 = bcValues2[bcId];
        TPZBndCond *bcDomain = matLayer->CreateBC(matLayer, bcId, bcTypes[bcId], val1, val2);
        approxCreator.InsertMaterialObject(bcDomain);
    }

    TPZCompMesh *cmesh;
    //cmesh->SetName("CompMesh");
    if(approxCreator.HybridType() == HybridizationType::ENone)
        cmesh = approxCreator.CreateClassicH1ApproximationSpace();
    else
        cmesh = approxCreator.CreateApproximationSpace();
    cmesh->SetName("CompMesh");

    return cmesh;
}

TPZCompMesh *CreateMPCompMesh(TPZGeoMesh* gmesh, json inputFile){

    // TPZApproxCreator approxCreator(gmesh);
    // TPZH1ApproxCreator *h1approxCreator = dynamic_cast<TPZH1ApproxCreator*>(approxCreator); 
    //downcasting—converting a base class pointer to a derived class pointer
    // Input data
    STATE grav = 9.81;
    STATE rho = 2.8e3;
    STATE Ef = 50e9;
    STATE nuf = 0.25;
    STATE fxf = 0; 
    STATE fyf = 0;
    TPZFMatrix<STATE> bcValue(4,2,0.0);
    bcValue(0,0) = 0; // top x
    bcValue(0,1) = -56e6; // top y
    bcValue(1,0) = 0; // bottom x
    bcValue(1,1) = 0; // bottom y
    bcValue(2,0) = 0; // left x
    bcValue(2,1) = 0; // left y
    bcValue(3,0) = 0; // right x
    bcValue(3,1) = 0; // right y
    STATE Er = 50e9;
    STATE nur = 0.25;
    STATE fxr = 0;
    STATE fyr = 0;
    int planestrain = 0; // Plain strain state

    // Create and Set-Up Approximation Spaces
    TPZH1ApproxCreator approxCreator(gmesh);
    approxCreator.ProbType() = ProblemType::EElastic;
    approxCreator.HybridType() = HybridizationType::EStandard;
    approxCreator.SetShouldCondense(false);
    approxCreator.SetDefaultOrder(1);
    approxCreator.SetExtraInternalOrder(0);

    // Add Materials
    //TPZElasticity2D *matElas = new TPZElasticity2D(EFarFieldId, Ef, nuf, fxf, fyf, planestrain);
    //TPZElasticity2D *matReservoir = new TPZElasticity2D(EReservoirId,  Er, nur, fxr, fyr, planestrain);
    TPZHybridElasticity2D *matElas = new TPZHybridElasticity2D(EMatId,  Er, nur, fxr, fyr, planestrain);

    //matElas->SetForcingFunction(gravityLoad,1);
    //matReservoir->SetForcingFunction(gravityLoad,1);
    TPZFMatrix<STATE> pre_sigma(3,3,0.0);
    pre_sigma(0,0) = 20e6; //pore pressure
    pre_sigma(1,1) = 20e6;
    pre_sigma(2,2) = 20e6;

    //matReservoir->SetPreStress(pre_sigma(0,0), pre_sigma(1,1), pre_sigma(0,1), pre_sigma(2,2));

    approxCreator.InsertMaterialObject(matElas);


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
    TPZBndCond *left = matElas->CreateBC(matElas, EbcLeft, DirType, val1, val2);
    val2[0] = bcValue(3,0);
    val2[1] = bcValue(3,1);
    TPZBndCond *right = matElas->CreateBC(matElas, EbcRight, DirType, val1, val2);
    approxCreator.InsertMaterialObject(bottom);
    approxCreator.InsertMaterialObject(top);
    approxCreator.InsertMaterialObject(left);
    approxCreator.InsertMaterialObject(right);

    TPZCompMesh *cmesh;
    if(approxCreator.HybridType() == HybridizationType::ENone)
        cmesh = approxCreator.CreateClassicH1ApproximationSpace();
    else
        cmesh = approxCreator.CreateApproximationSpace();

    return cmesh;
}


void SetAnalysis(TPZLinearAnalysis* an, TPZCompMesh* cmesh){

    #ifdef PZ_USING_MKL
    TPZSSpStructMatrix<STATE> matMixed(cmesh);
    #else
    TPZFStructMatrix<STATE> matMixed(cmesh);
    #endif
    matMixed.SetNumThreads(0);
    an->SetStructuralMatrix(matMixed);

    TPZStepSolver<STATE> step;
    step.SetDirect(ELU); // Cholesky?
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

void inSituStress(const TPZVec<REAL> &loc, TPZVec<REAL> &result){
    std::ifstream filejson("/home/marina/programming/Biot-Research/Biot/Inputs/Example5.json");

    json inputFile = json::parse(filejson,nullptr,true,true,true); 

    STATE sigV = inputFile["InSitu_Stress"]["Sigma_v"];
    STATE sigH = inputFile["InSitu_Stress"]["Sigma_H"];
    STATE sigh = inputFile["InSitu_Stress"]["Sigma_h"];
    STATE Pp = inputFile["InSitu_Stress"]["Pore-Pressure"];
    result[0] = sigH*loc[1];
    result[1] = sigV*loc[1];
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