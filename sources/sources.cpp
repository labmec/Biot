#include "sources.h"
#include "TPZNullMaterial.h"

TPZGeoMesh* GenerateGeoMesh(std::string filename, json inputFile, std::string meshName) {
    
    TPZGmshReader GeometryFine;
    TPZGeoMesh *gmesh;
    REAL l = 1.0;
    GeometryFine.SetCharacteristiclength(l);


    TPZManVector<std::map<std::string,int>,4> dim_name_and_physical_tag(4);

    int dim = inputFile["Dimension"];

    for(auto& domain : inputFile["DomainData"]){
        if(domain.find("Name") == domain.end()) DebugStop(); // check if the information exists
        dim_name_and_physical_tag[dim][domain["Name"]] = domain["matId"];
    }

    for(auto& fault : inputFile["FaultData"]){
        if(fault.find("Name") == fault.end()) DebugStop();
        dim_name_and_physical_tag[dim-1][fault["Name"]] = fault["matId"];
    }

    for(auto& bc : inputFile["BCs"]){
        if(bc.find("Name") == bc.end()) DebugStop(); 
        dim_name_and_physical_tag[bc["Dim"]][bc["Name"]] = bc["matId"];
    }

    GeometryFine.SetDimNamePhysical(dim_name_and_physical_tag);
    gmesh = GeometryFine.GeometricGmshMesh(filename,nullptr,false);
    gmesh->SetName(meshName);

    return gmesh;
}

