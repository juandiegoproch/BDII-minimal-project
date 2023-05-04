#pragma once
#include<string>
#include<vector>
#include<fstream>
struct RegistroTornados
{
    typedef long KeyType;

    KeyType getKey()
    {
        return magnitude;
    }

    char date[11]{0};
    char state[3]{0};
    long magnitude;
};

RegistroTornados TornadosFromCSVline(std::string tuple)
{
    RegistroTornados reg;

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
        reg.date[x] = attrs[0][x];
    }
    for (int y = 0; y < attrs[1].size(); y++) {
        reg.state[y] = attrs[1][y];
    }
    reg.magnitude = stol(attrs[2]);

    return reg;

}

std::vector<RegistroTornados> TornadosFromCSVtovec(std::string filename){
    RegistroTornados regtovec;
    std::vector<RegistroTornados> vecfinal;
    std::ifstream file;
    file.open(filename, std::ios::in);

    if (file.fail()) {
        exit(1);
    }

    std::string datatotal = "";

    while(getline(file, datatotal)){
        regtovec = TornadosFromCSVline(datatotal);
        vecfinal.push_back(regtovec);
    }

    return vecfinal;

}

std::string to_string(RegistroTornados t)
{
    return string(t.date) + "\t" + string(t.state) + "\t" + std::to_string(t.magnitude);
}