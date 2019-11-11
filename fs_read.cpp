#include"filesys.h"

int file_read(int fd)
{
    int flag=0,id;
    ll pos;
    for(auto it:sblock.file_inode_position_map)    {
        if(it.fd==fd)   {
            flag=1;
            pos = it.position;
            id=it.inodeid;
            break;
        }
    }
    if(flag==0) {
        return -1;
    }
    
    FILE *disk_ptr;
    fseek(disk_ptr, (sblock.num_of_blocks_for_sb+sblock.number_of_blocks_for_data_bmap+sblock.num_of_blocks_for_inode_bmap+num_of_inodes) * block_size, SEEK_SET);
    struct inode in1 = array_of_inodes[id];
    ll fs = in1.filesize;
    ll *bptr;
    bool flag1 = false;
    bptr=new ll[num_of_direct_pointer];
    bptr = in1.block_ptr;
    ll no_of_blocks = floor(fs/block_size);
    int rem=fs%block_size;
    if(!rem)
    {
        flag1 = true;
    }
    char total_buf[fs];
    int count=0;
    disk_ptr+=(bptr[0]*block_size);
    while(no_of_blocks>0){
        char read_buf[block_size];
        //FILE *ptr;
        //ptr = (disk_ptr+(bptr[count]*block_size));
        fread(read_buf, sizeof(char), block_size, disk_ptr);
        //memcpy(total_buf,read_buf,sizeof(read_buf));
        // string str="";
        // while(ptr !=  (ptr+block_size))
        // {
        //     str+=(*ptr);
        //     strcat(read_buf,str.c_str());
        //     ptr++;
        // }
        strcat(total_buf,read_buf);
        no_of_blocks--;
        disk_ptr+=block_size;
        //count++;
    }
    if(!flag1)
    {
        char read_buf[block_size];
        ll *ptr;
        ptr = &(bptr[count]);
        string str="";
        while(ptr !=  (ptr+ pos))
        {
            str+=(*ptr);
            strcat(read_buf,str.c_str());
            ptr++;
        }
        strcat(total_buf,read_buf);
    }
    cout<<total_buf<<endl;
    return pos;    
}
