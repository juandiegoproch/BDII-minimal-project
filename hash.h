#pragma once

#include <vector>
#include <fstream>
#include <bitset>
#include <cmath>

using namespace std;

const int fillfactor = 5;   // const int because its used to initialize in the bucket struct, the array size.

struct Directory{
    char* key;
    long pos_fisica;
};

template<typename RegisterType>
struct Bucket {
    RegisterType keys[fillfactor];
    int count = 0;
    int local_depth = 1;
    int next = -1;

    void insert(RegisterType reg){
        keys[count] = reg;
        count++;
    }

    bool isfull(){ return count == fillfactor; }
};

template <typename RegisterType>
class ExtendibleHash{ 
private:

    string hash_file_name;
    string buckets_file_name;

    int global_depth = 1;
    int max_depth = 3;

public:

    ExtendibleHash(string hfn, string bfn, int md, int gd=1){
        hash_file_name = hfn;
        buckets_file_name = bfn;  
        max_depth = md;
        global_depth = gd;
        
        initialize_buckets();   // Creates / Read buckets
    }

    bool remove(typename RegisterType::KeyType key){    // Remove
        char* i = hashFunc(key, global_depth);
        int index = get_bucket_pos_from_index(i);
        Bucket<RegisterType> bucket = bucket_from_bin(i);

        while(true){
            for(int j = 0; j < bucket.count; j++){
                if(bucket.keys[j].getKey() == key) {
                    redo_bucket(bucket, j);
                    write_bucket(index, bucket);
                    return true;
                }
            }
            if(bucket.next != -1){
                index = bucket.next;
                bucket = read_bucket((bucket.next*sizeof(Bucket<RegisterType>))+sizeof(int));
            } else {
                break;
            }
        }
        return false;
    }

    vector<RegisterType> search(typename RegisterType::KeyType key){    // Search
        vector<RegisterType> result;
        char* i = hashFunc(key, global_depth);
        int index = get_bucket_pos_from_index(i);
        Bucket<RegisterType> bucket = bucket_from_bin(i);

        while(true){
            for(int j = 0; j < bucket.count; j++){
                if(bucket.keys[j].getKey() == key)
                {
                    result.push_back(bucket.keys[j]);
                    return result;
                }
            }
            if(bucket.next != -1){
                bucket = read_bucket((bucket.next*sizeof(Bucket<RegisterType>))+sizeof(int));
            } else {
                break;
            }
        }
        return result;
    }

    void insert(RegisterType reg){                      // Insert
        //cout << get_num_buckets() << endl;
        if(find(reg.getKey())){
            //cout << "Register already exist." << endl;
            return;
        }

        char* i = hashFunc(reg.getKey(), global_depth); // returns binary string
        int index = get_bucket_pos_from_index(i);    // 
        Bucket<RegisterType> bucket = bucket_from_bin(i);

        //cout << "trying to insert " << reg.getKey() << " in bucket " << index << endl;
        //display_bucket(bucket);
        //cout << "----\n";

        if(!bucket.isfull())
        {                            // Insert the key into the bucket
                                    //cout << "normal insert" << endl;
            bucket.insert(reg);
            write_bucket(index, bucket);  
            //read_dir();  
            return;    
        } 
        else 
        {
            if(bucket.local_depth == global_depth)
            {                           // cout << "Collision problem" << endl;
                if(bucket.local_depth+1 > max_depth)
                {                       // cout << "Extend Horizontally" << endl;
                    while(bucket.isfull())
                    {                   // cout << "Searching space" << endl;
                        if(bucket.next != -1)
                        {
                            index = bucket.next;
                            bucket = read_bucket((index*sizeof(Bucket<RegisterType>))+sizeof(int));
                        } else {                        //cout << "Create & insert extension" << endl;
                            bucket.next = get_num_buckets();
                            write_bucket(index, bucket);

                            Bucket<RegisterType> new_bucket;
                            new_bucket.local_depth = bucket.local_depth;
                            new_bucket.insert(reg);
                            write_bucket(get_num_buckets(), new_bucket);
                            return;
                        }
                    }                   // cout << "Found space" << endl;
                    bucket.insert(reg);
                    write_bucket(index,bucket);
                    return;
                }                       //cout << "Split" << endl;
                global_depth++; 
                split(bucket, index);
                duplicate_dir(index+pow(2,global_depth-1));
                insert(reg);
                return;
            } else {   // Dont rehash everything, just needs a new bucket and indexes change.
                                        //cout << "splitting bucket" << endl;
                split(bucket, index);
                reindex(index);
                insert(reg);
                return;
            }
        }
    }

