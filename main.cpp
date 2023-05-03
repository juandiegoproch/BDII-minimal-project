#define DEBUG
#define FWARNINGS
#include "avlfile.h"
#include "RegistroNBA.h"
#include "RegistroTornados.h"
#include "pseudosqlparser.h"

int main(void)
{

    avlFileManager<RegistroNBA> fmanager("avlfile.avl");
    /*
    RegistroNBA RegTemp{"A",0,0,"AAB",0};
    RegistroNBA RegTemp2{"B",1,10,"ACB",10};
    RegistroNBA RegTemp3{"B",1,22,"AXX",5};
    RegistroNBA RegTemp4{"C",2,12,"ALB",2};
    fmanager.add(RegTemp);
    fmanager.add(RegTemp2);
    fmanager.add(RegTemp3);
    fmanager.add(RegTemp4);
    */
    fmanager.showFileAVLtree();

    vector<RegistroNBA> vec2 = fmanager.rangeSearch(-1,-1);
    //vector<RegistroNBA> vec = fmanager.search(-1);
    for(auto i:vec2){
        cout<<"Record: "<<endl;
        cout<<"Hometeam: "<<i.home_team<<" - Matchupid: "<<i.matchup_id<<" - Awaypoints: "<<i.away_points<<endl;
    }


}