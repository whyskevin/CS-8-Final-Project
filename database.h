#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <fstream>
#include "constants.h"
#include "table.h"
#include "../../my CS 8/parse_map/parser.h"
#include "../../my CS 8/parse_map/parse_pair.h"
#include "hashnode.h"
#include "index_tree.h"
#include <string>

using namespace std;

class Database{
public:
    Database();
    ~Database();

    //Runs the program. Will continuously take commands until we encounter and invalid or "end" command
    void run();

    //Commands
    HashNode* createTable(string tableName);
    void loadTable(string tblName); //.tbl name as in the file being loaded
    void storeTable(string tableName);
    void drop(string tableName);
    Table* select(string tableName);

    //Create helper functions
    void create_empty(Parse_Pair columns);
    HashNode* search(HashNode*& checkMe, string tableName);

    //Select helper function
    vector <Record*> concat(vector<Record*> one, vector<Record*> two); //Concats two to end of one
    Index_Tree* selected_column_exists(vector <Index_Tree*> selected, string COLUMN_FIELD);
    int where_condition(string operation);

    //Functions for relational operators. There is no "range" operation
    void equality_condition(Index_Pair* newPair, Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY);
    void greater_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY);
    void less_than_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY);
    void greater_equal_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY);
    void less_than_equal_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY);

    void print(string tableName);

    //Member Variables
private:
    vector <int> indices; //Used for relational operations
    Table* selectedTable;
    Table* newTable; //This is for the select function
    vector <Index_Tree*> selectedColumns;
    HashNode* insertTable(string tableName);
    int hashTable(string tableName); //Hashes table name to produce an index
    HashNode ** HashTable; //Indexed hashTable. Stores tables
};

//Constructors ------------
Database::Database(){
    //Instantiates the HashTable
    HashTable = new HashNode* [HTABLE_SIZE];
    for(unsigned int i = 0; i < HTABLE_SIZE; i++){
        HashTable[i] = NULL;
    }
    selectedTable = nullptr;
}

Database::~Database(){
    for(unsigned int i = 0; i < HTABLE_SIZE; i++){
        HashTable[i] = NULL;
    }
    delete []HashTable;
}

