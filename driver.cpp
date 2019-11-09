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
    cout<<"6: close File"<<endl;
    cout<<"7: delete File"<<endl;
    cout<<"8: print file decriptor"<<endl;
    cout<<"9: print bit map"<<endl;
    cout<<"10: print inode"<<endl;
    cout<<"11: print file inode position map"<<endl;
        
    string s="Customdisk"; 
//    string f = "test.txt";   

    char dname[Buffer_Size];
//    char fname[Buffer_Size];

    strcpy(dname, s.c_str());
//    strcpy(fname, f.c_str());

    while(true)
    {
        int choice;
        cout<<endl;
        cout<<"enter choice"<<endl;
        cin>>choice;
        if(choice==1)   {
        cout << make_fs(dname);
        }
        else if(choice==2)  {            
            int g=mount_fs(dname);
            write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
            cout<<endl;
            cout<<"mount"<<endl;
        }
        else if(choice == 3)
        {
            int g=umount_fs(dname);
            write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
            cout<<endl;
            cout<<"unmount"<<endl;
        }
        else if(choice == 4)
        {
            cout<<"enter filename"<<endl;
            string f;
            cin>>f;
            char fname[Buffer_Size];
            strcpy(fname, f.c_str());
            int g = fs_create(fname);
            write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
            cout<<endl;
            cout<<"Create file "<<endl;
        }
        else if(choice == 5)
        {
            cout<<"enter filename"<<endl;
            string f;
            cin>>f;
            char fname[Buffer_Size];
            strcpy(fname, f.c_str());
            int g = fs_open(fname);
            write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
            cout<<endl;
            cout<<"Open file "<<endl;
        }
        else if(choice == 6)
        {
            int fd;
            cin>>fd;
            int g = fs_close(fd);
            write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
            cout<<endl;
            cout<<"Close file "<<endl;
        }
        else if(choice == 7)
        {
            cout<<"enter filename"<<endl;
            string f;
            cin>>f;
            char fname[Buffer_Size];
            strcpy(fname, f.c_str());
            int g = fs_delete(fname);
            write(0, to_string(g).c_str(), strlen(to_string(g).c_str()));
            cout<<endl;
            cout<<"Delete file "<<endl;
        }
        else if(choice == 8)
        {
            ptintfd();
        }
        else if(choice == 9)
        {
            printbmap();
        }
        else if(choice == 10)
        {
            printinode();
        }
        else if(choice == 11)
        {
            pritfileinodemap();
        }
        else
        {
            break;
        }
        
    }
    
    cout<<"end"<<endl;
    return 0;
}