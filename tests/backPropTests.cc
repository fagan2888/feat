#include "testsHeader.h"

/**
Notes
Add import from util for 2d Gauss
**/

bool isNodeDx(Node* n) {
	return NULL != dynamic_cast<NodeDx*>(n); 
}

ArrayXd limited(ArrayXd x)
{
    //cout << "\n From limited function\n INPUT \n"<<x<<"\n";
    x = (isnan(x)).select(0,x);
    x = (x < MIN_DBL).select(MIN_DBL,x);
    x = (x > MAX_DBL).select(MAX_DBL,x);
    
    //cout << "\n From limited function\n OUTPUT \n"<<x<<"\n";
    return x;
};

/*ArrayXd evaluateProgram(NodeVector& program, MatrixXd data, VectorXd labels)

	FT::Stacks stack;

	std::cout << "Running evaluation.\n";
	// Iterate through program and calculate results 
	for (const auto& n : program) {
		std::cout << "Running: " << n->name << "\n";
		n->evaluate(data, labels, z, stack);
		std::cout << "result:" << stack_f[stack_f.size() - 1] << "\n--";
	}

	std::cout << "Returning result.\n" << stack_f[stack_f.size() - 1] << "\n-----------------\n";
	return pop(stack_f);
}*/

Node* parseToNode(std::string token) {
	if (token == "+") {
    	return new FT::NodeAdd({1.0, 1.0});
    } else if (token == "-") {
    	return new FT::NodeSubtract();
    } else if (token == "/") {
    	return new FT::NodeDivide();
    } else if (token == "*") {
    	return new FT::NodeMultiply();
    } else if (token == "cos") {
    	return new FT::NodeCos({1.0});
    } else if (token == "sin") {
    	return new FT::NodeSin();
    } else if (token == "x0") {
    	return new FT::NodeVariable<double>(0);
    } else if (token == "x1") {
    	return new FT::NodeVariable<double>(1);
    } else if (token == "exponent") {
    	return new FT::NodeExponent();
    } else if (token == "max") {
    	return new FT::NodeMax();
    } else if (token == "xor") {
    	return new FT::NodeXor();
    } else if (token == "step") {
    	return new FT::NodeStep();
    }
}

class TestBackProp
{
    public:
    
        TestBackProp(int iters=1000, double n=0.1, double a=0.9)
        {
            this->iters = iters;
		    this->n = n;
            this->epT = 0.01*this->n;   // min learning rate
		    this->a = a;
		    this->engine = new FT::AutoBackProp("mse", iters, n, a);
        }
        