void Database::run(){
    Parser parser;
    Parse_Pair* command_pair = NULL;
    Parse_Pair* column_command_pair = NULL;
    Parse_Pair* where_command_pair = NULL;
    Index_Tree* temporaryColumn = NULL;
    parser.take_command();
    unsigned int COMMAND = parser.get_command();
    while(COMMAND != 0){
        //        cout << "DEBUG: COMMAND " <<COMMAND << endl;
        switch(COMMAND){
        case 1: //Create
        {
            command_pair = parser.return_fields("create"); //Returns <create, tableName> pair
            string table_name = command_pair->arguments.front();
            cout << "Create:" << table_name << endl;
            column_command_pair = parser.return_fields("column");
            if(column_command_pair != nullptr){ // <column, colname......> exists
                HashNode* newTable = createTable(table_name);
                string temp;
                for(unsigned int i = 0; i < (unsigned int)column_command_pair->arguments.size(); i++){
                    if(column_command_pair->arguments.at(i) == "STRING"){
                        newTable->tablePtr->load_fieldNames(temp, STRING);
                    } else if(column_command_pair->arguments.at(i) == "INT"){
                        newTable->tablePtr->load_fieldNames(temp , INTEGER);
                    }else  if(column_command_pair->arguments.at(i) == "DOUBLE"){
                        newTable->tablePtr->load_fieldNames(temp, DOUBLE);
                    }
                    temp = column_command_pair->arguments.at(i);
                }
            }else{
                cout << "Second case. Creating table with select" << endl;
            }
            break;
        }
        case 2: //Load
            command_pair = parser.return_fields("load");
            loadTable(command_pair->arguments.front()); //In this context, there is only one argument
            //            selectedTable->print_all_trees();
            break;
        case 3: //Store
            command_pair = parser.return_fields("store");
            storeTable(command_pair->arguments.front());
            break;
        case 4: //Drop
            command_pair = parser.return_fields("drop");
            drop(command_pair->arguments.front());
            break;
        case 5: //Insert
        {
            command_pair = parser.return_fields("into"); //Retrieves the pair with the tablName
            Table* target_table = select(command_pair->arguments.front()); //Gets the table if it exists
            if(target_table != NULL){
                column_command_pair = parser.return_fields("insert");
                string feed_to_record;
                if(column_command_pair->arguments.size() == target_table->column_names.size()){
                    for(unsigned int i = 0; i < (unsigned int)column_command_pair->arguments.size(); i++){
                        feed_to_record += " ";
                        feed_to_record += column_command_pair->arguments.at(i);
                    }
                    //Check that the arguments fit the columns of the table
                    target_table->load_record(feed_to_record);
                }else{
                    cout << "Values don't match table fields." << endl;
                }
            }else{
                cout << "Table doesn't exist" << endl;
            }
            break;
        }
        case 6: //Print
            command_pair = parser.return_fields("print");
            print(command_pair->arguments.front()); //Print should only have one argument
            break;
        case 7: //Select
        {
            bool one_table = false;
            command_pair = parser.return_fields("from");
            column_command_pair = parser.return_fields("column");
            vector <string> case2_table_names (command_pair->arguments); //Copies Table Names for Case 2
            //Only capable of selecting from a single table atm
            if((unsigned int)command_pair->arguments.size() == 1){
                one_table = true;
            }
            while(!command_pair->arguments.empty()){
                selectedTable = select(command_pair->arguments.back()); //SelectedTable is assigned if the table is found. Only temporarily
                cout << "SELECTED TABLE:" << *selectedTable << endl;
                if(selectedTable != nullptr){
                    //                    cout << "Command Pair: " << *command_pair <<endl;
                    if(column_command_pair->arguments.front() != "*"){
                        for(unsigned int k = 0; k < (unsigned int) column_command_pair->arguments.size(); k++){
                            //Check if the columns exist within the table.
                            //                            cout << "Looking for column:" << column_command_pair->arguments.back() << endl;
                            if(selectedTable->column_exists(column_command_pair->arguments.at(k))){ //Take from the back. That way we can pop_back
                                //Get a pointer which points to those columns
                                //                                cout << "COLUMN EXISTS" << endl;
                                temporaryColumn = selectedTable->return_column(column_command_pair->arguments.at(k));
                                //                                cout << "TEMP COL:" << *temporaryColumn <<endl;
                                selectedColumns.push_back(temporaryColumn);
                            }else{
                                cout << "Invalid Command:" << command_pair->arguments.back() << " does not exist" <<endl;
                                break;
                            }
                        }
                    }else{ //If "*" is the column command
                        for(unsigned int n = (unsigned int) selectedTable->column_names.size()-1; n >= 0; --n){
                            string colName = selectedTable->column_names.at(n);
                            temporaryColumn = selectedTable->return_column(colName);
                            //                            cout << *temporaryColumn << endl;
                            selectedColumns.push_back(temporaryColumn);
                            if(n == 0){
                                break;
                            }
                        }
                    }
                }
                command_pair->arguments.pop_back(); //Increment
            }
            //            cout << "ARGUMENTS NOW " << endl;
            //At this point we have everything "selected". What's left is the "where" clause and creating the selected table
            //Check for conditions...
            //Creating a new Table. This will be the new Table
            newTable = new Table("select");
            Index_Tree* temp_selected;

            vector<Index_Tree*> selectedColumns_COPY (selectedColumns);
            //            cout << "SELECTED COLUMNS_COPY SIZE " << selectedColumns_COPY.size() << endl;

            where_command_pair = parser.return_fields("where"); //Gets the "where" command and its conditions
            if(one_table){ //Case 1: ONLY ONE TABLE TO SELECT FROM -----------------------


                //Find the columns you want to remove
                while(!selectedColumns.empty()){
                    //                    cout << "SELECTED COLUMNS:" << selectedColumns.back() << endl;
                    temp_selected = selectedColumns.back();
                    newTable->import_index_trees(temp_selected); //Inserting the selected columns into the newTable
                    selectedColumns.pop_back();
                }

                //CREATE A NEW FILE_RECORD. COPY ALL THE ELEMENTS OF THE SELECTED TABLE INTO IT!!!
                vector<Record*> IMPORT_COPY;
                for(unsigned int i = 0; i < selectedTable->file_record.size(); i++){
                    Record* COPY = new Record(selectedTable->file_record.at(i));
                    IMPORT_COPY.push_back(COPY);
                }


                if(where_command_pair->arguments.size() == 0){ //The where clause does not exist.
                    cout << "Where clause has no arguments. Returning empty table..." << endl;
                    newTable->import_file_record (IMPORT_COPY);
                    Table* empty = new Table();
                    newTable = empty;
                }else{ //The where clause exists
                    //                    cout << "WHERE CLAUSE EXISTS" <<endl;
                    //                    cout << *where_command_pair << endl;
                    if (!where_command_pair->arguments.empty()){ //There are arguments inside the VECTOR
                        string PAIR_VALUE = where_command_pair->arguments.back();
                        //                        cout << "PAIR_VAL:" << PAIR_VALUE << endl;
                        where_command_pair->arguments.pop_back();
                        string OPERATION = where_command_pair->arguments.back();
                        //                        cout << "OPERATION:" << OPERATION << endl;
                        where_command_pair->arguments.pop_back();
                        string COLUMN_FIELD = where_command_pair->arguments.back(); //Get the column of interest
                        //                        cout << "COLUMN_FIELD:" << COLUMN_FIELD << endl;
                        where_command_pair->arguments.pop_back();
                        //Check if COLUMN_FIELD appears in the selectedColumns
                        Index_Tree* where_selected_column = selected_column_exists(selectedColumns_COPY,COLUMN_FIELD);
                        if(where_selected_column != NULL){ //Condition column appears in the selectedColumns
                            where_command_pair->arguments.pop_back(); //Pop COLUMN_FIELD
                            //                            cout << "LOOKING FOR CONDITION" << endl;
                            Index_Pair* newPair = new Index_Pair(PAIR_VALUE);
                            if(where_selected_column->search_Tree(newPair)){ //If the pair exists within the Index_Tree
                                Index_Pair* found = where_selected_column->found_search;
                                switch(where_condition(OPERATION)){ //Gets the operator next
                                case 1: // Operator ==
                                {
                                    equality_condition(newPair, found, where_selected_column, IMPORT_COPY);
                                    one_table = false;
                                    break;
                                }
                                case 2:
                                {// > Operator
                                    greater_condition(found, where_selected_column, IMPORT_COPY);
                                    break;
                                }
                                case 3: // < Operator
                                {
                                    less_than_condition(found, where_selected_column, IMPORT_COPY);
                                    break;
                                }
                                case 4:
                                {
                                    greater_equal_condition(found, where_selected_column, IMPORT_COPY);
                                    break;
                                }
                                case 5:
                                {
                                    less_than_equal_condition(found, where_selected_column, IMPORT_COPY);
                                    break;
                                }
                                default:
                                {
                                    cout << "Default case. Why are you here?" << endl;
                                    break;
                                }
                                } //Switch case end

                            }//Clear containers used in relation operations
                            indices.clear();
                            IMPORT_COPY.clear();
                        }else if (where_selected_column == NULL){
                            cout << "Where condition: Specified Column DNE" << endl;
                        }
                    }
                } //End of the check: if there is a condition
                //After the columns and the file_record is copied into the new table, we assign it as the new selectedTable
                selectedTable = newTable; //Typically used for non-condition operation
            }else{ //Case 2: MORE THAN ONE TABLE TO SELECT FROM ---------------------
                cout << "Case: More than one table specified. Not implemented." << endl;
                //                for(int i =  0 ; i < (int)case2_table_names.size()-1; i++){
                //                    cout << "TABLE NAMES:" << case2_table_names.at(i) << endl;
                //                }
                //                for(int j = 0; j < (int)selectedColumns.size(); j++){
                //                    cout << "SELECTED COLUMNS:" << *selectedColumns.at(j) << endl;
                //                }
            }
            break;
        }
        }
        parser.take_command();
        COMMAND = parser.get_command();
    }
    cout << "Program end..." << endl;
    return;
}

