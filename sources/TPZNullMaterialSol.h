#ifndef TPZNULLMATERIALSOL_H
#define TPZNULLMATERIALSOL_H

#include "TPZNullMaterialCS.h"

class TPZNullMaterialSol : public TPZNullMaterialCS<STATE> {

public:

    TPZNullMaterialSol(int matid, int dimension, int nstate) : TPZNullMaterialCS<STATE>(matid, dimension, nstate) {}

    TPZNullMaterialSol() : TPZNullMaterialCS() {
        fDim = 1;
        fNState = 1;
    }

    std::string Name() const override { return "TPZNullMaterialSol"; }

    int VariableIndex(const std::string &name) const override;

    int NSolutionVariables(int var) const override;

    void Solution(const TPZVec<TPZMaterialDataT<STATE>> &datavec, int var, TPZVec<STATE> &solOut) override;

    void Contribute(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight,
                    TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef) override {}

    void ContributeBC(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight,
                      TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef, TPZBndCondT<STATE> &bc) override {}

    int ClassId() const override;

    void Write(TPZStream &buf, int withclassid) const override;

    void Read(TPZStream &buf, void *context) override;

    void SetPorePressure(STATE porePress, STATE hydroPress);


protected:

    /** @brief Problem pore-pressure */
    STATE fpp = 0.0;

    STATE fPreStressXX = 0.0;

    STATE fPreStressYY = 0.0;

    STATE fPreStressZZ = 0.0;

};


#endif