        void run(Individual& ind, const Data& d,
                            const Parameters& params)
        {
            vector<size_t> roots = ind.program.roots();
            double min_loss;
            double current_loss, current_val_loss;
            vector<vector<double>> best_weights;
            // batch data
            MatrixXd Xb, Xb_v;
            VectorXd yb, yb_v;
            std::map<string, std::pair<vector<ArrayXd>, vector<ArrayXd> > > Zb, Zb_v;
            /* cout << "y: " << d.y.transpose() << "\n"; */ 
            Data db(Xb, yb, Zb, params.classification);
            Data db_val(Xb_v, yb_v, Zb_v, params.classification);
            db_val.set_validation();    // make this a validation set
            d.get_batch(db_val, params.bp.batch_size);     // draw a batch for the validation data
            
            int patience = 3;   // number of iterations to allow validation fitness to not improve
            int missteps = 0;

            this->epk = n;  // starting learning rate
            /* params.msg("running backprop on " + ind.get_eqn(), 2); */
            params.msg("=========================",3);
            params.msg("Iteration,Train Loss,Val Loss,Weights",3);
            params.msg("=========================",3);
            for (int x = 0; x < this->iters; x++)
            {
                cout << "\n\nIteration " << x << "\n";
                /* cout << "get batch\n"; */
                // get batch data for training
                d.get_batch(db, params.bp.batch_size); 
                /* cout << "db.y: " << db.y.transpose() << "\n"; */ 
                // Evaluate forward pass
                MatrixXd Phi; 
                /* cout << "forward pass\n"; */
                vector<Trace> stack_trace = engine->forward_prop(ind, db, Phi, params);

                // Evaluate backward pass
                size_t s = 0;
                for (int i = 0; i < stack_trace.size(); ++i)
                {
                    while (!ind.program.at(roots[s])->isNodeDx()) ++s;
                    cout << "running backprop on " << ind.program_str() << " from "
                          << roots.at(s) << " to " 
                         << ind.program.subtree(roots.at(s)) << "\n";
                    
                    backprop(stack_trace.at(i), ind.program, ind.program.subtree(roots.at(s)), 
                            roots.at(s), 1.0, Phi.row(0), db, params.class_weights);
                }

                current_val_loss = metrics::squared_difference(db_val.y, Phi.row(0)).mean();
                
                if (x==0 || current_val_loss < min_loss)
                {
                    params.msg("current value loss: " + std::to_string(current_val_loss), 3);
                    min_loss = current_val_loss;
                    best_weights = ind.program.get_weights();
                    params.msg("new min loss: " + std::to_string(min_loss), 3);
                }
                else
                {
                    ++missteps;
                    cout << "missteps: " << missteps << "\n";
                    params.msg("current value loss: " + std::to_string(current_val_loss), 3);
                    params.msg("new min loss: " + std::to_string(min_loss), 3);
                    params.msg("",3);           // update learning rate
                }
                
                /* double alpha = double(x)/double(iters); */
                /* this->epk = (1 - alpha)*this->epk + alpha*this->epT; */  
                cout << "Verbosity is " << params.verbosity << "\n";
                if (params.verbosity>2)
                {
                    cout << x << ", " 
                     << current_loss << ", " 
                     << current_val_loss << ", ";
                     engine->print_weights(ind.program);
                }
            }
            params.msg("",3);
            params.msg("=========================",3);
            params.msg("done=====================",3);
            params.msg("=========================",3);
            ind.program.set_weights(best_weights);
        }
        
        void backprop(Trace& stack, NodeVector& program, int start, int end, 
                                double Beta, const VectorXd& yhat, 
                                const Data& d,
                                vector<float> sw)    
        {
            /* cout << "Backward pass \n"; */
            vector<ArrayXd> derivatives;
            // start with derivative of cost function wrt ML output times dyhat/dprogram output, which
            // is equal to the weight the model assigned to this subprogram (Beta)
            // push back derivative of cost function wrt ML output
            /* cout << "Beta: " << Beta << "\n"; */ 
            derivatives.push_back(metrics::d_squared_difference(d.y, yhat).array() * Beta); //*phi.array()); 
            /* cout << "Cost derivative: " << derivatives[derivatives.size() -1 ]<< "\n"; */ 
            // Working according to test program */
            /* pop<ArrayXd>(&f_stack); // Get rid of input to cost function */
            vector<FT::BP_NODE> executing; // Stores node and its associated derivatves
            // Currently I don't think updates will be saved, might want a pointer of nodes so don't 
            // have to restock the list
            // Program we loop through and edit during algorithm (is this a shallow or deep copy?)
            /* cout << "copy program \n"; */
            vector<Node*> bp_program = program.get_data(start, end);         
            /* cout << "Initializing backprop systems.\n"; */
            while (bp_program.size() > 0) {
                /* cout << "Size of program: " << bp_program.size() << "\n"; */
                Node* node = pop<Node*>(&bp_program);
                /* cout << "Evaluating: " << node->name << "\n"; */
                /* cout << "executing stack: " ; */ 
                /* for (const auto& bpe : executing) cout << bpe.n->name << " " ; cout << "\n"; */
                /* cout << "bp_program: " ; */ 
                /* for (const auto& bpe : bp_program) cout << bpe->name << " " ; cout << "\n"; */
                /* cout << "derivatives size: " << derivatives.size() << "\n"; */ 
                vector<ArrayXd> n_derivatives;

                if (node->isNodeDx() && node->visits == 0 && node->arity['f'] > 0) {
                    NodeDx* dNode = dynamic_cast<NodeDx*>(node); // Could probably put this up one and have the if condition check if null
                    /* cout << "evaluating derivative\n"; */
                    // Calculate all the derivatives and store them, then update all the weights and throw away the node
                    for (int i = 0; i < node->arity['f']; i++) {
                        dNode->derivative(n_derivatives, stack, i);
                    }
                    /* cout << "updating derivatives\n"; */
                    dNode->update(derivatives, stack, this->epk, this->a);
                    // dNode->print_weight();
                    /* cout << "popping input arguments\n"; */
                    // Get rid of the input arguments for the node
                    for (int i = 0; i < dNode->arity['f']; i++) {
                        pop<ArrayXd>(&stack.f);
                    }
                    for (int i = 0; i < dNode->arity['c']; i++) {
                        pop<ArrayXi>(&stack.c);
                    }
                    for (int i = 0; i < dNode->arity['b']; i++) {
                        pop<ArrayXb>(&stack.b);
                    }
                    if (!n_derivatives.empty()) {
                        derivatives.push_back(pop_front<ArrayXd>(&n_derivatives));
                    }

                    executing.push_back({dNode, n_derivatives});
                }
                /* else */
                /*     cout << "not NodeDx or visits reached or no floating arity\n"; */
                /* cout << "next branch\n"; */
                // Choosing how to move through tree
                if (node->arity['f'] == 0 || !node->isNodeDx()) {
            
                    // Clean up gradients and find the parent node
                    /* cout << "popping derivatives\n"; */
                    if (!derivatives.empty())
                        pop<ArrayXd>(&derivatives);	// TODO check if this fixed
                    engine->next_branch(executing, bp_program, derivatives);
                } 
                else 
                {
                    node->visits += 1;
                    if (node->visits > node->arity['f']) 
                    {
                        engine->next_branch(executing, bp_program, derivatives);
                    }
                }
            }

            // point bp_program to null
            for (unsigned i = 0; i < bp_program.size(); ++i)
                bp_program[i] = nullptr;

            /* cout << "Backprop terminated\n"; */
            //print_weights(program);
        }
        