    // -- Debug functions -- //

    void display_indexes(){     // Used for debug, prints all directories, indexes and position
        read_dir();
    }

    void display_buckets(){     // Display each index with its bucket respectively
        vector<Directory> dirs;
        ifstream file(hash_file_name, ios::in | ios::binary);
        for(int i = 0; i < pow(2, global_depth); i++){
            Directory dir;
            file.read((char*)&dir, sizeof(Directory));
            dirs.push_back(dir);
        }
        file.close();
        // Get directories.
        cout << "[" << global_depth << "]" << endl;
        for(const auto x: dirs){
            Bucket<RegisterType> bucket = read_bucket(x.pos_fisica);
            cout << "[" << x.key << "." << bucket.local_depth << "] ";
            display_bucket(bucket);
        }
        cout << "Successfully read: " << get_num_buckets() << " buckets." << endl;
    }
 
    void print_all_buckets(){   // Used for debug, prints all buckets and their contents (no indexes)
        for(int i = 0; i < get_num_buckets(); i++){
            cout << "[" << i << "] ";
            Bucket<RegisterType> bucket = read_bucket((i*sizeof(Bucket<RegisterType>))+sizeof(int));
            for(int j = 0; j < bucket.count; j++){
                cout << bucket.keys[j].getKey() << ",";
            }
            cout << endl;
        }
    }

    vector<RegisterType> range_search(typename RegisterType::KeyType start, typename RegisterType::KeyType end){
        vector<RegisterType> res;
        for(int i = 0; i < get_num_buckets(); i++){
            Bucket<RegisterType> bucket = read_bucket((i*sizeof(Bucket<RegisterType>))+sizeof(int));
            for(int j = 0; j < bucket.count; j++){
                if(bucket.keys[j].getKey() > start && bucket.keys[j].getKey() < end)
                    res.push_back(bucket.keys[j]);
            }
        }
        return res;
    }


private:

    // -- Important functions -- //

    void initialize_buckets(){                          // Constructor function. Writes the first empty directories or reads them from file.
        ifstream file(buckets_file_name, ios::in | ios::binary);
        if(file.good()){
            file.seekg(0, ios::beg);
            file.read((char*)&global_depth, sizeof(int));
        }
        else {
            ofstream file(buckets_file_name, ios::out | ios::binary);
            file.write((char*)&global_depth, sizeof(int));
            int num_buckets = pow(2, global_depth);
            for(int i = 0; i < num_buckets; i++){
                Bucket<RegisterType> b;
                b.local_depth = global_depth;
                file.write((char*)&b, sizeof(Bucket<RegisterType>));
            }
            
            file.close();

            ofstream dir_file(hash_file_name, ios::out | ios::binary);
            for(int i = 0; i < num_buckets; i++){
                Directory dir;
                dir.key = to_binary(i, global_depth);
                dir.pos_fisica = sizeof(int) + (sizeof(Bucket<RegisterType>)*i);
                dir_file.write((char*)&dir, sizeof(Directory));
            }
            dir_file.close();
        }
    }

    void split(Bucket<RegisterType> bucket, int index){ // split a bucket in two buckets and update/insert both.
        Bucket<RegisterType> new_b;
        Bucket<RegisterType> old_b;
        new_b.local_depth = bucket.local_depth+1;
        old_b.local_depth = bucket.local_depth+1;

        char* first_index = hashFunc(bucket.keys[0].getKey(), global_depth);;

        for(int i = 0; i < bucket.count; i++){
            char* ind = hashFunc(bucket.keys[i].getKey(), global_depth);

            //cout << "index: " << index << endl;
            //cout << "rehashing " << bucket.keys[i].getKey() << " with index: " << ind << endl;

            if(ind[0] == '0') old_b.insert(bucket.keys[i]);
            else new_b.insert(bucket.keys[i]);
        }

        write_bucket(index, old_b);
        //cout << "wrote old bucket with now " << old_b.count << " elements\n";
        write_bucket(get_num_buckets(), new_b);
        //cout << "wrote new bucket with " << new_b.count << " elements\n";
    }
   