TPZCompMesh *CreateCompMesh(TPZGeoMesh* gmesh, json inputFile, int HybridType, std::set<int> &matIDpostProcess){

    int fHybridType = inputFile["Simulation"]["ApproximationType"];
    int pOrder = inputFile["Simulation"]["pOrder"];
    int internalOrder = inputFile["Simulation"]["internalOrder"];


    int nLayers = inputFile["nLayers"];
    std::set<int> layerIds;
    std::set<int> faultIds;
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

    for(auto& fault : inputFile["FaultData"]){
        if(fault.find("Name") == fault.end()) DebugStop(); // check if the information exists
        int faultId = fault["matId"];
        faultIds.insert(faultId);
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
    approxCreator.SetProbType(ProblemType::EElastic);
    if(HybridType) {
        approxCreator.SetHybridType(HybridizationType::EStandard); // EStandard, EStandardSquared, ESemi
        approxCreator.SetHybridizeBoundary(); //? how hybridized some bcs
        approxCreator.SetShouldCondense(true); //! change this
    }
    else {
        approxCreator.SetHybridType(HybridizationType::ENone);
        approxCreator.SetShouldCondense(false);
    }
    approxCreator.IsRigidBodySpaces() = true;
    approxCreator.SetDefaultOrder(pOrder);
    approxCreator.SetExtraInternalOrder(internalOrder);


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
    TPZElasticity2D *matLayer = nullptr;
    // TPZHybridElasticity2D *matLayer = nullptr;
    // if(fHybridType) matLayer = dynamic_cast<TPZHybridElasticity2D*>(matLayer);

    for(int layId : layerIds){
        if(HybridType) matLayer = new TPZHybridElasticity2D(layId, Elas[layId], nu[layId], fxf, fyf, planestrain);
        else matLayer = new TPZElasticity2D(layId, Elas[layId], nu[layId], fxf, fyf, planestrain);
        matLayer->SetPreStress(preStress[layId][0], preStress[layId][1], preStress[layId][2], 0.0);
        //matLayer->SetForcingFunction(inSituStress,1);
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
    std::string CompMeshName;
    if(HybridType){
        cmesh = approxCreator.CreateApproximationSpace();
        CompMeshName = "CompMesh_Hyb";
    }
    else{ 
        cmesh = approxCreator.CreateClassicH1ApproximationSpace();
        CompMeshName = "CompMesh_H1";
    }
    cmesh->SetName(CompMeshName);

    int LagMatId = approxCreator.HybridData().fLagrangeMatId; 
    int dim = cmesh->Dimension() - 1;
    int nstate = 2;

    if(cmesh->FindMaterial(LagMatId)){
        cmesh->DeleteMaterial(LagMatId);
        auto nullmat = new TPZNullMaterialSol(LagMatId, dim, nstate);
        cmesh->InsertMaterialObject(nullmat);
        matIDpostProcess.insert(LagMatId);
    }

    if (auto* cmesh_mult = dynamic_cast<TPZMultiphysicsCompMesh*>(cmesh)) {
        auto& meshvec = cmesh_mult->MeshVector();
        TPZCompMesh* flux_mesh = meshvec[0];
        for(int faultId : faultIds){
            TPZNullMaterial<STATE> *matFrac0 = new TPZNullMaterial<STATE>(faultId, 1, 2);
            TPZNullMaterialSol *matFrac = new TPZNullMaterialSol(faultId, 1, 2);
            flux_mesh->InsertMaterialObject(matFrac0);
            cmesh->InsertMaterialObject(matFrac);
        }

        for (int el = 0; el < gmesh->NElements(); el++){
        
            TPZGeoEl* gel = gmesh->Element(el);
        
            int elId = gel->MaterialId();
        
            if (elId != LagMatId) continue;
        
            int nsides = gel->NSides();
            int nnodes = gel->NCornerNodes();
        
            for(int side = nnodes; side < nsides; side++){
                TPZGeoElSide gelside(gel, side);
                for(int faultId : faultIds){
                    bool hasFaultNeigh = gelside.HasNeighbour(faultId); 
                    if (hasFaultNeigh) {
                        gel->SetMaterialId(faultId);
                        break;
                    }
                }
            }
        }
    }

    return cmesh;
}

void DuplicateConnectFracture(TPZGeoMesh *gmesh, TPZCompMesh *cmesh, std::set<int> fracBcIds){
    int nels = gmesh->NElements();
    std::set<int> neighIndices; // set to store el indeces that has already been analyzed
    for (int64_t el = 0; el < cmesh->NElements(); el++){
        TPZCompEl *cel = cmesh->Element(el);
        TPZGeoEl *gel = cel->Reference();
        int meshDim = gmesh->Dimension();
        int elDim = gel->Dimension(); 
        int matId = gel->MaterialId();


        if(elDim != meshDim - 1 || matId < 200) continue; 
        if(neighIndices.find(gel->Index()) != neighIndices.end()) continue;

        int iside = gel->NSides() - 1;

        TPZGeoElSide gelside(gel, iside);
        TPZCompElSide celside = gelside.Reference();

        TPZGeoElSide neighBCside = gelside.HasNeighbour(200); 
        TPZGeoElSide neighDarcyside = gelside.HasNeighbour(EMatId); 
        TPZGeoElSide neighDarcy2side = neighDarcyside.HasNeighbour(EMatId);

        TPZGeoElSide neighbour = gelside.Neighbour();
        int neighIndex = 0;
        while(neighbour != gelside){
            if(fracBcIds.find(neighbour.Element()->MaterialId()) != fracBcIds.end()){
                neighIndex = neighbour.Element()->Index();
                auto it = neighIndices.find(neighIndex);
                if (it == neighIndices.end()){
                    neighBCside = neighbour;
                    neighIndices.insert(neighIndex);
                }
            }
            neighbour = neighbour.Neighbour();
        }


        if (!neighBCside || !neighDarcyside || !neighDarcy2side) DebugStop();


        TPZCompElSide celBCneigh = neighBCside.Reference(); 
        TPZCompElSide celDarcyneigh = neighDarcyside.Reference();
        TPZCompElSide celDarcy2neigh = neighDarcy2side.Reference();

        int connRight = celDarcyneigh.ConnectIndex();
        int connLeft = celDarcy2neigh.ConnectIndex();

        celDarcyneigh.SplitConnect(celDarcy2neigh);

        connRight = celDarcyneigh.ConnectIndex();
        connLeft = celDarcy2neigh.ConnectIndex();

        celside.Element()->SetConnectIndex(0, connRight);
        celBCneigh.Element()->SetConnectIndex(0, connLeft);

        cmesh->ExpandSolution();
        cmesh->ComputeNodElCon();
    }
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
    STATE rho = 2.8e3; //! Ver pegar de cada layer
    result[0] = 0;
    result[1] = rho*grav*loc[1];
    result[2] = 0;
}

void inSituStress(const TPZVec<REAL> &loc, TPZVec<REAL> &result){
    std::ifstream filejson("/home/marina/programming/Biot-Research/Biot/Inputs/Ex1.json");

    json inputFile = json::parse(filejson,nullptr,true,true,true); 

    STATE sigV = inputFile["InSitu_Stress"]["Sigma_v"];
    STATE sigH = inputFile["InSitu_Stress"]["Sigma_H"];
    STATE sigh = inputFile["InSitu_Stress"]["Sigma_h"];
    STATE Pp = inputFile["InSitu_Stress"]["Pore-Pressure"];
    result[0] = sigh*loc[1];
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


void PostProc(TPZCompMesh *cmesh, TPZManVector<std::string, 10> fields) {

}


TPZVec<REAL> ComputeError(TPZVec<TPZCompEl*> &celVecH1, TPZVec<TPZCompEl*> &celVecHyb, TPZFMatrix<STATE> &elSolMat){

    REAL error = 0;
    REAL elerror = 0;
    REAL energyH1 = 0;
    REAL energyHyb = 0;
    TPZVec<STATE> errorVec(1, 0.0);
    TPZVec<STATE> energyVec(2, 0.0);
    TPZVec<STATE> elenergyVec(2, 0.0);

    int nelsH1 = celVecH1.size();
    int nelsHyb = celVecHyb.size();

    for(int el = 0; el < nelsH1; el++){

        if(!celVecH1[el] || !celVecHyb[el]) continue;

        int64_t H1index = celVecH1[el]->Index();

        errorVec = CalcElementError(celVecH1[el], celVecHyb[el]);
        elSolMat(H1index,0) = std::sqrt(errorVec[0]);
        error += errorVec[0];

        // elenergyVec = CalcElementEnergy(celVecH1[el], celVecHyb[el]);
        // energyH1 += elenergyVec[0];
        // energyHyb += elenergyVec[1];
    }

    errorVec[0] = std::sqrt(error);
    // energyVec[0] = (1./2.)*energyH1;
    // energyVec[1] = (1./2.)*energyHyb;

    return errorVec;
    // return energyVec;
}

TPZVec<STATE> CalcElementError(TPZCompEl* celH1, TPZCompEl* celHyb){

    REAL error = 0.;
    TPZVec<REAL> errorVec(1, 0.0);
    if(!celH1 || !celHyb) {
        std::cout << "No computational element found!\n";
        return errorVec;
    }

    int dim = celH1->Dimension();
    int matId = celH1->Reference()->MaterialId();

    TPZMaterial *matH1 = celH1->Material();
    TPZMaterial *matHyb = celHyb->Material();

    TPZElasticity2D *matElas = dynamic_cast<TPZElasticity2D*>(matH1); 
    TPZElasticity2D *matElasHyb = dynamic_cast<TPZElasticity2D*>(matHyb); 
    //downcasting—converting a base class pointer to a derived class pointer

    //const TPZIntPoints intrule = celH1->GetIntegrationRule(); 
    TPZGeoEl *gel = celH1->Reference();
    TPZIntPoints *intrule = gel->CreateSideIntegrationRule(gel->NSides()-1,4); //! WHAT ORDER
    int npoints = intrule->NPoints();

    TPZVec<REAL> xi(dim, 0.0);
    TPZFNMatrix<9,REAL> jac(dim,dim),jacinv(dim,dim),axes(dim,3); //jacobian
    REAL detjac;

    TPZVec<STATE> stressH1(3, 0.0);
    TPZVec<STATE> strainH1(3, 0.0);
    TPZVec<STATE> stressHyb(3, 0.0);
    TPZVec<STATE> strainHyb(3, 0.0);
    TPZFNMatrix<9,REAL> D(3, 3, 0.0);
    TPZFNMatrix<9,REAL> InvD(3, 3, 0.0); 
    REAL detD = 0;

    // Performing numerical integration
    for (int point = 0; point < npoints; point++) {
        REAL weight;
        intrule->Point(point, xi, weight);
        gel->Jacobian(xi, jac, axes, detjac, jacinv);

        celH1->Solution(xi, 10, stressH1); 
        celH1->Solution(xi, 11, strainH1); 
        celHyb->Solution(xi, 10, stressHyb);
        celHyb->Solution(xi, 11, strainHyb);

        matElas->ComputeD(xi, D); //! DID SOMETHING ILEGAL
        D.DeterminantInverse(detD, InvD);

        TPZFMatrix<STATE> fluxH1(3, 1, 0.0);
        TPZManVector<STATE,3> PreStress = matElas->GetPreStress();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                fluxH1(i,0) += D(i,j)*strainH1[j];
            }
            fluxH1(i,0) += PreStress[i];
        }

        //? strain D strain 
        // for (int i = 0; i < 3; i++) { 
        //     for (int j = 0; j < 3; j++) {
        //         error += (strainH1[i]-strainHyb[i])*D(i,j)*(strainH1[j]-strainHyb[j]);
        //     }
        // }

        //? stress D^-1 stress 
        // for (int i = 0; i < 3; i++) { 
        //     for (int j = 0; j < 3; j++) {
        //         error += (stressH1[i]-stressHyb[i])*InvD(i,j)*(stressH1[j]-stressHyb[j]);
        //     }
        // }

        //? (stressHyb - D strainH1) D^-1 (stressHyb - D strainH1)
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                error += (fluxH1(i,0)-stressHyb[i])*InvD(i,j)*(fluxH1(j,0)-stressHyb[j]);
            }
        }

        errorVec[0] += error*weight*fabs(detjac);
    }
    delete intrule;
    return errorVec;
}

