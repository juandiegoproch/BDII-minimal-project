#pragma once

#include <string>
#include <sstream>
#include <locale>
#include <vector>
#include <stdexcept>
#include "RegistroNBA.h"
#include "RegistroTornados.h"
#include "avlfile.h"
#include "hash.h"

#ifdef DEBUG
    #include <iostream>
#endif

std::string chooseRandString(std::vector<std::string> words)
{
    int index_raw = rand();

    return words[index_raw%words.size()];
}

std::string parseSql(std::string sentence, avlFileManager<RegistroNBA>& avlfmanager, ExtendibleHash<RegistroNBA>& hashTable)
{
    for (char& i : sentence)
        i = std::toupper(i);
    std::istringstream sentence_stream(sentence);

    #ifdef DEBUG
        std::cout << "[DEBUG] <parseSql> Preprocessed string: " << sentence << std::endl;
    #endif

    string currentWord;
    sentence_stream >> currentWord;
    if (currentWord == "INSERT")
    {
        sentence_stream >> currentWord;
        if (currentWord != "INTO")
        {
            return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
        }
            // INSERT INTO <here we decide where exactly>
        sentence_stream >> currentWord;

        if (currentWord == "NBA_AVL")
        {
            // INSERT INTO NBA_AVL FROM/VALUE
            sentence_stream >> currentWord;
            if (currentWord == "FROM")
            {
                std::string FileName;
                sentence_stream >> FileName;
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO NBA_AVL FROM" << FileName << std::endl;
                #endif

                std::vector<RegistroNBA> registros;
                NBAFromCsvToVec(FileName,registros);
                int insertedCount = 0;
                while (!registros.empty())
                {
                    avlfmanager.insert(registros.back());
                    insertedCount++;
                    registros.pop_back();
                }

                return "Inserted " + std::to_string(insertedCount);
            }
            else if (currentWord == "VALUE")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO NBA_AVL VALUE" << std::endl;
                #endif

                // correct syntaxis should look like "(home_team,matchup_id,home_points,away_team,away_points)"
                // first i must peel back the 
                std::string args;
                getline(sentence_stream,args);

                if (args.size() < 1)
                    return "Syntax error: Invalid empty value \n";
                args = args.substr(1,args.size() - 1); //get rid of the \n and " " at end and start
                // if not enclosed in (), error
                if (args.size() < 2 || *args.begin() != '(' || *args.end() == ')' )
                    return "Invalid value \n";
                args = args.substr(1, args.size() - 2);
                RegistroNBA regnba;
                try
                {
                    regnba = RegNBAfromCSVline(args);
                    
                }
                catch (std::invalid_argument)
                {
                    return "Invalid value: mismatched types. \n Hint: type ordering is ht,id,hp,at,ap \n";
                }
                // only if try has run
                avlfmanager.insert(regnba);

                return "Inserted Succesfully \n";
            }
            
        }
        else if (currentWord == "TORNADO_HASH")
        {
            // INSERT INTO TORNADO_HASH FROM/VALUE
            sentence_stream >> currentWord;
            if (currentWord == "VALUE")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO TORNADO_HASH VALUE" << FileName << std::endl;
                #endif
                // correct syntaxis should look like "(home_team,matchup_id,home_points,away_team,away_points)"
                // first i must peel back the 
                std::string args;
                getline(sentence_stream,args);
                
                if (args.size() == 0)
                    return "Syntax Error: Invalid empty value \n";
                args = args.substr(1,args.size() - 1); //get rid of the \n and " " at end and start
                // if not enclosed in (), error
                if (args.size() < 2 || *args.begin() != '(' || *args.end() == ')' )
                    return "Invalid value \n";
                args = args.substr(1, args.size() - 2);

                /*
                RegistroTornados regtor;
                try
                {
                    regtor = TornadosFromCSVline(args);
                    
                }
                catch (std::invalid_argument)
                {
                    return "Invalid value: mismatched types. \n Hint: type ordering is ht,id,hp,at,ap \n";
                }
                // only if try has run
                hashTable.insert(regtor);*/

                return "Inserted Succesfully \n";

            }
            else if (currentWord == "FROM")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO TORNADO_HASH FROM" << std::endl;
                #endif
                std::string FileName;
                sentence_stream >> FileName;

                for (auto& c :FileName)
                {
                    c = std::tolower(c);
                }
                
                /*
                vector<string> il = {"MO","OH","AR","IL","TX","LA","MS","TN","OK","FL","AL","SC","KS","IA","NE","SD","WY","NC","GA","ND","MN","WI","IN","PA","NM","CT","CO","WV","MD","KY","CA","VA","NJ","MI","MA","NH","OR","NY","MT","AZ","UT","ME","VT","ID","WA","DE","HI","PR","AK","NV","RI","DC","VI"};
                RegistroTornados reg;

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
                }*/

                std::vector<RegistroNBA> registros;
                try{
                    NBAFromCsvToVec(FileName, registros);
                }
                catch (char const*)
                {
                    return "File cannot be opened or is malformed \n";
                }

                int insertedCount = 0;
                while (registros.size() > 330)
                {
                    cout << to_string(registros.back()) << endl;
                    hashTable.insert(registros.back());
                    insertedCount++;
                    registros.pop_back();
                }

                return "Inserted " + std::to_string(12*31*1000);

            }
        }
        else
        {
            return string("Relation \"") + currentWord + string("\" does not exist. \n");
        }
    }
    else if (currentWord == "SELECT")
    {
        sentence_stream >> currentWord;
        if (currentWord != "*")
        {
            return "Error: this implementation does not support specific attribute selection. Use SELECT * ... \n";
        }

        sentence_stream >> currentWord;
        if (currentWord != "FROM")
        {
            return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
        }

        // SELECT * FROM NBA_AVL/TORNADO_HASH EQUALS/BETWEEN ...

        sentence_stream >> currentWord;
        if (currentWord == "NBA_AVL")
        {
            sentence_stream >> currentWord;
            if (currentWord == "BETWEEN")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM NBA_AVL BETWEEN" << std::endl;
                #endif

                std::string start_s, end_s;

                sentence_stream >> start_s;
                sentence_stream >> end_s;

                auto result = avlfmanager.rangeSearch(atol(start_s.c_str()),atol(end_s.c_str()));
                
                // generate the output string

                std::string output = " | matchup_id | home_team | away_team | home_points | away_points | \n";
                for (const auto& i:result)
                    output += to_string(i) + "\n";
                return output;
            }
            else if (currentWord == "EQUALS")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM NBA_AVL EQUALS" << std::endl;
                #endif
                std::string key;
                sentence_stream >> key;

                long keyl = atol(key.c_str());
                auto result = avlfmanager.search(keyl);

                std::string output = " | matchup_id | home_team | away_team | home_points | away_points | \n";
                for (const auto& i:result)
                    output += to_string(i) + "\n";
                return output;
            }
            else 
            {
                return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
            }
        }
        else if (currentWord == "TORNADO_HASH")
        {
            sentence_stream >> currentWord;
            if (currentWord == "BETWEEN")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM TORNADO_HASH BETWEEN" << std::endl;
                #endif

                return string("Range Search not supported in extensible hashin \n");

            }
            else if (currentWord == "EQUALS")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM TORNADO_HASH EQUALS" << std::endl;
                #endif
                long key;
                sentence_stream >> key;

                auto result = hashTable.search(key);

                std::string output = " | date | state | magnitude | \n";
                for (auto i : result)
                    output += to_string(i) + "\n";
                return output;

            }
            else 
            {
                return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
            }
        }
        else
        {
            return string("Relation \"") + currentWord + string("\" does not exist. \n");
        }
    }
    else if (currentWord == "DELETE")
    {
        sentence_stream >> currentWord;
        if (currentWord != "FROM")
            string("Syntax Error: Unrecognised keyword \"") + currentWord + string("\" \n");
        sentence_stream >> currentWord;
        if (currentWord == "NBA_AVL")
        {
            std::string key;
            sentence_stream >> key;
            long keyl = atol(key.c_str());
            bool worked = avlfmanager.remove(keyl);
            if (worked)
                return "Deleted \n";
            else
            {
                return "Not found \n";
            }

        }
        else if (currentWord == "TORNADO_HASH")
        {
            long key;
            sentence_stream >> key;
            bool worked = hashTable.remove(key);
            if (worked)
                return "Deleted \n";
            else
            {
                return "Not found \n";
            }

        }
        else
        {
            return string("Relation \"") + currentWord + string("\" does not exist. \n");
        }
    }
    else
    {
        return string("Syntax Error: Unrecognised keyword \"") + currentWord + string("\" \n");
    }
    return string("");
};