#define DEBUG
#define FWARNINGS
#include "avlfile.h"
#include "RegistroNBA.h"
#include "RegistroTornados.h"
#include "pseudosqlparser.h"
#include <iostream>

using namespace std;

int main(void)
{

    avlFileManager<RegistroNBA> fmanager("avlfile.avl");
    
    string command;
    while (true)
    {
        getline(cin,command);
        cout << parseSql(command,fmanager);
    }


}