TPZVec<REAL> CalcElementEnergy(TPZCompEl* celH1, TPZCompEl* celHyb){

    REAL energyH1 = 0.;
    REAL energyHyb = 0.;
    TPZVec<REAL> energyVec(2, 0.0);
    if(!celH1 || !celHyb) {
        std::cout << "No computational element found!\n";
        return energyVec;
    }

    int dim = celH1->Dimension();
    int matId = celH1->Reference()->MaterialId();

 
    TPZMaterial *matH1 = celH1->Material();
    TPZMaterial *matHyb = celHyb->Material();

    TPZElasticity2D *matElas = dynamic_cast<TPZElasticity2D*>(matH1); 
    TPZHybridElasticity2D *matElasHyb = dynamic_cast<TPZHybridElasticity2D*>(matHyb); 
    //downcasting—converting a base class pointer to a derived class pointer

    //const TPZIntPoints intrule = celH1->GetIntegrationRule(); //! Ask
    TPZGeoEl *gel = celH1->Reference();
    TPZIntPoints *intrule = gel->CreateSideIntegrationRule(gel->NSides()-1,4); //! WHAT ORDER
    int npoints = intrule->NPoints();

    TPZVec<REAL> xi(dim, 0.0);
    TPZFNMatrix<9,REAL> jac(dim,dim),jacinv(dim,dim),axes(dim,3); //jacobian
    REAL detjac;

    TPZVec<STATE> stressH1(3, 0.0);
    TPZVec<STATE> strainH1(3, 0.0);
    TPZVec<STATE> stressHyb(3, 0.0);
    TPZVec<STATE> strainHyb(3, 0.0);
    TPZFNMatrix<9, STATE> D(3, 3, 0.0); 

    // Performing numerical integration
    for (int point = 0; point < npoints; point++) {
        REAL weight;
        intrule->Point(point, xi, weight);
        gel->Jacobian(xi, jac, axes, detjac, jacinv);

        celH1->Solution(xi, 10, stressH1); 
        celH1->Solution(xi, 11, strainH1); 
        celHyb->Solution(xi, 10, stressHyb);
        celHyb->Solution(xi, 11, strainHyb);

        matElas->ComputeD(xi, D); //! DID SOMETHING ILEGAL

        //? strain stress
        for (int i = 0; i < dim; i++) { 
            for (int j = 0; j < dim; j++) {
                energyH1 += (strainH1[i])*(stressH1[j]);
            }
        }

        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                energyHyb += strainHyb[i]*stressHyb[j];
            }
        }

        //? strain D strain 
        // for (int i = 0; i < 3; i++) { 
        //     for (int j = 0; j < 3; j++) {
        //         energyH1 += (strainH1[i])*D(i,j)*(strainH1[j]);
        //     }
        // }

        // for (int i = 0; i < dim; i++) {
        //     for (int j = 0; j < dim; j++) {
        //         energyHyb += strainHyb[i]*D(i,j)*strainHyb[j];
        //     }
        // }

        energyVec[0] += energyH1*weight*fabs(detjac);
        energyVec[1] += energyHyb*weight*fabs(detjac);
    }
    delete intrule;
    return energyVec;
}

