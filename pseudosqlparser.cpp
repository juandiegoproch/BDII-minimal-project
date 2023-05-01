#include <string>
#include <sstream>
#include <locale>

void parseSql(std::string sentence)
{

    for (char& i : sentence)
        i = std::toupper(i);
    std::istringstream sentence_stream(sentence);

    // read first word
    std::string fword;
    sentence_stream >> fword;
    if (fword == "")
    {
        // Empty command Error
    }
    else if (fword == "CREATE")
    {
        sentence_stream >> fword;
        if (fword == "TABLE")
        {
            // CREATE TABLE <tablename> <REGTYPE> <FILE>
            std::string tablename;
            sentence_stream >> tablename;

            std::string register_type; // the name of an exixting type of register to be used as template
            sentence_stream >> tablename;

            std::string file_to_load;
            sentence_stream >> tablename;

            // create the table using this data
        }
        else
        {
            // ERROR: CREATE ONLY SUPPORTS TABLE
        }
        // check for create table, index type
    }
};