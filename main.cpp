//#define DEBUG
//#define FWARNINGS

#include "avlfile.h"
#include "hash.h"
#include "pseudosqlparser.h"
#include "RegistroNBA.h"
#include "RegistroTornados.h"
#include <iostream>

using namespace std;


int main(void)
{

    avlFileManager<RegistroNBA> fmanager("db_files/avlfile.avl");
    ExtendibleHash<RegistroTornados> hashtable("db_files/hash_index.dat","db_files/hash_data.dat",32);
    
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
std::string chooseRandString(std::vector<std::string> words)
{
    int index_raw = rand();

    return words[index_raw%words.size()];
}

int main(){
    ExtendibleHash<RegistroTornados> hashTable("hash.dat", "buckets.dat", 8);
    vector<string> il = {"MO","OH","AR","IL","TX","LA","MS","TN","OK","FL","AL","SC","KS","IA","NE","SD","WY","NC","GA","ND","MN","WI","IN","PA","NM","CT","CO","WV","MD","KY","CA","VA","NJ","MI","MA","NH","OR","NY","MT","AZ","UT","ME","VT","ID","WA","DE","HI","PR","AK","NV","RI","DC","VI"};

    RegistroTornados reg;
    while(true){
        int option;
        cout << "[0] : display\n[1] : insert\n[2] : search\n[3] : delete\n[4] : exit | ";
        cin >> option;
        switch(option){
            case 0:
                //hashTable.display_indexes();
                //hashTable.display_buckets();
                hashTable.print_all_buckets();
            break;
            case 1:

                for(int year = 1900; year < 2000; year++) {
                    for(int month = 1; month <= 12; month++) {
                        for(int day = 1; day <= 31; day++) {
                            // create a date string in the format "YYYY-MM-DD"
                            std::string date = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
                            std::string state = chooseRandString(il);
                            long mag = rand()%8 + 1;
                            // assign the date string to the date member of the RegistroTornados object
                            strncpy(reg.date, date.c_str(), sizeof(reg.date));
                            strncpy(reg.state, state.c_str(), sizeof(reg.state));
                            memcpy(&reg.magnitude, &mag, sizeof(reg.magnitude));
                            // insert the RegistroTornados object into the hash table
                            hashTable.insert(reg);
                            cout << reg << endl;
                        }
                    }
                }
            break;
            case 2:
                cout << "\nEnter home_team: ";
                cin >> reg.date;
                cout << "Found: " << hashTable.search(reg.getKey()) << endl;
            break;
            case 3:
                cout << "\nEnter name: ";
                cin >> reg.date;
                cout << "Remove: " << hashTable.remove(reg.getKey()) << endl;
            break;
            case 4:
                return 0;
            break;
        }
    }
    return 0;
};
*/