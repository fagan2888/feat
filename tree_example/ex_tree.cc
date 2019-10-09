#include <algorithm>
#include <string>
#include <iostream>
#include "tree.hh"
#
using namespace std;

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

        virtual int evaluate( int x=0, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0) = 0;
        /* { */
        /*     cout << "evaluate!!\n"; */
        /*     return 1; */
        /* } */
};

class NodeVariable : public Node
{
    public:
        int value;
        NodeVariable(){name = "x"; this->value = 1;}
        NodeVariable(int value){name = "x"; this->value = value;}

        int evaluate( int x=0, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            return  this->value;
        }
};

class NodeAdd : public Node
{
    public:
        NodeAdd(){name = "add";}
        int evaluate( int x=0, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            return  child1->eval(x) + child2->eval(x);
        }
};

class NodeTimes : public Node
{
    public:
        NodeTimes(){name = "times";}
        int evaluate( int x=0, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            return  child1->eval(x) * child2->eval(x);
        }
};

class NodeSum : public Node
{
    public:
        NodeSum(){name = "sum";}
        int evaluate( int x=0, tree_node_<Node*>* child1=0, 
                tree_node_<Node*>* child2=0)
        {
            int sum=0;
            /* tree<Node*>::sibling_iterator start(child1); */
            /* tree<Node*>::sibling_iterator end(child2); */
            /* cout << "looping thru sum\n"; */
            /* std::for_each(start,end, */ 
            /*     [&sum,&x](tree<Node*>::sibling_iterator const& elem) */
            /*     { */
            /*         sum += elem.node->eval(x); */
            /*     } */
            /* ); */
            tree_node_<Node*>* sib = child1;
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