void GetCompEls(TPZGeoMesh* gmesh, TPZCompMesh *cmeshH1, TPZCompMesh *cmeshHyb, TPZVec<TPZCompEl*> &celVecH1, TPZVec<TPZCompEl*> &celVecHyb, std::set<int> matId){

    int64_t nels = gmesh->NElements();
    int64_t nels1 = cmeshH1->NElements();
    int64_t nels2 = cmeshHyb->NElements();
    // celVecH1.Resize(nels);
    // celVecHdiv.Resize(nels);
    

    for(int64_t el = 0; el < nels1; el++){

        TPZCompEl *celH1 = cmeshH1->Element(el);
        TPZGeoEl *gel = celH1->Reference();
        if(!gel) DebugStop();
        int64_t gelId = gel->Index();
        int elMatId = gel->MaterialId();
        if (matId.find(elMatId) == matId.end()) continue;
        //TPZCondensedCompEl *condelH1 = dynamic_cast<TPZCondensedCompEl *>(celH1);

        celVecH1[gelId] = celH1;
    }

    // for(int64_t el = 0; el < nels2; el++){

    //     TPZCompEl *celHyb = cmeshHyb->Element(el);
    //     TPZCondensedCompEl *condelHyb = dynamic_cast<TPZCondensedCompEl *>(celHyb);
    //     if(!condelHyb) continue;
    //     TPZCompEl *refcelHyb = condelHyb->ReferenceCompEl();
    //     int64_t celId = condelHyb->ReferenceIndex();
    //     TPZGeoEl *gel = refcelHyb->Reference();
    //     if(!gel) DebugStop();
    //     int64_t gelId = gel->Index();
    //     int elMatId = gel->MaterialId();
    //     if (matId.find(elMatId) == matId.end()) continue;

    //     celVecHyb[gelId] = celHyb;
    // }

    for (auto gel : gmesh->ElementVec()){

        TPZCompEl *celHyb = gel->Reference();
        int64_t gelId = gel->Index();
        int elMatId = gel->MaterialId();
        if (matId.find(elMatId) == matId.end()) continue;
        if(!celHyb) continue;

        celVecHyb[gelId] = celHyb;
    }

}