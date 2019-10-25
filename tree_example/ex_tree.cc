#include <algorithm>
#include <string>
#include <iostream>
#include "tree.hh"
#include "state.h"
#include "node.h"

using namespace std;



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
