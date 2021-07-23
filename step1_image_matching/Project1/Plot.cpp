#include "myVideo.h"
#include "Veh.h"
#include <iostream>

void Plot(vector<vector<Veh*> > V){
    ofstream out("zline.xls");
    if (!out) {
        cout << "open lxs unsuccessful";
        throw;
    }
    for (auto &i : V){
        for (auto &j : i){
            out << j->num << " \t" << j->glotime << " \t" << j->x << " \t" << j->y\
             << " \t" << j->spd << " \t" << j->acc << " \t" << j->lnum << endl;
        }
    }
    out.close();
    
}




