#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <queue>
#include <functional>
#include <cstring>
#include "RegistroNBA.h"

using namespace std;

/*
    A Register Type requires
    having a publicly accesible typedef for KeyType
    and a publicly accesible function getKey wich returns the key
*/

template<typename RegisterType>
struct AVLFileNode
{
    RegisterType data;
    int left;
    int right;
    int next;
    int parent;
    int height = 0;

    AVLFileNode(){
        parent = left = right = next = -1;
    }

    AVLFileNode(RegisterType R){
        parent = left = right = next = -1;
        this->data = R;
    }
};

template<typename RegisterType>
class avlFileManager
{
    std::string fname;
public:
    vector<RegistroNBA> rangeSearch(typename RegisterType::KeyType idstart, typename RegisterType::KeyType idend) {
        vector<RegistroNBA> vec; //Vector que retornare
        fstream MyFileRead; //Creo mi ifstream para lectura
        MyFileRead.open(fname, ios::in | ios::binary); //Lectura
        MyFileRead.seekg(0); //Me ubico en la posicion de ese registro derecho
        AVLFileNode<RegisterType> noderoot;
        MyFileRead.read((char *) &noderoot, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro derecho
        recursiveRangeSearch(MyFileRead, vec, noderoot, idstart, idend);
        MyFileRead.close();
        if(vec.empty()){
            return vec;
        }
        return vec;
    }

    avlFileManager(std::string filename)
    {
        fname = filename;
        // check for existance of file and create if necesary

        ifstream file;
        file.open(fname);
        if (!file.is_open())
        {
            ofstream fileo;
            fileo.open(fname,ios::trunc | ios::binary);
            fileo.close();
        }
        file.close(); // just in case!
    }
    vector<RegisterType> search(typename RegisterType::KeyType id)
    {
            vector<RegistroNBA> vec; //Vector que retornare

            ifstream MyFileRead; //Creo mi ifstream para lectura
            MyFileRead.open(fname, ios::in | ios::binary); //Lectura
            int node_ptr = 0; //El node_ptr comienza en 0
            int iteration = 0;
            AVLFileNode<RegisterType> nodeforsearch;
            MyFileRead.read((char*)&nodeforsearch,sizeof(AVLFileNode<RegisterType>)); //Comienzo en la root

            while (nodeforsearch.data.getKey() != id) { //Si la root.id es distinta a la cual me piden buscar, continuo iterativamente
                iteration++;
                if (nodeforsearch.data.getKey() < id) { //Si el id es mayor a la root, voy por derecha ->
                    node_ptr = nodeforsearch.right; //El node_ptr ahora será el registro con numero del right
                    MyFileRead.seekg(node_ptr); //Me ubico en la posicion de ese registro derecho
                    if(iteration>1){
                        return vec;
                    }
                    MyFileRead.read((char *) &nodeforsearch, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro derecho
                } else if (nodeforsearch.data.getKey() > id) { //Si el id es menor a la root, voy por izquierda <-
                    node_ptr = nodeforsearch.left; //El node_ptr ahora será el registro con numero del left
                    MyFileRead.seekg(node_ptr); //Me ubico en la posicion de ese registro izquierdo
                    if(iteration>1){
                        return vec;
                    }
                    MyFileRead.read((char *) &nodeforsearch, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro izquierdo
                }
            }

            while (nodeforsearch.next != -1)
            {
                RegistroNBA reg = nodeforsearch.data;
                vec.push_back(reg);
                node_ptr = nodeforsearch.next;
                MyFileRead.seekg(node_ptr);
                MyFileRead.read((char*)&nodeforsearch,sizeof(AVLFileNode<RegisterType>));
            }


            RegistroNBA reg = nodeforsearch.data;
            vec.push_back(reg);
            MyFileRead.close();
            return vec;
    }
    bool add_iter(RegisterType reg)
    {

        // crear un nuevo nodo
        ofstream MyFileWrite;
        MyFileWrite.open(fname, ios::out | ios::in | ios::binary | ios::ate);

        int new_reg = MyFileWrite.tellp();
        AVLFileNode<RegisterType> new_node_ram(reg);
        MyFileWrite.write((char*)&new_node_ram,sizeof(AVLFileNode<RegisterType>));

        MyFileWrite.close();

        // abrimos el archivo para buscar donde insertar
        // de aca salimos con un nodo y un sitio: left, right next

        ifstream MyFileRead;
        MyFileRead.open(fname, ios::in | ios::binary);

        // leer el root
        int node_ptr = 0;
        int direction = 1; // 0 is left, 1 is next and 2 is right
        AVLFileNode<RegisterType> node_value;
        MyFileRead.seekg(node_ptr);
        MyFileRead.read((char*)&node_value,sizeof(AVLFileNode<RegisterType>));

        // avanzar por el arbol
        while (node_ptr != -1)
        {

            if (node_value.data.getKey() < new_node_ram.data.getKey())
            {
                // el que acabo de insertar es mayor
                if (node_value.right == -1)
                {
                    // si eso fuera -1 es que hay sitio
                    // el nodo en el que estoy es al que tengo que hacerle un hijo
                    direction = 2; // tiene que ser un hijo en la derecha
                    node_value.right = new_reg;
                    break; // salir del bucle!
                }
                node_ptr = node_value.right;
                MyFileRead.seekg(node_ptr);
                MyFileRead.read((char*)&node_value,sizeof(AVLFileNode<RegisterType>));
            }
            else if (node_value.data.matchup_id > new_node_ram.data.matchup_id)
            {
               // el que acabo de insertar es menor
                if (node_value.left == -1)
                {
                    // si eso fuera -1 es que hay sitio
                    // el nodo en el que estoy es al que tengo que hacerle un hijo
                    direction = 0; // tiene que ser un hijo en la izquierda
                    node_value.left = new_reg;
                    break; // salir del bucle!
                }
                node_ptr = node_value.left;
                MyFileRead.seekg(node_ptr);
                MyFileRead.read((char*)&node_value,sizeof(AVLFileNode<RegisterType>));
            }
            else // if(node_value.data.matchup_id == new_node_ram.data.matchup_id)
            {
                while (node_value.next != -1)
                {
                    node_ptr = node_value.next;
                    MyFileRead.seekg(node_ptr);
                    MyFileRead.read((char*)&node_value,sizeof(AVLFileNode<RegisterType>));
                }

                if (node_ptr != new_reg) // si el head es el que acabo de insertar, entonces se va a colocar como su propio next
                                         // por lo tanto, evitemos que los nodos sean su propio next!
                {
                    direction = 1;
                    node_value.next = new_reg;
                }
                break;
            }
        }
        // crear el nodo que tengo que updatear en la ram con la info correcta

        MyFileRead.close();

        // updatear en el archivo

        MyFileWrite.open(fname, ios::in | ios::out | ios::binary | ios::ate);
        MyFileWrite.seekp(node_ptr);
        MyFileWrite.write((char*)&node_value,sizeof(AVLFileNode<RegisterType>));

        MyFileWrite.close();
        return 1;
    }

    bool add(RegisterType r)
    {
        // create the new node
        AVLFileNode<RegisterType> new_reg(r);

        fstream file;
        #ifdef FWARNINGS
            file.exceptions(ios::failbit);
        #endif
        file.open(fname, ios::in | ios::out | ios::binary | ios::ate);

        if (file.is_open())
        {
            int pos_new = file.tellg();
            file.write((char*)&new_reg,sizeof(AVLFileNode<RegisterType>));

            // check if inserting the head
            if (pos_new == 0)
            {
                // just store the head
                file.close();
                return true;
            }

            recursiveAdd(new_reg,pos_new,0,file); // if we get here, we allready are in the root
            file.close();
        }
        else
            return false;
        return true;
    }

    bool remove(typename RegisterType::KeyType key)
    {
        // initialize everything
        vector<int> visitedStack;

        fstream file;

        #ifdef FWARNINGS
            file.exceptions(ios::failbit);
        #endif

        // check for file existance and at least 1 item

        file.open(fname,ios::in | ios::binary | ios::ate);
        bool file_opens_and_exists1 = file.tellg() == (int)ios::beg;
        file.close();
        if (file_opens_and_exists1) return false; // file is empty or unexistant

        // now open it good
        file.open(fname,ios::in | ios::out | ios::binary | ios::ate);

        
        // look for the registry:

            // read the root to initialize the thingy

        int node_ptr = 0;
        AVLFileNode<RegisterType> node_v;

        file.seekg(node_ptr);

        file.read((char*)&node_v,sizeof(AVLFileNode<RegisterType>));
        visitedStack.push_back(node_ptr);

        while (node_v.data.getKey() != key)
        {
            visitedStack.push_back(node_ptr);
            if (node_v.data.getKey() < key)
            {
                // go right
                node_ptr = node_v.right;
            }
            else if (node_v.data.getKey() > key)
            {
                // go left
                node_ptr = node_v.left;
            }

            if (node_ptr == -1) return false;

            file.seekg(node_ptr);

            file.read((char*)&node_v,sizeof(AVLFileNode<RegisterType>));
        }

        // cases: no children, 1 child, 2 children
        if (node_v.right == -1 && node_v.left == -1)
        {
            if (node_ptr == 0)// if deleting the head and no children, nuke the file
            {
                file.close();
                std::remove(fname.c_str());
                file.open(fname,ios::out | ios::binary);
                file.close();
                return true;
            }
            // else just set the pointer to this to -1 in the previous
            int parent_ptr = visitedStack[visitedStack.size() - 1]; // im guaranteed at least 1 node
            AVLFileNode<RegisterType> parent_v;
            file.seekg(parent_ptr);
            file.read((char*)&parent_v, sizeof(AVLFileNode<RegisterType>));
            
            if (node_ptr == parent_v.left)
            {
                //dehang from left
                parent_v.left = -1;
            }
            else
            {
                //dehang from right
                parent_v.right = -1;
            }
            
            file.seekp(parent_ptr);
            file.write((char*)&parent_v,sizeof(AVLFileNode<RegisterType>));
        }
        else if (node_v.left == -1)
        {
            // promote right
            int right_ptr = node_v.right;
            AVLFileNode<RegisterType> right_v;
            file.seekg(right_ptr);
            file.read((char*)&right_v,sizeof(AVLFileNode<RegisterType>));
            node_v = right_v;
            file.seekp(node_ptr);
            file.write((char*)&node_v,sizeof(AVLFileNode<RegisterType>));
        }
        else if (node_v.right == -1)
        {
            //promote left
            int left_ptr = node_v.left;
            AVLFileNode<RegisterType> left_v;
            file.seekg(left_ptr);
            file.read((char*)&left_v,sizeof(AVLFileNode<RegisterType>));
            node_v = left_v;
            file.seekp(node_ptr);
            file.write((char*)&node_v,sizeof(AVLFileNode<RegisterType>));
        }
        else
        {
            // swap with succesor and delete succesor. Balance up
            int succesor_ptr = node_v.right;
            AVLFileNode<RegisterType> succesor_v;
            file.seekg(succesor_ptr);
            file.read((char*)&succesor_v,sizeof(AVLFileNode<RegisterType>));

            while (succesor_v.left != -1)
            {
                visitedStack.push_back(succesor_ptr);
                succesor_ptr = succesor_v.left;
                file.seekg(succesor_ptr);
                file.read((char*)&succesor_v,sizeof(AVLFileNode<RegisterType>));
            }

            
            int prev_succesor_ptr = visitedStack[visitedStack.size() - 1]; // this node isnt pushed yet
            AVLFileNode<RegisterType> prev_succesor_v;
            file.seekg(prev_succesor_ptr);
            file.read((char*)&prev_succesor_v,sizeof(AVLFileNode<RegisterType>));

            if (prev_succesor_ptr == node_ptr)
            {
                // succesor is immediately at right of node (update both for consistency)
                prev_succesor_v.right = -1;
                node_v.right = -1;
                // swap the data
                node_v.data = prev_succesor_v.data = succesor_v.data;
                node_v.next = prev_succesor_v.next = succesor_v.next;
            }
            else
            {
                // succesor is hanged on left
                prev_succesor_v.left = -1;
                // swap the data
                node_v.data = succesor_v.data;
                node_v.next = succesor_v.next;
            }

            file.seekp(prev_succesor_ptr);
            file.write((char*)&prev_succesor_v,sizeof(AVLFileNode<RegisterType>));
            file.seekp(node_ptr);
            file.write((char*)&node_v,sizeof(AVLFileNode<RegisterType>));
        }
        
        while (!visitedStack.empty())
        {
            int ptr = visitedStack[visitedStack.size() - 1];
            
            update_height(ptr,file);
            balance(ptr,file);

            visitedStack.pop_back();
        }

        file.close();
        return true;
    }
private:

    void recursiveRangeSearch(fstream& file, vector<RegistroNBA> &vec, AVLFileNode<RegisterType>& node, typename RegisterType::KeyType idstart, typename RegisterType::KeyType idend) {
        int node_ptr = 0; //El node_ptr comienza en 0

        if (node.data.getKey() < idstart) {
            if(node.right != -1) {
                node_ptr = node.right; //El node_ptr ahora será el registro con numero del right
                file.seekg(node_ptr); //Me ubico en la posicion de ese registro derecho
                file.read((char *) &node, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro derecho
                recursiveRangeSearch(file, vec, node, idstart, idend);
            }
        }else if(node.data.getKey() > idend) {
            if(node.left != -1) {
                node_ptr = node.left; //El node_ptr ahora será el registro con numero del right
                file.seekg(node_ptr); //Me ubico en la posicion de ese registro derecho
                file.read((char *) &node, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro derecho
                recursiveRangeSearch(file, vec, node, idstart, idend);
            }else{
                return;
            }
        }else{
            vec.push_back(node.data);
            if(node.next != -1) {
                AVLFileNode nodetemp = node;
                while (node.next != -1) {
                    node_ptr = node.next;
                    file.seekg(node_ptr);
                    file.read((char *) &node, sizeof(AVLFileNode<RegisterType>));
                    vec.push_back(node.data);
                }
                node = nodetemp;
            }

            if(node.left != -1) {
                node_ptr = node.left; //El node_ptr ahora será el registro con numero del right
                file.seekg(node_ptr); //Me ubico en la posicion de ese registro derecho
                file.read((char *) &node, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro derecho
                recursiveRangeSearch(file, vec, node, idstart, idend);
            }else if(node.right != -1) {
                node_ptr = node.right; //El node_ptr ahora será el registro con numero del right
                file.seekg(node_ptr); //Me ubico en la posicion de ese registro derecho
                file.read((char *) &node, sizeof(AVLFileNode<RegisterType>)); //Leo ese registro derecho
                recursiveRangeSearch(file, vec, node, idstart, idend);
            }

        }
    }



    void recursiveAdd(AVLFileNode<RegisterType>& to_insert,int adressof_new,int root_node_ptr, fstream& file)
    {
        // read up the root node
        file.seekg(root_node_ptr);
        AVLFileNode<RegisterType> root_data;

        file.read((char*)&root_data,sizeof(AVLFileNode<RegisterType>));

        // perform ifs
        if (to_insert.data.getKey() < root_data.data.getKey())
        {
            // attempt to isert on left
            if (root_data.left == -1)
            {
                // insert on left
                // sort out pointers

                root_data.left = adressof_new;

                file.seekp(root_node_ptr);

                file.write((char*)&root_data,sizeof(AVLFileNode<RegisterType>));

                // update the reg itself to know its parent

                to_insert.parent = root_node_ptr;
                file.seekp(adressof_new);

                file.write((char*)&to_insert,sizeof(AVLFileNode<RegisterType>));

            }
            else
            {
                recursiveAdd(to_insert,adressof_new,root_data.left,file);
            }
            
        }
        else if (to_insert.data.getKey() > root_data.data.getKey())
        {
            // attempt to insert on right
            if (root_data.right == -1)
            {
                // insert on left
                // sort out pointers

                root_data.right = adressof_new;

                file.seekp(root_node_ptr);

                file.write((char*)&root_data,sizeof(AVLFileNode<RegisterType>));

                // update the reg itself to know its parent

                to_insert.parent = root_node_ptr;
                file.seekp(adressof_new);

                file.write((char*)&to_insert,sizeof(AVLFileNode<RegisterType>));
            }
            else
            {
                recursiveAdd(to_insert,adressof_new,root_data.right,file);
            }
        }
        else
        {
            // definitelly insert on the end of the nexts
            int linked_root_ptr = root_node_ptr;

            while (root_data.next != -1)
            {
                linked_root_ptr = root_data.next;

                file.seekg(linked_root_ptr);

                file.read((char*)&root_data,sizeof(AVLFileNode<RegisterType>));
            }
            
            // now set up the pointers
            file.seekp(linked_root_ptr);

            root_data.next = adressof_new;

            file.write((char*)&root_data,sizeof(AVLFileNode<RegisterType>));

            // update the reg itself to know its parent

            to_insert.parent = linked_root_ptr;
            file.seekp(adressof_new);

            file.write((char*)&to_insert,sizeof(AVLFileNode<RegisterType>));
        }

        update_height(root_node_ptr,file);
        balance(root_node_ptr,file);
    }


    void update_height(int node_ptr,fstream& file)
    {
        // read the node
        AVLFileNode<RegisterType> avlfilenode;

        file.seekg(node_ptr);

        file.read((char*)&avlfilenode, sizeof(AVLFileNode<RegisterType>));

        // updates the height of this node
        
        // get left height
        int lheight = 0;
        if (avlfilenode.left == -1)
            lheight = -1;
        else
        {
            AVLFileNode<RegisterType> temp_l;
            file.seekg(avlfilenode.left);

            file.read((char*)&temp_l,sizeof(AVLFileNode<RegisterType>));

            lheight = temp_l.height;
        }
        // get right height
        int rheight = 0;
        if (avlfilenode.right == -1)
            rheight = -1;
        else
        {
            AVLFileNode<RegisterType> temp_r;
            file.seekg(avlfilenode.right);

            file.read((char*)&temp_r,sizeof(AVLFileNode<RegisterType>));

            rheight = temp_r.height;
        }

        // store updated node

        avlfilenode.height = max(lheight,rheight) + 1;

        file.seekp(node_ptr);

        file.write((char*)&avlfilenode,sizeof(AVLFileNode<RegisterType>));
    }

    int balance_factor(int node_ptr,fstream& file)
    {
        if (node_ptr == -1)
            return 0;
        file.seekg(node_ptr);

        AVLFileNode<RegisterType> root_node_data, left_node, right_node;

        file.read((char*)&root_node_data,sizeof(AVLFileNode<RegisterType>));

        int lheight, rheight; 
        // find heights of children

        if (root_node_data.left == -1) //left
        {
            lheight = -1;
        }
        else
        {
            file.seekg(root_node_data.left);

            file.read((char*)&left_node,sizeof(AVLFileNode<RegisterType>));

            lheight = left_node.height;
        }

        if (root_node_data.right == -1) //right
        {
            rheight = -1;
        }
        else
        {
            file.seekg(root_node_data.right);

            file.read((char*)&right_node,sizeof(AVLFileNode<RegisterType>));

            rheight = right_node.height;
        }

        return lheight - rheight;
    }

    void balance(int node_ptr,fstream& file)
    {
        #ifdef DEBUG
            if (!file.good())
            {
                std::cout << "[ERROR] File error before balancing" << std::endl;
            }
        #endif
        int b_fact = balance_factor(node_ptr,file);
        
        if (b_fact > 1)
        {
            //unbalanced to the left
            file.seekg(node_ptr);

            AVLFileNode<RegisterType> root_node_data;

            file.read((char*)&root_node_data,sizeof(AVLFileNode<RegisterType>));

            int left_balance_fact = balance_factor(root_node_data.left,file);

            if (left_balance_fact < 0)
            {
                rotate_left(root_node_data.left,file);
            }

            rotate_right(node_ptr,file);

        }
        else if (b_fact < -1)
        {
            //unbalanced to the right
            file.seekg(node_ptr);

            AVLFileNode<RegisterType> root_node_data;
            file.read((char*)&root_node_data,sizeof(AVLFileNode<RegisterType>));

            int right_balance_fact = balance_factor(root_node_data.right,file);

            if (right_balance_fact > 0)
            {
                rotate_right(root_node_data.right,file);
            }
            
            rotate_left(node_ptr,file);
        }
    }

    void rotate_right(int top,fstream& file)
    {
        /*
        
        Algorithm goes as follows:

        Say we have the following tree that we want to rotate left:
        *top, vers and bottom refer to the phisiscal positions
                         A (top) <- head
                       /   \ 
                   B(vers)  i
                   /    \ 
            C (Bottom)   j   
               /   \
              k     g 
                    
        First change the vers left to point to top, and top left to point to vers right
                         
                   B(vers)  
                   /     \       
            C (Bottom)    A (top)  <- head
               /   \    /   \ 
              k     g   j    i
        
        Swap the phisical positions of top and vers, which automatically gives head back to the logical top.

                   B(top)  <- head
                   /     \       
            C (Bottom)    A (vers) 
               /   \    /   \ 
              k     g   j    i

        */
        int bottom, vers;
        // read node itself
        file.seekg(top);

        //fetch my relevant info

        AVLFileNode<RegisterType> top_v, vers_v, bottom_v;

        file.read((char*)&top_v,sizeof(AVLFileNode<RegisterType>));

        vers = top_v.left;

        file.seekg(vers);

        file.read((char*)&vers_v,sizeof(AVLFileNode<RegisterType>));

        bottom = vers_v.left;

        // do the pointer modification
        int old_right = vers_v.right;
        vers_v.right = vers; // because of the copying, this will swap with top, but pointer doesnt follow the data
        top_v.left = old_right;

        // store with vers on top and top on vers

        file.seekp(top);

        file.write((char*)&vers_v,sizeof(AVLFileNode<RegisterType>));

        file.seekp(vers);

        file.write((char*)&top_v,sizeof(AVLFileNode<RegisterType>));

        update_height(vers,file);
        update_height(top,file);

        #ifdef DEBUG
            if (!file.good())
            {
                std::cout << "[ERROR] File error produced on right rotate" << std::endl;
            }
        #endif
    }

    void rotate_left(int node,fstream& file)
    {
        AVLFileNode<RegisterType> node_v, rchild_v;
        int rchild;

        file.seekg(node);

        file.read((char*)&node_v,sizeof(AVLFileNode<RegisterType>));

        rchild = node_v.right;

        file.seekg(rchild);

        file.read((char*)&rchild_v,sizeof(AVLFileNode<RegisterType>));

        // pointer modifications
        node_v.right = rchild_v.left;
        rchild_v.left = rchild;

        // store swapped

        file.seekp(node);

        file.write((char*)&rchild_v,sizeof(AVLFileNode<RegisterType>));

        file.seekp(rchild);

        file.write((char*)&node_v,sizeof(AVLFileNode<RegisterType>));

        update_height(rchild,file);
        update_height(node,file);

        #ifdef DEBUG
            if (!file.good())
            {
                std::cout << "[ERROR] File error produced on left rotate" << std::endl;
            }
        #endif
    };

#ifdef DEBUG

public:
    // Debugging Functions
    void showFlat()
    {
        ifstream file;
        file.open(fname, ios::in | ios::binary);
        while (!file.eof())
        {
            int pos =  file.tellg();
            AVLFileNode<RegisterType> a;
            file.read((char*)&a,sizeof(AVLFileNode<RegisterType>));
            cout << pos << ":: "<< "pk: " << a.data.getKey() << "; flags: left = " << a.left << ", right: " << a.right  << ", next: " << a.next  << " H: " << a.height << endl;
        }
        std::cout << endl;

        file.clear();
        file.close(); 
    }

    void showFileAVLtree()
    {
        ifstream file;
        file.open(fname, ios::in | ios::binary);
        showFileAVLtree(file,0,0);
        file.close();
    }


    void ror(int node)
    {
        fstream file;
        file.open(fname,ios::in | ios::out | ios::binary | ios::ate);

        rotate_right(node,file);

        file.close();

    }

    void rol(int node)
    {
        fstream file;
        file.open(fname,ios::in | ios::out | ios::binary | ios::ate);

        rotate_left(node,file);

        file.close();

    }
private:

    #define TAB_LEN 4

    void showFileAVLtree(ifstream& file, int nodeptr, int depth)
    {
        if (nodeptr == -1)
            return;
        
        file.seekg(nodeptr);
        AVLFileNode<RegisterType> node_read; // this is the node in the tree (first of the linked list)
        file.read((char*)&node_read,sizeof(AVLFileNode<RegisterType>));

        // print the whole linked list


        AVLFileNode<RegisterType> linked_node; // this is the node in the tree (first of the linked list)
        int node_linked_ptr = nodeptr;

        for (int i = 0; i<depth*TAB_LEN;i++)
            cout << ' ';

        while (node_linked_ptr!= -1)
        {
            file.seekg(node_linked_ptr);
            file.read((char*)&linked_node,sizeof(AVLFileNode<RegisterType>));
            // show it
            cout << "["<<linked_node.data.matchup_id << ": " << linked_node.data.home_team << " vs " << linked_node.data.away_team << " H: " << linked_node.height <<  "] -->  ";
            // advance it
            node_linked_ptr = linked_node.next;
        }
        cout << " X " << endl;

        // read the register and print it
        showFileAVLtree(file,node_read.left,depth+1);
        showFileAVLtree(file,node_read.right,depth+1);
    }

#endif
};