#include "TPZNullMaterialSol.h"
#include "TPZMaterialDataT.h"
#include "pzaxestools.h"
#include <cstring>

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

    return -1;
}

int TPZNullMaterialSol::NSolutionVariables(int var) const {

    switch(var) {
		case 0:
			return 2;
        case 1:
        case 2:
        case 3:
            return 1;
        case 4:
            return 3;
		default:
			return TPZMaterial::NSolutionVariables(var);
	}  
}

void TPZNullMaterialSol::Solution(const TPZVec<TPZMaterialDataT<STATE>> &datavec, int var, TPZVec<STATE> &solOut) {

    TPZManVector<STATE, 10> Sol;
    TPZManVector<STATE, 10> Solxy;
    const TPZFMatrix<REAL> &axes = datavec[0].axes;
    TPZFNMatrix<9,REAL> normal(1, 2, 0.0);

    TPZFNMatrix<9,STATE> rot(2, 2, 0.0);
    rot(0, 1) = 1;
    rot(1, 0) = -1;

    // for (int i = 0; i < 2; i++){
    //     for (int j = 0; j < 2; j++){
    //         normal(0, j) += axes(0, j) * rot(i,j);
    //     }
    // }
    normal(0, 0) = axes(0, 1);
    normal(0, 1) = -axes(0, 0);
    REAL n_norm = std::sqrt(normal(0, 0)*normal(0, 0)+normal(0, 1)*normal(0, 1));
    REAL t_norm = std::sqrt(axes(0, 0)*axes(0, 0)+axes(0, 1)*axes(0, 1));

    Sol = datavec[0].sol[0];

    if (var == 1) {
        STATE trac_normal = Sol[0]*normal(0, 0)+Sol[1]*normal(0, 1);
        // TPZManVector<STATE, 3> SigN(2, 0.0); 
        // SigN[0] = trac_normal * normal(0,0);
        // SigN[1] = trac_normal * normal(0,1);
        solOut[0] = trac_normal;
        return;
    }
    else if (var == 2) {
        STATE trac_tan = Sol[0]*axes(0, 0)+Sol[1]*axes(0, 1);
        // TPZManVector<STATE, 3> SigT(2, 0.0);
        // SigT[0] = trac_tan * axes(0,0);
        // SigT[1] = trac_tan * axes(0,1);
        solOut[0] = trac_tan;
        return;
    }
    else if (var == 3) {
        STATE trac_normal = Sol[0]*normal(0, 0)+Sol[1]*normal(0, 1);
        STATE trac_tan = Sol[0]*axes(0, 0)+Sol[1]*axes(0, 1);
        if (std::abs(trac_normal) < 1.0e-9) DebugStop();
        solOut[0] = std::abs(trac_tan/trac_normal);
    }
    else if (var == 4){
        solOut[2] = std::sqrt(Sol[0]*Sol[0]+Sol[1]*Sol[1]);
        return;
    }
    else { //!TO CHANGE
        //TBase::Solution(data,var,Solout);
        return;
    }

}