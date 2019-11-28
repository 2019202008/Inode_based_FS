#ifndef fs_read
#define fs_read
#include"filesys.h"

int file_read(char *disk_name,int fd)	
{	
    int flag=0,id;	
    ll pos;int p=0;	
    for(auto it:sblock.file_inode_position_map)    {	
        if(it.fd==fd && strcmp(it.mode.c_str(), "r")==0)   {	
            flag=1;	
            pos = it.position;	
            id=it.inodeid;	
//            cout << p << " " << it.fd << " FID " << (it.position) << endl;	
            break;	
        }	
        p++;	
    }	
    if(flag==0) {	
        return -1;	
    }	
//    cout<<"pos: "<<pos<<" id: "<<id<<endl;	
    FILE *disk_ptr;	
    disk_ptr = fopen(disk_name,"r");	
    //   cout<<"disk_ptr before: "<<disk_ptr<<endl;	
    ll base_index =(sblock.num_of_blocks_for_sb + 	
                    sblock.number_of_blocks_for_data_bmap + 	
                    sblock.num_of_blocks_for_inode_bmap +	
                    num_of_inodes) * block_size;	
    //   cout<<"base index: "<<base_index<<endl;	
    fseek(disk_ptr, base_index, SEEK_SET);	
        
    //   cout<<"disk_ptr: "<<disk_ptr<<endl;	
    for(int aoin=0; aoin<10; aoin++){	
 //       cout << array_of_inodes[aoin].filesize << " ";	
    }	
 //   cout << endl;	
    struct inode in1 = array_of_inodes[id];	
    ll fs = in1.filesize;	
    //   cout<<"fs: "<<fs<<endl;	
    // fs = sblock.file_inode_position_map[p].position;	
    // cout<<"fs: "<<fs<<endl;	
        ll *bptr;	
        bool flag1 = false;	
        bptr=new ll[num_of_direct_pointer];	
        bptr = in1.block_ptr;	
        //cout<<"bptr: "<<bptr<<endl;	
        ll no_of_blocks = (floor(fs/block_size) <= num_of_direct_pointer)?	
                            floor(fs/block_size):num_of_direct_pointer;	
//        cout<<"noofblocks: "<<no_of_blocks<<endl;	
        int rem=0;	
        if(no_of_blocks < floor(fs/block_size)){	
            rem=0;	
        }	
        else{	
            rem=fs%block_size;	
        }	
//        cout<<"rem: "<<rem<<endl;	
        if(rem!=0)	
        {	
            flag1 = true;	
        }    	
        int count=0;	
        ll relative_index = base_index + block_size * bptr[count];	
        //cout << endl << base_index << " " << relative_index << endl;	
        fseek(disk_ptr, relative_index, SEEK_SET);	
        string str="";	
        while(no_of_blocks>0){	
    //           cout<<"In loop"<<endl;	
            char read_buf[block_size+2];	
    
            fread(read_buf, sizeof(char), block_size , disk_ptr);	
            read_buf[block_size]='\0';	
            string s(read_buf);	
            str+=s;	
//            cout<<"read_buf: "<<read_buf<<endl;	
            //strcat(total_buf,read_buf);	
            //cout<<"total: "<<total_buf<<endl;	
            no_of_blocks--;	
            count++;	
            ll relative_index1 = base_index + block_size * bptr[count];	
            //cout << endl << base_index << " " << relative_index << endl;	
            fseek(disk_ptr, relative_index1, SEEK_SET);	
            //count++;	
        }	
//        cout << endl << "base index: "<<base_index << " relative index: " << relative_index << endl;	
        if(flag1)	
        {	
            char read_buf1[rem+2];	
    
            int fr;	
            fr=fread(read_buf1, sizeof(char), rem, disk_ptr);	
            read_buf1[rem]='\0';	
    //           cout<<"fr: "<<fr<<endl;	
//            cout<<"read_buf: "<<read_buf1<<endl;	
            string s(read_buf1);	
            str+=s;        	
    
    
    
    
        }	
 //   cout << endl;	
    if(str.length() <= fs){	
        for(int node=0; node<num_of_inode_pointer; node++){	
            // cout << in1.inodepointer[node] << " " ;	
        }	
    }	
    fs = in1.filesize - str.length();int c=0;	
    while(fs > 0 && in1.inodepointer[c]!= -1){	
 //       cout << "c" << c << " "; 	
        struct inode tmp = array_of_inodes[in1.inodepointer[c++]];	
        ll *bptr;	
        bool flag1 = false;	
        bptr=new ll[num_of_direct_pointer];	
        bptr = tmp.block_ptr;        	
        ll no_of_blocks = (floor(fs/block_size) <= num_of_direct_pointer)?	
                            floor(fs/block_size):num_of_direct_pointer;	
//        cout<<"noofblocks: "<<no_of_blocks<<endl;	
        int rem=fs%block_size;	
//        cout<<"rem: "<<rem<<endl;	
        if(rem!=0)	
        {	
            flag1 = true;	
        }    	
        int count=0;	
        ll relative_index = base_index + block_size * bptr[count];	
        //cout << endl << base_index << " " << relative_index << endl;	
        fseek(disk_ptr, relative_index, SEEK_SET);        	
        while(no_of_blocks>0){	
//            cout<<"In loop"<<endl;	
            char read_buf[block_size+2];	
    
            fread(read_buf, sizeof(char), block_size , disk_ptr);	
            read_buf[block_size]='\0';	
            string s(read_buf);	
            str+=s;	
    //           cout<<"read_buf: "<<read_buf<<endl;            	
    
    
            no_of_blocks--;	
            count++;	
            ll relative_index1 = base_index + block_size * bptr[count];            	
            fseek(disk_ptr, relative_index1, SEEK_SET);            	
        }	
//        cout << endl << "base index: "<<base_index << " relative index: " << relative_index << endl;	
        if(flag1)	
        {	
            char read_buf1[rem+2];	
    
            int fr;	
            fr=fread(read_buf1, sizeof(char), rem, disk_ptr);	
            read_buf1[rem]='\0';	
//            cout<<"fr: "<<fr<<endl;	
//            cout<<"read_buf: "<<read_buf1<<endl;	
            string s(read_buf1);	
            str+=s;       	
    
    
    
    
        }       	
        fs = in1.filesize - str.length();	
    }	
    cout<<str<<endl;	
    fclose(disk_ptr);	
    return fs;    	
}	


