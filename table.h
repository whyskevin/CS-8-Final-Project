#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include "constants.h"
#include "../../my CS 8/MapClass/map.h"
//#include "../../my CS 8/Map-BTree/map.h"
//#include <map>
#include "record.h"
#include <queue>
#include "index_tree.h"

using namespace std;

class Table
{
public:
    //Constructor
    Table();
    Table(string tableName);
    ~Table();

    //Methods
    // Load methods -----
    bool load(string file_name);
    void tokenize_columns(string first_line);
    void load_fieldNames(string fieldName, unsigned int DATA_TYPE);
    void load_record(string fromFile);

    // Select methods -----
    bool column_exists(string name_of_column);
    Index_Tree* return_column(string name_of_column);
    void import_file_record(vector<Record*> importMe);
    void import_index_trees(Index_Tree* importMe);

    Index_Tree* merge(Index_Tree* one, Index_Tree* two);

    // Print methods ----- Want to double as Store functions as well
    void print_all_trees();
    friend ostream& operator <<(ostream& out, Table& printMe);

    //Member variables
    string table_name;
    Map <string, Index_Tree*> dictionary; // Column - Index Tree
    vector <string> preserved_column_names;
    vector <Record*> file_record;
    vector <string> column_names; //Use vector...for index. You've used queues before
};

//Constructor
Table::Table(){
    table_name = "";
}

Table::~Table(){
    table_name = "";
}

Table::Table(string tableName){
    table_name = tableName;
}

//Methods
bool Table::load(string file_name){
    string line;
    ifstream in (file_name);
    if(in.is_open()){
        getline(in,line); //First line will always be field names and data types
        tokenize_columns(line); //Tokenizes all field names/data types and creates Index_Trees of each field
        while(getline(in, line)){ //Then we continue to take all the fields
            load_record(line); //Records are then loaded into the "file_record" and parsed
        }
        in.close();
        return true;
    }else{
        cout << "COULD NOT FIND THE FILE" << endl;
        return false;
    }
}

//Precondition: First line is the string of column names and their types
//Postcondition: Creates Index_Trees with Column Name and the types
void Table::tokenize_columns(string first_line){
    first_line.pop_back(); //Delimiter removal. Extra space
    string former_string;
    char str[1024];
    strcpy(str, first_line.c_str());
    char *literal;
    literal = strtok(str, " ,()'\0\n\r\t");
    while(literal != NULL){
        //If any of these conditions are satisfied, the parsing will take the column name and the column type
        if(strcmp(literal,"STRING") == 0){
            load_fieldNames(former_string, STRING);
        }else if(strcmp(literal, "INT") == 0){
            load_fieldNames(former_string, INTEGER);
        }else if(strcmp(literal, "DOUBLE") == 0){
            load_fieldNames(former_string, DOUBLE);
        }
        former_string = string(literal);
        literal = strtok(NULL, " ,()'\0\n\r\t");
    }
}


void Table::load_fieldNames(string fieldName, unsigned int DATA_TYPE){
    Index_Tree* newTree = new Index_Tree(fieldName, DATA_TYPE);
    dictionary.Insert(fieldName, newTree);
    column_names.push_back(fieldName); //Temporary queue to hold all the fieldNames
}

//Precondition: Column/Index_Trees are created inside the given Table
//Takes input from a file, parses, and creates a Record
//Searches a tree for a pair, if that pair exists, we concat the indices. else the new Index_Pair is added to the Index_Tree
void Table::load_record(string fromFile){
//    cout << "ENTER LOAD RECORD" << endl;
    Record* newRecord = new Record(fromFile); //Record parses the received line and stores into each field
    //        cout << "DEBUG: Loading Record:" << *newRecord << endl;
    int record_index = (int)file_record.size(); //Index where the record will be placed
    newRecord->set_index(record_index); //Passes that index into the record
    file_record.push_back(newRecord); //The record is stored in the file_record

    Index_Tree* temp = NULL;
    for(unsigned int i = 0; i < (unsigned int)newRecord->fields.size(); i++){
        string value = newRecord->fields.at(i);
        string column_name = column_names.at(i);
        Index_Pair* newPair = new Index_Pair(value); //A new pair is created here.
        temp = dictionary[column_name]; //Retrieves the corresponding Index_Tree
        //Check if this pair already exists within in tree
        if(temp->search_Tree(newPair)){ //newPair value is found inside the tree. found_search is not NULL.
            newPair = temp->found_search;
            newPair->insert_index(record_index); // Index is inserted to the pair. The check above determines which pair is inserted
            newRecord->insert(column_name);
        }else{
            newPair->insert_index(record_index); // Index is inserted to the pair. The check above determines which pair is inserted
            newRecord->insert(column_name);
            temp = dictionary[column_name]; //Gets the index tree at key "value"
            temp->insert(newPair);
        }
    }
}

bool Table::column_exists(string name_of_column){
    string from_column_names_vector;
    for(unsigned int i = 0; i < (unsigned int)column_names.size(); i++){ //Linear search..
        from_column_names_vector = column_names.at(i);
        if(from_column_names_vector == name_of_column){
            return true;
        }
    }
    return false;
}

Index_Tree* Table::return_column(string name_of_column){
    return dictionary[name_of_column]; //Since the column already exists. No need to worry
}

void Table::import_file_record(vector<Record*> importMe){
    Record* temp;
    for(unsigned int i = 0; i < (unsigned int) importMe.size(); i++){
        temp = importMe.at(i); //Get a record
        temp->select_fields(preserved_column_names);
    }
    file_record = importMe; //Used for selection. Only copies the records of the selected "column names"
}

void Table::import_index_trees(Index_Tree* importMe){
    dictionary.Insert(importMe->fieldName, importMe);
    column_names.push_back(importMe->fieldName);
    preserved_column_names.push_back(importMe->fieldName); //Used in import_file_record
}

void Table::print_all_trees(){
    for(unsigned int i = 0; i < (unsigned int)column_names.size(); i++){
        string col = column_names.at(i);
        Index_Tree* temp = dictionary[col];
        temp->tree.book_print(1);
    }
}

ostream& operator <<(ostream& out, Table& printMe){
    string temp;
    Index_Tree* temp_tree;
    //Print the column names and the types
    cout << "Print table: " << printMe.table_name <<endl;
    for(unsigned int i = 0; i < (unsigned int) printMe.column_names.size(); i++){
        temp = printMe.column_names.at(i); //Take the column name from the column name vector
        temp_tree = printMe.dictionary[temp]; //Extract the Index_Tree from the dictionary
        out << *temp_tree;
        if(i < (printMe.column_names.size()-1)){
            out << " , ";
        }
    }
    out << " " << endl;
    for(unsigned int j = 0; j < (unsigned int) printMe.file_record.size(); j++){
        out << *printMe.file_record.at(j);
    }
    return out;
}

#endif // TABLE_H