HashNode* Database::createTable(string tableName){
    return insertTable(tableName); //Creates a HashNode with a new Table and inserts it into the Chain Hash
}

void Database::storeTable(string tableName){
    Table* storeMe = select(tableName);
    //    cout << "STOREME:" << *storeMe << endl;
    string fileName = tableName + ".tbl"; //Writes to a .txt for debug purposes
    ofstream out;
    out.open(fileName);
    if(out.is_open()){
        cout << "Storing file..." << endl;
        out << *storeMe;
    }else{
        cout << "Filed Stored:" << fileName << endl;
    }
    out.close();
}

void Database::drop(string tableName){
    int table_index = hashTable(tableName); //Find the index where the table should reside
    HashNode* checkMe = HashTable[table_index];
    HashNode* whatToRemove = search(checkMe, tableName);
    if(whatToRemove == NULL){
        cout << "Table does not exist. Nothing to delete..." << endl;
        return;
    }
    //Pointer to previous node is the node previous to whatToRemove
    HashNode *prevNode = whatToRemove->prev;
    //    cout << "After prevNode \n";
    HashNode *nextNode = whatToRemove->next;
    if(whatToRemove == HashTable[table_index]){
        //        cout << "I'm the head" << endl;
        HashTable[table_index] = nextNode;
        if(nextNode != nullptr){
            //        cout << "NEW HEAD" << *nextNode << endl;
        }
        return;
    }
    //        cout << "After nextNode \n";
    if(prevNode != nullptr){
        prevNode->next = whatToRemove->next;
    }
    //        cout << "After prevNode->next \n";
    if(nextNode != nullptr){
        nextNode->prev = whatToRemove->prev;
    }
    //When whatToRemove is finished
    whatToRemove->next = nullptr;
    whatToRemove->prev = nullptr;
    delete whatToRemove;
    cout << "End \n";

}

