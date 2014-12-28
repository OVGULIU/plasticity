// created: 2014-12-20 0:31:58
// version: develop
// url: git@github.com:prisms-center/IntegrationTools.git
// commit: 3e86b7184d2ec2de450d3668f7dd88dcd0396838

#ifndef PLIBRARY_HH
#define PLIBRARY_HH

#include<cstring>
#include<vector>
#include "IntegrationTools/PFunction.hh"
#include "IntegrationTools/PPieceWise.hh"

namespace PRISMS
{

    /// Library where you can find functions and basis sets
    ///
    namespace PLibrary
    {
        // Use these functions to checkout objects which manage their own memory

        void checkout( std::string name, PSimpleFunction< std::vector<double>, double > &simplefunc);
        void checkout( std::string name, PSimpleFunction< double*, double > &simplefunc);

        void checkout( std::string name, PFunction< std::vector<double>, double > &func);
        void checkout( std::string name, PFunction< double*, double > &func);




        // Use these functions to checkout new 'Base' objects which the user must delete

        void checkout( std::string name, PSimpleBase< std::vector<double>, double > *&simplefunc);
        void checkout( std::string name, PSimpleBase< double*, double > *&simplefunc);

        void checkout( std::string name, PFuncBase< std::vector<double>, double > *&func);
        void checkout( std::string name, PFuncBase< double*, double > *&func);



    }

}


#endif