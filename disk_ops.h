#ifndef disk_ops
#define disk_ops 

#include "filesys.h"
#include "fs_read.h"
#include "filewrite.h"

static int isactive;
struct superblock sblock;

FILE *disk_ptr;
bool inode_bmap[num_of_inodes];
bool data_bmap[number_of_blocks_for_data];
struct inode array_of_inodes[num_of_inodes];
string file_descriptors[num_of_fd];
string path="root";

int make_fs(char* disk_name)
{   
    char buf[block_size];
    int len;
    
    if(access(disk_name, F_OK) != -1)
    {
        return 0;
    }

    disk_ptr = fopen(disk_name,"w");

    fseek(disk_ptr, 0, SEEK_SET);
    
    for(ll i = 0; i < block_size; i++)
    {
        buf[i] = 0;
    }

    for(ll i =0; i<num_of_blocks; i++)
    {
        fwrite(buf, sizeof(char), block_size, disk_ptr);
    }
    
    for(ll i=0; i<num_of_inodes; i++)
    {
        for(ll j = 0; j<num_of_direct_pointer; j++)
        {
            array_of_inodes[i].block_ptr[j] = -1;
        }
    }

    for(ll i= 0; i<num_of_inodes; i++)
    {
        inode_bmap[i] = false;
    }

    for(ll i= 0; i<number_of_blocks_for_data; i++)
    {
        data_bmap[i] = false;
    }

    fseek(disk_ptr, 0, SEEK_SET);
    len = sizeof(struct superblock);
    char sblock_buf[len];
    memset(sblock_buf, 0, len);
    memcpy(sblock_buf, &sblock, sizeof(sblock));
    fwrite(sblock_buf, sizeof(char), sizeof(sblock), disk_ptr);

    fseek(disk_ptr, (sblock.num_of_blocks_for_sb) * block_size, SEEK_SET);
    len = sizeof(data_bmap);
    char dbmap_buff[len];
    memset(dbmap_buff, 0, len);
    memcpy(dbmap_buff, data_bmap, len);
    fwrite(dbmap_buff, sizeof(char), len, disk_ptr);

    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size, SEEK_SET);
    len = sizeof(inode_bmap);
    char ibmap_buff[len];
    memset(ibmap_buff, 0, len);
    memcpy(ibmap_buff, inode_bmap, len);
    fwrite(ibmap_buff, sizeof(char), len, disk_ptr);

    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap + sblock.num_of_blocks_for_inode_bmap) * block_size, SEEK_SET);
    len = sizeof(array_of_inodes);
    char inode_buff[len];
    memset(inode_buff, 0, len);
    memcpy(inode_buff, array_of_inodes, len);
    fwrite(inode_buff, sizeof(char), len, disk_ptr);

    struct inode_directory i_directory;

    i_directory.parent = "root";
    i_directory.directory_name = "root";
    i_directory.path = "root";

    sblock.directory.push_back(i_directory);

    fclose(disk_ptr);

    return 1;
}

int mount_fs(char* disk_name)
{
    int len;    
    disk_ptr = NULL;

    disk_ptr = fopen(disk_name, "r");

    if (disk_ptr == NULL)
    {
        cout<<"Unablt to mount"<<endl;
        return 0;
    }

    fseek(disk_ptr,0, SEEK_SET);

    len = sizeof(sblock);
    char sblock_buff[len];
    memset(sblock_buff, 0, len);
    fread(sblock_buff, sizeof(char), len, disk_ptr);
    memcpy(&sblock, sblock_buff, len);

//    cout<<"block size"<<block_size<<endl;
//    cout<<"num of blocks for sb"<<sblock.num_of_blocks_for_sb<<endl;
//    cout<<"disk ptr"<< (sblock.num_of_blocks_for_sb) * block_size<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb) * block_size, SEEK_SET);
    len = sizeof(data_bmap);
    char dbmap_buff[len];
    memset(dbmap_buff, 0, len);
    fread(dbmap_buff, sizeof(char), len, disk_ptr);
    memcpy(data_bmap, dbmap_buff, len);

//    cout<<"num of blocks for data bmap"<<sblock.number_of_blocks_for_data_bmap<<endl;
//    cout<<"disk ptr"<< (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size, SEEK_SET);
    len = sizeof(inode_bmap);
    char ibmap_buff[len];
    memset(ibmap_buff, 0, len);
    fread(ibmap_buff, sizeof(char), len, disk_ptr);
    memcpy(inode_bmap, ibmap_buff, len);

//    cout<<"num of blocks for indoe bmap"<<sblock.num_of_blocks_for_inode_bmap<<endl;
//    cout<<"disk ptr"<< (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap + sblock.num_of_blocks_for_inode_bmap) * block_size<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap + sblock.num_of_blocks_for_inode_bmap) * block_size, SEEK_SET);
    len = sizeof(array_of_inodes);
    char inode_buff[len];
    memset(inode_buff, 0, len);
    fread(inode_buff, sizeof(char), len, disk_ptr);
    memcpy(&array_of_inodes, inode_buff, len);

    for(ll i =0; i<num_of_fd; i++)
    {
        file_descriptors[i] = "";
    }

    isactive = 1;
    return 1;
}