HashNode* Database::search(HashNode*& checkMe, string tableName){
    //    cout << "ENTER SEARCH" << endl;
    while(checkMe->next != NULL){
        if(checkMe->tableName == tableName){
            //            cout << "Found" <<endl;
            return checkMe;
        }
        checkMe = checkMe->next;
    }if(checkMe->next == NULL){
        if(checkMe->tableName == tableName){
            //            cout << "found at tail" << endl;
            return checkMe;
        }else{
            return NULL;
        }
    }
}



Table* Database::select(string tableName){
    int where_table_is = hashTable(tableName);
    HashNode* node_in_table = HashTable[where_table_is];
    //Preliminary check. If the table we seek is the first at the index
    if(node_in_table->returnTableName() == tableName){ //Table is found. Return the table
        return node_in_table->returnTable();
    }
    if(node_in_table == NULL){ //There are no Tables stored at that index
        return nullptr;
    }else{
        //        cout << "HashTable has multiple entries" << endl;
        //Index has more than one entry. We then search through the index
        while(node_in_table->next != NULL){
            if(node_in_table->returnTableName() == tableName){ //Table is found. Return the table
                return node_in_table->returnTable();
            }
        }
        //Table isn't found
        return nullptr;
    }
}

//Takes data from a .tbl file. It will create a new Table
void Database::loadTable(string tblName){
    string tableName;
    char temp;
    for(unsigned int i = 0; i < (unsigned int)tblName.size(); i++){
        temp = tblName[i];
        if(temp == '.'){
            break;
        }
        tableName += temp;
    }
    //    cout << "DEBUG: tableName = " << tableName << endl;
    HashNode* newTable = insertTable(tableName);
    if(newTable->tablePtr->load(tblName)){ //If the file is found and the table is filled, we return true
        selectedTable = newTable->tablePtr; //Loaded table becomes the most recent, selected tabl
    } //Pass the tblName into the Table object. It will start parsing the contents
    //    cout << "SELECTED TABLE:" << *selectedTable << endl;
}

vector <Record*> Database::concat(vector<Record*> one, vector<Record*> two){ //Concats two to end of one
    vector<Record*> newRecord (one);
    for(unsigned int i = 0; i < (unsigned int)two.size(); i++){
        newRecord.push_back(two.at(i));
    }
    return newRecord;
}

