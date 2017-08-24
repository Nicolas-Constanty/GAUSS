//
//  UtilitiesEigen.h
//  Gauss
//
//  Created by David Levin on 2/11/17.
//
//

#ifndef UtilitiesEigen_h
#define UtilitiesEigen_h

#include <algorithm>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Utilities.h>
#include <World.h>


//some useful types
namespace Eigen {
    template<typename DataType>
    using Vector3x = Eigen::Matrix<DataType, 3,1>;
    
    template<typename DataType>
    using VectorXx = Eigen::Matrix<DataType, Eigen::Dynamic, 1>;
    
    template<typename DataType>
    using MatrixXx = Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>;
    
    //Comes up a lot in constitutive models
    template<typename DataType>
    using Matrix66x = Eigen::Matrix<DataType, 6,6>;
    
    //useful maps
    template<typename DataType>
    using Map3x = Eigen::Map<Vector3x<DataType> >;
    
    
}

namespace Gauss {
    //state ptr direct to eigen map for a single property (position or velocity)
    template<unsigned int Property, typename World>
    Eigen::Map<Eigen::VectorXd> mapStateEigen(World &world) {
        std::pair<double *, unsigned int> ptr = world.getState().template getStatePtr<Property>();
        return Eigen::Map<Eigen::VectorXd>(ptr.first, ptr.second);
    }

    //state ptr for the whole thing
    template<typename World>
    Eigen::Map<Eigen::VectorXd> mapStateEigen(World &world) {
        std::pair<double *, unsigned int> ptr = world.getState().getStatePtr();
        return Eigen::Map<Eigen::VectorXd>(ptr.first, ptr.second);
    }
    
    template<typename DOF, typename DataType, typename ...SystemTypes, typename ...ForceTypes, typename ...ConstraintTypes>
    inline Eigen::Map<Eigen::VectorXd> mapDOFEigen(DOF &dof, World<DataType,
                                                   std::tuple<SystemTypes...>,
                                                   std::tuple<ForceTypes...>,
                                                   std::tuple<ConstraintTypes...> > &world) {
        std::pair<double *, unsigned int> qPtr = dof.getPtr(world.getState());
        //set position DOF and check
        return Eigen::Map<Eigen::VectorXd>(qPtr.first, dof.getNumScalarDOF());

    }
    
    template<typename DOF, typename DataType>
    inline Eigen::Map<Eigen::VectorXd> mapDOFEigen(DOF &dof, const State<DataType> &state) {
        std::pair<double *, unsigned int> qPtr = dof.getPtr(state);
        //set position DOF and check
        return Eigen::Map<Eigen::VectorXd>(qPtr.first, dof.getNumScalarDOF());
        
    }
    
    //Modal Analysis using Spectra
#ifdef GAUSS_SPECTRA
    //Temp test Spectre
    #include <GenEigsComplexShiftSolver.h>
    #include <SymGEigsSolver.h>
    #include <MatOp/SparseGenMatProd.h>
    #include <MatOp/SparseCholesky.h>
    #include <SymGEigsSolver.h>
    #include <stdexcept>

    //solve sparse generalized eigenvalue problem using spectra
    //solve the gevp Ax = lambda*Bx
    template<typename DataType, int Flags, typename Indices>
    auto generalizedEigenvalueProblem(const Eigen::SparseMatrix<DataType, Flags, Indices> &A,
                                      const Eigen::SparseMatrix<DataType, Flags,Indices> &B,
                                      unsigned int numVecs) {
    
        //Spectra seems to freak out if you use row storage, this copy just ensures everything is setup the way the solver likes
        Eigen::SparseMatrix<DataType> K = A;
        Eigen::SparseMatrix<DataType> M = B;
        
            
        Spectra::SparseGenMatProd<DataType> Aop(K);
        Spectra::SparseCholesky<DataType>   Bop(M);
        
        // Construct eigen solver object, requesting the largest three eigenvalues
        Spectra::SymGEigsSolver<DataType, Spectra::SMALLEST_MAGN, Spectra::SparseGenMatProd<DataType>, Spectra::SparseCholesky<DataType>, Spectra::GEIGS_CHOLESKY > eigs(&Aop, &Bop, numVecs, B.rows()-10);
        
        // Initialize and compute
        eigs.init();
        int nconv = eigs.compute();
        
        // Retrieve results
        if(eigs.info() == Spectra::SUCCESSFUL) {
            return std::make_pair(eigs.eigenvectors(), eigs.eigenvalues());
        } else {
            std::cout<<"Failure: "<<eigs.info()<<"\n";
            exit(1);
            return std::make_pair(eigs.eigenvectors(), eigs.eigenvalues());
        }
        
    }
    
#else
    template<class ...Params>
    void generalizedEigenvalueProblem(Params ...params) {
        std::cout<<"Spectra not installed\n";
    }
#endif

}
#endif /* UtilitiesEigen_h */
