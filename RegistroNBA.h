#pragma once

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
    // esta funcion recibe algo como (<values>) y retorna un registro completo
}