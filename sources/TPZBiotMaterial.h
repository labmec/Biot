#ifndef TPZBIOTMATERIAL_H
#define TPZBIOTMATERIAL_H

#include "TPZMatCombinedSpaces.h"
#include "TPZMatErrorCombinedSpaces.h"
#include "Material/DarcyFlow/TPZDarcyFlow.h"
#include "Material/Elasticity/TPZElasticity2D.h"
#include "Material/Elasticity/TPZElasticity3D.h"
#include "Material/DarcyFlow/TPZIsotropicPermeability.h"

/**
 * @brief This class implements a approximation for the poro-elasticity problem based on Biot's consolidation theory.
 * Deformation of a porous (elastic) matrix together with the flow of the fluid.
 */

class TPZBiotMaterial : public TPZMatBase<STATE, TPZMatCombinedSpacesT<STATE>,TPZMatErrorCombinedSpaces<STATE>>
{

    // type alias to improve constructor readability
    using TBase = TPZMatBase<STATE, TPZMatCombinedSpacesT<STATE>, TPZMatErrorCombinedSpaces<STATE>>;

public:
    /**
     * @brief Default constructor
     */
    TPZBiotMaterial();

    /**
	 * @brief Class constructor
	 * @param [in] id material id
	 * @param [in] dim problem dimension
	 */
    TPZBiotMaterial(int id, int dim);

    TPZBiotMaterial(const TPZBiotMaterial &copy);

    TPZBiotMaterial& operator=(const TPZBiotMaterial &copy);

    int Dimension() const override { return this->fDim; }

    /**
	 * @brief Returns the number of state variables
	 */
    int NStateVariables() const override { return 1; }

    /**
	 * @brief Returns the number of errors to be evaluated
     *
     * Returns the number of errors to be evaluated, that is, the number of error norms associated
     * with the problem.
     */
    int NEvalErrors() const override { return 3; }

    virtual void SetDimension(int dim);

    /**
     * @brief It computes a contribution to the stiffness matrix and load vector at one integration point
     * @param[in] data stores all input data
     * @param[in] weight is the weight of the integration rule
     * @param[out] ek is the element matrix
     * @param[out] ef is the rhs vector
     */
    void Contribute(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight, TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef) override;

    /**
     * @brief It computes a contribution to the stiffness matrix and load vector at one BC integration point
     * @param[in] data stores all input data
     * @param[in] weight is the weight of the integration rule
     * @param[out] ek is the element matrix
     * @param[out] ef is the rhs vector
     * @param[in] bc is the boundary condition material
     */
    void ContributeBC(const TPZVec<TPZMaterialDataT<STATE>> &datavec, REAL weight, TPZFMatrix<STATE> &ek, TPZFMatrix<STATE> &ef,
                      TPZBndCondT<STATE> &bc) override;

    /**
     * @brief Returns an integer associated with a post-processing variable name
     * @param [in] name string containing the name of the post-processing variable. Ex: "Pressure".
     */
    int VariableIndex(const std::string &name) const override;

    /**
     * @brief Returns an integer with the dimension of a post-processing variable
     * @param [in] var index of the post-processing variable, according to TPZDarcyFlow::VariableIndex method.
     */
    [[nodiscard]] int NSolutionVariables(int var) const override;

    /**
     * @brief Returns the solution associated with the var index based on the
     * finite element approximation at a point
     * @param [in] data material data associated with a given integration point
     * @param [in] var index of the variable to be calculated
     * @param [out] solOut vector to store the solution
     */
    void Solution(const TPZMaterialDataT<STATE> &data, int var, TPZVec<STATE> &solOut);

    /**
     * @brief Get the dimensions of the solution for each state variable
     *
     * This will be used for initializing the corresponding TPZMaterialData
     * @param [out] u_len solution dimension
     * @param [out] du_row number of rows of the derivative
     * @param [out] du_col number of columns of the derivative
     */
    void GetSolDimensions(uint64_t &u_len, uint64_t &du_row, uint64_t &du_col) const;

    /**
     * @brief Calculates the approximation error at a point
     * @param [in] data material data of the integration point
     * @param [out] errors calculated errors
     */
    void Errors(const TPZMaterialDataT<STATE> &data, TPZVec<REAL> &errors);

    /** @brief Fills in the name of the errors that are computed */
    virtual void ErrorNames(TPZVec<std::string> &names) const;

    /*
     * @brief fill requirements for volumetric contribute
     */
    void FillDataRequirements(TPZMaterialData &data) const;

    /*
     * @brief fill requirements for boundary contribute
     */
    void FillBoundaryConditionDataRequirements(int type, TPZMaterialData &data) const;

    /**
     * @brief Returns an unique class identifier
     */
    [[nodiscard]] int ClassId() const override;

    /**
     * @brief Creates another material of the same type
     */
    [[nodiscard]] TPZMaterial *NewMaterial() const override;

    /**
     * @brief Prints data associated with the material.
     */
    void Print(std::ostream & out) const override;

protected:
    /**
     * @brief Problem dimension
     */
    int fDim;
    bool fNoLinearContext = false;
};

#endif