    private:
        int iters;
        double n;
        double epT;
        double a;
        double epk;
        FT::AutoBackProp* engine;
        
};

FT::NodeVector programGen() {
	FT::NodeVector program;
	std::string txt = "x0 x1 +";

	char ch = ' ';
	size_t pos = txt.find( ch );
    size_t initialPos = 0;

    // Decompose statement
    std::string token;
    while( pos != std::string::npos ) {
    	token = txt.substr( initialPos, pos - initialPos );

        program.push_back(unique_ptr<Node>(parseToNode(token)));

        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    token = txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 );
    program.push_back(unique_ptr<Node>(parseToNode(token)));

    return program;
}

void testDummyProgram(FT::NodeVector p0, int iters) {
	
	std::cout << "Testing program: [";
	
	for (const auto& n : p0) {
		std::cout << n->name << ", ";
	}
	std::cout << "]\n";
	
	std::cout << "Number of iterations are "<< iters <<"\n";

	// Create input data and labels
	MatrixXd x(2, 10);
	VectorXd y(10);
	x.row(0) << -0.44485052, -0.49109715,  0.88231917,  0.94669031, -0.80300709,
       -0.581858  , -0.91693663, -0.98437617, -0.52860637, -0.89671113;
    x.row(1) << 0.89560483,  0.87110481, -0.47065155,  0.32214509,  0.59596947,
        0.81329039,  0.39903285,  0.17607827,  0.84886707, -0.44261626;

    y << 1.79711347,  1.63112011,  0.35268371,  2.85981589,  0.18189424,
        1.27615517, -0.63677472, -1.44051753,  1.48938848, -3.12127104;
	    

    std::map<string, std::pair<vector<ArrayXd>, vector<ArrayXd> > > Z; 
	// Params
	double learning_rate = 0.1;
    int bs = 1; 
    FT::Individual ind;
    ind.program = p0;
    FT::Feat feat;
    feat.set_verbosity(3);
    
    feat.set_shuffle(false);
                      
    Data data(x, y, Z);
    
	TestBackProp* engine = new TestBackProp(iters, learning_rate, 0);	

    engine->run(ind, data, feat.params); // Update pointer to NodeVector internally

    std::cout << "test program returned:\n";
	for (const auto& n : ind.program) {
		std::cout << n->name << ": ";
		NodeDx* nd = dynamic_cast<NodeDx*>(n.get());
		if (nd != NULL) {
			std::cout << " with weight";
			for (int i = 0; i < nd->arity['f']; i++) {
				std::cout << " " << nd->W[i];
			}
		}
		std::cout << "\n";
	}

	// Make sure internal NodeVector updated
}