string file_read_for_gui(char *disk_name, int fd)
{
      int flag=0,id;
    ll pos;int p=0;
    for(auto it:sblock.file_inode_position_map)    {
        if(it.fd==fd && strcmp(it.mode.c_str(), "r")==0)   {
            flag=1;
            pos = it.position;
            id=it.inodeid;
 //           cout << p << " " << it.fd << " FID " << (it.position) << endl;
            break;
        }
        p++;
    }
    if(flag==0) {
        return "";
    }
 //   cout<<"pos: "<<pos<<" id: "<<id<<endl;
    FILE *disk_ptr;
    disk_ptr = fopen(disk_name,"r");
 //   cout<<"disk_ptr before: "<<disk_ptr<<endl;
    ll base_index =(sblock.num_of_blocks_for_sb + 
                    sblock.number_of_blocks_for_data_bmap + 
                    sblock.num_of_blocks_for_inode_bmap +
                    num_of_inodes) * block_size;
 //   cout<<"base index: "<<base_index<<endl;
    fseek(disk_ptr, base_index, SEEK_SET);
    
//    cout<<"disk_ptr: "<<disk_ptr<<endl;
 //   for(int aoin=0; aoin<10; aoin++){
 //       cout << array_of_inodes[aoin].filesize << " ";
//    }
 //   cout << endl;
    struct inode in1 = array_of_inodes[id];
    ll fs = in1.filesize;
 //   cout<<"fs: "<<fs<<endl;
    // fs = sblock.file_inode_position_map[p].position;
    // cout<<"fs: "<<fs<<endl;
        ll *bptr;
        bool flag1 = false;
        bptr=new ll[num_of_direct_pointer];
        bptr = in1.block_ptr;
        //cout<<"bptr: "<<bptr<<endl;
        ll no_of_blocks = (floor(fs/block_size) <= num_of_direct_pointer)?
                           floor(fs/block_size):num_of_direct_pointer;
 //       cout<<"noofblocks: "<<no_of_blocks<<endl;
        int rem=0;
        if(no_of_blocks < floor(fs/block_size)){
            rem=0;
        }
        else{
            rem=fs%block_size;
        }
  //      cout<<"rem: "<<rem<<endl;
        if(rem!=0)
        {
            flag1 = true;
        }    
        int count=0;
        ll relative_index = base_index + block_size * bptr[count];
        //cout << endl << base_index << " " << relative_index << endl;
        fseek(disk_ptr, relative_index, SEEK_SET);
        string str="";
        while(no_of_blocks>0){
 //           cout<<"In loop"<<endl;
            char read_buf[block_size+2];
            fread(read_buf, sizeof(char), block_size , disk_ptr);
            read_buf[block_size]='\0';
            string s(read_buf);
            str+=s;
  //          cout<<"read_buf: "<<read_buf<<endl;
            //strcat(total_buf,read_buf);
            //cout<<"total: "<<total_buf<<endl;
            no_of_blocks--;
            count++;
            ll relative_index1 = base_index + block_size * bptr[count];
            //cout << endl << base_index << " " << relative_index << endl;
            fseek(disk_ptr, relative_index1, SEEK_SET);
            //count++;
        }
 //       cout << endl << "base index: "<<base_index << " relative index: " << relative_index << endl;
        if(flag1)
        {
            char read_buf1[rem+2];
            int fr;
            fr=fread(read_buf1, sizeof(char), rem, disk_ptr);
            read_buf1[rem]='\0';
//            cout<<"fr: "<<fr<<endl;
 //           cout<<"read_buf: "<<read_buf1<<endl;
            string s(read_buf1);
            str+=s;        
        }
 //   cout << endl;
 /*   if(str.length() <= fs){
        for(int node=0; node<num_of_inode_pointer; node++){
            cout << in1.inodepointer[node] << " " ;
        }
    } */
    fs = in1.filesize - str.length();int c=0;
    while(fs > 0 && in1.inodepointer[c]!= -1){
 //       cout << "c" << c << " "; 
        struct inode tmp = array_of_inodes[in1.inodepointer[c++]];
        ll *bptr;
        bool flag1 = false;
        bptr=new ll[num_of_direct_pointer];
        bptr = tmp.block_ptr;        
        ll no_of_blocks = (floor(fs/block_size) <= num_of_direct_pointer)?
                           floor(fs/block_size):num_of_direct_pointer;
//        cout<<"noofblocks: "<<no_of_blocks<<endl;
        int rem=fs%block_size;
 //       cout<<"rem: "<<rem<<endl;
        if(rem!=0)
        {
            flag1 = true;
        }    
        int count=0;
        ll relative_index = base_index + block_size * bptr[count];
        //cout << endl << base_index << " " << relative_index << endl;
        fseek(disk_ptr, relative_index, SEEK_SET);        
        while(no_of_blocks>0){
//            cout<<"In loop"<<endl;
            char read_buf[block_size+2];
            fread(read_buf, sizeof(char), block_size , disk_ptr);
            read_buf[block_size]='\0';
            string s(read_buf);
            str+=s;
  //          cout<<"read_buf: "<<read_buf<<endl;            
            no_of_blocks--;
            count++;
            ll relative_index1 = base_index + block_size * bptr[count];            
            fseek(disk_ptr, relative_index1, SEEK_SET);            
        }
 //       cout << endl << "base index: "<<base_index << " relative index: " << relative_index << endl;
        if(flag1)
        {
            char read_buf1[rem+2];
            int fr;
            fr=fread(read_buf1, sizeof(char), rem, disk_ptr);
            read_buf1[rem]='\0';
 //           cout<<"fr: "<<fr<<endl;
 //           cout<<"read_buf: "<<read_buf1<<endl;
            string s(read_buf1);
            str+=s;        
        }       
        fs = in1.filesize - str.length();
    }
//    cout<<str<<endl;
    fclose(disk_ptr);
    return str; 
}
#endif
