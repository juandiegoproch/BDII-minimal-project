//#define DEBUG
//#define FWARNINGS
#include "avlfile.h"
#include "RegistroNBA.h"
#include "RegistroTornados.h"
#include "pseudosqlparser.h"
#include "hash.h"
#include <iostream>

using namespace std;

int main(void)
{

    avlFileManager<RegistroNBA> fmanager("db_files/avlfile.avl");
    ExtendibleHash<RegistroTornados> hashtable("db_files/hash_index.hnd","db_files/hash_data.hnd",8);
    
    std::cout << "Welcome to OURSQL, our best effort at providing the simplest thing that could pass as a DBMS! \n";
    std::cout << R"(
        Some comments on syntax:
            Although our query language is similar to SQL, it is certainly not. Many of the usual features are missing
            due to limitations of our system. Others have been restructured in a way that better fits
            the constraints of our design.

            Some key differences:

                Line breaks mark the end of statements, not ;

                SELECT attr,attr2 ... FROM table does not work, only SELECT * FROM TABLE

                Table creation must be done at the code level.

                SELECT must filter, and the filtering is done using two keywords: EQUALS and BETWEEN,
                    these act only upon the primary key, wich must be set in code.
                
                DELETE works on the key and only the key: no complex WHERE statements are supported.

                The system is case insensitive.
    )";
    string command;
    while (true)
    {
        getline(cin,command);
        cout << parseSql(command,fmanager,hashtable);
    }

}

/*
int main(void)
{
    avlFileManager<RegistroNBA> fmanager("avlfile.avl");
    fmanager.dumpAVL("dumpfile.csv");
}
*/