int umount_fs(char *disk_name)
{
    int len;
    if (!isactive)
    {
        cout<<"disk is already unmounted"<<endl;
        return 0;
    }

    if(access(disk_name,F_OK) == -1)
    {
        cout<<"Unable to access disk"<<endl;
        return 0;
    }

    disk_ptr = fopen(disk_name, "w");
    if (disk_ptr == NULL)
    {
        cout<<"unable to unmount"<<endl;
        return 0;
    }

    fseek(disk_ptr,0, SEEK_SET);

//   cout<<"block size"<<block_size<<endl;
//    cout<<"num of blocks for sb"<<sblock.num_of_blocks_for_sb<<endl;
//    cout<<"disk ptr"<< (sblock.num_of_blocks_for_sb) * block_size<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb) * block_size, SEEK_SET);
    len = sizeof(data_bmap);
    char dbmap_buff[len];
    memset(dbmap_buff, 0, len);
    memcpy(dbmap_buff, data_bmap, len);
    fwrite(dbmap_buff, sizeof(char), len, disk_ptr);    

//    cout<<"num of blocks for data bmap"<<sblock.number_of_blocks_for_data_bmap<<endl;
//    cout<<"disk ptr"<< (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size, SEEK_SET);
    len = sizeof(inode_bmap);
    char ibmap_buff[len];
    memset(ibmap_buff, 0, len);
    memcpy(ibmap_buff, inode_bmap, len);
    fwrite(ibmap_buff, sizeof(char), len, disk_ptr);

//    cout<<"num of blocks for indoe bmap"<<sblock.num_of_blocks_for_inode_bmap<<endl;
//    cout<<"disk ptr"<< (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap + sblock.num_of_blocks_for_inode_bmap) * block_size<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap + sblock.num_of_blocks_for_inode_bmap) * block_size, SEEK_SET);
    len = sizeof(array_of_inodes);
    char inode_buff[len];
    memset(inode_buff, 0, len);
    memcpy(inode_buff, array_of_inodes, len);
    fwrite(inode_buff, sizeof(char), len, disk_ptr);

    memset(data_bmap, false, sizeof(data_bmap));
    memset(inode_bmap, false, sizeof(inode_bmap));

    for(int k=0; k< num_of_fd; k++){
        file_descriptors[k]="";
    }

    sblock.file_inode_position_map.clear();
    memset(sblock.filedescriptor_bmap,  false, sizeof(sblock.filedescriptor_bmap));

    fseek(disk_ptr,0,SEEK_SET);
    len = sizeof(struct superblock);
    char sblock_buff[len];
    memset(sblock_buff, 0, len);
    memcpy(sblock_buff, &sblock, sizeof(sblock));
    fwrite(sblock_buff, sizeof(char), sizeof(sblock), disk_ptr);

    fclose(disk_ptr);
    isactive = 0;
    return 1;
}

int fs_create(char *name)
{
    int is_inode_free = 0;
    int is_root_present = 0;
    int is_directory_present = 0;
    int is_data_block_free = 0;
    ll free_inode_num = -1;
    ll free_data_block_num = -1;
    string filename = string(name);

    vector<inode_directory>::iterator rit;
    for(rit = sblock.directory.begin(); rit != sblock.directory.end(); rit++)
    {
        if((*rit).directory_name == "root")
        {
            is_root_present = 1;
            break;
        }
    }

    if(is_root_present == 0)
    {
//        cout<<"here"<<endl;
        struct inode_directory i_directory;

        i_directory.parent = "root";
        i_directory.directory_name = "root";
        i_directory.path = "root";

        sblock.directory.push_back(i_directory);
    }

    for(ll i=0; i< num_of_inodes; i++)
    {
        if(inode_bmap[i] == false)
        {
            is_inode_free = 1;
            free_inode_num = i;
            break;
        }
    }

    if(is_inode_free == 0)
    {
        cout<<"no free inodes"<<endl;
        return -1;
    }

    for(ll i=0; i< number_of_blocks_for_data; i++)
    {
        if(data_bmap[i] == false)
        {
            is_data_block_free = 1;
            free_data_block_num = i;
            break;
        }
    }

    if(is_data_block_free == 0)
    {
        cout<<"No free data blocks"<<endl;
        return -1;
    }

    struct inode ide;

    ide.filename = filename;
    ide.filepath = path;
    ide.filesize = 0;
    ide.is_directory = false;
    ide.block_ptr[0] = free_data_block_num;
    for(ll i =1; i<num_of_direct_pointer; i++)
    {
        ide.block_ptr[i] = -1;
    }

    array_of_inodes[free_inode_num] = ide;
    inode_bmap[free_inode_num] = true;
    data_bmap[free_data_block_num] = true;

    vector<inode_directory>::iterator it,dit;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == path)
        {
            is_directory_present = 1;
            dit = it;
            break;
        }
    }

    if(is_directory_present == 1)
    {
        (*dit).sub_directory_or_file_name.push_back(filename);
    }

    return 1;
}

