#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <bitset>
#include <cmath>
using namespace std;

const int fillfactor = 3;

struct RegistroNBA{
    typedef std::string KeyType;

    KeyType getKey(){
        return home_team;
    }
    char home_team[4]{0};
    long matchup_id = 1223334444;
    long home_points;
    char away_team[4]{0};
    long away_points;

    friend ostream& operator<<(ostream& os, RegistroNBA reg){
        os << "key: " << reg.getKey() << " | matchup_id: " << reg.matchup_id << endl;
        return os;
    }
};

struct RegistroTornados{
    typedef std::string KeyType;

    KeyType getKey(){
        return date;
    }

    char date[11];
    char state[2];
    long magnitude = 8;

    friend ostream& operator<<(ostream& os, RegistroTornados reg){
        os << "key: " << reg.getKey() << " | magnitude: " << reg.magnitude << endl;
        return os;
    }
};

template <typename TK, typename TV>
struct Register {
    TK name;
    TV age;
    TK get_key(){ return name; }

    friend ostream& operator<<(ostream& os, const Register<TK,TV>& reg){
        os << "Name: " << reg.name << " | age: " << reg.age << endl;
        return os;
    }
};

struct Directory{
    string key;
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

    bool isfull(){
        return count == fillfactor;
    }
};

template <typename RegisterType>
class ExtendibleHash{
private:
    string hash_file_name;
    string buckets_file_name;
    int global_depth = 1;
    int max_depth = 3;

public:

    ExtendibleHash(string hfn, string bfn, int max_depth, int gd=1){
        hash_file_name = hfn;
        buckets_file_name = bfn;    
        global_depth = gd;
        
        initialize_buckets();   // Creates / Read buckets
    }

    void print_all_buckets(){
        for(int i = 0; i < get_num_buckets(); i++){
            cout << "[" << i << "] ";
            Bucket<RegisterType> bucket = read_bucket((i*sizeof(Bucket<RegisterType>))+sizeof(int));
            for(int j = 0; j < bucket.count; j++){
                cout << bucket.keys[j].getKey() << ",";
            }
            cout << endl;
        }
    }

