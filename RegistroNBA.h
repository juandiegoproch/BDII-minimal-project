#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <algorithm>
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

    //orden: 1, 

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

    if (attrs.size() < 4)
        throw std::invalid_argument("Too short");
    
    for (int x = 0; x < std::min(attrs[0].size(),(size_t)3); x++) {
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

void NBAFromCsvToVec(std::string filename,std::vector<RegistroNBA>& vecfinal) {
    RegistroNBA regtovec;
    std::ifstream file;
    file.open(filename, std::ios::in);

    if (!file.is_open()) {
        exit(1);
    }

    std::string datatotal = "";

    while(getline(file, datatotal)){
        regtovec = RegNBAfromCSVline(datatotal);
        vecfinal.push_back(regtovec);
    }
}

std::string to_string(RegistroNBA a)
{
    return std::to_string(a.matchup_id) + "\t" +
    std::string(a.home_team) + "\t" +
    std::string(a.away_team) + "\t" +
    std::to_string(a.home_points) + "\t" +
    std::to_string(a.away_points);
}