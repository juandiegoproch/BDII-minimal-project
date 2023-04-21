#define DEBUG
#define FWARNINGS

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include "RegistroNBA.h"

using namespace std;


struct AVLFileNode
{
    RegistroNBA data;
    int left;
    int right;
    int next;
    int parent;
    int height = 0;

    AVLFileNode(){
        parent = left = right = next = -1;
    }

    AVLFileNode(RegistroNBA R){
        parent = left = right = next = -1;
        this->data = R;
    }
};

class avlfileManager
{
    std::string fname;
public:

    avlfileManager(std::string filename)
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
    vector<RegistroNBA> search(long id)
    {
        return vector<RegistroNBA>();
    }
    vector<RegistroNBA> rangeSearch(long id)
    {
        return vector<RegistroNBA>();
    }
    bool add_iter(RegistroNBA reg)
    {

        // crear un nuevo nodo
        ofstream MyFileWrite;
        MyFileWrite.open(fname, ios::out | ios::in | ios::binary | ios::ate);

        int new_reg = MyFileWrite.tellp();
        AVLFileNode new_node_ram(reg);
        MyFileWrite.write((char*)&new_node_ram,sizeof(AVLFileNode));

        MyFileWrite.close();

        // abrimos el archivo para buscar donde insertar
        // de aca salimos con un nodo y un sitio: left, right next

        ifstream MyFileRead;
        MyFileRead.open(fname, ios::in | ios::binary);

        // leer el root
        int node_ptr = 0;
        int direction = 1; // 0 is left, 1 is next and 2 is right
        AVLFileNode node_value;
        MyFileRead.seekg(node_ptr);
        MyFileRead.read((char*)&node_value,sizeof(AVLFileNode));

        // avanzar por el arbol
        while (node_ptr != -1)
        {

            if (node_value.data.matchup_id < new_node_ram.data.matchup_id)
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
                MyFileRead.read((char*)&node_value,sizeof(AVLFileNode));
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
                MyFileRead.read((char*)&node_value,sizeof(AVLFileNode));
            }
            else // if(node_value.data.matchup_id == new_node_ram.data.matchup_id)
            {
                while (node_value.next != -1)
                {
                    node_ptr = node_value.next;
                    MyFileRead.seekg(node_ptr);
                    MyFileRead.read((char*)&node_value,sizeof(AVLFileNode));
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
        MyFileWrite.write((char*)&node_value,sizeof(AVLFileNode));

        MyFileWrite.close();
        return 1;
    }

    bool add(RegistroNBA r)
    {
        // create the new node
        AVLFileNode new_reg(r);

        fstream file;
        #ifdef FWARNINGS
            file.exceptions(ios::failbit);
        #endif
        file.open(fname, ios::in | ios::out | ios::binary | ios::ate);

        if (file.is_open())
        {
            int pos_new = file.tellg();
            file.write((char*)&new_reg,sizeof(AVLFileNode));

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

    bool remove(long id)
    {
        return false;
    }
private:
    void recursiveAdd(AVLFileNode& to_insert,int adressof_new,int root_node_ptr, fstream& file)
    {
        // read up the root node
        file.seekg(root_node_ptr);
        file.seekp(root_node_ptr);
        AVLFileNode root_data;

        file.read((char*)&root_data,sizeof(AVLFileNode));

        // perform ifs
        if (to_insert.data.matchup_id < root_data.data.matchup_id)
        {
            // attempt to isert on left
            if (root_data.left == -1)
            {
                // insert on left
                // sort out pointers

                root_data.left = adressof_new;

                file.seekg(root_node_ptr);
                file.seekp(root_node_ptr);

                file.write((char*)&root_data,sizeof(AVLFileNode));

                // update the reg itself to know its parent

                to_insert.parent = root_node_ptr;
                file.seekg(adressof_new);
                file.seekp(adressof_new);

                file.write((char*)&to_insert,sizeof(AVLFileNode));

            }
            else
            {
                recursiveAdd(to_insert,adressof_new,root_data.left,file);
            }
            
        }
        else if (to_insert.data.matchup_id > root_data.data.matchup_id)
        {
            // attempt to insert on right
            if (root_data.right == -1)
            {
                // insert on left
                // sort out pointers

                root_data.right = adressof_new;

                file.seekg(root_node_ptr);
                file.seekp(root_node_ptr);

                file.write((char*)&root_data,sizeof(AVLFileNode));

                // update the reg itself to know its parent

                to_insert.parent = root_node_ptr;
                file.seekg(adressof_new);
                file.seekp(adressof_new);

                file.write((char*)&to_insert,sizeof(AVLFileNode));
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
                file.seekp(linked_root_ptr);

                file.read((char*)&root_data,sizeof(AVLFileNode));
            }
            
            // now set up the pointers
            file.seekg(linked_root_ptr);
            file.seekp(linked_root_ptr);

            root_data.next = adressof_new;

            file.write((char*)&root_data,sizeof(AVLFileNode));

            // update the reg itself to know its parent

            to_insert.parent = linked_root_ptr;
            file.seekg(adressof_new);
            file.seekp(adressof_new);

            file.write((char*)&to_insert,sizeof(AVLFileNode));
        }

        update_height(root_node_ptr,file);
        balance(root_node_ptr,file);
    }


    void update_height(int node_ptr,fstream& file)
    {
        // read the node
        AVLFileNode avlfilenode;

        file.seekg(node_ptr);
        file.seekp(node_ptr);

        file.read((char*)&avlfilenode, sizeof(AVLFileNode));

        // updates the height of this node
        
        // get left height
        int lheight = 0;
        if (avlfilenode.left == -1)
            lheight = -1;
        else
        {
            AVLFileNode temp_l;
            file.seekg(avlfilenode.left);
            file.seekp(avlfilenode.left);

            file.read((char*)&temp_l,sizeof(AVLFileNode));

            lheight = temp_l.height;
        }
        // get right height
        int rheight = 0;
        if (avlfilenode.right == -1)
            rheight = -1;
        else
        {
            AVLFileNode temp_r;
            file.seekg(avlfilenode.right);
            file.seekp(avlfilenode.right);

            file.read((char*)&temp_r,sizeof(AVLFileNode));

            rheight = temp_r.height;
        }

        // store updated node

        avlfilenode.height = max(lheight,rheight) + 1;

        file.seekg(node_ptr);
        file.seekp(node_ptr);

        file.write((char*)&avlfilenode,sizeof(AVLFileNode));
    }

    int balance_factor(int node_ptr,fstream& file)
    {
        if (node_ptr == -1)
            return 0;
        file.seekg(node_ptr);
        file.seekp(node_ptr);

        AVLFileNode root_node_data, left_node, right_node;

        file.read((char*)&root_node_data,sizeof(AVLFileNode));

        int lheight, rheight; 
        // find heights of children

        if (root_node_data.left == -1) //left
        {
            lheight = -1;
        }
        else
        {
            file.seekg(root_node_data.left);
            file.seekp(root_node_data.left);

            file.read((char*)&left_node,sizeof(AVLFileNode));

            lheight = left_node.height;
        }

        if (root_node_data.right == -1) //right
        {
            rheight = -1;
        }
        else
        {
            file.seekg(root_node_data.right);
            file.seekp(root_node_data.right);

            file.read((char*)&right_node,sizeof(AVLFileNode));

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
            file.seekp(node_ptr);

            AVLFileNode root_node_data;

            file.read((char*)&root_node_data,sizeof(AVLFileNode));

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
            file.seekp(node_ptr);

            AVLFileNode root_node_data;
            file.read((char*)&root_node_data,sizeof(AVLFileNode));

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
        file.seekp(top);

        //fetch my relevant info

        AVLFileNode top_v, vers_v, bottom_v;

        file.read((char*)&top_v,sizeof(AVLFileNode));

        vers = top_v.left;

        file.seekg(vers);
        file.seekp(vers);

        file.read((char*)&vers_v,sizeof(AVLFileNode));

        bottom = vers_v.left;

        // do the pointer modification
        int old_right = vers_v.right;
        vers_v.right = vers; // because of the copying, this will swap with top, but pointer doesnt follow the data
        top_v.left = old_right;

        // store with vers on top and top on vers

        file.seekg(top);
        file.seekp(top);

        file.write((char*)&vers_v,sizeof(AVLFileNode));

        file.seekg(vers);
        file.seekp(vers);

        file.write((char*)&top_v,sizeof(AVLFileNode));

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
        AVLFileNode node_v, rchild_v;
        int rchild;

        file.seekg(node);
        file.seekp(node);

        file.read((char*)&node_v,sizeof(AVLFileNode));

        rchild = node_v.right;

        file.seekg(rchild);
        file.seekp(rchild);

        file.read((char*)&rchild_v,sizeof(AVLFileNode));

        // pointer modifications
        node_v.right = rchild_v.left;
        rchild_v.left = rchild;

        // store swapped

        file.seekg(node);
        file.seekp(node);

        file.write((char*)&rchild_v,sizeof(AVLFileNode));

        file.seekg(rchild);
        file.seekp(rchild);

        file.write((char*)&node_v,sizeof(AVLFileNode));

        update_height(rchild,file);
        update_height(node,file);

        #ifdef DEBUG
            if (!file.good())
            {
                std::cout << "[ERROR] File error produced on left rotate" << std::endl;
            }
        #endif
    };
public:
#ifdef DEBUG
    // Debugging Functions
    void showFlat()
    {
        ifstream file;
        file.open(fname, ios::in | ios::binary);
        while ((int)file.tellg() == ios::end)
        {
            AVLFileNode a;
            file.read((char*)&a,sizeof(AVLFileNode));
            cout << "pk: " << a.data.matchup_id << "; flags: left = " << ((a.left == -1)?-1:a.left/(long)sizeof(AVLFileNode)) << ", right: " << ((a.right == -1)?-1:a.right/(long)sizeof(AVLFileNode)) << ", next: " << ((a.next== -1)?-1:a.next/(long)sizeof(AVLFileNode))  << " H: " << a.height << endl;
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
        AVLFileNode node_read; // this is the node in the tree (first of the linked list)
        file.read((char*)&node_read,sizeof(AVLFileNode));

        // print the whole linked list


        AVLFileNode linked_node; // this is the node in the tree (first of the linked list)
        int node_linked_ptr = nodeptr;

        for (int i = 0; i<depth*TAB_LEN;i++)
            cout << ' ';

        while (node_linked_ptr!= -1)
        {
            file.seekg(node_linked_ptr);
            file.read((char*)&linked_node,sizeof(AVLFileNode));
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

int main(void)
{

    avlfileManager fmanager("avlfile.avl");
    /*
    for (int i = 0; i<10000; i++)
    {
        int key = rand()%10000;

        fmanager.add(RegistroNBA{"AAA",key,0,"AAB",0});

    }
    cout << endl;
    */
    fmanager.showFileAVLtree();


}