int fs_open(char *name)
{
    string filename = string(name);
    int is_file_present = 0;
    ll file_inode_num = -1;
    int is_filedescriptor_free = 0;
    ll file_descriptor_num = -1;
    int is_file_already_open = 0;
    string filepath = "";

    for(ll i = 0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename && array_of_inodes[i].filepath == path)
        {
            is_file_present = 1;
            file_inode_num = i;
            break;
        }
    }

    if(is_file_present == 0)
    {
        cout<<"file not found"<<endl;
        return -1;
    }

    for(ll i = 0; i<num_of_fd; i++)
    {
        if(sblock.filedescriptor_bmap[i] == false)
        {
            is_filedescriptor_free = 1;
            file_descriptor_num = i;
            break;
        }
    }

    if(is_filedescriptor_free == 0)
    {
        cout<<"no free file descriptor"<<endl;
        return -1;
    }

    string mode;
    cout<<"Please enter file open mode"<<endl;
    cin>>mode;

    if(mode == "w")
    {
        for(auto i = sblock.file_inode_position_map.begin(); i != sblock.file_inode_position_map.end(); i++)
        {
            if((*i).inodeid == file_inode_num && (*i).mode == mode)
            {
                is_file_already_open = 1;
                break;
            }
        }
    }

    if(is_file_already_open == 1)
    {
        cout<<"The file is already open in write mode"<<endl;
        return -1;
    }

    filepath = path+"/"+filename;
    file_descriptors[file_descriptor_num] = filepath;

    struct file_inode_position f_i_p_map;
    f_i_p_map.inodeid = file_inode_num;
    f_i_p_map.fd = file_descriptor_num;
    f_i_p_map.position = 0;
    f_i_p_map.mode = mode;
    sblock.file_inode_position_map.push_back(f_i_p_map);

    sblock.filedescriptor_bmap[file_descriptor_num] = true;
    // if(strcmp(mode.c_str(), "w")==0)
    // {
    //     file_write_operation();
    // }
    // else if(strcmp(mode.c_str(), "a")==0)
    // {

    // }
    // else
    // {
    //     cout << "Incorrect choice, please enter r w for write, a for append";
    // }
    

    return 1;
}

int fs_create_for_copy(char *name,string filepath)
{
    int is_inode_free = 0;
    int is_root_present = 0;
    int is_directory_present = 0;
    int is_data_block_free = 0;
    ll free_inode_num = -1;
    ll free_data_block_num = -1;
    string filename = string(name);

//    cout<<"fs create for copy "<<filename<<" "<<filepath<<endl;
    for(ll i=0; i< num_of_inodes; i++)
    {
        if(inode_bmap[i] == false)
        {
            is_inode_free = 1;
            free_inode_num = i;
            break;
        }
    }

    if(is_inode_free == 0)
    {
        return -1;
    }

    for(ll i=0; i< number_of_blocks_for_data; i++)
    {
        if(data_bmap[i] == false)
        {
            is_data_block_free = 1;
            free_data_block_num = i;
            break;
        }
    }

    if(is_data_block_free == 0)
    {
        return -1;
    }

    struct inode ide;

    ide.filename = filename;
    ide.filepath = filepath;
    ide.filesize = 0;
    ide.is_directory = false;
    ide.block_ptr[0] = free_data_block_num;
    for(ll i =1; i<num_of_direct_pointer; i++)
    {
        ide.block_ptr[i] = -1;
    }

    array_of_inodes[free_inode_num] = ide;
    inode_bmap[free_inode_num] = true;
    data_bmap[free_data_block_num] = true;

    vector<inode_directory>::iterator it,dit;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == filepath)
        {
            is_directory_present = 1;
            dit = it;
            break;
        }
    }

    if(is_directory_present == 1)
    {
//        cout<<"dit "<<(*dit).path<<endl;
        (*dit).sub_directory_or_file_name.push_back(filename);
    }

    return 1;
}

int fs_open_for_copy(char* name, string file_path)
{
    string filename = string(name);
    int is_file_present = 0;
    ll file_inode_num = -1;
    int is_filedescriptor_free = 0;
    ll file_descriptor_num = -1;
    int is_file_already_open = 0;
    string filepath = "";

    for(ll i = 0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename && array_of_inodes[i].filepath == file_path)
        {
            is_file_present = 1;
            file_inode_num = i;
            break;
        }
    }

    if(is_file_present == 0)
    {
        return -1;
    }

    for(ll i = 0; i<num_of_fd; i++)
    {
        if(sblock.filedescriptor_bmap[i] == false)
        {
            is_filedescriptor_free = 1;
            file_descriptor_num = i;
            break;
        }
    }

    if(is_filedescriptor_free == 0)
    {
        return -1;
    }

    string mode = "w";

    filepath = file_path+"/"+filename;
    file_descriptors[file_descriptor_num] = filepath;

    struct file_inode_position f_i_p_map;
    f_i_p_map.inodeid = file_inode_num;
    f_i_p_map.fd = file_descriptor_num;
    f_i_p_map.position = 0;
    f_i_p_map.mode = mode;
    sblock.file_inode_position_map.push_back(f_i_p_map);

    sblock.filedescriptor_bmap[file_descriptor_num] = true;
    
    return file_descriptor_num;
}

