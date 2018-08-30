/* FEAT
copyright 2017 William La Cava
license: GNU/GPL v3
*/
#include "n_exponent.h"
    	  	
namespace FT{

    NodeExponent::NodeExponent(vector<double> W0)
    {
	    name = "^";
	    otype = 'f';
	    arity['f'] = 2;
	    arity['b'] = 0;
	    complexity = 4;

        if (W0.empty())
        {
            for (int i = 0; i < arity['f']; i++) {
                W.push_back(r.rnd_dbl());
            }
        }
        else
            W = W0;
    }

    /// Evaluates the node and updates the stack states. 
    void NodeExponent::evaluate(const Data& data, Stacks& stack)
    {
	    /* ArrayXd x1 = stack.pop<double>(); */
        /* ArrayXd x2 = stack.pop<double>(); */

        stack.push<double>(limited(pow(this->W[0] * stack.pop<double>(), 
                                       this->W[1] * stack.pop<double>())));
    }

    /// Evaluates the node symbolically
    void NodeExponent::eval_eqn(Stacks& stack)
    {
        stack.push<double>("(" + stack.popStr<double>() + ")^(" + stack.popStr<double>() + ")");
    }

    ArrayXd NodeExponent::getDerivative(Trace& stack, int loc)
    {
        ArrayXd& x1 = stack.get<double>()[stack.size<double>()-1];
        ArrayXd& x2 = stack.get<double>()[stack.size<double>()-2];
        
        switch (loc) {
            case 3: // Weight for the power
                return limited(pow(this->W[0] * x1, this->W[1] * x2) * limited(log(this->W[0] * x1)) * x2);
            case 2: // Weight for the base
                return limited(this->W[1] * x2 * pow(this->W[0] * x1, this->W[1] * x2) / this->W[0]);
            case 1: // Power
                return limited(this->W[1]*pow(this->W[0] * x1, this->W[1] * x2) * limited(log(this->W[0] * x1)));
            case 0: // Base
            default:
                return limited(this->W[1] * x2 * pow(this->W[0] * x1, this->W[1] * x2) / x1);
        } 
    }
    
    NodeExponent* NodeExponent::clone_impl() const { return new NodeExponent(*this); }
      
    NodeExponent* NodeExponent::rnd_clone_impl() const { return new NodeExponent(); } 
}
