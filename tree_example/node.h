#include "tree.hh"
#include <string>
#include "data.h"
using std::cout;
using std::string;

class Node
{
    public:
        string name;
        Node(string name)
        {
            this->name = name;
        }
        Node() = default;
        ~Node() = default;
        Node(const Node&) = default;

        Node& operator=(Node && other) = default;

        /* bool operator==(Node && other){return false;}; */
        bool operator==(const Node & other){return this->name==other.name;};

        void swap(Node& b)
        {
            using std::swap;
            swap(this->name,b.name);
        }

        /* virtual T evaluate(const Data& d, tree_node_<Node*>* child1=0, */ 
        /*         tree_node_<Node*>* child2=0) = 0; */
};

template <typename T>
class NodeVariable : public Node
{
    public:
        int index;
        NodeVariable(){name = "x"; this->value = 1;}
        NodeVariable(int value){name = "x"; this->index = value;}

        T evaluate(const Data& d, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            
            return d.get<T>(this->value);
        }
};

template <typename T>
class NodeAdd : public Node
{
    public:
        NodeAdd(){name = "add";}
        T evaluate(const Data& d, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            return  child1->eval(d) + child2->eval(d);
        }
};

template <typename T>
class NodeTimes : public Node
{
    public:
        NodeTimes(){name = "times";}
        T evaluate(const Data& d, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            /* return  child1->eval(x) * child2->eval(x); */
            return child1->eval(d) + child2->eval(d);
        }
};

template <typename T>
class NodeSum : public Node
{
    public:
        NodeSum(){name = "sum";}
        T evaluate(const Data& d, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            T sum=0;
            tree_node_<Node*>* sib = child1;
            while (sib != 0)
            {
                cout << "+= " << sib->data->name << "\n";
                sum += sib->eval(d);
                sib = sib->next_sibling;
            }
            return  sum;
        }
};

class NodeTree: public tree<Node*>
{
    public:
       /* void make_program(const NodeVector& functions, */ 
       /*                     const NodeVector& terminals, int max_d, */  
       /*                     const vector<float>& term_weights, */ 
       /*                     const vector<float>& op_weights, */ 
       /*                     char otype, const vector<char>& term_types){}; */
       void mutate(){}; 
       void cross(NodeTree& other){}; 

};