int fs_close(int filedes)
{
    ll is_file_descriptor_open = 0;

    vector<file_inode_position>::iterator it;
    for(auto i = sblock.file_inode_position_map.begin(); i != sblock.file_inode_position_map.end(); i++)
    {
        if((*i).fd == filedes)
        {
            is_file_descriptor_open = 1;
            it = i;
            break;
        }
    }

    if(is_file_descriptor_open == 0)
    {
        cout<<"File descriptor not found"<<endl;
        return -1;
    }

    file_descriptors[filedes] = "";
    sblock.file_inode_position_map.erase(it);
    sblock.filedescriptor_bmap[filedes] = false;

    return 1;
}


void reset_inode(int inode_num)
{
    for(ll i = 0; i< num_of_direct_pointer; i++)
    {
        if(array_of_inodes[inode_num].block_ptr[i] >= 0 && array_of_inodes[inode_num].block_ptr[i] < number_of_blocks_for_data)
        {
            data_bmap[array_of_inodes[inode_num].block_ptr[i]] = false;
        }
        array_of_inodes[inode_num].block_ptr[i] = -1;
    }

    inode_bmap[inode_num] = false;
    array_of_inodes[inode_num].filename = "";
    array_of_inodes[inode_num].filesize = 0;
    array_of_inodes[inode_num].filepath = "";
    array_of_inodes[inode_num].is_directory = false;
    array_of_inodes[inode_num].indirect_data_ptr_present = true;
}

int fs_delete(char *name)
{
    string filename = string(name);
    int is_file_present = 0;
    ll inode_num = -1;
    int is_file_open = 0;
    string filepath = "";
    bool is_indirect_pointer_present = false;

    for(ll i = 0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename && array_of_inodes[i].filepath == path)
        {
            is_file_present = 1;
            inode_num = i;
            break;
        }
    }

    if(is_file_present == 0)
    {
        cout<<"File not found"<<endl;
        return -1;
    }

    filepath = path + "/" + filename;

    for(ll i=0; i<num_of_fd; i++)
    {
        if(file_descriptors[i] == filepath)
        {
            is_file_open = 1;
            break;
        }
    }

    if(is_file_open == 1)
    {
        cout<<"the file is open"<<endl;
        return -1;
    }

    is_indirect_pointer_present = array_of_inodes[inode_num].indirect_data_ptr_present;

    if(is_indirect_pointer_present == false)
    {
        reset_inode(inode_num);
    }
    else
    {
        for(ll i=0; i < num_of_inode_pointer; i++)
        {
            if(array_of_inodes[inode_num].inodepointer[i] >= 0 && array_of_inodes[inode_num].inodepointer[i] < num_of_inodes)
            {
                reset_inode(array_of_inodes[inode_num].inodepointer[i]);
            }

            array_of_inodes[inode_num].inodepointer[i] = -1;
        }

        reset_inode(inode_num);
    }

    vector<inode_directory>::iterator it,dit;
    vector<string>::iterator tempit;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == path)
        {
            vector<string>::iterator sit;
            for(sit = (*it).sub_directory_or_file_name.begin(); sit != (*it).sub_directory_or_file_name.end(); sit++)
            {
                if((*sit) == filename)
                {
                    tempit = sit;
                    dit = it;
                    break;
                }
            }
        }
    }

    (*dit).sub_directory_or_file_name.erase(tempit);
    
    return 1;
}

int create_directory(string directory_name)
{
    int ispresent = 0;
    int is_root_present = 0;
    int is_directory_present = 0;
    int is_inode_free = 0;
    int free_inode_num = -1;
    string original_path = "";
    vector<string> stack_of_paths;
    string temp_parent = "";

    vector<inode_directory>::iterator rit;
    for(rit = sblock.directory.begin(); rit != sblock.directory.end(); rit++)
    {
        if((*rit).directory_name == "root")
        {
            is_root_present = 1;
            break;
        }
    }

    if(is_root_present == 0)
    {
 //       cout<<"create directory"<<endl;
        struct inode_directory i_directory;

        i_directory.parent = "root";
        i_directory.directory_name = "root";
        i_directory.path = "root";

        sblock.directory.push_back(i_directory);
    }

    vector<inode_directory>::iterator it;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).directory_name == directory_name && (*it).parent == path)
        {
            ispresent = 1;
            break;
        }
    }

    if(ispresent == 1)
    {
        cout<<"same direcory present"<<endl;
        return -1;
    }

    for(ll i=0; i< num_of_inodes; i++)
    {
        if(inode_bmap[i] == false)
        {
            is_inode_free = 1;
            free_inode_num = i;
            break;
        }
    }

    if(is_inode_free == 0)
    {
        cout<<"No inode free"<<endl;
        return -1;
    }

    struct inode ide;

    ide.filename = directory_name;
    ide.filepath = path;
    ide.filesize = 0;
    ide.is_directory = 1;

    for(ll i =0; i<num_of_direct_pointer; i++)
    {
        ide.block_ptr[i] = -1;
    }

    for(ll i = 0; i<num_of_inode_pointer; i++)
    {
        ide.inodepointer[i] = -1;
    }

    array_of_inodes[free_inode_num] = ide;
    inode_bmap[free_inode_num] = true;

   /* stack_of_paths.push_back(directory_name);
    temp = path;
    while(true)
    {
        if(temp == "/")
            break;

        stack_of_paths.push_back(temp);
        
        vector<inode_directory>::iterator it;
        for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
        {
            if((*it).directory_name == temp)
            {
                vector<inode_directory>::iterator it1;
                for(it1 = sblock.directory.begin(); it1 != sblock.directory.end(); it1++)
                {
                    if((*it1).directory_name == temp && (*it).path == (*it1).path)
                    {
                        temp = (*it).parent;
                        break;
                    }
                }
            }
        }
    }

    while(!stack_of_paths.empty())
    {
        original_path = "/" + stack_of_paths.back();
        stack_of_paths.pop_back();
    } */

    original_path = path + "/" + directory_name;
    struct inode_directory i_directory;

    i_directory.parent = path;
    i_directory.directory_name = directory_name;
    i_directory.path = original_path;

    sblock.directory.push_back(i_directory);

    vector<inode_directory>::iterator it1,dit;
    for(it1 = sblock.directory.begin(); it1 != sblock.directory.end(); it1++)
    {
        if((*it1).path == path)
        {
            is_directory_present = 1;
            dit = it1;
            break;
        }
    }

    if(is_directory_present == 1)
    {
        (*dit).sub_directory_or_file_name.push_back(directory_name);
    }

    return 1;
}