    bool remove(typename RegisterType::KeyType key){
        string i = hashFunc(key, global_depth);
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

    void display_indexes(){
        read_dir();
    }

    void display_buckets(){
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

    
    RegisterType search(typename RegisterType::KeyType key){
        string i = hashFunc(key, global_depth);
        int index = get_bucket_pos_from_index(i);
        Bucket<RegisterType> bucket = bucket_from_bin(i);

        while(true){
            for(int j = 0; j < bucket.count; j++){
                if(bucket.keys[j].getKey() == key) return bucket.keys[j];
            }
            if(bucket.next != -1){
                bucket = read_bucket((bucket.next*sizeof(Bucket<RegisterType>))+sizeof(int));
            } else {
                break;
            }
        }
        throw exception();
    }

    void insert(RegisterType reg){
        if(find(reg.getKey())){
            cout << "Register already exist." << endl;
            return;
        }

        string i = hashFunc(reg.getKey(), global_depth); // returns binary string
        int index = get_bucket_pos_from_index(i);    // 
        Bucket<RegisterType> bucket = bucket_from_bin(i);

        cout << "trying to insert " << reg.getKey() << " in bucket " << index << endl;
        display_bucket(bucket);
        cout << "----\n";

        if(!bucket.isfull())
        {                            // Insert the key into the bucket
                                    cout << "normal insert" << endl;
            bucket.insert(reg);
            write_bucket(index, bucket);  
            read_dir();  
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

private:

    Bucket<RegisterType> bucket_from_bin(string bin){
        int index = std::stoi(bin, nullptr, 2);
        ifstream file(hash_file_name, ios::in | ios::binary);
        Directory direc;
        file.seekg(index * sizeof(Directory), ios::beg);
        file.read((char*)&direc, sizeof(Directory));
        file.close();
        return read_bucket(direc.pos_fisica);
    }

    void display_bucket(Bucket<RegisterType> bucket){
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

    bool find(typename RegisterType::KeyType key){
        string i = hashFunc(key, global_depth);
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

    void reindex(int index){
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

    void split(Bucket<RegisterType> bucket, int index){
        Bucket<RegisterType> new_b;
        Bucket<RegisterType> old_b;
        new_b.local_depth = bucket.local_depth+1;
        old_b.local_depth = bucket.local_depth+1;

        string first_index = hashFunc(bucket.keys[0].getKey(), global_depth);;

        for(int i = 0; i < bucket.count; i++){
            string ind = hashFunc(bucket.keys[i].getKey(), global_depth);

            cout << "index: " << index << endl;
            cout << "rehashing " << bucket.keys[i].getKey() << " with index: " << ind << endl;

            if(ind[0] == '0') old_b.insert(bucket.keys[i]);
            else new_b.insert(bucket.keys[i]);
        }

        write_bucket(index, old_b);
        cout << "wrote old bucket with now " << old_b.count << " elements\n";
        write_bucket(get_num_buckets(), new_b);
        cout << "wrote new bucket with " << new_b.count << " elements\n";
    }

    int get_dir_position(Directory direc){
        return std::stoi(direc.key, nullptr, 2);
    }

    vector<Directory> get_indexes(int index){
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

    void duplicate_dir(int index){
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
                    cout << "found!" << endl;
                    x.pos_fisica = ((get_num_buckets()-1)*sizeof(Bucket<RegisterType>))+sizeof(int);
                }
                x.key = to_binary(i, global_depth);
                write_file.write((char*)&x, sizeof(Directory));
                i++;
            }
        }
        write_file.close();
        print_all_buckets();
        read_dir();
    }

    int get_num_buckets(){
        std::ifstream file(buckets_file_name, ios::in | ios::binary);
        file.seekg(0, ios::end);
        int length = file.tellg();
        //cout << "len: " << length << " sizeof(Bucket): " << sizeof(Bucket) << endl;
        length -= sizeof(int);
        file.seekg(0, ios::beg);
        file.close();
        return length/(sizeof(Bucket<RegisterType>));
    }

    void read_dir(){
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

    void initialize_buckets(){
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

    void redo_bucket(Bucket<RegisterType> &bucket, int index){
        bucket.count--;
        for(int i = index; i < bucket.count; i++){
            if(i < bucket.count) bucket.keys[i] = bucket.keys[i+1];
        }
    }

    int get_bucket_pos_from_index(string index){
        int index_new = std::stoi(index, nullptr, 2);
        ifstream file(hash_file_name, ios::in | ios::binary);
        Directory direc;
        file.seekg(index_new * sizeof(Directory), ios::beg);
        file.read((char*)&direc, sizeof(Directory));
        file.close();
        return ((direc.pos_fisica-sizeof(int))/sizeof(Bucket<RegisterType>));
    }

    /*Bucket<RegisterType> get_bucket_from_hash(int index){
        ifstream file(hash_file_name, ios::in | ios::binary);
        Directory dir;
        file.seekg(index * sizeof(Directory));
        file.read((char*)&dir, sizeof(Directory));
        file.close();
        return read_bucket(dir.pos_fisica);
    }*/

    void write_direc(Directory direc){
        int pos = get_dir_position(direc);
        fstream file(hash_file_name, ios::in | ios::out | ios::binary);
        file.seekp((pos * sizeof(Directory)), ios::beg);
        file.write(reinterpret_cast<const char*>(&direc), sizeof(Directory));
        file.close();
    }

    void write_bucket(int pos, Bucket<RegisterType> bucket){      // Opens file
        fstream file(buckets_file_name, ios::in | ios::out | ios::binary);
        file.seekp((pos * sizeof(Bucket<RegisterType>)) + sizeof(int), ios::beg);
        file.write(reinterpret_cast<const char*>(&bucket), sizeof(Bucket<RegisterType>));
        file.close();                               // Write bucket into the file
    }
    
    Bucket<RegisterType> read_bucket(int pos){
        ifstream file(buckets_file_name, ios::binary | ios::in); 
        file.seekg(pos, ios::beg);
        Bucket<RegisterType> bucket;
        file.read(reinterpret_cast<char*>(&bucket), sizeof(Bucket<RegisterType>));
        file.close();
        return bucket;
    }

    std::string to_binary(int num, int digits) {
        std::bitset<32> bits(num); // convert num to binary representation
        return bits.to_string().substr(32 - digits); // extract the last 'digits' bits and return as a string
    }

    string hashFunc(string key, int digits){
        int sum = 0;
        for(const char c : key) sum += int(c);
        int n = pow(2, digits);
        return to_binary(sum % int(pow(2,max_depth)), digits);
    }

    string hashFunc(int key, int digits){
        return to_binary(key % int(pow(2, max_depth)), digits);
    }

public:

    void buckets_disp(){
        for(int i = 0; i < get_num_buckets(); i++){
            Bucket<RegisterType> bucket = read_bucket((i * sizeof(Bucket<RegisterType>)) + sizeof(int));
            for(int j = 0; j < bucket.count; j++){
                cout << bucket.keys[j].get_key() << ", ";
            }
            cout << endl;
        }
    }

    ~ExtendibleHash(){
        fstream output_file(buckets_file_name, ios::binary | ios::in | ios::out);
        output_file.seekp(0, ios::beg);
        output_file.write(reinterpret_cast<const char*>(&global_depth), sizeof(int));
        output_file.close();
    }
};

int main(){
    ExtendibleHash<RegistroTornados> hashTable("hash.dat", "buckets.dat", 3);

    RegistroTornados reg;
    while(true){
        int option;
        cout << "[0] : display\n[1] : insert\n[2] : search\n[3] : delete\n[4] : exit | ";
        cin >> option;
        switch(option){
            case 0:
                hashTable.display_indexes();
                hashTable.display_buckets();
                hashTable.print_all_buckets();
            break;
            case 1:
                cout << "\nEnter home_team: ";
                cin >> reg.date;
                hashTable.insert(reg);
            break;
            case 2:
                cout << "\nEnter home_team: ";
                cin >> reg.date;
                cout << "Found: " << hashTable.search(reg.date) << endl;
            break;
            case 3:
                cout << "\nEnter name: ";
                cin >> reg.date;
                cout << "Remove: " << hashTable.remove(reg.date) << endl;
            break;
            case 4:
                return 0;
            break;
        }
    }
    return 0;
};