    void duplicate_dir(int index){                      // Duplicate every directory (for the rehash).
        vector<Directory> dirs;
        dirs.clear(); // clear the vector
        ifstream read_file(hash_file_name, ios::in | ios::binary);
        for(int i = 0; i < pow(2, global_depth-1); i++){
            Directory dir;
            read_file.read((char*)&dir, sizeof(Directory));
            dirs.push_back(dir);
        }
        read_file.close();

        ofstream write_file(hash_file_name, ios::out | ios::binary);
        int i = 0;
        for(int j = 0; j < 2; j++){
            for(auto &x: dirs){
                if(i == index){
                    //cout << "found!" << endl;
                    x.pos_fisica = ((get_num_buckets()-1)*sizeof(Bucket<RegisterType>))+sizeof(int);
                }
                x.key = to_binary(i, global_depth);
                write_file.write((char*)&x, sizeof(Directory));
                i++;
            }
        }
        write_file.close();
        //print_all_buckets();
        //read_dir();
    }

    void reindex(int index){                            // after duplicating the buckets, this indexates them
        vector<Directory> direcs = get_indexes(index);
        for(int i = 0; i < direcs.size(); i++)
        {
            if(i % 2 != 0)
            {
                direcs[i].pos_fisica = ((get_num_buckets()-1)*sizeof(Bucket<RegisterType>))+sizeof(int);
                write_direc(direcs[i]);
            }
        }  
        return;
    }

    void redo_bucket(Bucket<RegisterType> &bucket, int index){    // For the delete, restructures the bucket in case after the delete it is messed up.
        bucket.count--;
        for(int i = index; i < bucket.count; i++){
            if(i < bucket.count) bucket.keys[i] = bucket.keys[i+1];
        }
    }

    // -- Writting functions -- //

    void write_direc(Directory direc){                            // Updates directory
        int pos = get_dir_position(direc);
        fstream file(hash_file_name, ios::in | ios::out | ios::binary);
        file.seekp((pos * sizeof(Directory)), ios::beg);
        file.write(reinterpret_cast<const char*>(&direc), sizeof(Directory));
        file.close();
    }

    void write_bucket(int pos, Bucket<RegisterType> bucket){      // Updates bucket.
        fstream file(buckets_file_name, ios::in | ios::out | ios::binary);
        file.seekp((pos * sizeof(Bucket<RegisterType>)) + sizeof(int), ios::beg);
        file.write(reinterpret_cast<const char*>(&bucket), sizeof(Bucket<RegisterType>));
        file.close();                               
    }

    // -- Reading functions -- //

    void read_dir(){                                    // Displays directories
        vector<Directory> dirs;
        ifstream file(hash_file_name, ios::in | ios::binary);
        for(int i = 0; i < pow(2, global_depth); i++){
            Directory dir;
            file.read((char*)&dir, sizeof(Directory));
            dirs.push_back(dir);
        }
        cout << "-- Directory:" << endl;
        for(const auto x: dirs){
            cout << x.key << " - " << x.pos_fisica << endl;
        }
        cout << "-----\n";
    }

    void display_bucket(Bucket<RegisterType> bucket){   // Displays content of the given bucket
        for(int i = 0; i < bucket.count; i++){
            cout << bucket.keys[i].getKey() << ", ";
        }
        if(bucket.next == -1){
            cout << bucket.next << endl;
        } else {
            cout << "-> ";
            display_bucket(read_bucket((bucket.next*sizeof(Bucket<RegisterType>))+sizeof(int)));
        }
    }

    int get_dir_position(Directory direc){              // Given a directory, it returns its position.
        return std::stoi(direc.key, nullptr, 2);
    }

    int get_num_buckets(){                              // Return how many written buckets are on the file.
        std::ifstream file(buckets_file_name, ios::in | ios::binary);
        file.seekg(0, ios::end);
        int length = file.tellg();
        //cout << "len: " << length << " sizeof(Bucket): " << sizeof(Bucket) << endl;
        length -= sizeof(int);
        file.seekg(0, ios::beg);
        file.close();
        return length/(sizeof(Bucket<RegisterType>));
    }

