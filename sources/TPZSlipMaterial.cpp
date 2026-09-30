#include "TPZSlipMaterial.h"
#include <cstring>

void TPZSlipMaterial::Contribute(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight, 
                                    TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef) {

    TPZFMatrix<REAL> &dphi = datavec[0].dphix;
    TPZFMatrix<REAL> &phiT = datavec[0].phi;
    int64_t nshape = phiT.Rows();

    //const  TPZFNMatrix<9,REAL> &axes = datavec[0].axes;
    TPZFNMatrix<9,REAL> axes(fNState, fDim, 0.0);
    TPZFNMatrix<9,REAL> normal(fNState, 1, 0.0);

    if (fDim+1 == 3) {
        for(int i = 0; i < 3; i++) {
            axes(i, 0) = datavec[0].axes(0, i);
            axes(i, 1) = datavec[0].axes(1, i);
        }
        normal(0, 0) = axes(1, 0) * axes(2, 1) - axes(2, 0) * axes(1, 1);
        normal(1, 0) = axes(2, 0) * axes(0, 1) - axes(0, 0) * axes(2, 1);
        normal(2, 0) = axes(0, 0) * axes(1, 1) - axes(1, 0) * axes(0, 1);
    }
    else {
        axes(0, 0) = datavec[0].axes(0, 0);
        axes(1, 0) = datavec[0].axes(0, 1);
        normal(0, 0) = axes(1, 0);
        normal(1, 0) = -axes(0, 0);
    }

    REAL n_norm = std::sqrt(normal(0, 0)*normal(0, 0) + normal(1, 0)*normal(1, 0)); //! Generalize this
    REAL t_norm = std::sqrt(axes(0, 0)*axes(0, 0) + axes(1, 0)*axes(1, 0)); //! Generalize this
    if (n_norm > 1e-12) {
        normal(0, 0) /= n_norm;
        normal(1, 0) /= n_norm;
    }

    // TPZFNMatrix<400,STATE> phivec(fNState, nshape*fNState, 0.0); // Voigt notation
    // for (int i = 0; i < nshape; i++) {
    //     for (int s = 0; s < fNState; s++) {
    //         phivec(s, fNState * i + s) = phiT(i, 0);
    //     }
    // }

    TPZFNMatrix<400,STATE> phivec(nshape*fNState, fNState, 0.0); // Voigt notation
    TPZFNMatrix<400,STATE> phivecN(nshape*fNState, fNState, 0.0); // Voigt notation
    TPZFNMatrix<400,STATE> phivecT(nshape*fNState, fNState, 0.0); // Voigt notation

    for (int i = 0; i < nshape; i++) {
        for (int s = 0; s < fNState; s++) {
            phivec(fNState * i + s, s) = phiT(i, 0);
        }
    }

    phivec.Multiply(normal,phivecN);
    phivec.Multiply(axes,phivecT);


    REAL factor = fStiffFault[0] * weight; 
    ek.AddContribution(0, 0, phivecN, 0, phivecN, 1, factor); 
    factor = fStiffFault[1] * weight;
    ek.AddContribution(0, 0, phivecT, 0, phivecT, 1, factor); 
}

int TPZSlipMaterial::ClassId() const {
    return Hash("TPZSlipMaterial") ^ TBase::ClassId() << 1;
}

void TPZSlipMaterial::Write(TPZStream &buf, int withclassid) const {
    TPZMaterial::Write(buf, withclassid);
    if (fDim < 1 || fDim > 3) {
        DebugStop();
    }
    buf.Write(&fDim);
    buf.Write(&fNState);
}

void TPZSlipMaterial::Read(TPZStream &buf, void *context) {
    TPZMaterial::Read(buf, context);
    buf.Read(&fDim);
    buf.Read(&fNState);
}

void TPZSlipMaterial::FillDataRequirements(TPZVec<TPZMaterialDataT<STATE>> &datavec) const {
    for (auto i = 0; i < datavec.size(); i++) {
        datavec[i].SetAllRequirements(false);
        datavec[i].fActiveApproxSpace = false;
        datavec[i].fNeedsSol = true;
    }
}

int TPZSlipMaterial::VariableIndex(const std::string &name) const {
    if (!strcmp("SigN", name.c_str())) return 1;
    if (!strcmp("SigT", name.c_str())) return 2;
    if (!strcmp("SigT_SigN", name.c_str())) return 3;
    if (!strcmp("TractionNorm", name.c_str())) return 4;
    if (!strcmp("PorePressure", name.c_str())) return 5;
    if (!strcmp("SlipTendency", name.c_str())) return 6;
    if (!strcmp("Failure", name.c_str())) return 7;

    return -1;
}

int TPZSlipMaterial::NSolutionVariables(int var) const {

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

void TPZSlipMaterial::SetPorePressure(STATE porePress, STATE hydroPress) {
    fpp = porePress;
	fPreStressXX = hydroPress;
	fPreStressYY = hydroPress;
    fPreStressZZ = hydroPress;
}

void TPZSlipMaterial::SetFaultStiff(TPZVec<STATE> &Kfault){
    fStiffFault = Kfault;
}

void TPZSlipMaterial::SetCriterionParameters(REAL cohesion, REAL friction) {
    fCohesion = cohesion;
    double my_pi = 3.14159265359;
    fAngle = friction * my_pi / 180.0;
}

REAL TPZSlipMaterial::failureCriteria(TPZVec<STATE> &tension) {

    REAL result = std::abs(tension[1] / (fCohesion - tension[0] * std::tan(fAngle)));

    return result;
}

REAL TPZSlipMaterial::slipTendency(const TPZVec<TPZMaterialDataT<STATE>> &datavec) {

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
    STATE trac_tan = 0.0;
    for (int i = 0; i < fDim+1; i++) {
        trac_normal += Sol[i]*normal(0, i);
        trac_tan += Sol[i]*axes(0, i);
    }

    TPZVec<STATE> tension({trac_normal, trac_tan});

    return failureCriteria(tension);
}

void TPZSlipMaterial::Solution(const TPZVec<TPZMaterialDataT<STATE>> &datavec, int var, TPZVec<STATE> &solOut) {

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
        solOut[0] = trac_tan2;
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