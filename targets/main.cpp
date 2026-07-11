#include "sources.h"

void PrimalElasticity2D(int refLevel, std::ofstream &outfile) {

#ifdef PZ_LOG
    TPZLogger::InitializePZLOG();
#endif

    std::ifstream filejson("/home/marina/programming/Biot-Research/Biot/Inputs/Test1.json");

    json fInputFile = json::parse(filejson,nullptr,true,true,true); 

    //---------------------------- Reading input file ---------------------------------------
    std::string fMeshName = fInputFile["MeshName"];
    std::string fSigTSigN = "LagrangePostProc";
    std::string fMeshDirectory = fInputFile["MeshDirectory"];
    int fHybridType = fInputFile["Simulation"]["ApproximationType"];
    int refUniform = refLevel;
    int refDir = refLevel;

    //---------------------------- Geometric Mesh -------------------------------------------
    TPZGeoMesh *gmesh = GenerateGeoMesh(fMeshDirectory, fInputFile);
    {
        TPZCheckGeom check(gmesh);
        check.UniformRefine(refUniform);
    }
    PrintGeoMesh(gmesh);

    //---------------------------- Computational Mesh ---------------------------------------
    std::set<int> matIDpostProcess;
    TPZCompMesh *cmesh = CreateCompMesh(gmesh, fInputFile, 0, matIDpostProcess);
    TPZCompMesh *cmesh_mult = CreateCompMesh(gmesh, fInputFile, 1, matIDpostProcess);

    // PrintCompMesh(cmesh);
    // PrintCompMesh(cmesh_mult);
    
    //--------------------------------- Analysis  --------------------------------------------
    TPZLinearAnalysis *AnH1 = new TPZLinearAnalysis(cmesh);
    TPZLinearAnalysis *AnHyb = new TPZLinearAnalysis(cmesh_mult, RenumType::ENone);

    SetAnalysis(AnH1, cmesh);
    SetAnalysis(AnHyb, cmesh_mult);

    //--------------------------------- Post-Process / Error Estimation  --------------------------------------------
    PrintCompMesh(cmesh);
    PrintCompMesh(cmesh_mult);
    PrintGeoMesh(gmesh);
    std::set<int> matIDvolels;
    matIDvolels.insert(1); //! Hard coded

    int64_t nels = gmesh->NElements();
    int64_t nelsH1 = cmesh->NElements();
    TPZVec<TPZCompEl*> celH1(nels, nullptr); 
    TPZVec<TPZCompEl*> celHyb(nels, nullptr);
    TPZSolutionMatrix &solMat = cmesh->ElementSolution();
    solMat.Redim(nelsH1,1);
    GetCompEls(gmesh, cmesh, cmesh_mult, celH1, celHyb, matIDvolels);
    TPZVec<STATE> error = ComputeError(celH1, celHyb, solMat);
    outfile << cmesh_mult->NEquations() << "\t" << error[0] << std::endl;
    // TPZVec<STATE> energy = ComputeError(celH1, celHyb, solMat);
    // outfile << cmesh_mult->NEquations() << "\t" << energy[0] << "\t" << energy[1] << std::endl;

    {
        const std::string plotfile = fMeshName + "_H1";
        constexpr int vtkRes{0};
        TPZManVector<std::string, 3> fields = {"Displacement", "Pressure", "SigmaX", "SigmaY", "TauXY", "EstimatedError"};
        auto vtk = TPZVTKGenerator(cmesh, fields, plotfile, vtkRes);
        vtk.Do();
    }

    {
        const std::string plotfile = fMeshName + "_Hybrid";
        constexpr int vtkRes{0};
        TPZManVector<std::string, 3> fields = {"Displacement", "Pressure", "SigmaX", "SigmaY", "TauXY"};
        auto vtk = TPZVTKGenerator(cmesh_mult, fields, plotfile, vtkRes);
        vtk.Do();
    }

    {
        const std::string plotfile = fSigTSigN;
        constexpr int vtkRes{0};
        TPZManVector<std::string, 3> fields = {"SigN", "SigT", "SigT_SigN"};
        auto vtk = TPZVTKGenerator(cmesh_mult, matIDpostProcess, fields, plotfile, vtkRes);
        vtk.SetNThreads(0);
        vtk.Do();
    }
}


int main() {

#ifdef PZ_LOG
    TPZLogger::InitializePZLOG();
#endif

    gRefDBase.InitializeRefPatterns(2);
    
    std::ofstream fileEnergy("energy.txt", std::ios::app);
    fileEnergy << "\nnEq " << " EnergyH1" << " EnergyHyb" << std::endl;
    int refMax = 3;
    for(int ref = 0; ref <= refMax; ref++)
        PrimalElasticity2D(ref, fileEnergy);
    return 0;
}