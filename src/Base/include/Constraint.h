#ifndef _CONSTRAINT_H
#define _CONSTRAINT_H

namespace Gauss {
    
    class ConstraintIndex {
    public:
        
        inline ConstraintIndex(unsigned int localId, unsigned int globalId, unsigned int numRows) {
            setIds(localId, globalId, numRows);
        }
        
        inline void setIds(unsigned int localId, unsigned int globalId, unsigned int numRows) {
            m_localId = localId;
            m_globalId = globalId;
            m_numRows = numRows;
        }
        
        inline unsigned int getNumScalarDOF() {
            return m_numRows;
        }
        
        inline unsigned int getNumScalarDOF() const {
            return m_numRows;
        }
        
        inline unsigned int getLocalId() const {
            return m_localId;
        }
        
        inline unsigned int getGlobalId() const {
            return m_globalId;
        }
        
        virtual inline void offsetGlobalId(unsigned int offset) {
            m_globalId += offset;
        }

        
        
    protected:
        
        unsigned int m_localId;
        unsigned int m_globalId;
        unsigned int m_numRows;
        
    private:
    };
    
    template<typename DataType, typename Impl >
    class Constraint
    {
    public:
        template<typename ...Params>
        Constraint(Params ... params) : m_impl(params...), m_index(0,0,m_impl.getNumRows()) {}
        ~Constraint() { }
        
        //value of constraint (supports vector valued constraint functions for points and what not)
        template<typename Vector>
        inline void getFunction(Vector &f,  const State<DataType> &state) {
            m_impl.getFunction(f, state, m_index);
        }
        
        inline ConstraintIndex & getIndex() { return m_index; }
        
        //get DOFs that this constraint is acting on
        auto & getDOF(unsigned int index) {
            return m_impl.getDOF(index);
        }
        
        //how many DOFs are involved in this constraint
        unsigned int getNumDOF() { return m_impl.getNumDOF(); }
        
        template <typename Matrix, unsigned int Operation=0>
        inline void getGradient(Matrix &g,  const State<DataType> &state) {
            m_impl.template getGradient<Matrix,Operation>(g, state, m_index);
        }
        
    protected:
        
        Impl m_impl;
        ConstraintIndex m_index;
    
    private:
    };

}

#endif