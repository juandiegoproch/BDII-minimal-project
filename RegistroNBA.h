#pragma once

#include <string>
#include<vector>
#include<fstream>
struct RegistroNBA
{
    // KeyType must be a type that can be compared using >, < and ==. 
    // getKey must allways return KeyType
    // If the internal representation of the key does not comply with this 
    // restriction, it must be casted into a type that does, and in wich stuff "means the same": 
    // ej. don't cast a char* to an int!
    
    typedef long KeyType;

    KeyType getKey() 
    {
        return matchup_id;
    }

    char home_team[4]{0}; // home team short
    long matchup_id; // this is pk!
    long home_points;
    char away_team[4]{0}; // away team short
    long away_points;
    // other fields ignored because they cluttered stuff
};

RegistroNBA RegNBAfromCSVline(std::string tuple)
{
    RegistroNBA reg;

    std::vector<std::string> attrs;
    std::string save = "";
    int contador =-1;
    for(auto i:tuple){
        contador++;
        if(contador != tuple.size()-1) {
            if (i != ',') {
                save += i;
            } else {
                attrs.push_back(save);
                save = "";
            }
        }else{
            save += i;
            attrs.push_back(save);
            save = "";
        }
    }

    for (int x = 0; x < attrs[0].size(); x++) {
        reg.home_team[x] = attrs[0][x];
    }

    reg.matchup_id = stol(attrs[1]);

    reg.home_points = stol(attrs[2]);

    for (int y = 0; y < attrs[3].size(); y++) {
        reg.away_team[y] = attrs[3][y];
    }

    reg.away_points = stol(attrs[4]);


    return reg;
}

std::vector<RegistroNBA> NBAFromCsvToVec(std::string filename) {
    RegistroNBA regtovec;
    std::vector<RegistroNBA> vecfinal;
    std::ifstream file;
    file.open(filename, std::ios::in);

    if (file.fail()) {
        exit(1);
    }

    std::string datatotal = "";

    while(getline(file, datatotal)){
        regtovec = RegNBAfromCSVline(datatotal);
        vecfinal.push_back(regtovec);
    }

    return vecfinal;
}
