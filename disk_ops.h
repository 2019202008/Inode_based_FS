#include "filesys.h"

static int isactive;
struct superblock sblock;
FILE *disk_ptr;
bool inode_bmap[num_of_inodes];
bool data_bmap[number_of_blocks_for_data];
struct inode array_of_inodes[num_of_inodes];
string file_descriptors[num_of_fd];

int make_fs(char* disk_name)
{   
    char buf[block_size];
    int len;
    
    if(access(disk_name, F_OK) != -1)
    {
        // cout<<"here"<<access(disk_name, F_OK)<<endl;
        return 0;
    }

    disk_ptr = fopen(disk_name,"w");

    for(ll i =0; i<num_of_blocks; i++)
    {
         fwrite(buf, 0, block_size, disk_ptr);
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

    fclose(disk_ptr);

    return 1;
}

int mount_fs(char* disk_name)
{
    int len;    
    disk_ptr = NULL;
    cout<<"Hey1 "<<disk_name<<endl;
    disk_ptr = fopen(disk_name, "r");
    cout<<"Hey2"<<endl;
    if (disk_ptr == NULL)
    {
        return 0;
    }

    len = sizeof(sblock);
    char sblock_buff[len];
    memset(sblock_buff, 0, len);
    fread(sblock_buff, sizeof(char), len, disk_ptr);
    memcpy(&sblock, sblock_buff, len);
    cout<<"Hey3"<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb) * block_size, SEEK_SET);
    len = sizeof(data_bmap);
    char dbmap_buff[len];
    memset(dbmap_buff, 0, len);
    fread(dbmap_buff, sizeof(char), len, disk_ptr);
    memcpy(data_bmap, dbmap_buff, len);
cout<<"Hey4"<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size, SEEK_SET);
    len = sizeof(inode_bmap);
    char ibmap_buff[len];
    memset(ibmap_buff, 0, len);
    fread(ibmap_buff, sizeof(char), len, disk_ptr);
    memcpy(inode_bmap, ibmap_buff, len);
cout<<"Hey5"<<endl;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap + sblock.num_of_blocks_for_inode_bmap) * block_size, SEEK_SET);
    len = sizeof(array_of_inodes);
    char inode_buff[len];
    memset(inode_buff, 0, len);
    fread(inode_buff, sizeof(char), len, disk_ptr);
    memcpy(&array_of_inodes, inode_buff, len);
cout<<"Hey6"<<endl;
    for(ll i =0; i<num_of_fd; i++)
    {
        file_descriptors[i] = "";
    }
    cout<<"Hey7"<<endl;
    isactive = 1;
    cout<<"Hey8"<<endl;
    write(0, "hello geeks\n", strlen("hello geeks\n"));
    return 1;
}

int umount_fs(char *disk_name)
{
    int len;
    if (!isactive)
    {
        return 0;
    }

    if(access(disk_name,F_OK) == -1)
    {
        return 0;
    }

    disk_ptr = fopen(disk_name, "w");
    if (disk_ptr == NULL)
    {
        return 0;
    }

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

    memset(data_bmap, false, sizeof(data_bmap));
    memset(inode_bmap, false, sizeof(inode_bmap));
    for(int k=0; k< num_of_fd; k++){
        file_descriptors[k]="";
    }
    // memset(file_descriptors, "", sizeof(file_descriptors));
    sblock.file_inode_position_map.clear();
    memset(sblock.filedescriptor_bmap,  false, sizeof(sblock.filedescriptor_bmap));

    isactive = 0;
}

int fs_create(char *name)
{
    int is_inode_free = 0;
    int is_data_block_free = 0;
    ll free_inode_num = -1;
    ll free_data_block_num = -1;
    string filename = string(name);

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
    ide.filesize = 0;
    ide.block_ptr[0] = free_data_block_num;

    array_of_inodes[free_inode_num] = ide;
    inode_bmap[free_inode_num] = true;
    data_bmap[free_data_block_num] = true;

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

    for(ll i = 0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename)
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

    string mode;
    cout<<"Please enter file open mode"<<endl;
    cin>>mode;

    if(mode == "w" || mode == "a")
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
        return -1;
    }

    file_descriptors[file_descriptor_num] = filename;

    struct file_inode_position f_i_p_map;
    f_i_p_map.inodeid = file_inode_num;
    f_i_p_map.fd = file_descriptor_num;
    f_i_p_map.position = 0;
    f_i_p_map.mode = mode;
    sblock.file_inode_position_map.push_back(f_i_p_map);

    sblock.filedescriptor_bmap[file_descriptor_num] = true;

    return 1;
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
        return -1;
    }

    file_descriptors[filedes] = "";
    sblock.file_inode_position_map.erase(it);
    sblock.filedescriptor_bmap[filedes] = false;

    return 1;
}


int fs_delete(char *name)
{
    string filename = string(name);
    int is_file_present = 0;
    ll inode_num = -1;
    int is_file_open = 0;

    for(ll i = 0; i<num_of_inodes; i++)
    {
        if(array_of_inodes[i].filename == filename)
        {
            is_file_present = 1;
            inode_num = i;
            break;
        }
    }

    if(is_file_present == 0)
    {
        return -1;
    }

    for(ll i=0; i<num_of_fd; i++)
    {
        if(file_descriptors[i] == filename)
        {
            is_file_open = 1;
            break;
        }
    }

    if(is_file_open == 1)
    {
        return -1;
    }

    for(ll i = 0; i< num_of_direct_pointer; i++)
    {
        data_bmap[array_of_inodes[inode_num].block_ptr[i]] = false;
        array_of_inodes[inode_num].block_ptr[i] = -1;
    }

    inode_bmap[inode_num] = false;
    array_of_inodes[inode_num].filename = "";
    array_of_inodes[inode_num].filesize = 0;    
    return 1;
}