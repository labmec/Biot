#include "TPZNullMaterialSol.h"
#include "TPZMaterialDataT.h"
#include "pzaxestools.h"
#include <cstring>

void TPZNullMaterialSol::Contribute(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight, 
                                    TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef) {
    if (!fStiffFault) return;
    TPZFMatrix<REAL> &dphi = datavec[0].dphix;
    TPZFMatrix<REAL> &phiT = datavec[0].phi;
    int64_t nshape = phiT.Rows();

    const  TPZFNMatrix<9,REAL> &axes = datavec[0].axes;
    TPZFNMatrix<9,REAL> normal(1, 3, 0.0);
    TPZFNMatrix<9,REAL> Rot(fNState, fNState, 0.0), RotT(fNState, fNState, 0.0);
    Rot(0, 0) = axes(0, 0);
    Rot(1, 0) = axes(0, 1);
    Rot(0, 1) = axes(0, 1);
    Rot(1, 1) = -axes(0, 0);


    TPZFNMatrix<400,STATE> phivecT(fNState, nshape*fNState, 0.0); // Voigt notation
    TPZFNMatrix<400,STATE> KphivecT(fNState, nshape*fNState, 0.0); // Voigt notation
    TPZFNMatrix<10,STATE> stiffXY(fNState, fNState, 0.0); // Voigt notation
    TPZFNMatrix<10,STATE> aux(fNState, fNState, 0.0); // Voigt notation
    for (int i = 0; i < nshape; i++) {
        for (int s = 0; s < fNState; s++) {
            phivecT(s, fNState * i + s) = phiT(i, 0);
        }
    }

    Rot.Multiply(fStiffFault,aux);
    aux.Multiply(Rot,stiffXY);
    stiffXY.Multiply(phivecT,KphivecT);

    REAL factor = weight; 
    ek.AddContribution(0, 0, KphivecT, 1, phivecT, 0, factor); 
}

int TPZNullMaterialSol::ClassId() const {
    return Hash("TPZNullMaterialSol") ^ (TPZNullMaterialCS<STATE>::ClassId() << 1);
}

void TPZNullMaterialSol::Write(TPZStream &buf, int withclassid) const {
    TPZMaterial::Write(buf, withclassid);
    if (fDim < 1 || fDim > 3) {
        DebugStop();
    }
    buf.Write(&fDim);
    buf.Write(&fNState);
}

void TPZNullMaterialSol::Read(TPZStream &buf, void *context) {
    TPZMaterial::Read(buf, context);
    buf.Read(&fDim);
    buf.Read(&fNState);
}

int TPZNullMaterialSol::VariableIndex(const std::string &name) const {
    if (!strcmp("SigN", name.c_str())) return 1;
    if (!strcmp("SigT", name.c_str())) return 2;
    if (!strcmp("SigT_SigN", name.c_str())) return 3;
    if (!strcmp("TractionNorm", name.c_str())) return 4;
    if (!strcmp("PorePressure", name.c_str())) return 5;
    if (!strcmp("SlipTendency", name.c_str())) return 6;
    if (!strcmp("Failure", name.c_str())) return 7;

    return -1;
}

int TPZNullMaterialSol::NSolutionVariables(int var) const {

    switch(var) {
		case 0:
			return 2;
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
            return 1;
        case 4:
            return 3;
		default:
			return TPZMaterial::NSolutionVariables(var);
	}  
}

void TPZNullMaterialSol::SetPorePressure(STATE porePress, STATE hydroPress) {
    fpp = porePress;
	fPreStressXX = hydroPress;
	fPreStressYY = hydroPress;
    fPreStressZZ = hydroPress;
}

void TPZNullMaterialSol::SetFaultStiff(TPZFMatrix<STATE> &Kfault){
    fStiffFault = Kfault;
}

void TPZNullMaterialSol::SetCriterionParameters(REAL cohesion, REAL friction) {
    fCohesion = cohesion;
    double my_pi = 3.14159265359;
    fAngle = friction * my_pi / 180.0;
}

