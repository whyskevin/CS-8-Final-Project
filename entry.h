#ifndef ENTRY_H
#define ENTRY_H

#include <iostream>
#include <string>

using namespace std;

//Entries are entries of fields within the table
struct Entry{
    Entry();
    Entry(string literal, unsigned int TYPE);

    unsigned int return_data_type();
    friend ostream& operator << (ostream& out, const Entry &ntry);

    //Entries can have operations
    unsigned int DATA_TYPE;
    string data;
};

Entry::Entry(){
    data = "";
    DATA_TYPE = 0;
}

Entry::Entry(string literal, unsigned int TYPE){
    DATA_TYPE = TYPE;
    data = literal;
}

unsigned int Entry::return_data_type(){
    return DATA_TYPE;
}

#endif // ENTRY_H
