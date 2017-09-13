#ifndef INDEX_PAIR_H
#define INDEX_PAIR_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

struct Index_Pair{
    Index_Pair();
    Index_Pair(string val);

    void insert_index(int index);
    //Operator overloading
    unsigned int hash_for_comparison() const;

    friend bool operator == ( const Index_Pair &left, const Index_Pair &right);
    friend bool operator > ( Index_Pair &left,  Index_Pair &right);
    friend bool operator < (const Index_Pair &left, const Index_Pair &right);
    friend bool operator <= (const Index_Pair &left, const Index_Pair &right);
    friend bool operator >= ( const Index_Pair &left, const Index_Pair &right);
    friend bool operator != (const Index_Pair &left, const Index_Pair &right);

    void isGreater() ;

    friend ostream& operator <<(ostream& out, const Index_Pair& printMe);

    string value; //Yao
    unsigned int hash;
    vector <int> indices; //0 , 4, 11, 25
    Index_Pair* next;
    Index_Pair* prev;
    bool greater;
    bool less;
};

Index_Pair::Index_Pair(){
    value = "";
    hash = 0;
    next = NULL;
    prev = NULL;
    greater = false;
    less = false;
}

Index_Pair::Index_Pair(string val){
    value = val;
    hash = hash_for_comparison();
    next = NULL;
    prev = NULL;
    greater = false;
    less = false;
}

void Index_Pair::insert_index(int index){
    indices.push_back(index);
}

unsigned int Index_Pair::hash_for_comparison() const{
    int key = 0;
    for(int i = 0; i < (int)value.size(); i++){
        key += (int) value[i];
    }
//    cout << "HashNode: " << key << endl;
    return key;
}

bool operator == ( const Index_Pair &left, const Index_Pair &right){
//    cout << "==" << endl;
//    return (left.hash == right.hash);
        return (left.value == right.value);
//    return(left.value.compare(right.value) == 0);
}

bool operator > ( Index_Pair &left,  Index_Pair &right){
//    cout << ">" << endl;
//    return (left.hash > right.hash);
    return (left.value > right.value);
}

bool operator < (const Index_Pair &left, const Index_Pair &right){
//    cout << "<" << endl;
//    return (left.hash < right.hash);
    return(left.value < right.value);
}

bool operator >= (const Index_Pair &left, const Index_Pair &right){
//    cout << ">=" << endl;
//    return (left.hash >= right.hash);
    return(left.value >= right.value);

}

bool operator <= (const Index_Pair &left, const Index_Pair &right){
//    cout << ">=" << endl;
//    return (left.hash >= right.hash);
    return(left.value <= right.value);

}

bool operator != (const Index_Pair &left, const Index_Pair &right){
//    cout << "!=" <<endl;
//    return (left.hash != right.hash);
    return(left.value != right.value);
}

void Index_Pair::isGreater() {
    greater = true;
    less = false;
}

ostream& operator <<(ostream& out, const Index_Pair& printMe){

    unsigned int size = (unsigned int)printMe.indices.size();
    out << "[" << printMe.value << "|";
    for(unsigned int i = 0; i < size; i++){
        out << printMe.indices.at(i);
        if(i < size-1){
            out << " , ";
        }
    }
    out << "]";
    return out;
}

#endif // INDEX_PAIR_H
