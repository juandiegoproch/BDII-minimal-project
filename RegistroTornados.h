#pragma once

struct RegistroTornados
{
    char date[11];
    char state[2];
    long magnitude;
};

RegistroNBA TornadosFromCSVline(std::string tuple)
{
    // esta funcion recibe algo como (<values>) y retorna un registro completo
}