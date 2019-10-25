#include <algorithm>
#include <string>
#include <iostream>
#include "tree.hh"
#include "state.h"
#include "node.h"

using namespace std;


template <typename T>
class NodeAdd : public Node<T>
{
    public:
        NodeAdd(){name = "add";}
        T evaluate(const Data& d, tree_node_<Node<T>*>* child1=0, 
                tree_node_<Node<T>*>* child2=0)
        {
            State& s = child1->eval(d);
            s.set<int>(
                    s.get<int>() 
                    + child2->eval(d).get<int>()
                    );
            return s;
            /* return  child1->eval(x)<> + child2->eval(x); */
        }
};

template <typename T>
class NodeTimes : public Node<T>
{
    public:
        NodeTimes(){name = "times";}
        State evaluate(const Data& d, tree_node_<Node<T>*>* child1=0, 
                tree_node_<Node<T>*>* child2=0)
        {
            /* return  child1->eval(x) * child2->eval(x); */
            State& s = child1->eval(x);
            s[int] *= child2->eval(x)[int];
            return s;
        }
};

template <typename T>
class NodeSum : public Node<T>
{
    public:
        NodeSum(){name = "sum";}
        State evaluate(const Data& d, tree_node_<Node<T>*>* child1=0, 
                tree_node_<Node<T>*>* child2=0)
        {
            T sum=0;
            tree_node_<Node<T>*>* sib = child1;
            while (sib != 0)
            {
                cout << "+= " << sib->data->name << "\n";
                sum += sib->eval(x);
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

int main(int, char **)
{
    
    cout << "oh hey\n";
    NodeTree tr;
    NodeTree::iterator top, one, two, loc, sum, times;

    top=tr.begin();
    one=tr.insert(top, new NodeAdd());
    /* cout << "top: " << (*top)->name << endl; */ 

    cout << "one: " << (*one)->name << endl; 
    two=tr.append_child(one, new NodeVariable(10));
    sum = tr.append_child(one, new NodeSum());
    tr.append_child(sum, new NodeVariable(7));
    tr.append_child(sum, new NodeVariable(3));
    tr.append_child(sum, new NodeVariable(2));
    tr.append_child(sum, new NodeVariable(-1));
    times = tr.append_child(sum, new NodeTimes());
    tr.append_child(times, new NodeVariable(5));
    tr.append_child(times, new NodeVariable(4));

    loc = tr.begin(); 

    while(loc!=tr.end()) 
    {
        for(int i=0; i<tr.depth(loc); ++i)
            cout << " ";
        cout << (*loc)->name << endl;
        ++loc;
        /* cout << endl; */
    }
    cout << "===\n";
    NodeTree::pre_order_iterator poi = tr.begin();
    int out = poi.node->eval(1);
    cout << "output: " << out << endl;
}
