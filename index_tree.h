#ifndef INDEX_TREE_H
#define INDEX_TREE_H
#include <iostream>
#include <string>
#include "../../my CS 8/set/set.h"
#include "index_pair.h"

using namespace std;

class Index_Tree{
public:
    Index_Tree();
    Index_Tree(string field_name, unsigned int TYPE);
    ~Index_Tree();

    void insert(Index_Pair*& insertMe);
    bool search_Tree(Index_Pair * pair);
    friend ostream& operator << (ostream& out, Index_Tree& printme);

    //private:
    unsigned int DATA_TYPE;
    Index_Pair* found_search;
    string fieldName;
    BTree_Set <Index_Pair*> tree;
};

Index_Tree::Index_Tree(){
    DATA_TYPE = 0;
    found_search = NULL;
}

Index_Tree::~Index_Tree(){
    DATA_TYPE = 0;
    found_search = NULL;
    //    delete tree;
}

Index_Tree::Index_Tree(string field_name, unsigned int TYPE){
    //    cout << "NEW TREE:"<< field_name << endl;
    fieldName = field_name;
    DATA_TYPE = TYPE;
    found_search = NULL;
}

void Index_Tree::insert(Index_Pair*& insertMe){
    tree.insert(insertMe);
//        tree.book_print(1);// Prints the entire tree
}

bool Index_Tree::search_Tree(Index_Pair *pair){
    found_search = tree.search(pair);
    if(found_search != NULL){
//        cout << "FOUND SEARCH" <<endl;
        return true;
    }else{
//        cout << "FOUND SEARCH NULL "<< endl;
        return false;
    }
}


ostream& operator << (ostream& out, Index_Tree& printme){
    string type;
    switch((int) printme.DATA_TYPE){
    case 1:
        type = "STRING";
        break;
    case 2:
        type = "INT";
        break;
    case 3:
        type = "DOUBLE";
        break;
    }
    string column_plus_type = printme.fieldName + " " + type;
    out << column_plus_type;
    return out;
}
#endif // INDEX_TREE_H
