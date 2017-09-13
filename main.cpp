#include <iostream>
#include "database.h"
#include "../../my CS 8/AVL/avl_tree.h"
#include "table.h"
#include <vector>
#include "record.h"
#include "index_tree.h"
#include "index_pair.h"
using namespace std;

void test_table();
void test_record();
void test_btree();

int main()
{
    Database d;
    d.run();

    return 0;
}

void test_table(){
    //    Table t;
    //    t.load_fieldNames("firstName", 1);
    //    t.load_fieldNames("lastName", 1);
    //    t.load_fieldNames("age", 2);
    //    t.load_record("Kevin , Yao , 20");
    //    Record r ("Kevin , Yao , 20");
    //    cout << r << endl;
    //    cout << *t.file_record.at(0);
    //    t.print();
    //    Index_Tree* temp = t.dictionary["firstName"];
    //    temp->print();
    //    cout << *temp;

}

void test_record(){
    //    Testing copy constructor for selecting certain elements in a Record --------
    //    vector <string> preserve;
    //    preserve.push_back("firstName");
    //    Record r1 ("Kevin , Yao , 20");
    //    Record r2 ("Linn , Mattison , 22");
    //    r1.insert("firstName");
    //    r1.insert("lastName");
    //    r1.insert("age");
    //    r2.insert("firstName");
    //    r2.insert("lastName");
    //    r2.insert("age");
    //    r2.select_fields(preserve);
    //    Record* r3 = new Record(r2);
    //    cout << "R1 : " << r1 << endl;
    //    cout << "R2: "<< r2 <<endl;
    //    cout << "R3 : " << *r3 << endl;
}

void test_btree()
{
    //    Testing the GREATER THAN AND SEARCH FUNCTION OF A TREE-------
    //    Index_Pair* pair = new Index_Pair ("20");
    //    pair->insert_index(1);
    //    pair->insert_index(2);
    //    pair->insert_index(3);
    //    Index_Pair* pair2 = new Index_Pair ("21");
    //    pair2->insert_index(5);
    //    Index_Pair* pair3 = new Index_Pair ("23");
    //    pair3->insert_index(4);

    //    Index_Pair* pair4 = new Index_Pair ("22");
    //    Index_Pair* pair5 = new Index_Pair ("25");

    //    Index_Tree t ("lastNames", 1);
    //    t.insert(pair);
    //    t.insert(pair2);
    //    t.insert(pair3);
    //    t.tree.book_print(1);
    //    t.insert(pair4);
    //    t.tree.book_print(1);
    //    t.insert(pair5);
    //    t.tree.book_print(1);
    ////    tree.tree.in_order_print();
    ////    tree.tree.reverse_print();
    //    cout << endl;
    //    vector<Index_Pair*> GREATER ;
    //    t.tree.in_order_for_greater(GREATER, pair2);
    //    cout << "SIZE:" << GREATER.size() << endl;
    //    for(int i = 0; i < GREATER.size(); i++){
    //        cout << *GREATER.at(i) << endl;
    //    }

    //    t.tree.in_order_print(pair3);

    //    if(tree.search_Tree(pair3)){
    //        cout << "TRUE" << endl;
    //    }
}
