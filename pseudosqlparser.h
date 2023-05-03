#pragma once

#include <string>
#include <sstream>
#include <locale>
#include "avlfile.h"

#ifdef DEBUG
    #include <iostream>
#endif

std::string parseSql(std::string sentence, avlFileManager<RegistroNBA>& avlfmanager)
{

    for (char& i : sentence)
        i = std::toupper(i);
    std::istringstream sentence_stream(sentence);

    #ifdef DEBUG
        std::cout << "[DEBUG] <parseSql> Preprocessed string: " << sentence << std::endl;
    #endif

    string currentWord;
    sentence_stream >> currentWord;
    if (currentWord == "INSERT")
    {
        sentence_stream >> currentWord;
        if (currentWord != "INTO")
        {
            return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
        }
            // INSERT INTO <here we decide where exactly>
        sentence_stream >> currentWord;

        if (currentWord == "NBA_AVL")
        {
            // INSERT INTO NBA_AVL FROM/VALUE
            sentence_stream >> currentWord;
            if (currentWord == "FROM")
            {
                std::string FileName;
                sentence_stream >> FileName;
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO NBA_AVL FROM" << FileName << std::endl;
                #endif
            }
            else if (currentWord == "VALUE")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO NBA_AVL VALUE" << std::endl;
                #endif
            }
            
        }
        else if (currentWord == "TORNADO_HASH")
        {
            // INSERT INTO TORNADO_HASH FROM/VALUE
            sentence_stream >> currentWord;
            if (currentWord == "FROM")
            {
                std::string FileName;
                sentence_stream >> FileName;
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO TORNADO_HASH FROM" << FileName << std::endl;
                #endif
            }
            else if (currentWord == "VALUE")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] INSERT INTO TORNADO_HASH VALUE" << std::endl;
                #endif
            }
        }
        else
        {
            return string("Relation \"") + currentWord + string("\" does not exist. \n");
        }



    }
    else if (currentWord == "SELECT")
    {
        sentence_stream >> currentWord;
        if (currentWord != "*")
        {
            return "Error: this implementation does not support specific attribute selection. Use SELECT * ... \n";
        }

        sentence_stream >> currentWord;
        if (currentWord != "FROM")
        {
            return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
        }

        // SELECT * FROM NBA_AVL/TORNADO_HASH EQUALS/BETWEEN ...

        sentence_stream >> currentWord;
        if (currentWord == "NBA_AVL")
        {
            sentence_stream >> currentWord;
            if (currentWord == "BETWEEN")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM NBA_AVL BETWEEN" << std::endl;
                #endif
            }
            else if (currentWord == "EQUALS")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM NBA_AVL EQUALS" << std::endl;
                #endif
            }
            else 
            {
                return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
            }
        }
        else if (currentWord == "TORNADO_HASH")
        {
            sentence_stream >> currentWord;
            if (currentWord == "BETWEEN")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM TORNADO_HASH BETWEEN" << std::endl;
                #endif
            }
            else if (currentWord == "EQUALS")
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG] SELECT * FROM TORNADO_HASH EQUALS" << std::endl;
                #endif
            }
            else 
            {
                return string("Syntax Error: Unexpected keyword \"") + currentWord + string("\" \n");
            }
        }
        else
        {
            return string("Relation \"") + currentWord + string("\" does not exist. \n");
        }
    }
    else
    {
        return string("Syntax Error: Unrecognised keyword \"") + currentWord + string("\" \n");
    }
    return string("");
};