int change_directory(string new_path)
{
    int ispresent = 0;

    vector<inode_directory>::iterator it;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == new_path)
        {
            ispresent = 1;
            break;
        }
    }

    if(ispresent == 0)
    {
        cout<<"Unable to change directory"<<endl;
        return -1;
    }

    path = new_path;
    return 1;
}

int fs_open_forgui(char *name)
{
    string filename = string(name);
    int is_file_present = 0;
    ll file_inode_num = -1;
    int is_filedescriptor_free = 0;
    ll file_descriptor_num = -1;
    int is_file_already_open = 0;
    string filepath = "";

    for(ll i = 0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename && array_of_inodes[i].filepath == path)
        {
            is_file_present = 1;
            file_inode_num = i;
            break;
        }
    }

    if(is_file_present == 0)
    {
        return -1;
    }

    for(ll i = 0; i<num_of_fd; i++)
    {
        if(sblock.filedescriptor_bmap[i] == false)
        {
            is_filedescriptor_free = 1;
            file_descriptor_num = i;
            break;
        }
    }
//    cout<<"in 724"<<endl;
    if(is_filedescriptor_free == 0)
    {
        return -1;
    }

    string mode = "r";

    filepath = path+"/"+filename;
    file_descriptors[file_descriptor_num] = filepath;

    struct file_inode_position f_i_p_map;
    f_i_p_map.inodeid = file_inode_num;
    f_i_p_map.fd = file_descriptor_num;
    f_i_p_map.position = 0;
    f_i_p_map.mode = mode;
    sblock.file_inode_position_map.push_back(f_i_p_map);

    sblock.filedescriptor_bmap[file_descriptor_num] = true;
 //   cout<<"before return from fs open"<<endl;
    return file_descriptor_num;
}

string file_read_fromgui(string path1, string filename)
{
    ll temp_index = -1;
    ll fd = -1;
    int ispresent = 0;

    for(ll i = 0; i < num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename && array_of_inodes[i].filepath == path1)
        {
            ispresent = 1;
            temp_index = i;
            break;
        }
    }
//    cout<<"here 882"<<endl;
    if(ispresent == 0)
    {
        return "";
    }

    char name[Buffer_Size];
    strcpy(name, filename.c_str());
//    cout<<"before fs open"<<endl;
    int file_des = fs_open_forgui(name);
//    cout<<"before file inode position map"<<endl;
    vector<file_inode_position>::iterator it;
    for(it = sblock.file_inode_position_map.begin(); it != sblock.file_inode_position_map.end(); it++)
    {
        if((*it).inodeid == temp_index && (*it).mode == "r")
        {
            fd = (*it).fd;
            break;
        }
    }
//    cout<<"disk name"<<endl;
    string s = "Customdisk";
    char disk_name[Buffer_Size];
    strcpy(disk_name,s.c_str());
//    cout<<"before file read "<<disk_name<<endl;
    string content = file_read_for_gui(disk_name, fd);

    int t = fs_close(file_des);

    return content;
}



