#include "filewrite.h"
#include "disk_ops.h"
#include "fs_read.h"
#include "my_gui.h"
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
    cout<<"12: file write"<<endl;
    cout<<"13: file read"<<endl;
    cout<<"14: Create directoryt"<<endl;
    cout<<"16: Rename"<<endl;
    cout<<"17: Move"<<endl;
    cout<<"18: change directory"<<endl;
    cout<<"19: print inode directory"<<endl;
    cout<<"20: print files under directory"<<endl;	
    cout<<"21: file  seek"<<endl;	
    cout<<"22: file read for gui"<<endl;
    cout<<"23: show gui"<<endl;
    cout<<"24: Copy files"<<endl;
    cout<<"25: print current working directory"<<endl;
        
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
            cout<<"Open file "<<endl;
        }
        else if(choice == 6)
        {
            int fd;
            cout<<"Enter file descriptor"<<endl;
            cin>>fd;
            int g = fs_close(fd);
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
        else if(choice == 12)
        {
            file_write_operation(dname);            
        }
        else if(choice == 13)
        {
            int fd1;
            cout<<"Enter file descriptor:"<<endl;
            cin>>fd1;
            int x;
            x=file_read(dname,fd1);            
        }
        else if(choice == 14)
        {
            string directory_name;
            cout<<"Enter directory name"<<endl;
            cin>>directory_name;
            create_directory(directory_name);
        }
        else if(choice == 16)
        {
            string old_name;
            string new_name;
            cout<<"Enter file or directory name to be renamed"<<endl;
            cin>>old_name;
            cout<<"Enter new name"<<endl;
            cin>>new_name;
            rename(old_name, new_name);
        }
        else if(choice == 17)
        {
            string source;
            string destination;
            cout<<"Enter source"<<endl;
            cin>>source;
            cout<<"Enter destination"<<endl;
            cin>>destination;
            move(source,destination);
        }
        else if(choice == 18)
        {
            string new_path;
            cout<<"Enter the path"<<endl;
            cin>>new_path;
            change_directory(new_path);
        }
        else if(choice == 19)
        {
            printinodedirectory();
        }
                else if(choice == 20)	
	        {	
	            string path;	
	            cout<<"enter path"<<endl;	
	            cin>>path;	
	            printfilesdirectories(path);	
	        }	
	        else if(choice == 21)	
	        {	
	            int fdes;	
	            ll position;	
	            cout<<"Enter file descriptor"<<endl;	
	            cin>>fdes;	
	            cout<<"Enter position"<<endl;	
	            cin>>position;	
	            file_seek(fdes, position);	
	        }	
	        else if(choice == 22)	
	        {	
	            string path;	
	            string filename;	
	            cin>>path;	
	            cin>>filename;	
	            file_read_fromgui(path, filename);
            }
        else if(choice==23) {
            int returned=0;
            while(1)    {
                returned=showgui();
                if(returned==1) {
                    returned=0;
                    break;
                }
            }
        }
        else if(choice == 24)
        {
            string filename;
            string destination;
            cout<<"enter the filename"<<endl;
            cin>>filename;
            cout<<"enter the destination"<<endl;
            cin>>destination;
            copy_files(filename, destination);
        }
        else if(choice == 25)
        {
            print_cwd();
        }
        else
        {
            break;
        }
        
    }    
    cout<<"end"<<endl;
    return 0;
}