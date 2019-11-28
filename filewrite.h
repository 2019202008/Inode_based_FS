#ifndef filewrite
#define filewrite

#include "filesys.h"
int find_next_free_data_block(int index){
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
int find_next_free_inode_block(int index){
    bool found=false;
    int pos=-1;
    if(index < (num_of_inodes-1)){
        for(int i=index+1; i<num_of_inodes; i++){
            if(!inode_bmap[i]){
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
void fillInode( ll &blocks_to_write, ll &base_index ,ll &relative_index, struct inode temp,
                int &k, char *arr, FILE *disk_ptr, int &baseinode,
                int &p){
//    cout << "blocks_to_write " << blocks_to_write << " base_index " << base_index <<
 //           " relative_index " << relative_index << " k " << k << " baseinode " << baseinode << endl;
    int c=0, z=0, sz=0;
    int fb = temp.block_ptr[0];
    char buff[block_size+1];    
    for(int i=0; i<blocks_to_write; i++){
        temp.block_ptr[c++] = fb;
//        cout << relative_index << " z = " << z << " fb " << fb << endl;
        fseek(disk_ptr, relative_index, SEEK_SET);
        memset(buff, '\0', block_size);        
        sz = min(block_size, k);
        k-=block_size;
        strncpy(buff, arr+z, sz);
        buff[block_size]='\0';        
        z+=sz;
 //       cout << buff << endl;
            fwrite(buff, sizeof(char), sizeof(buff)-1, disk_ptr);             
        data_bmap[fb] = true;
        int pos = find_next_free_data_block(fb);
        if(pos == -1){
            cout << "No data blocks left ";
            break;
        }
        relative_index = base_index + block_size * pos;
        fb = pos;        
    }
    // cout << temp.block_ptr[0] << " " << temp.block_ptr[1] << endl;
    // cout << z << endl;
    temp.filesize = z;
    array_of_inodes[baseinode] = temp;
    // sblock.file_inode_position_map[p].position = z;
    inode_bmap[baseinode]=true;
    // cout << p << " FIP " <<sblock.file_inode_position_map[p].position << endl;
    // for(auto it:sblock.file_inode_position_map)    {
    //     cout << it.position <<" " <<it.inodeid<<" "<<it.baseinode<<endl;
    // }
    //fclose(disk_ptr);  
}
int file_write(int fd, string s, char* disk_name){
    FILE *disk_ptr;
    int len = s.length();int p=0;
    bool fdfound=false;
    struct file_inode_position fpos;
    int baseinode=0;
    for(auto x=sblock.file_inode_position_map.begin(); x!=sblock.file_inode_position_map.end(); x++){
        if(fd == (*x).fd && strcmp((*x).mode.c_str(), "w")==0){
            fpos = (*x);
            baseinode = (*x).inodeid;
            fdfound=true;
            break;
        }
        p++;
    }
    if(!fdfound){
        return -1;
    }    
    struct inode temp = array_of_inodes[baseinode];
    char buff[block_size+1];    
    ll blocks_to_write = ceil(len*1.0/block_size);    
    ll base_index =(sblock.num_of_blocks_for_sb + 
                    sblock.number_of_blocks_for_data_bmap + 
                    sblock.num_of_blocks_for_inode_bmap +
                    num_of_inodes) * block_size;
    disk_ptr = fopen(disk_name,"r+");
    fseek(disk_ptr, base_index, SEEK_SET);
    // ll relative_index = base_index + block_size * temp.block_ptr[0];    
    // fseek(disk_ptr, relative_index, SEEK_SET);
    int i=0;
    char arr[len+1];
    strcpy(arr, s.c_str());   
    // int fb = temp.block_ptr[0];
    // int c=0, z=0, sz=0;
    int k=len;
//    cout << blocks_to_write << endl;  
    if(blocks_to_write <= num_of_inode_pointer){  //direct write
        ll relative_index = base_index + block_size * temp.block_ptr[0];    
        fseek(disk_ptr, relative_index, SEEK_SET);
 //       cout << endl << base_index << " " << relative_index << endl;
        fillInode(blocks_to_write, base_index, relative_index,
                  temp, k, arr, disk_ptr, baseinode, p);   
        array_of_inodes[baseinode].filesize = s.length();
        sblock.file_inode_position_map[p].position = s.length();     
    }
    else if(blocks_to_write > ((num_of_inode_pointer+1)*num_of_direct_pointer)){
        cout << "File size is very large" << endl;
        return -1;
    }    
    else{   //single indirect        
//        cout << blocks_to_write<<endl;
 //       cout << len << endl;
        int number_of_inodes_required = int(ceil(blocks_to_write*1.0/num_of_direct_pointer));
        // cout << endl << base_index << " " << relative_index << endl;
//        cout << "number_of_inodes_required "<<number_of_inodes_required << endl;
        vector <int> inodes_for_indirect_write_arr;         
 //       cout << "baseinode " <<baseinode << endl;     
        // inodes_for_indirect_write_arr.push_back(baseinode);
        int val=number_of_inodes_required;int fd1=baseinode;
        while(val--){
            inodes_for_indirect_write_arr.push_back(baseinode);
            baseinode=find_next_free_inode_block(baseinode);
        }
//        cout << "inodes_for_indirect_write_arr.size()" << inodes_for_indirect_write_arr.size()<<endl;
        if(inodes_for_indirect_write_arr.size() < number_of_inodes_required){
            cout << "Not enough space to write file" << endl;
            return -1;
        }
        ll x=num_of_direct_pointer;  
        baseinode=fd1;
        struct inode temp1 = array_of_inodes[baseinode]; 
        array_of_inodes[baseinode].indirect_data_ptr_present=true;
        for(int i=1; i<inodes_for_indirect_write_arr.size(); i++){
            array_of_inodes[baseinode].inodepointer[i-1]= inodes_for_indirect_write_arr[i];
            array_of_inodes[inodes_for_indirect_write_arr[i]].indirect_data_ptr_present=false;  
            inode_bmap[inodes_for_indirect_write_arr[i]]=true;
        } 
        int b0=array_of_inodes[baseinode].block_ptr[0];
        for(int i=1; i<inodes_for_indirect_write_arr.size(); i++){
            for(int blk=0; blk<5; blk++){
                b0 = find_next_free_data_block(b0);
            }
            array_of_inodes[inodes_for_indirect_write_arr[i]].block_ptr[0]=b0;
        }
        int jmp=0;       
        for(int i=0; i<inodes_for_indirect_write_arr.size(); i++){
            struct inode temp = array_of_inodes[inodes_for_indirect_write_arr[i]];                          
 //           cout << inodes_for_indirect_write_arr[i] << " ";
            if(blocks_to_write >= num_of_direct_pointer){
                x=num_of_direct_pointer;
            }
            else{
                x=blocks_to_write;
            }
 //           cout << "BTW " << x << endl;               
            ll relative_index = base_index + block_size * temp.block_ptr[0];    
            fseek(disk_ptr, relative_index, SEEK_SET);            
 //           cout<< "JMP "<< jmp << " relative_index "<< relative_index 
 //               << " temp.block_ptr[0] " << temp.block_ptr[0]<< " k "<< k << endl;
            fillInode(x, base_index, relative_index,
                    temp, k, arr+jmp, disk_ptr, inodes_for_indirect_write_arr[i], p);  
            jmp=x*block_size;           
            blocks_to_write-=num_of_direct_pointer;              
        }
//        cout << endl;
        array_of_inodes[baseinode].filesize = s.length();
        sblock.file_inode_position_map[p].position = s.length();
    }
    fclose(disk_ptr);  
    ll fs2 = sblock.file_inode_position_map[p].position; 
//    cout<< "p" << p<< "fs2: "<<fs2<<endl; 
    return 1;
}
void file_write_operation(char* disk_name){
    cout << "Please enter the file descriptor ";
    int fd;
    cin>> fd;
    // getline(cin,st);
    cout << endl;
    cout << "Enter text to write ";
    string str;
    //cin >> str;
    cin.get();
    getline(cin,str);
    // gets(str);
    int mode  = file_write(fd, str, disk_name);
    if(mode == -1){
        cout << "Cant write in file " << endl; 
    }
    else{
        cout << "File written " << endl;
    }
} 
#endif