int rename(string old_name, string new_name)
{
    int is_directory_present = 0;
    string original_path = path + "/" + old_name;
    string new_path = path + "/" + new_name;
    int last_index = -1;
    int temp_index = -1;
    string temp_path = "";
    int is_present = 0;

    vector<inode_directory>::iterator it,tempdit;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == original_path && (*it).parent == path)
        {
            is_directory_present = 1;
            tempdit = it;
            break;
        }
    }

    if(is_directory_present == 1)
    {
    /*    temp_path = (*tempdit).path;
        for(ll i =0 ;i < temp_path.length(); i++)
        {
            if(temp_path[i] == '/')
                last_index = i;
        }

        last_index = last_index + 1;

        for(ll i = 0; i< new_name.length(); i++)
        {
            temp_path[last_index] = new_name[i];
            last_index++;
        }

        (*tempdit).directory_name = new_name;
        (*tempdit).path = temp_path; */
        cout<<"Unable to rename"<<endl;
        return -1;
    }
    
    for(ll i=0; i< num_of_inodes; i++)
    {
        if(array_of_inodes[i].filepath == path && array_of_inodes[i].filename == old_name)
        {
            array_of_inodes[i].filename = new_name;
        }
    }

    vector<inode_directory>::iterator it,dit;
    vector<string>::iterator tempit;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == path)
        {
            vector<string>::iterator sit;
            for(sit = (*it).sub_directory_or_file_name.begin(); sit != (*it).sub_directory_or_file_name.end(); sit++)
            {
                if((*sit) == old_name)
                {
                    tempit = sit;
                    dit = it;
                    break;
                }
            }
        }
    }

    (*dit).sub_directory_or_file_name.erase(tempit);
    (*dit).sub_directory_or_file_name.push_back(new_name);

    for(ll i =0; i< num_of_fd; i++)
    {
        if(file_descriptors[i] == original_path)
        {
            file_descriptors[i] = new_path;
        }
    }

    return 1;
}

int file_seek(int fdes, ll position, char *disk_name)
{
    int ispresent = 0;
    vector<file_inode_position>::iterator it,fit;
    for(it = sblock.file_inode_position_map.begin(); it != sblock.file_inode_position_map.end(); it++)
    {
        if((*it).fd == fdes)
        {
            ispresent = 1;
            fit = it;
            break;
        }
    }
    if(ispresent == 0)
    {
        cout<<"Unable to fseek"<<endl;
        return -1;
    }
    if((*fit).mode == "w" || (*fit).mode == "a")
    {
        return -1;
    }
    if(array_of_inodes[(*fit).inodeid].filesize < position)
    {
        cout<<"Fseek not poossible"<<endl;
        return -1;
    }
    ll fs=array_of_inodes[(*fit).inodeid].filesize;
    int inodenum = (*fit).inodeid;
    (*fit).position = position;
    FILE *disk_ptr;	
    disk_ptr = fopen(disk_name,"r");
    ll base_index =(sblock.num_of_blocks_for_sb + 	
                    sblock.number_of_blocks_for_data_bmap + 	
                    sblock.num_of_blocks_for_inode_bmap +	
                    num_of_inodes) * block_size;	
    fseek(disk_ptr, base_index, SEEK_SET);	
    struct inode in1 = array_of_inodes[inodenum];
    ll *bptr;	
    bool flag1 = false;	
    bptr=new ll[num_of_direct_pointer];	
    bptr = in1.block_ptr;
    ll no_of_blocks = ceil(fs*1.0/block_size);    	
    int count=0;	
    // ll relative_index = base_index + block_size * bptr[count];
    // fseek(disk_ptr, relative_index, SEEK_SET);
    char read_buf[block_size+2];  	
    cout << ftell(disk_ptr) << endl;
    int dpointers=0;
    while(no_of_blocks>0 && dpointers<5){    
        if(position <= 0){
            break;
        }        
        ll relative_index = base_index + block_size * bptr[dpointers++];
        fseek(disk_ptr, relative_index, SEEK_SET);
        memset(read_buf, '\0', block_size); 
        int seekval = (block_size <= position) ? block_size : position;   
        size_t result; 
        result = fread(read_buf, sizeof(char), seekval , disk_ptr);
        string sbuf(read_buf);    
        // read_buf[sbuf.length()] = '\0';
        cout << read_buf << endl;
        cout << result << endl;
        cout << ftell(disk_ptr) << " ";                    
        position-=result;
        // sbuf.length(); 
        no_of_blocks--;       
    }
    int cnt=0;
    while(in1.inodepointer[cnt]!= -1){	
        dpointers=0;
        struct inode tmp = array_of_inodes[in1.inodepointer[cnt++]];	
        bptr = tmp.block_ptr;
        count=0;
        ll relative_index = base_index + block_size * bptr[count];
        fseek(disk_ptr, relative_index, SEEK_SET);
        cout << ftell(disk_ptr) << endl;
        while(no_of_blocks>0 && dpointers<5){    
            if(position <= 0){
                break;
            }
            dpointers++;
            memset(read_buf, '\0', block_size); 
            int seekval = (block_size <= position) ? block_size : position;   
            size_t result; 
            result = fread(read_buf, sizeof(char), seekval , disk_ptr);
            string sbuf(read_buf);    
            // read_buf[sbuf.length()] = '\0';
            cout << read_buf << endl;
            cout << result << endl;
            cout << ftell(disk_ptr) << " ";                    
            position-=result;
            // sbuf.length(); 
            no_of_blocks--;       
        }
    }
    fclose(disk_ptr);
    cout << endl;
    return 1;
}

vector<string> list_files_directories(string directory_path)
{
    vector<string> list;

    vector<inode_directory>::iterator it;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == directory_path)
        {
            list = (*it).sub_directory_or_file_name;
        }
    }

    path = directory_path;
    return list;
}