    int get_bucket_pos_from_index(char* index) {
        // Given an index, it returns the bucket with that position.
        int index_new = std::stoi(index, nullptr, 2);
        ifstream file(hash_file_name, ios::in | ios::binary);
        Directory direc;
        file.seekg(index_new * sizeof(Directory), ios::beg);
        file.read((char*)&direc, sizeof(Directory));
        file.close();
        return ((direc.pos_fisica-sizeof(int))/sizeof(Bucket<RegisterType>));
    }


    Bucket<RegisterType> bucket_from_bin(string bin){   // Given a binary string ex. 010101 it returns the bucket with that index.
        int index = std::stoi(bin, nullptr, 2);
        ifstream file(hash_file_name, ios::in | ios::binary);
        Directory direc;
        file.seekg(index * sizeof(Directory), ios::beg);
        file.read((char*)&direc, sizeof(Directory));
        file.close();
        return read_bucket(direc.pos_fisica);
    }

    Bucket<RegisterType> read_bucket(int pos){          // Given an int, it returns the bucket in that logical position.
        ifstream file(buckets_file_name, ios::binary | ios::in); 
        file.seekg(pos, ios::beg);
        Bucket<RegisterType> bucket;
        file.read(reinterpret_cast<char*>(&bucket), sizeof(Bucket<RegisterType>));
        file.close();
        return bucket;
    }

    vector<Directory> get_indexes(int index){           // Returns all directories pointing to a bucket with the given index.
        vector<Directory> dirs;
        std::ifstream file(hash_file_name, ios::in | ios::binary);
        // get value of the key passed in the args[]
        Directory dir;
        file.seekg(index*sizeof(Directory), ios::beg);
        file.read((char*)&dir, sizeof(Directory));
        int x = dir.pos_fisica;
        file.seekg(0, ios::beg);
        for(int i = 0; i < pow(2,global_depth); i++){
            Directory t_dir;
            file.seekg(i*sizeof(Directory), ios::beg);
            file.read((char*)&t_dir, sizeof(Directory));
            if(t_dir.pos_fisica == x) {
                dirs.push_back(t_dir);
            }
        }
        return dirs;
    }

    // -- Auxiliar functions -- //

    bool find(typename RegisterType::KeyType key){      // The search function but with boolean as the return-type
        char* i = hashFunc(key, global_depth);
        int index = get_bucket_pos_from_index(i);
        Bucket<RegisterType> bucket = bucket_from_bin(i);

        while(true){
            for(int j = 0; j < bucket.count; j++){
                if(bucket.keys[j].getKey() == key) return true;
            }
            if(bucket.next != -1){
                bucket = read_bucket((bucket.next*sizeof(Bucket<RegisterType>))+sizeof(int));
            } else {
                break;
            }
        }
        return false;
    }

    char* to_binary(int num, int digits) {        
        // Turns an integer into a binary string of length 'digits' and returns it as a char array
        std::bitset<32> bits(num); // convert num to binary representation
        std::string binary_str = bits.to_string().substr(32 - digits); // extract the last 'digits' bits and convert to a string
        char* binary = new char[10]; // allocate memory for the char array
        for (int i = 0; i < digits; i++) {
            binary[i] = binary_str[i]; // copy each character to the 'binary' array
        }
        binary[digits] = '\0'; // add null character to terminate the string
        return binary;
    }

    char* hashFunc(std::string key, int digits){
        int sum = 0;
        for(int i = 0; i < key.length(); i++){
            sum += int(key[i]);
        }
        int n = pow(2, digits);
        return to_binary(sum % int(pow(2,max_depth)), digits);
    }


    char* hashFunc(int key, int digits){               // Hash function for ints
        return to_binary(key % int(pow(2, max_depth)), digits);
    }

public:
    ~ExtendibleHash(){
        fstream output_file(buckets_file_name, ios::binary | ios::in | ios::out);
        output_file.seekp(0, ios::beg);
        output_file.write(reinterpret_cast<const char*>(&global_depth), sizeof(int));
        output_file.close();
    }
};