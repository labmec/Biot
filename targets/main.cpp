#include "sources.h"

void PrimalElasticity2D(int refLevel, std::ofstream &outfile) {

// #ifdef PZ_LOG
//     TPZLogger::InitializePZLOG();
// #endif

    std::ifstream filejson("/home/marina/programming/Biot-Research/Biot/Inputs/Ex5.json");
    //std::ifstream filejson3d("/home/marina/programming/Biot-Research/Biot/Inputs/Ex5-3d.json");

    json fInputFile = json::parse(filejson,nullptr,true,true,true); 

    //---------------------------- Reading input file ---------------------------------------
    std::string fMeshName = fInputFile["MeshName"];
    std::string fSigTSigN = "LagrangePostProc";
    std::string fMeshDirectory = fInputFile["MeshDirectory"];
    int fHybridType = fInputFile["Simulation"]["ApproximationType"];
    int fNSteps = fInputFile["Simulation"]["Steps"];
    int refUniform = refLevel;
    int refDir = refLevel;
    std::set<int> matIDBCs;
    std::set<int> matIDvolEls;
    std::set<int> matIDpostProcess;

    //---------------------------- Geometric Mesh -------------------------------------------
    TPZGeoMesh *gmesh = GenerateGeoMesh(fMeshDirectory, fInputFile, matIDvolEls, matIDBCs);
    {
        TPZCheckGeom check(gmesh);
        check.UniformRefine(refUniform);
    }
    PrintGeoMesh(gmesh);

    // TPZGeoMesh *gmesh3D = ExtrudeMesh(gmesh, 20.0, 5, matIDBCs);

    // PrintGeoMesh(gmesh3D);

    //---------------------------- Computational Mesh ---------------------------------------
    //TPZCompMesh *cmesh = CreateCompMesh(gmesh, fInputFile, 0, matIDpostProcess);
    TPZCompMesh *cmesh_mult = CreateCompMesh(gmesh, fInputFile, 1, matIDpostProcess);
    
    //fInputFile = json::parse(filejson3d,nullptr,true,true,true); 
    //TPZCompMesh *cmesh_mult = CreateCompMesh3D(gmesh3D, fInputFile, 1, matIDpostProcess);
    
    //--------------------------------- Analysis  --------------------------------------------
    //TPZLinearAnalysis *AnH1 = new TPZLinearAnalysis(cmesh);
    TPZLinearAnalysis *AnHyb = new TPZLinearAnalysis(cmesh_mult, RenumType::ENone);

    //SetAnalysis(AnH1, cmesh);
    SetAnalysis(AnHyb, cmesh_mult);

    //--------------------------------- Post-Process / Error Estimation  --------------------------------------------
    std::map<REAL, TPZVec<REAL>> postProcSol;

    //PrintCompMesh(cmesh);
    PrintCompMesh(cmesh_mult);
    PrintGeoMesh(gmesh);
    

    // int64_t nels = gmesh->NElements();
    // int64_t nelsH1 = cmesh->NElements();
    // TPZVec<TPZCompEl*> celH1(nels, nullptr); 
    // TPZVec<TPZCompEl*> celHyb(nels, nullptr);
    // TPZSolutionMatrix &solMat = cmesh->ElementSolution();
    // solMat.Redim(nelsH1,1);
    // GetCompEls(gmesh, cmesh, cmesh_mult, celH1, celHyb, matIDvolels);
    // TPZVec<STATE> error = ComputeError(celH1, celHyb, solMat);
    // outfile << cmesh_mult->NEquations() << "\t" << error[0] << std::endl;
    // TPZVec<STATE> energy = ComputeError(celH1, celHyb, solMat);
    // outfile << cmesh_mult->NEquations() << "\t" << energy[0] << "\t" << energy[1] << std::endl;

    // {
    //     const std::string plotfile = fMeshName + "_H1";
    //     constexpr int vtkRes{0};
    //     TPZManVector<std::string, 3> fields = {"Displacement", "Pressure", "SigmaX", "SigmaY", "TauXY", "EstimatedError"};
    //     auto vtk = TPZVTKGenerator(cmesh, fields, plotfile, vtkRes);
    //     vtk.Do();
    // }

    std::set<int> line = {203};

    for(int step = 0; step < fNSteps; step++){
        {
            const std::string plotfile = fMeshName + "_Hybrid";
            constexpr int vtkRes{0};
            TPZManVector<std::string, 10> fields = {"Displacement", "PorePressure", "SigmaX", "SigmaY", "StressEffecY", "StressEffecX"};
            auto vtk = TPZVTKGenerator(cmesh_mult, fields, plotfile, vtkRes);
            vtk.SetStep(step);
            vtk.Do();
        }
        {
            const std::string plotfile = fSigTSigN;
            constexpr int vtkRes{0};
            TPZManVector<std::string, 10> fields = {"SigN", "SigT", "SigT_SigN"};
            auto vtk = TPZVTKGenerator(cmesh_mult, matIDpostProcess, fields, plotfile, vtkRes);
            vtk.SetStep(step);
            vtk.Do();
        }
        LinePlot(gmesh, cmesh_mult, matIDpostProcess, matIDvolEls, postProcSol);
        outfile << "{";
        for(auto& sol : postProcSol){
            outfile << "{" << sol.first << "," << sol.second << "},\n";
        }
        outfile << "}";
        ApplyPreStress(cmesh_mult, fInputFile, step+1);
        SetAnalysis(AnHyb, cmesh_mult);
    }

    {
        const std::string plotfile = fMeshName + "_Hybrid";
        constexpr int vtkRes{0};
        TPZManVector<std::string, 10> fields = {"Displacement", "PorePressure", "SigmaX", "SigmaY", "StressEffecY", "StressEffecX"};
        auto vtk = TPZVTKGenerator(cmesh_mult, fields, plotfile, vtkRes);
        vtk.SetStep(fNSteps);
        vtk.Do();
    }
    {
        const std::string plotfile = fSigTSigN;
        constexpr int vtkRes{0};
        TPZManVector<std::string, 10> fields = {"SigN", "SigT", "SigT_SigN"};
        auto vtk = TPZVTKGenerator(cmesh_mult, matIDpostProcess, fields, plotfile, vtkRes);
        vtk.SetStep(fNSteps);
        vtk.Do();
    }

    PrintCompMesh(cmesh_mult);
    PrintGeoMesh(gmesh);

}


int main() {

#ifdef PZ_LOG
    TPZLogger::InitializePZLOG();
#endif

    gRefDBase.InitializeRefPatterns(2);
    
    //std::ofstream fileEnergy("energy.txt", std::ios::app);
    std::ofstream fileEnergy("linePostProc.txt", std::ios::out);
    fileEnergy << "\nnEq " << " Error" << std::endl;
    //fileEnergy << "\nnEq " << " EnergyH1" << " EnergyHyb" << std::endl;
    int refMax = 0;
    for(int ref = 0; ref <= refMax; ref++)
        PrimalElasticity2D(ref, fileEnergy);
    return 0;
}