int move(string source,string destination)
{
    string temp_path = "";
    int ispresent = 0;
    int is_destination_present = 0;
    int tempindex = -1;
    bool isdirectory = false;
    int last_index = -1;
    string temp_name = "";
    int isduplicate = 0;

    for(ll i =0; i< num_of_inodes; i++)
    {
        temp_path = array_of_inodes[i].filepath + "/"+ array_of_inodes[i].filename;
        if(temp_path == source)
        {
            ispresent = 1;
            tempindex = i;
            break;
        }
    }

    if(ispresent == 0)
    {
        cout<<"File not found"<<endl;
        return -1;
    }

    for(ll i = 0; i<num_of_fd; i++)
    {
        if(file_descriptors[i] == source)
        {
            cout<<"File is open"<<endl;
            return -1;
        }
    }
 //   cout<<"source present checked"<<endl;

    vector<inode_directory>::iterator it,temdit;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == destination)
        {
            is_destination_present = 1;
            temdit = it;
            break;
        }
    }

    if(is_destination_present == 0)
    {
        cout<<"Destination not found"<<endl;
        return -1;
    }

 //   cout<<"destination present checked"<<endl;

    if(source == "root")
    {
        cout<<"Unable to move file"<<endl;
        return -1;
    }

    for(ll i =0 ;i < source.length(); i++)
    {
        if(source[i] == '/')
            last_index = i;
    }

    last_index++;

    for(ll i =last_index; i < source.length(); i++)
    {
        temp_name = temp_name + source[i];
    }

//    cout<<"temp name "<<temp_name<<endl;

    for(auto it = (*temdit).sub_directory_or_file_name.begin(); it != (*temdit).sub_directory_or_file_name.end(); it++)
    {
        if((*it) == temp_name)
        {
            isduplicate = 1;
            break;
        }
    }

    if(isduplicate == 1)
    {
        cout<<"Unable to move file"<<endl;
        return -1;
    }

 //   cout<<"duplicate checked"<<endl;

    isdirectory = array_of_inodes[tempindex].is_directory;
       
    if(isdirectory == true)
    {
        cout<<"Unable to move file"<<endl;
        return -1;
    }
    else
    {
//        cout<<"destination "<<destination<<endl;
//        cout<<"tem dit "<<(*temdit).directory_name<<endl;

        (*temdit).sub_directory_or_file_name.push_back(temp_name);
        
        temp_path = array_of_inodes[tempindex].filepath;

        vector<inode_directory>::iterator it1;
        for(it1 = sblock.directory.begin(); it1 != sblock.directory.end(); it1++)
        {
            if((*it1).path == temp_path)
            {
                for(auto it2 = (*it1).sub_directory_or_file_name.begin(); it2 != (*it1).sub_directory_or_file_name.end(); it2++)
                {
                    if((*it2) == temp_name)
                    {
//                        cout<<"it2 "<<(*it2)<<endl;
                        (*it1).sub_directory_or_file_name.erase(it2);
                        break;
                    }
                }
            }
        }
        array_of_inodes[tempindex].filepath = destination;
    }

    return 1; 
}

void print_cwd()
{
    cout<<"PATH: "<<path<<endl;
}

void ptintfd()
{
    cout<<"File descriptor with name file_descriptors"<<endl;

    for(ll i=0; i<num_of_fd; i++)
    {
        cout<<"i "<<i<<" filename "<<file_descriptors[i]<<endl;
    }
}

void printbmap()
{
    cout<<"inode bit map"<<endl;
    for(ll i=0; i<num_of_inodes; i++)
    {
        cout<<"i "<<i<<" bit "<<inode_bmap[i]<<endl;
    }

    cout<<"data bit map"<<endl;
    for(ll i=0; i<number_of_blocks_for_data; i++)
    {
        cout<<"i "<<i<<" bit "<<data_bmap[i]<<endl;
    }

    cout<<"file descriptor bit map"<<endl;
    for(ll i=0; i<num_of_fd; i++)
    {
        cout<<"i "<<i<<" bit "<<sblock.filedescriptor_bmap[i]<<endl;
    }
}

void printinode()
{
    cout<<"inodes"<<endl;
    for(ll i =0 ; i<num_of_inodes; i++)
    {
        cout<<"i "<<i<<" file name "<<array_of_inodes[i].filename<<" file size "<<array_of_inodes[i].filesize<<" file path "<<array_of_inodes[i].filepath;
        for(ll j = 0; j < num_of_direct_pointer; j++)
        {
            cout<<" block pointer "<<j<<" pointing to "<<array_of_inodes[i].block_ptr[j];
        }
        cout<<endl;
    }
}

void pritfileinodemap()
{
    cout<<"file inode position map"<<endl;
    for(auto i = sblock.file_inode_position_map.begin(); i != sblock.file_inode_position_map.end(); i++)
    {
        cout<<"inode id"<< (*i).inodeid<<" file descriptor "<<(*i).fd<<" mode "<<(*i).mode<<" position "<<(*i).position<<endl;
    }
}

