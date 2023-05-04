#pragma once
#include<string>
#include<vector>
#include<fstream>

struct RegistroTornados{
    typedef std::string KeyType;

    KeyType getKey(){
        return date;
    }

    char date[11];
    char state[2];
    long magnitude = 8;

    friend std::ostream& operator<<(std::ostream& os, RegistroTornados reg){
        os << "key: " << reg.getKey() << " | magnitude: " << reg.magnitude << std::endl;
        return os;
    }
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

std::string to_string(RegistroTornados t)
{
    return string(t.date) + "\t" + string(t.state) + "\t" + std::to_string(t.magnitude);
}

std::vector<RegistroTornados> TornadosFromCSVtovec(std::string filename){
    RegistroTornados regtovec;
    std::vector<RegistroTornados> vecfinal;
    std::ifstream file;
    file.open(filename, std::ios::in);

    if (file.fail()) {
        throw "";
    }

    std::string datatotal = "";

    while(getline(file, datatotal)){
        regtovec = TornadosFromCSVline(datatotal);
        vecfinal.push_back(regtovec);
    }

    cout << "size of vector: " << vecfinal.size() << endl;

    return vecfinal;

}