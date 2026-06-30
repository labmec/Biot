#include "TPZNullMaterialSol.h"
#include "TPZMaterialDataT.h"
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
    if (!strcmp("SigN_SigT", name.c_str())) return 1;
    return -1;
}

int TPZNullMaterialSol::NSolutionVariables(int var) const {

    switch(var) {
		case 0:
			return 2;
        case 1:
            return 1;
		default:
			return TPZMaterial::NSolutionVariables(var);
	}  
}

void TPZNullMaterialSol::Solution(const TPZVec<TPZMaterialDataT<STATE>> &datavec, int var, TPZVec<STATE> &sol) {
    sol.Resize(NSolutionVariables(var));
    for (int i = 0; i < sol.size(); i++) {
        sol[i] = 0.0;
    }

    const TPZVec<STATE> &Sol = datavec[0].sol[0];

    if (var == 1) {
        if (Sol.NElements() > 0) {
            sol[0] = Sol[0];
        }
        return;
    }

    if (var == 0) {
        for (int i = 0; i < sol.size() && i < Sol.size(); i++) {
            sol[i] = Sol[i];
        }
    }
}