Index_Tree* Database::selected_column_exists(vector <Index_Tree*> selected, string COLUMN_FIELD){
    //    cout << "SEARCHING FOR COLUMN_FIELD:" << COLUMN_FIELD << endl;
    for(unsigned int i = 0; i < (unsigned int)selected.size(); i++){
        Index_Tree* temp = selected.at(i);
        //        cout << "TEMP FIELDNAME" << temp->fieldName << endl;
        if(temp->fieldName == COLUMN_FIELD){
            //            cout << COLUMN_FIELD << " exists in the selected columns" << endl;
            return temp;
        }
    }
    return NULL;
}


//Relation Operator Functions
void Database::equality_condition(Index_Pair* newPair, Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY){
    newPair->indices = found->indices; //Copies all indices of the == relation
    vector <Record*> TEMP_FILE_RECORD;
    for(unsigned int i = 0; i < (unsigned int) newPair->indices.size(); i++){
        Record* newRecord = IMPORT_COPY.at(newPair->indices.at(i));
        TEMP_FILE_RECORD.push_back(newRecord);
    }
    newTable->import_file_record (TEMP_FILE_RECORD);
    selectedTable = newTable; //Assign the new table to selectedTable
}

void Database::greater_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY){
    vector <Index_Pair*> all_greater_pairs;
    where_selected_column->tree.in_order_for_greater(all_greater_pairs, found);
    //Vector now has all Index_Pairs that is > than the found
    for(unsigned int i = 0; i < (unsigned int) all_greater_pairs.size(); i++){
        Index_Pair* temp = all_greater_pairs.at(i);
        for(unsigned k = 0; k < (unsigned int) temp->indices.size(); k++){
            indices.push_back(temp->indices.at(k)); //Extract all indices
        }
    }
    vector <Record*> TEMP_FILE_RECORD;
    for(unsigned int i = 0; i < (unsigned int) indices.size(); i++){
        Record* newRecord = IMPORT_COPY.at(indices.at(i));
        TEMP_FILE_RECORD.push_back(newRecord);
    }
    newTable->import_file_record (TEMP_FILE_RECORD);
    selectedTable = newTable; //Assign the new table to selectedTable
}

void Database::less_than_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY){
    vector <Index_Pair*> all_less_pairs;
    where_selected_column->tree.in_order_for_less(all_less_pairs, found);
    //Vector now has all Index_Pairs that is > than the found
    for(unsigned int i = 0; i < (unsigned int) all_less_pairs.size(); i++){
        Index_Pair* temp = all_less_pairs.at(i);
        for(unsigned k = 0; k < (unsigned int) temp->indices.size(); k++){
            indices.push_back(temp->indices.at(k)); //Extract all indices
        }
    }
    vector <Record*> TEMP_FILE_RECORD;
    for(unsigned int i = 0; i < (unsigned int) indices.size(); i++){
        Record* newRecord = IMPORT_COPY.at(indices.at(i));
        TEMP_FILE_RECORD.push_back(newRecord);
    }
    newTable->import_file_record (TEMP_FILE_RECORD);
    selectedTable = newTable; //Assign the new table to selectedTable
}

void Database::greater_equal_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY){
    vector <Index_Pair*> all_greater_equal_pairs;
    where_selected_column->tree.in_order_for_greater_equal(all_greater_equal_pairs, found);
    for(unsigned int i = 0; i < (unsigned int) all_greater_equal_pairs.size(); i++){
        Index_Pair* temp = all_greater_equal_pairs.at(i);
        for(unsigned k = 0; k < (unsigned int) temp->indices.size(); k++){
            indices.push_back(temp->indices.at(k)); //Extract all indices
        }
    }
    vector <Record*> TEMP_FILE_RECORD;
    for(unsigned int i = 0; i < (unsigned int) indices.size(); i++){
        Record* newRecord = IMPORT_COPY.at(indices.at(i));
        TEMP_FILE_RECORD.push_back(newRecord);
    }
    newTable->import_file_record (TEMP_FILE_RECORD);
    selectedTable = newTable; //Assign the new table to selectedTable
}

