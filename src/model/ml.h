/* FEAT
copyright 2017 William La Cava
license: GNU/GPL v3
*/
#ifndef ML_H
#define ML_H

//external includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#include <shogun/base/some.h>                                                                       
#include <shogun/base/init.h>
#include <shogun/machine/Machine.h>
#include <shogun/lib/common.h>                                                                      
#include <shogun/labels/RegressionLabels.h>                                                         
#include <shogun/labels/MulticlassLabels.h>
#include <shogun/features/Features.h>
#include <shogun/machine/LinearMachine.h>
#include <shogun/regression/LeastAngleRegression.h>
#include <shogun/regression/LinearRidgeRegression.h>
//#include <shogun/machine/RandomForest.h>
#include <shogun/regression/svr/LibLinearRegression.h>
/* #include <shogun/classifier/svm/LibLinear.h> */
#include <shogun/ensemble/MeanRule.h>
#include <shogun/ensemble/MajorityVote.h>
#include <shogun/machine/LinearMulticlassMachine.h>
#pragma GCC diagnostic pop
#include <cmath>
// internal includes
#include "shogun/MyCARTree.h"
#include "shogun/MulticlassLogisticRegression.h"
#include "shogun/MyMulticlassLibLinear.h"
#include "shogun/MyLibLinear.h"
#include "shogun/MyRandomForest.h"
#include "../params.h"
#include "../eval/scorer.h"
#include "../util/utils.h"


// stuff being used
using std::string;
using std::dynamic_pointer_cast;
using std::shared_ptr; 
using std::make_shared;
using std::cout;
namespace sh = shogun;
using sh::EProblemType; 
using sh::EProbHeuristicType;
using sh::CBinaryLabels;
using sh::CMulticlassLabels;
using sh::CLabels;

namespace FT{

    using namespace Util;
     
    /**
     * @namespace FT::Model
     * @brief namespace containing ML methods used in Feat
     */
    namespace Model{
        
        enum ML_TYPE {
                      LARS,   // Least Angle Regression
                      Ridge,  // Ridge Regression
                      RF,     // Random Forest
                      SVM,    // Support Vector Machines
                      CART,   // Classification and Regression Trees
                      LR,      // l2-penalized Logistic Regression
                      L1_LR      // L1-penalized Logistic Regression
                     };
        /*!
         * @class ML
         * @brief class that specifies the machine learning algorithm to pair with Feat. 
         */
        class ML 
        {
            public:
            	
                /* ML(const Parameters& params, bool norm=true); */
                ML(string ml="LinearRidgeRegression", bool norm=true, 
                        bool classification = false, int n_classes = 2);

                void init();
            
                ~ML();

                // map ml string names to enum values. 
                std::map<string, ML_TYPE> ml_hash;
                // return vector of weights for model. 
                vector<float> get_weights();
                
                // train ml model on X and return label object. 
                shared_ptr<CLabels> fit(MatrixXf& X, VectorXf& y, 
                        const Parameters& params, bool& pass,
                        const vector<char>& dtypes=vector<char>());

                // train ml model on X and return estimation y. 
                VectorXf fit_vector(MatrixXf& X, VectorXf& y, const Parameters& params, bool& pass,
                             const vector<char>& dtypes=vector<char>());

                // predict using a trained ML model, returning a label object. 
                shared_ptr<CLabels> predict(MatrixXf& X, bool print=false);
                
                // predict using a trained ML model, returning a vector of predictions. 
                VectorXf predict_vector(MatrixXf& X);
     
                // predict using a trained ML model, returning a vector of predictions. 
                ArrayXXf predict_proba(MatrixXf& X);
               
                /// utility function to convert CLabels types to VectorXd types. 
                VectorXf labels_to_vector(const shared_ptr<CLabels>& labels);

                /* VectorXd predict(MatrixXd& X); */
                // set data types (for tree-based methods)            
                void set_dtypes(const vector<char>& dtypes);
                ///returns bias for linear machines  
                float get_bias();
                ///tune algorithm parameters
                void tune(const Data& d, const Parameters& params);

                shared_ptr<sh::CMachine> p_est;     ///< pointer to the ML object
                ML_TYPE ml_type;                    ///< user specified ML type
                string ml_str; ///< user specified ML type (string)
                sh::EProblemType prob_type; ///< type of learning problem; binary, multiclass 
                                                    ///  or regression 
                Normalizer N;                       ///< normalization
                int max_train_time; ///< max seconds allowed for training
                bool normalize; ///< control whether ML normalizes its input 
                                /// before training
                float C;        // regularization parameter

            private:
                vector<char> dtypes; 
        };

    }
}


#endif
