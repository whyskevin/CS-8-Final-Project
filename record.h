#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <cstring>
#include "entry.h"
#include <vector>

using namespace std;

struct Record{
    Record();
    Record(Record*& copyMe);
    Record(string fromFile);
    void set_index(int i);

    void insert(string column_name);

    int returnIndex();

    //Select Method--------
    void select_fields(vector <string> preserve_me);

    friend ostream& operator <<(ostream& out, const Record& printMe);

    //Variables
    int index;
    vector <string> fieldNames;
    vector <string> fields;
};

Record::Record(){}

Record::Record(Record*& copyMe){
//    cout << "COPY" << endl;
    vector<string> fN (copyMe->fieldNames);
    vector<string> f (copyMe->fields);
    fieldNames = fN;
    fields = f;
}

Record::Record(string fromFile){
    char str[1024];
    strcpy(str, fromFile.c_str());
    char *literal;
    literal = strtok(str, " ,()'");
    while(literal != NULL){
        fields.push_back(literal);
        literal = strtok(NULL, " ,()'");
    }
}

void Record::set_index(int i){
    index = i;
}

int Record::returnIndex(){
    return index;
}

void Record::insert(string column_name){
    fieldNames.push_back(column_name);
}

void Record::select_fields(vector <string> preserve_me){
    vector <string> temp_fields;
    for(unsigned int i = 0; i < preserve_me.size(); i++){
        for(unsigned int j = 0; j < fieldNames.size(); j++){
            if(preserve_me.at(i) == fieldNames.at(j)){
                //Then we know that j is what we want to keep
                temp_fields.push_back(fields.at(j)); //Then temp_fields will take what we want to preserve from the fields
            }
        }
    }
    //After this, temp_fields is reassigned as the member "fields" as is the fieldNames
    fieldNames.clear();
    fieldNames = preserve_me;
    fields.clear();
    fields = temp_fields;
}


ostream& operator <<(ostream& out, const Record& printMe){
    unsigned int size = (unsigned int) printMe.fields.size();
    for(unsigned int i = 0; i < size; i++){
        out << printMe.fields.at(i);
        if(i < size-1){
           out << " , ";
        }
    }
    out << endl;
    return out;
}

#endif // RECORD_H
