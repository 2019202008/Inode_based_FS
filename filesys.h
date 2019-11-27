#ifndef filesys
#define filesys

#include <bits/stdc++.h>
#include <unistd.h>

#define block_size 512	
#define num_of_blocks 2*20	
#define num_of_fd 3	
#define ll long long int	
#define num_of_direct_pointer 5	
#define num_of_inodes 2*5	
#define number_of_blocks_for_data 2*10
#define Buffer_Size 50
#define num_of_inode_pointer 5

using namespace std;
struct file_inode_position
{
    ll inodeid;
    ll fd;
    string mode;
    ll position;
};

struct inode_directory
{
    string parent;

    string directory_name;

    vector<string> sub_directory_or_file_name;

    string path;
};

struct inode
{
    string filename;
    string filepath;
    ll filesize;
    ll block_ptr[num_of_direct_pointer];
    ll inodepointer[num_of_inode_pointer];
    struct inode* indirect_ptr[2];
    bool indirect_data_ptr_present, is_directory;
    inode(){
        is_directory = false;
        indirect_data_ptr_present = true;
        for(int i=0; i<num_of_inode_pointer; i++){	
            inodepointer[i]=-1;	
        }	
        for(int i=0; i<num_of_direct_pointer; i++){	
            block_ptr[i]=-1;	
        }
    }
};

extern bool inode_bmap[num_of_inodes];
extern bool data_bmap[number_of_blocks_for_data];
extern struct inode array_of_inodes[num_of_inodes];

struct superblock
{
    ll num_of_blocks_for_sb;

    ll num_of_blocks_for_inode_bmap;

    ll number_of_blocks_for_data_bmap;

    vector <file_inode_position> file_inode_position_map;

    vector<inode_directory> directory;

    bool filedescriptor_bmap[num_of_fd];

    superblock()    {
        num_of_blocks_for_sb = ceil(((float)sizeof(superblock)) / block_size);

        num_of_blocks_for_inode_bmap =  ceil(((float)sizeof(inode_bmap)) / block_size);

        number_of_blocks_for_data_bmap = num_of_blocks - (num_of_blocks_for_sb + num_of_blocks_for_inode_bmap + number_of_blocks_for_data + num_of_inodes);

    }
};

extern struct superblock sblock;

#endif