TEST(BackProp, DerivativeTest)
{
	Trace trace;
	//vector<ArrayXd> inputs;
	ArrayXd input1(5,1);
	input1(0,0) = 0;
	input1(1,0) = 1;
	input1(2,0) = 2;
	input1(3,0) = 3;
	input1(4,0) = 4;
	ArrayXd input2(5,1);
	input2(0,0) = 4;
	input2(1,0) = 3;
	input2(2,0) = 2;
	input2(3,0) = 1;
	input2(4,0) = 0;
	trace.f.push_back(input2);
	trace.f.push_back(input1);

	// ADD NODE CHECK -------------------------------------------------------------------------------
	NodeDx* toTest = new FT::NodeAdd();
	
	// Derivative wrt to first input
	ArrayXd expectedDerivative(5, 1);
	expectedDerivative(0,0) = toTest->W[0];
	expectedDerivative(1,0) = toTest->W[0];
	expectedDerivative(2,0) = toTest->W[0];
	expectedDerivative(3,0) = toTest->W[0];
	expectedDerivative(4,0) = toTest->W[0];
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 
              0.0001);
		
	expectedDerivative(0,0) = toTest->W[1];
	expectedDerivative(1,0) = toTest->W[1];
	expectedDerivative(2,0) = toTest->W[1];
	expectedDerivative(3,0) = toTest->W[1];
	expectedDerivative(4,0) = toTest->W[1];

	// Derivative wrt to second input
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 
              0.0001);

	// Derivative wrt to first weight
	expectedDerivative(0,0) = 0;
	expectedDerivative(1,0) = 1;
	expectedDerivative(2,0) = 2;
	expectedDerivative(3,0) = 3;
	expectedDerivative(4,0) = 4;
	
    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 3).matrix()).norm(), 0.0001);	
    
	// Derivative wrt to second weight
	expectedDerivative(0,0) = 4;
	expectedDerivative(1,0) = 3;
	expectedDerivative(2,0) = 2;
	expectedDerivative(3,0) = 1;
	expectedDerivative(4,0) = 0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 2).matrix()).norm(), 0.0001);
    

	// SUB NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeSubtract({1,1});
    
	expectedDerivative(0,0) = 1;
	expectedDerivative(1,0) = 1;
	expectedDerivative(2,0) = 1;
	expectedDerivative(3,0) = 1;
	expectedDerivative(4,0) = 1;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = -1;
	expectedDerivative(1,0) = -1;
	expectedDerivative(2,0) = -1;
	expectedDerivative(3,0) = -1;
	expectedDerivative(4,0) = -1;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
		
    expectedDerivative(0,0) = 4;
	expectedDerivative(1,0) = 3;
	expectedDerivative(2,0) = 2;
	expectedDerivative(3,0) = 1;
	expectedDerivative(4,0) = 0;

    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 2).matrix()).norm(), 0.0001);
    
    expectedDerivative(0,0) = -0;
	expectedDerivative(1,0) = -1;
	expectedDerivative(2,0) = -2;
	expectedDerivative(3,0) = -3;
	expectedDerivative(4,0) = -4;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 3).matrix()).norm(), 0.0001);

	// MULT NODE CHECK-------------------------------------------------------------------------------
	toTest = new FT::NodeMultiply({1,1});
	
	expectedDerivative(0,0) = 4;
	expectedDerivative(1,0) = 3;
	expectedDerivative(2,0) = 2;
	expectedDerivative(3,0) = 1;
	expectedDerivative(4,0) = 0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = 0;
	expectedDerivative(1,0) = 1;
	expectedDerivative(2,0) = 2;
	expectedDerivative(3,0) = 3;
	expectedDerivative(4,0) = 4;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);

	expectedDerivative(0,0) = 0;
	expectedDerivative(1,0) = 3;
	expectedDerivative(2,0) = 4;
	expectedDerivative(3,0) = 3;
	expectedDerivative(4,0) = 0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 2).matrix()).norm(), 0.0001);
	
    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 3).matrix()).norm(), 0.0001);
    
    // DIV NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeDivide({1,1});
	
	expectedDerivative(0,0) = MAX_DBL;	// Div by 0 (limited to 0)
	expectedDerivative(1,0) = 1.0/1;
	expectedDerivative(2,0) = 1.0/2;
	expectedDerivative(3,0) = 1.0/3;
	expectedDerivative(4,0) = 1.0/4;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	 
	expectedDerivative(0,0) = MIN_DBL;	// Div by 0
	expectedDerivative(1,0) = -3.0/1;
	expectedDerivative(2,0) = -2.0/4;
	expectedDerivative(3,0) = -1.0/9;
	expectedDerivative(4,0) = -0.0/16; 
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = MAX_DBL;	// Div by 0
	expectedDerivative(1,0) = 3.0/1;
	expectedDerivative(2,0) = 2.0/2;
	expectedDerivative(3,0) = 1.0/3;
	expectedDerivative(4,0) = 0.0/4;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 2).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = -MAX_DBL;	//Div by 0
	expectedDerivative(1,0) = -3.0/1;
	expectedDerivative(2,0) = -2.0/2;
	expectedDerivative(3,0) = -1.0/3;
	expectedDerivative(4,0) = -0.0/4;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 3).matrix()).norm(), 0.0001);

	// x^y NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeExponent({1.0,1.0});
	
	expectedDerivative(0,0) = 0 * pow(4,0)/4; 
	expectedDerivative(1,0) = 1 * pow(3,1)/3;
	expectedDerivative(2,0) = 2 * pow(2,2)/2;
	expectedDerivative(3,0) = 3 * pow(1,3)/1;
	expectedDerivative(4,0) = 4 * pow(0,4)/0; // div by 0
	
	ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = 1 * pow(4,0) * log(4); 
    expectedDerivative(1,0) = 1 * pow(3,1) * log(3);
	expectedDerivative(2,0) = 1 * pow(2,2) * log(2);
	expectedDerivative(3,0) = 1 * pow(1,3) * log(1);
	expectedDerivative(4,0) = 1 * pow(0,4) * log(0); // log 0
	
	ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = 0 * pow(4,0)/1;
	expectedDerivative(1,0) = 1 * pow(3,1)/1;
	expectedDerivative(2,0) = 2 * pow(2,2)/1;
	expectedDerivative(3,0) = 3 * pow(1,3)/1;
	expectedDerivative(4,0) = 4 * pow(0,4)/1;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 2).matrix()).norm(), 0.0001);
	
	expectedDerivative(4,0) = 4 * pow(0,4) * log(0); // Log by 0
	expectedDerivative(3,0) = 3 * pow(1,3) * log(1);
	expectedDerivative(2,0) = 2 * pow(2,2) * log(2);
	expectedDerivative(1,0) = 1 * pow(3,1) * log(3);
	expectedDerivative(0,0) = 0 * pow(4,0) * log(4);
	
	ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 3).matrix()).norm(), 0.0001);
	
	// COS NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeCos({1.0});
	
	expectedDerivative(0,0) = -1 * sin(4);
	expectedDerivative(1,0) = -1 * sin(3);
	expectedDerivative(2,0) = -1 * sin(2);
	expectedDerivative(3,0) = -1 * sin(1);
	expectedDerivative(4,0) = -1 * sin(0);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = -4 * sin(4);
	expectedDerivative(1,0) = -3 * sin(3);
	expectedDerivative(2,0) = -2 * sin(2);
	expectedDerivative(3,0) = -1 * sin(1);
	expectedDerivative(4,0) = -0 * sin(0);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
	
	// SIN NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeSin({1.0});
	
	expectedDerivative(0,0) = 1 * cos(4);
	expectedDerivative(1,0) = 1 * cos(3);
	expectedDerivative(2,0) = 1 * cos(2);
	expectedDerivative(3,0) = 1 * cos(1);
	expectedDerivative(4,0) = 1 * cos(0);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = 4 * cos(4);
	expectedDerivative(1,0) = 3 * cos(3);
	expectedDerivative(2,0) = 2 * cos(2);
	expectedDerivative(3,0) = 1 * cos(1);
	expectedDerivative(4,0) = 0 * cos(0);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
	
	// ^3 NODE CHECK  -------------------------------------------------------------------------------
	toTest = new FT::NodeCube({1.0});
	
	expectedDerivative(0,0) = 3 * pow(4,2);
	expectedDerivative(1,0) = 3 * pow(3,2);
	expectedDerivative(2,0) = 3 * pow(2,2);
	expectedDerivative(3,0) = 3 * pow(1,2);
	expectedDerivative(4,0) = 3 * pow(0,2);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = 3 * 64;
	expectedDerivative(1,0) = 3 * 27;
	expectedDerivative(2,0) = 3 *  8;
	expectedDerivative(3,0) = 3 *  1;
	expectedDerivative(4,0) = 3 *  0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
	
	// e^x NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeExponential({1.0});
	
	expectedDerivative(0,0) = 1 * exp(4);
	expectedDerivative(1,0) = 1 * exp(3);
	expectedDerivative(2,0) = 1 * exp(2);
	expectedDerivative(3,0) = 1 * exp(1);
	expectedDerivative(4,0) = 1 * exp(0);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);

	expectedDerivative(0,0) = 4 * exp(4);
	expectedDerivative(1,0) = 3 * exp(3);
	expectedDerivative(2,0) = 2 * exp(2);
	expectedDerivative(3,0) = 1 * exp(1);
	expectedDerivative(4,0) = 0 * exp(0);
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
    
	// GAUS NODE CHECK-------------------------------------------------------------------------------
	toTest = new FT::NodeGaussian({1.0});
	
	expectedDerivative(0,0) = 2 * (1 - 4) * exp(-pow(1 - 4, 2));
	expectedDerivative(1,0) = 2 * (1 - 3) * exp(-pow(1 - 3, 2));
	expectedDerivative(2,0) = 2 * (1 - 2) * exp(-pow(1 - 2, 2));
	expectedDerivative(3,0) = 2 * (1 - 1) * exp(-pow(1 - 1, 2));
	expectedDerivative(4,0) = 2 * (1 - 0) * exp(-pow(1 - 0, 2));
	
    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = 2 * (4 - 1) * exp(-pow(1 - 4, 2));
	expectedDerivative(1,0) = 2 * (3 - 1) * exp(-pow(1 - 3, 2));
	expectedDerivative(2,0) = 2 * (2 - 1) * exp(-pow(1 - 2, 2));
	expectedDerivative(3,0) = 2 * (1 - 1) * exp(-pow(1 - 1, 2));
	expectedDerivative(4,0) = 2 * (0 - 1) * exp(-pow(1 - 0, 2));
	
    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
    
	// LOG NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeLog({1.0});
	
	expectedDerivative(0,0) = 1.0/4;
	expectedDerivative(1,0) = 1.0/3;
	expectedDerivative(2,0) = 1.0/2;
	expectedDerivative(3,0) = 1;
	expectedDerivative(4,0) = MAX_DBL; // Check if this is intended
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = 1;
	expectedDerivative(1,0) = 1;
	expectedDerivative(2,0) = 1;
	expectedDerivative(3,0) = 1;
	expectedDerivative(4,0) = 1;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);

	// LOGIT NODE CHECK------------------------------------------------------------------------------
	toTest = new FT::NodeLogit({1.0});
	
	expectedDerivative(0,0) = (1 * exp(1 * 4))/pow(exp(1 * 4) + 1, 2);
	expectedDerivative(1,0) = (1 * exp(1 * 3))/pow(exp(1 * 3) + 1, 2);
	expectedDerivative(2,0) = (1 * exp(1 * 2))/pow(exp(1 * 2) + 1, 2);
	expectedDerivative(3,0) = (1 * exp(1 * 1))/pow(exp(1 * 1) + 1, 2);
	expectedDerivative(4,0) = (1 * exp(1 * 0))/pow(exp(1 * 0) + 1, 2);
    
    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = (4 * exp(1 * 4))/pow(exp(1 * 4) + 1, 2);
	expectedDerivative(1,0) = (3 * exp(1 * 3))/pow(exp(1 * 3) + 1, 2);
	expectedDerivative(2,0) = (2 * exp(1 * 2))/pow(exp(1 * 2) + 1, 2);
	expectedDerivative(3,0) = (1 * exp(1 * 1))/pow(exp(1 * 1) + 1, 2);
	expectedDerivative(4,0) = (0 * exp(1 * 0))/pow(exp(1 * 0) + 1, 2);
    
    ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
    
	// RELU NODE CHECK------------------------------------------------------------------------------
    toTest = new FT::NodeRelu({1.0});
    
    expectedDerivative(0,0) = 1;
    expectedDerivative(1,0) = 1;
    expectedDerivative(2,0) = 1;
    expectedDerivative(3,0) = 1;
    expectedDerivative(4,0) = 0.01;
    
    ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
    expectedDerivative(0,0) = 4;
    expectedDerivative(1,0) = 3;
    expectedDerivative(2,0) = 2;
    expectedDerivative(3,0) = 1;
    expectedDerivative(4,0) = 0.01;
    
    ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);

	// SQRT NODE CHECK-------------------------------------------------------------------------------
	toTest = new FT::NodeSqrt({1.0});
	
	expectedDerivative(0,0) = 1/(2 * sqrt(4));
	expectedDerivative(1,0) = 1/(2 * sqrt(3));
	expectedDerivative(2,0) = 1/(2 * sqrt(2));
	expectedDerivative(3,0) = 1/(2 * sqrt(1));
	expectedDerivative(4,0) = 1/(2 * sqrt(0)); // divide by zero
    
    ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = 4/(2 * sqrt(4));
	expectedDerivative(1,0) = 3/(2 * sqrt(3));
	expectedDerivative(2,0) = 2/(2 * sqrt(2));
	expectedDerivative(3,0) = 1/(2 * sqrt(1));
	expectedDerivative(4,0) = 0/(2 * sqrt(0)); //divide by zero
	
	ASSERT_LE((limited(expectedDerivative).matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
	
	// ^2  NODE CHECK -------------------------------------------------------------------------------
	toTest = new FT::NodeSquare({1.0});
	
	expectedDerivative(0,0) = 2 * 1 * 4;
	expectedDerivative(1,0) = 2 * 1 * 3;
	expectedDerivative(2,0) = 2 * 1 * 2;
	expectedDerivative(3,0) = 2 * 1 * 1;
	expectedDerivative(4,0) = 2 * 1 * 0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
    
	expectedDerivative(0,0) = 2 * 16;
	expectedDerivative(1,0) = 2 *  9;
	expectedDerivative(2,0) = 2 *  4;
	expectedDerivative(3,0) = 2 *  1;
	expectedDerivative(4,0) = 2 *  0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);

	// TANH NODE CHECK-------------------------------------------------------------------------------
	toTest = new FT::NodeTanh({1.0});
	
	expectedDerivative(0,0) = 0.0013409506830258968799702;
	expectedDerivative(1,0) = 0.00986603716544019127315616968;
	expectedDerivative(2,0) = 0.07065082485316446568624765586105;
	expectedDerivative(3,0) = 0.41997434161402606939449673904170;
	expectedDerivative(4,0) = 1;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 0).matrix()).norm(), 0.0001);
	
	expectedDerivative(0,0) = 4 * 0.0013409506830258968799702;
	expectedDerivative(1,0) = 3 * 0.00986603716544019127315616968;
	expectedDerivative(2,0) = 2 * 0.07065082485316446568624765586105;
	expectedDerivative(3,0) = 1 * 0.41997434161402606939449673904170;
	expectedDerivative(4,0) = 0;
	
	ASSERT_LE((expectedDerivative.matrix() - toTest->getDerivative(trace, 1).matrix()).norm(), 0.0001);
	
}


TEST(BackProp, PropogationTest)
{
    int iters = 100;
	FT::NodeVector program = programGen();
 	cout << "Running with : " << iters << endl << endl;
	testDummyProgram(program, iters);
}