REAL TPZNullMaterialSol::failureCriteria(TPZVec<STATE> &tension) {

    REAL result = std::abs(tension[1] / (fCohesion - tension[0] * std::tan(fAngle)));

    return result;
}

void TPZNullMaterialSol::Solution(const TPZVec<TPZMaterialDataT<STATE>> &datavec, int var, TPZVec<STATE> &solOut) {

    TPZManVector<STATE, 10> Sol;
    TPZManVector<STATE, 10> Solxy;
    const  TPZFNMatrix<9,REAL> &axes = datavec[0].axes;
    TPZFNMatrix<9,REAL> normal(1, 3, 0.0);

    if (fDim+1 == 3) {
        normal(0, 0) = axes(0, 1) * axes(1, 2) - axes(0, 2) * axes(1, 1);
        normal(0, 1) = axes(0, 2) * axes(1, 0) - axes(0, 0) * axes(1, 2);
        normal(0, 2) = axes(0, 0) * axes(1, 1) - axes(0, 1) * axes(1, 0);
    }
    else {
        normal(0, 0) = axes(0, 1);
        normal(0, 1) = -axes(0, 0);
    }

    REAL n_norm = std::sqrt(normal(0, 0)*normal(0, 0) + normal(0, 1)*normal(0, 1) + normal(0, 2)*normal(0, 2));
    REAL t_norm = std::sqrt(axes(0, 0)*axes(0, 0) + axes(0, 1)*axes(0, 1)); //! Generalize this
    if (n_norm > 1e-12) {
        normal(0, 0) /= n_norm;
        normal(0, 1) /= n_norm;
        normal(0, 2) /= n_norm;
    } 

    Sol = datavec[0].sol[0]; // Em termos de XY
    Sol[0] += fPreStressXX;
    Sol[1] += fPreStressYY;
    if (fDim+1 == 3) Sol[2] += fPreStressZZ;

    STATE trac_normal = 0.0;
    TPZManVector<STATE, 3> t_n({0.0, 0.0, 0.0});
    STATE trac_tan = 0.0;
    STATE trac_tan2 = 0.0;
    TPZManVector<STATE, 3> t_t({0.0, 0.0, 0.0});
    TPZManVector<STATE, 3> t_t2({0.0, 0.0, 0.0});
    for (int i = 0; i < fDim+1; i++) {
        trac_normal += Sol[i]*normal(0, i);
        trac_tan2 += Sol[i]*axes(0, i);
    }
    for (int i = 0; i < fDim+1; i++) {
        t_n[i] = trac_normal*normal(0, i);
        t_t2[i] = trac_tan2*axes(0, i); 
    }
    for (int i = 0; i < fDim+1; i++) {
        t_t[i] = Sol[i]-t_n[i]; 
        trac_tan += t_t[i]*t_t[i];
    }
    trac_tan = std::sqrt(trac_tan);

    if (var == 1) {
        solOut[0] = trac_normal;
        return;
    }
    else if (var == 2) {
        solOut[0] = trac_tan;
        return;
    }
    else if (var == 3) {
        if (std::abs(trac_normal) < 1.0e-12) DebugStop();
        solOut[0] = std::abs(trac_tan/trac_normal);
        return;
    }
    else if (var == 4){
        for (int i = 0; i < fDim+1; i++)
            solOut[2] += Sol[i]*Sol[i];
        return;
    }
    else if (var == 5){
        solOut[0] = fPreStressXX;
        return;
    }
    else if (var == 6){
        TPZVec<STATE> tension({trac_normal, trac_tan});
        solOut[0] = failureCriteria(tension);
        return;
    }
    else if (var == 7){
        TPZVec<STATE> tension({trac_normal, trac_tan});

        if(failureCriteria(tension) > 1)
            solOut[0] = 1.0;
        else
            solOut[0] = 0.0;
        return;
    }
    else { //!TO CHANGE
        //TBase::Solution(data,var,Solout);
        return;
    }

}