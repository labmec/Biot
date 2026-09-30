#ifndef TPZSLIPMATERIAL_H
#define TPZSLIPMATERIAL_H

#include "TPZMatBase.h"
#include "TPZMatCombinedSpaces.h"
#include "pzaxestools.h"
#include "TPZMaterialDataT.h"

class TPZSlipMaterial : public TPZMatBase<STATE, TPZMatCombinedSpacesT<STATE>> {
    using TBase = TPZMatBase<STATE, TPZMatCombinedSpacesT<STATE>>;

public:

    TPZSlipMaterial(int matid, int dimension, int nstate) : TPZRegisterClassId(&TPZSlipMaterial::ClassId), TBase(matid) {
        if (dimension < 1 || dimension >3) {
            DebugStop();
        }
        fDim = dimension;
        fNState = nstate;
    }

    /** @brief Default constructor */
    TPZSlipMaterial() : TPZRegisterClassId(&TPZSlipMaterial::ClassId), TBase() {
        fDim = 1;
        fNState = 1;
    }

    [[nodiscard]] int Dimension() const override { return fDim; }

    [[nodiscard]] int NStateVariables() const override { return fNState; }

    std::string Name() const override { return "TPZSlipMaterial"; }

    int VariableIndex(const std::string &name) const override;

    int NSolutionVariables(int var) const override;

    void Solution(const TPZVec<TPZMaterialDataT<STATE>> &datavec, int var, TPZVec<STATE> &solOut) override;

    void Contribute(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight,
                    TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef) override;

    void ContributeBC(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight,
                      TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef, TPZBndCondT<STATE> &bc) override {}

    int ClassId() const override;

    void Write(TPZStream &buf, int withclassid) const override;

    void Read(TPZStream &buf, void *context) override;

    void FillDataRequirements(TPZVec<TPZMaterialDataT<STATE>> &datavec) const override;

    [[nodiscard]] TPZMaterial *NewMaterial() const override {
        return new TPZSlipMaterial(*this);
    }

    void SetPorePressure(STATE porePress, STATE hydroPress);

    void SetFaultStiff(TPZVec<STATE> &Kfault);

    void SetCriterionParameters(REAL cohesion, REAL friction);

    REAL failureCriteria(TPZVec<STATE> &tension);

    REAL slipTendency(const TPZVec<TPZMaterialDataT<STATE>> &datavec); 


protected:

    /** @brief Problem dimension */
    int fDim = -1;

    /// Number of state variables
    int fNState = 1;

    /** @brief Problem pore-pressure */
    STATE fpp = 0.0;

    STATE fPreStressXX = 0.0;

    STATE fPreStressYY = 0.0;

    STATE fPreStressZZ = 0.0;

    REAL fCohesion = 0.0;

    REAL fAngle = 0.0;

    /** @brief Cohesive coeffs applied to constitutive/conatct law on the fault 
     * for normal and tangential traction [Kn, Kt1, Kt2]*/
    TPZManVector<STATE,3> fStiffFault; 

};


#endif