void printinodedirectory()
{
    cout<<"Inode directory"<<endl;
    vector<inode_directory>::iterator it;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        cout<<"parent directory "<<(*it).parent<<" directory name "<<(*it).directory_name<<" path "<<(*it).path;
        for(auto i = (*it).sub_directory_or_file_name.begin(); i != (*it).sub_directory_or_file_name.end(); i++)
        {
            cout<<" file or directory "<<(*i);
        }
        cout<<endl;
    }
}

void printfilesdirectories(string pathname)
{
    cout<<"List of files or directories under "<<pathname<<endl;
    vector<inode_directory>::iterator it;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == pathname)
        {
            for(auto i = (*it).sub_directory_or_file_name.begin(); i != (*it).sub_directory_or_file_name.end(); i++)
            {
                cout<<" file or directory "<<(*i);
            }
        }    
        cout<<endl;
    }
}

int copy_files(string filename, string filepath)
{
    int is_file_present = 0;
    int is_directory_present = 0;
    int temp_inode_index = -1;

    for(ll i =0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filepath == path && array_of_inodes[i].filename == filename)
        {
            is_file_present = 1;
            temp_inode_index = i;
            break;
        }
    }
//    cout<<"here 929"<<endl;
    if(is_file_present == 0)
    {
        cout<<"File not found"<<endl;
        return -1;
    }

    if(array_of_inodes[temp_inode_index].is_directory == true)
    {
        cout<<"Unable to copy"<<endl;
        return -1;
    }

    vector<inode_directory>::iterator it;
    for(it = sblock.directory.begin(); it != sblock.directory.end(); it++)
    {
        if((*it).path == filepath)
        {
            is_directory_present = 1;
            break;
        }
    }
//    cout<<"here 949"<<endl;
    if(is_directory_present == 0)
    {
        cout<<"Destination not found"<<endl;
        return -1;
    }

    if(filepath == array_of_inodes[temp_inode_index].filepath)
    {
        string content = file_read_fromgui(path, filename);
 //       cout<<"content "<<content<<endl;
        string new_filename = filename + "(copy)";
        char buffer[Buffer_Size];
        strcpy(buffer,new_filename.c_str());
 //       cout<<"copy 962"<<endl;
        int t = fs_create_for_copy(buffer,filepath);
 //       cout<<"here 964"<<endl;
        if(t == -1)
        {
            cout<<"Unable to copy"<<endl;
            return -1;
        }
        int file_des = fs_open_for_copy(buffer,filepath);
 //       cout<<"here 970"<<endl;
        string disk_name = "Customdisk";
        char disk_name_buffer[Buffer_Size];
        strcpy(disk_name_buffer, disk_name.c_str());
//        cout<<"here 974 "<<disk_name_buffer<<endl;
        int s = file_write(file_des,content,disk_name_buffer);
 //       cout<<"here 976"<<endl;
        if(s == -1)
        {
            cout<<"Unable to copy"<<endl;
            return -1;
        }
        fs_close(file_des);
 //       cout<<"here 982"<<endl;
    }
    else
    {
        int is_file_present = 0;
//        cout<<"filepath "<<filepath<<" filename "<<filename<<endl;
        string content = file_read_fromgui(path, filename);
 //       cout<<"here 989 "<<content<<endl;
        vector<inode_directory>::iterator it;
        for(it = sblock.directory.begin(); it!= sblock.directory.end(); it++)
        {
            if((*it).path == filepath)
            {
 //               cout<<"here 995"<<endl;
                vector<string>::iterator sit;
                for(sit = (*it).sub_directory_or_file_name.begin(); sit != (*it).sub_directory_or_file_name.end(); sit++)
                {
  //                  cout<<"here 998"<<endl;
                    if((*sit) == filename)
                    {
                        is_file_present = 1;
                        break;
                    }
                }
            }
            if(is_file_present == 1)
            {
                break;
            }
        }

        if(is_file_present == 1)
        {
            filename = filename + "(copy)";
        }

        char buffer[Buffer_Size];
        strcpy(buffer,filename.c_str());
//        cout<<"here 1019 "<<buffer<<endl;
        int t = fs_create_for_copy(buffer,filepath);
//        printfilesdirectories("root");
 //       cout<<"here 1021"<<endl;
        if(t == -1)
        {
            cout<<"Unable to copy"<<endl;
            return -1;
        }
        int file_des = fs_open_for_copy(buffer,filepath);
//        cout<<"here 1027"<<endl;
        string disk_name = "Customdisk";
//        cout<<"disk name "<<disk_name<<endl;
        char disk_name_buffer[Buffer_Size];
        strcpy(disk_name_buffer, disk_name.c_str());
//        cout<<"here 1031 "<<file_des<<" "<<content<<" "<<disk_name_buffer<<endl;
    /*    printbmap();
        printfilesdirectories("root");
        printinode();
        pritfileinodemap();
        printinodedirectory();
        ptintfd(); */
        int s = file_write(file_des,content,disk_name_buffer);
//        cout<<"here 1033"<<endl;
        if(s == -1)
        {
            cout<<"Unable to copy"<<endl;
            return -1;
        }
        fs_close(file_des);
//        cout<<"here 1039"<<endl;
    }

    return 1;
    
}
#endif