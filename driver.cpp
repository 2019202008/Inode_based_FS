 #include "disk_ops.h"
//#include <bits/stdc++.h>
using namespace std;
int main()
{
    cout<<"1: Create File System"<<endl;
    cout<<"2: Mount"<<endl;
    cout<<"3: Unmount"<<endl;
    cout<<"4: Create File"<<endl;
    cout<<"5: Open File"<<endl;
    int choice;
    cin>>choice;    
    string s="Custom disk";    
    char dname[Buffer_Size];                        
    strcpy(dname, s.c_str());
    if(choice==1)   {
        cout << make_fs(dname);
    }
    else if(choice==2)  {
        cout << "Here"; 
        //int g=0;           
        int g=mount_fs(dname);
        write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
    }
    // switch(choice){
    //     case 1:  {          
    //         cout << make_fs(dname);       
    //         break; }   
    //     case 2:  {
    //         cout << "Here"; 
    //         //int g=0;           
    //         int g=mount_fs(dname);
    //         write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
    //        // cout<<"g "<<g;   
    //         break;   } 
    //     // case 3:       
    //     //     break;    
    //     // case 4:       
    //     //     break;    
    //     // case 5:       
    //     //     break;    
    //     default:{
    //         cout<<"abc"<<endl; }
    // }
    

    return 0;
}