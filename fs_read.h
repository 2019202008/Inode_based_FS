#ifndef fs_read
#define fs_read
#include"filesys.h"

int file_read(char *disk_name,int fd)
{
    int flag=0,id;
    ll pos;int p=0;
    for(auto it:sblock.file_inode_position_map)    {
        if(it.fd==fd)   {
            flag=1;
            pos = it.position;
            id=it.inodeid;
            cout << p << " " << it.fd << " FID " << (it.position) << endl;
            break;
        }
        p++;
    }
    if(flag==0) {
        return -1;
    }
    cout<<"pos: "<<pos<<" id: "<<id<<endl;
    FILE *disk_ptr;
    disk_ptr = fopen(disk_name,"r");
    cout<<"disk_ptr before: "<<disk_ptr<<endl;
    ll base_index =(sblock.num_of_blocks_for_sb + 
                    sblock.number_of_blocks_for_data_bmap + 
                    sblock.num_of_blocks_for_inode_bmap +
                    num_of_inodes) * block_size;
    cout<<"base index: "<<base_index<<endl;
    fseek(disk_ptr, base_index, SEEK_SET);
    
    cout<<"disk_ptr: "<<disk_ptr<<endl;
    struct inode in1 = array_of_inodes[id];
    ll fs = in1.filesize;
    cout<<"fs: "<<fs<<endl;
    ll *bptr;
    bool flag1 = false;
    bptr=new ll[num_of_direct_pointer];
    bptr = in1.block_ptr;
    //cout<<"bptr: "<<bptr<<endl;
    ll no_of_blocks = floor(fs/block_size);
    cout<<"noofblocks: "<<no_of_blocks<<endl;
    int rem=fs%block_size;
    cout<<"rem: "<<rem<<endl;
    if(rem!=0)
    {
        flag1 = true;
    }
    //char total_buf[fs+1];
    //total_buf[fs]='\0';
    int count=0;
    ll relative_index = base_index + block_size * bptr[count];
    //cout << endl << base_index << " " << relative_index << endl;
    fseek(disk_ptr, relative_index, SEEK_SET);
    string str="";
    while(no_of_blocks>0){
        cout<<"In loop"<<endl;
        char read_buf[block_size+2];
        fread(read_buf, sizeof(char), block_size , disk_ptr);
        read_buf[block_size]='\0';
        string s(read_buf);
        str+=s;
        cout<<"read_buf: "<<read_buf<<endl;
        //strcat(total_buf,read_buf);
        //cout<<"total: "<<total_buf<<endl;
        no_of_blocks--;
        count++;
        ll relative_index1 = base_index + block_size * bptr[count];
        //cout << endl << base_index << " " << relative_index << endl;
        fseek(disk_ptr, relative_index1, SEEK_SET);
        //count++;
    }
    cout << endl << "base index: "<<base_index << " relative index: " << relative_index << endl;
    if(flag1)
    {
        cout<<"I am here"<<endl;
        char read_buf1[rem+2];
        int fr;
        fr=fread(read_buf1, sizeof(char), rem, disk_ptr);
        read_buf1[rem]='\0';
        cout<<"fr: "<<fr<<endl;
        cout<<"read_buf: "<<read_buf1<<endl;
        string s(read_buf1);
        str+=s;
        //strcat(total_buf,read_buf);
    }
    //total_buf[fs]='\0';
    cout<<"final: "<<str<<endl;
    fclose(disk_ptr);
    return fs;    
}
#endif