void Database::less_than_equal_condition(Index_Pair* found, Index_Tree* where_selected_column, vector<Record*> IMPORT_COPY){

    vector <Index_Pair*> all_less_than_equal_pairs;
    where_selected_column->tree.in_order_for_less_equal(all_less_than_equal_pairs, found);
    for(unsigned int i = 0; i < (unsigned int) all_less_than_equal_pairs.size(); i++){
        Index_Pair* temp = all_less_than_equal_pairs.at(i);
        for(unsigned k = 0; k < (unsigned int) temp->indices.size(); k++){
            indices.push_back(temp->indices.at(k)); //Extract all indices
        }
    }
    vector <Record*> TEMP_FILE_RECORD;
    for(unsigned int i = 0; i < (unsigned int) indices.size(); i++){
        Record* newRecord = IMPORT_COPY.at(indices.at(i));
        TEMP_FILE_RECORD.push_back(newRecord);
    }
    newTable->import_file_record (TEMP_FILE_RECORD);
    selectedTable = newTable; //Assign the new table to selectedTable
}

//Takes the operator represented as a string and returns an integer to be used in a switch case
int Database::where_condition(string operation){
    if(operation == "=="){
        return 1;
    }else if(operation == ">"){
        return 2;
    }else if(operation == "<"){
        return 3;
    }else if(operation == ">="){
        return 4;
    }else if(operation == "<="){
        return 5;
    }
    return 0;
}

void Database::print(string tableName){
    if(tableName == "select"){
        cout << *selectedTable << endl;
    }else{
        int key = hashTable(tableName);
        HashNode* temp = HashTable[key];
        if(temp == NULL){ //Nothing exists at that index
            cout << "Invalid Command: Table does not exist" << endl;
        }else{
            while(temp != NULL){
                if(temp->returnTableName() == tableName){
                    //                    cout << "Temp is: " << *temp << endl;
                    cout << *temp->tablePtr << endl;
                    return;
                }else{
                    temp = temp->next;
                }
            }
            cout << "Invalid Command: Table does not exist" << endl;
        }
    }
}

HashNode* Database::insertTable(string tableName){
    int hashIndex = hashTable(tableName);
    //    cout << "DEBUG: INDEX: " << hashIndex << endl;
    HashNode *newTable = new HashNode(tableName);
    if(HashTable[hashIndex] == NULL){
        HashTable[hashIndex] = newTable;
        //        cout << "FIRST ADDRESS:" << endl;
        return newTable;
    }else{
        HashNode* temp = HashTable[hashIndex]; //Front of the chain
        //        cout << "TEMP ADDRESS:" << *temp << endl;
        //        cout << "NEWTABLE ADDRESS:" << *newTable << endl;
        while(temp->next != NULL){
            if(temp->tableName == newTable->tableName){ //Table already exists, replace temp with the new table
                //                cout << "TABLE REPLACED1" << endl;
                //                cout << "TEMP" << *temp << endl;
                HashNode* before_temp = temp->prev;
                HashNode* after_temp = temp->next;
                if(before_temp != NULL)
                    before_temp->next = newTable;
                if(after_temp != NULL)
                    after_temp->prev = newTable;

                newTable->prev = before_temp;
                newTable->next = after_temp;
                return newTable;
            }
            //            cout << "WALKER ADDRESS:" << *temp << endl;
            temp = temp->next;
        }
        //Once we arrive at the end. temp-> next == NULL
        if(temp->tableName != newTable->tableName){ //If the table names don't match. Insert the newTable normally
            temp->next = newTable;
            newTable->prev = temp;
            return newTable;
        }else{ //Replace the table if it already exists
            //            cout << "TABLE REPLACED2" << endl;
            if(temp->prev != NULL){
                //                cout <<" prev != nul" <<endl;
                HashNode* before_temp = temp->prev;
                before_temp->next = newTable;
                newTable->prev = temp->prev;
                newTable->next = temp->next;
                return newTable;
            }else{
                //                cout << "prev is nul" << endl;
                newTable->prev = temp->prev;
                newTable->next = temp->next;
                temp = newTable;
                return temp;
            }
        }
    }
    return newTable;
}

int Database::hashTable(string tableName) //Hashes table name to produce an index
{
    int key = 0;
    for(int i = 0; i < (int)tableName.size(); i++){
        key += (int) tableName[i];
    }
    //    cout << "Key: " << key << endl;
    return key % HTABLE_SIZE;
}

#endif // DATABASE_H
