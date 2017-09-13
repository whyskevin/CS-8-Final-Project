#ifndef HASHNODE_H
#define HASHNODE_H

#include <iostream>
#include <string>
#include "table.h"

using namespace std;

//HashNode will contain a pointer to a Table based on the TABLENAME
struct HashNode{
    HashNode();
    HashNode(string T);
    ~HashNode();

    Table* returnTable();
    string returnTableName();

    void set_name(string name);
    unsigned int hash() const;
    bool empty() const;

    friend bool operator < (const HashNode &first, const HashNode &second);
    friend bool operator >= (const HashNode &first, const HashNode &second);
    friend bool operator == (const HashNode &first, const HashNode &second);
    friend bool operator != (const HashNode &first, const HashNode &second);

    friend ostream& operator << (ostream& out, const HashNode &node);

    unsigned int comparison_hash;
    string tableName;
    Table* tablePtr;
    HashNode* next;
    HashNode* prev;
};

HashNode::HashNode(){
    tableName = "";
    comparison_hash = 0;
    next = NULL;
    prev = NULL;
    tablePtr = NULL;
}

HashNode::HashNode(string T){//Instantiates a new HashNode and a new Table
    tableName = T;
    comparison_hash = hash();
    tablePtr = new Table(T);
    next = NULL;
    prev = NULL;
}

HashNode::~HashNode(){
    tableName = "";
    comparison_hash = 0;
    next = NULL;
    prev = NULL;
    tablePtr = NULL;
}

Table* HashNode::returnTable(){
    return tablePtr;
}

string HashNode::returnTableName()
{
    return tableName;
}

void HashNode::set_name(string name){
    tableName = name;
}

unsigned int HashNode::hash() const //Hashes table name to produce a value
{
    int key = 0;
    for(int i = 0; i < (int)tableName.size(); i++){
        key += (int) tableName[i];
    }
    //        cout << "HashNode: " << key << endl;
    return key;
}

bool HashNode::empty() const{
    return (tableName == "");
}

bool operator < (const HashNode &first, const HashNode &second){ // first < second
    return (first.comparison_hash < second.comparison_hash); //second key is larger than the first
}

bool operator >= (const HashNode &first, const HashNode &second){
//    cout << "************************1111**************" << endl;
//    cout << "FIRST11111" << first << " SECOND" << second << endl;
    return(first.comparison_hash >= second.comparison_hash); //first key is larger than the second

}

bool operator == (const HashNode &first, const HashNode &second){
//    cout << "************************1111**************" << endl;
//    cout << "FIRST11111" << first << " SECOND" << second << endl;
    return (first.comparison_hash == second.comparison_hash); //first key is larger than the second

}

bool operator != (const HashNode &first, const HashNode &second){
//    cout << "************************1111**************" << endl;
//    cout << "FIRST11111" << first << " SECOND" << second << endl;
    return (first.comparison_hash != second.comparison_hash); //first key is larger than the second

}

ostream& operator << (ostream& out, const HashNode &node){
    out << "HashNode Table:" << node.tableName;
//    if(node.next != NULL){
//        out << "-> ??? ->";
//    }
//    out << "|||";

    return out;
}


#endif // HASHNODE_H
