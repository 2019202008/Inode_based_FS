#ifndef filewrite
#define filewrite

#include "filesys.h"
int find_next_free_block(int index){
    bool found=false;
    int pos=-1;
    if(index < (number_of_blocks_for_data-1)){
        for(int i=index+1; i<number_of_blocks_for_data; i++){
            if(!data_bmap[i]){
                pos = i;
                found = true;
                break;
            }
        }
    }
    if(found){
        return pos;
    }
    return pos;
}
int file_write(int fd, string s, char* disk_name){
    FILE *disk_ptr;
    int len = s.length();int p=0;
    bool fdfound=false;
    struct file_inode_position fpos;
    for(auto x=sblock.file_inode_position_map.begin(); x!=sblock.file_inode_position_map.end(); x++){
        if(fd == (*x).fd && strcmp((*x).mode.c_str(), "w")==0){
            fpos = (*x);
            fdfound=true;
            break;
        }
        p++;
    }
    if(!fdfound){
        return -1;
    }
    struct inode temp = array_of_inodes[fd];
    char buff[block_size+1];    
    temp.block_ptr[0];
    ll blocks_to_write = ceil(len*1.0/block_size);
    // cout << "Number of blocks to write " << blocks_to_write <<endl;
    // cout << temp.block_ptr[0] << " " << temp.block_ptr[1] << 
    // " " << temp.filename <<endl;
    ll base_index =(sblock.num_of_blocks_for_sb + 
                    sblock.number_of_blocks_for_data_bmap + 
                    sblock.num_of_blocks_for_inode_bmap +
                    num_of_inodes) * block_size;
    disk_ptr = fopen(disk_name,"r+");
    fseek(disk_ptr, base_index, SEEK_SET);
    ll relative_index = base_index + block_size * temp.block_ptr[0];
    cout << endl << base_index << " " << relative_index << endl;
    fseek(disk_ptr, relative_index, SEEK_SET);
    int i=0;
    char arr[len];
    strcpy(arr, s.c_str());
    //cout << find_next_free_block(temp.block_ptr[0]) << endl;    
    int fb = temp.block_ptr[0];
    int c=0, z=0, sz=0;
    int k=len;
    cout << blocks_to_write << endl;
    //FILE * disk_ptr1 = fopen(disk_name,"r");
    for(int i=0; i<blocks_to_write; i++){
        temp.block_ptr[c++] = fb;
        cout << relative_index << " z = " << z << " fb " << fb << endl;
            fseek(disk_ptr, relative_index, SEEK_SET);
        memset(buff, '\0', block_size);        
        sz = min(block_size, k);
        k-=block_size;
        strncpy(buff, arr+z, sz);
        buff[block_size]='\0';
        cout << buff << endl;
        z+=sz;
            fwrite(buff, sizeof(char), sizeof(buff)-1, disk_ptr); 
            //fseek(disk_ptr1, relative_index, SEEK_SET);
            // char buff2[block_size+1];
            // fread(buff2,sizeof(char),sizeof(buff)-1, disk_ptr1);
            // //buff2[sizeof(buff)-1]='\0';
            // cout << "Buff2 "<<buff2 << endl;
        data_bmap[fb] = true;
        int pos = find_next_free_block(fb);
        if(pos == -1){
            cout << "No data blocks left ";break;
        }
        relative_index = base_index + block_size * pos;
        fb = pos;        
    }
    cout << temp.block_ptr[0] << " " << temp.block_ptr[1] << endl;
    cout << z << endl;
    temp.filesize = z;
    array_of_inodes[fd] = temp;
    sblock.file_inode_position_map[p].position = z;
    cout << p << " FIP " <<sblock.file_inode_position_map[p].position << endl;
    for(auto it:sblock.file_inode_position_map)    {
        cout << it.position <<" " <<it.inodeid<<" "<<it.fd<<endl;
    }
    fclose(disk_ptr);
    //fclose(disk_ptr1);
    return 1;
}
void file_write_operation(char* disk_name){
    cout << "Please enter the file descriptor ";
    int fd;
    cin>> fd;
    cout << endl;
    cout << "Enter text to write ";
    string str;
    cin >> str;
    int mode  = file_write(fd, str, disk_name);
    if(mode == -1){
        cout << "Cant write in file " << endl; 
    }
    else{
        cout << "File written " << endl;
    }
}
#endif