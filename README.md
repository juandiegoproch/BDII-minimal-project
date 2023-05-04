<p align="center"><img src="https://imgur.com/QKXXaZ6.png" width=400></p>

# 💻 JOM Parser  ![a](https://img.shields.io/badge/C%2B%2B-20-blue) ![a](https://img.shields.io/badge/repo%20size-6mb-orange)

> Proyecto 1 con tema de "Organización de archivos" para Base de Datos II (CS2042) - Utec, con Prof. Heider Sanchez.

## Objetivo
El objetivo de nuestro proyecto es implementar y comparar dos estructuras de organización de archivos para una recuperación y manipulación de datos eficiente. Específicamente, compararemos el rendimiento de las siguientes estructuras:
- [AVL File](https://en.wikipedia.org/wiki/AVL_tree)
- [Extendible-Hashing](https://en.wikipedia.org/wiki/Extendible_hashing)

Para evaluar las estructuras implementaremos las siguientes funciones:
- **Search**: dada una llave, retornaremos los registros respectivos.
- **Range search**: Dado un rango de llaves, retornar todos los registros cuyas llaves se encuentren dentro del rango.
- **Add**: añadir un nuevo registro al archivo.
- **Remove**: eliminar un registro del archivo.

## 📊 Datos
Usaremos dos datasets de Kaggle para simular nuestro parser SQL, son [NBA](https://www.kaggle.com/datasets/ethanchen44/nba-playoff-predictions) y [Tornados](https://www.kaggle.com/datasets/danbraswell/us-tornado-dataset-1950-2021). Estos datasets contienen predicciones para los playoffs de la NBA y datos sobre tornados en EE.UU. Para cada dataset usaremos una estructura distinta y haremos un archivo que contenga los indices para cada registro de modo que facilite los accesos. Ambos datasets fueron extraidos de Kaggle y contienen alrededor de 3000 y 70000 registros.

## 📝 Resultados esperados
Tras realizar una comparacion sobre el performance del AVL y el Extendible-Hashing en base a las funciones implementadas usando los dos datasets vamos a medir las siguientes metricas:
- ⏳ Tiempo de ejecucion: Cuando demora en ejecutar cada funcion para cada dataset en cada estructura.
- 🛰 Uso de memoria: Cuanta memoria es requerida para guardar cada archivo en cada estructura.

Cabe recalcar que de antemano tenemos conocimiento sobre las fortalezas y debilidades de cada estructura. Los arboles AVL tienen busquedas, busquedas por rango e inserciones veloces pero el uso de memoria es alto. El Extensible-Hashing, usa menos memoria y soporta datasets grandes de manera eficiente sin embargo no soporta busquedas por rango.

Una vez acabado el proyecto, los resultados nos ayudarán a comprender mejor los pros y contras entre las diferentes estructuras de organización de archivos y su idoneidad para diferentes tipos de conjuntos de datos y operaciones.

## Técnicas de indexación
### AVL Tree


### Extensible Hashing


## Documentación Parser

📝 Insertar todos los registros de un csv a una tabla.  
```INSERT INTO table FROM file.csv```

🔎 Buscar registros con la llave dentro de un rango.  
```SELECT * FROM table BETWEEN start end```

🔍 Buscar registros con una llave especifica.  
```SELECT * FROM table EQUALS value```

🖍 Borrar registros que tengan una llave especifica.  
```DELETE FROM table value```

## Overview
```cpp
#include "avlfile.h"
#include "RegistroNBA.h"
#include "RegistroTornados.h"
#include "pseudosqlparser.h"
#include "hash.h"
#include <iostream>

using namespace std;

int main(void)
{
    avlFileManager<RegistroNBA> fmanager("db_files/avlfile.avl");
    ExtendibleHash<RegistroTornados> hashtable("db_files/hash_index.hnd","db_files/hash_data.hnd",8);
    
    std::cout << "Welcome to OURSQL, our best effort at providing the simplest thing that could pass as a DBMS! \n";
    std::cout << R"(
        Some comments on syntax:
            Although our query language is similar to SQL, it is certainly not. Many of the usual features are missing
            due to limitations of our system. Others have been restructured in a way that better fits
            the constraints of our design.

            Some key differences:

                Line breaks mark the end of statements, not ;

                SELECT attr,attr2 ... FROM table does not work, only SELECT * FROM TABLE

                Table creation must be done at the code level.

                SELECT must filter, and the filtering is done using two keywords: EQUALS and BETWEEN,
                    these act only upon the primary key, wich must be set in code.
                
                DELETE works on the key and only the key: no complex WHERE statements are supported.

                The system is case insensitive.
    )";
    string command;
    while (true)
    {
        getline(cin,command);
        cout << parseSql(command,fmanager,hashtable);
    }
}
```

## Configuración
En el folder de "BDII-MINIMAL-PROJECT", añade un csv del cual se quiera extraer los datos. Despues simplemente corre el archivo:
- ./main.exe : Windows
- ./a.out : Mac OS

Y se ejecutara el archivo, despues unicamente queda realizar los queries, para copiar la base de datos en una estructura simplemente es hacer INSERT INTO tabla FROM archivo.csv. 

> Antes de hacer eso se debe crear un **struct** con los registros y sus campos para posteriormente en el ```main.cpp``` reemplazar el ```RegistroNBA``` que viene por defecto con el registro creado. Para cambiar el **fill factor** del extensible hashing simplemente cambie el const int que se encuentra en el hash.h y despues vuelva a compilar.  

Ejemplo: ```g++ -std=c++20 main.cpp```

## Resultados Obtenidos

