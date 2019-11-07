#include "filesys.h"

static int isactive;
struct superblock sblock;
File *disk_ptr;
bool inode_bmap[num_of_inodes];
bool data_bmap[number_of_blocks_for_data];
struct inode array_of_inodes[num_of_inodes];
string file_descriptors[num_of_fd];

int make_fs(char* disk_name)
{
    char buf[block_size];
    int len;

    if(access(disk_name,F_OK) != -1)
    {
        return 0;
    }

    disk_ptr = fopen(disk_name,"w");

    for(ll i =0; i<disk_blocks; i++)
    {
         fwrite(buf, 0, block_size, disk_ptr);
    }

    for(ll i=0; i<num_of_inodes; i++)
    {
        array_of_inodes[i].block_ptr[0] = -1;
        array_of_inodes[i].block_ptr[1] = -1;
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

int mount_fs(char *disk_name)
{
    int len;
    disk_ptr = NULL;
    disk_ptr = fopen(disk_name, "r");
    if (disk_ptr == NULL)
    {
        return 0;
    }

    len = sizeof(sblock);
    char sblock_buff[len];
    memset(sblock_buff, 0, len);
    fread(sblock_buff, sizeof(char), len, disk_ptr);
    memcpy(&sblock, sblock_buff, len);

    fseek(disk_ptr, (sblock.num_of_blocks_for_sb) * block_size, SEEK_SET);
    len = sizeof(data_bmap);
    char dbmap_buff[len];
    memset(dbmap_buff, 0, len);
    fread(dbmap_buff, sizeof(char), len, disk_ptr);
    memcpy(data_bmap, dbmap_buff, len);

    fseek(disk_ptr, (sblock.num_of_blocks_for_sb + sblock.number_of_blocks_for_data_bmap) * block_size, SEEK_SET);
    len = sizeof(inode_bmap);
    char ibmap_buff[len];
    memset(ibmap_buff, 0, len);
    fread(ibmap_buff, sizeof(char), len, disk_ptr);
    memcpy(inode_bmap, ibmap_buff, len);

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
    memset(file_descriptors, "",sizeof(file_descriptors));
}