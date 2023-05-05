<p align="center"><img src="https://imgur.com/QKXXaZ6.png" width=400></p>

# 💻 JOM Parser  ![a](https://img.shields.io/badge/C%2B%2B-20-blue) ![a](https://img.shields.io/badge/repo%20size-6mb-orange)

> Proyecto 1 con tema de "Organización de archivos" para Base de Datos II (CS2042) - Utec, con Prof. Heider Sanchez.

## Objetivo
El objetivo de nuestro proyecto es implementar y comparar dos estructuras de organización de archivos para lograr una recuperación y manipulación de datos eficiente. Específicamente, compararemos el rendimiento de las siguientes estructuras:
- [AVL File](https://en.wikipedia.org/wiki/AVL_tree)
- [Extendible-Hashing](https://en.wikipedia.org/wiki/Extendible_hashing)

Para evaluar las estructuras implementaremos las siguientes funciones:
- **Search**: dada una llave, retornaremos los registros respectivos.
- **Range search**: Dado un rango de llaves, retornar todos los registros cuyas llaves se encuentren dentro del rango.
- **Add**: añadir un nuevo registro al archivo.
- **Remove**: eliminar un registro del archivo.

## 📊 Datos
Usaremos dos datasets de Kaggle para simular nuestro parser SQL: [NBA](https://www.kaggle.com/datasets/ethanchen44/nba-playoff-predictions) y [Tornados](https://www.kaggle.com/datasets/danbraswell/us-tornado-dataset-1950-2021). Estos datasets contienen predicciones para los playoffs de la NBA y datos sobre tornados en EE.UU. Respectivamente, usaremos una estructura de organización de archivo diferente para cada dataset y haremos un archivo que contenga los índices para cada registro de manera que facilite los accesos. Ambos datasets fueron extraídos de Kaggle y contienen alrededor de 3000 y 70000 registros, respectivamente.

## 📝 Resultados esperados
Después de realizar una comparación del rendimiento del AVL y el Extendible-Hashing en base a las funciones implementadas utilizando los dos datasets, mediremos las siguientes métricas:

- ⏳ Tiempo de ejecución: Tiempo requerido para ejecutar cada función en cada estructura para cada dataset.
- 🛰 Uso de memoria: Cantidad de memoria requerida para guardar cada archivo en cada estructura.

Es importante destacar que de antemano tenemos conocimiento sobre las fortalezas y debilidades de cada estructura. Los árboles AVL permiten búsquedas, búsquedas por rango e inserciones rápidas, pero su uso de memoria es alto. Por otro lado, el Extendible-Hashing utiliza menos memoria y soporta datasets grandes de manera eficiente, sin embargo, no soporta búsquedas por rango.

Una vez concluido el proyecto, los resultados obtenidos nos permitirán comprender mejor las ventajas y desventajas de cada estructura de organización de archivos y su idoneidad para diferentes tipos de conjuntos de datos y operaciones.

## Técnicas de indexación
### AVL Tree
A diferencia del Sequential File y el ISAM, el AVL ofrece una mayor eficiencia en la búsqueda y manipulación de registros individuales, lo que lo hace más adecuado para aplicaciones en las que se necesitan búsquedas frecuentes y eficientes de registros. Para aplicaciones que requieren busquedas por rango de valores en la clave de indexacion tambien es bueno.

Sin embargo, el ISAM puede ser más adecuado para aplicaciones con conjuntos de datos muy grandes debido a su capacidad para dividir los datos en múltiples bloques y distribuirlos en diferentes archivos. Además, el ISAM puede ser más eficiente en términos de uso de memoria ya que no necesita almacenar la estructura completa del árbol como lo hace el AVL File.

### Extensible Hashing
Tanto el Extensible Hashing como el B+ Tree son estructuras de organización de archivos eficientes y muy utilizadas en la actualidad. 

En cuanto a la inserción de nuevos registros, ambas estructuras son muy eficientes, pero el Extensible Hashing tiene una ventaja en este aspecto ya que es más fácil de balancear y manejar. En el B+ Tree, si se realiza una inserción que rompe la regla de balanceo del árbol, puede requerir una reorganización costosa y compleja. En otros tipos de extensible hashing, el B+ Tree es más eficiente en la eliminación de registros, ya que elimina los registros directamente de las hojas del árbol, mientras que en el Extensible Hashing se tiene que reorganizar las celdas libres. En este caso no es asi, lo hemos hecho de una manera para que las eliminaciones sean mas eficientes.

El B+ Tree es más adecuado para trabajar con conjuntos de datos grandes y dispersos, mientras que el Extensible Hashing es más adecuado para conjuntos de datos pequeños y densos. Además, el B+ Tree es más flexible y puede manejar una amplia gama de operaciones de búsqueda, como búsqueda por rango y búsqueda con múltiples claves, mientras que el Extensible Hashing solo puede manejar búsquedas por clave exacta.

En cuanto al uso de memoria, el Extensible Hashing es más eficiente, ya que solo requiere espacio adicional para el directorio hash, mientras que el B+ Tree requiere espacio adicional para todos los nodos del árbol. Por lo tanto, el Extensible Hashing es una buena opción para conjuntos de datos que pueden crecer rápidamente, ya que puede expandirse dinámicamente sin requerir grandes cantidades de memoria adicional.

Para ser sinceros, a final de cuentas elegimos el hash para no codear el B+ 😌👍 honestidad ante todo.

## Documentación Parser

📝 Insertar todos los registros de un csv a una tabla.  
```INSERT INTO table FROM file.csv```

🔎 Buscar registros con la llave dentro de un rango.  
```SELECT * FROM table BETWEEN start end```

🔍 Buscar registros con una llave especifica.  
```SELECT * FROM table EQUALS value```

🖍 Borrar registros que tengan una llave especifica.  
```DELETE FROM table value```


## Configuración
En el folder de "BDII-MINIMAL-PROJECT/datos", añade un csv del cual se quiera extraer los datos. Despues simplemente corre el archivo:
- ./main.exe : Windows
- ./a.out : Mac OS

Y se ejecutara el archivo, despues unicamente queda realizar los queries, para copiar la base de datos en una estructura simplemente es hacer INSERT INTO tabla FROM archivo.csv. 

> Antes de hacer eso se debe crear un **struct** con los registros y sus campos para posteriormente en el ```main.cpp``` reemplazar el ```RegistroNBA``` que viene por defecto con el registro creado. Para cambiar el **fill factor** del extensible hashing simplemente cambie el const int que se encuentra en el hash.h y despues vuelva a compilar.  

Ejemplo: ```g++ -std=c++20 main.cpp```

## Resultados Obtenidos
<p align="center"><img src="https://imgur.com/TXwXGPF.png" width=400></p>
<p align="center"><img src="https://imgur.com/Kdd2A9g.png" width=400></p>
<p align="center"><img src="https://imgur.com/Kdd2A9g.png" width=400></p>


## Video funcionamiento Extendible Hashing
Tras finalizar la clase, nos dimos cuenta que el error era que pusimos como max_index del extendible hashing 32 y al parecer eso genera problema no se por que, despues con un numero como 16 si funcionaba todo correctamente.
https://drive.google.com/drive/folders/1s8VDEkBeCJ9y53u2AWoeb6a0ySF9ZWo4?usp=share_link

