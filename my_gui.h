#ifndef my_gui
#define my_gui

#include "header_gui.h"
#include "disk_ops.h"

struct termios ttyorig;
vector<string> history,back_history;
string CURR_DIR;
//string path=CURR_DIR;
int x,y;
int EXIT=0;
void print_file(string path1)    {
    int flag=0,i=0;
    for(int i1=0;i1<path1.length();i1++)   {
        if(path1[i1]=='/')   {
            flag=1;
            i=i1;
            //break;
        }
    }
    string fname="",fpath="";
    if(flag==1) {
        for(int j=i+1;j<path1.length();j++)   {
            fname+=path1[j];
        }
    }
    else
    {
        fname=path1;
    }
    for(int j=0;j<i;j++)    {
        fpath+=path1[j];
    }
    int point;
    for(i=0;i<history.size();i++)   {
        if(history[i]==CURR_DIR)    {
            point=i;
            break;
        }
    }
    history.resize(point+1);
    string curr_path=path1;
    history.push_back(curr_path);
    CURR_DIR=curr_path;
    system("clear");
    //cout<<"i: "<<i<<endl;
    //cout<<"path1: "<<path1<<endl;
    //cout<<"fname: "<<fname<<" fpath: "<<fpath<<endl;
    cout<<"***** File "<<fname<<" contents: *****"<<endl;
    //cout<<"Not yet handled"<<endl;
    string s=file_read_fromgui(fpath,fname);
    cout<<s<<endl;
    x=1;
    y=1;
    gotoxy(x,y);
}

void print_dirs(string path)	{
    system("clear");
    cout<<"************** CURRENT DIRECTORY: "<<path<<" ***************"<<endl;;
    // cout<<endl;
    cout<<"-------------------------------------------------------"<<endl;
    cout<<"          List of files and directories here: "<<endl;
    cout<<endl;
    CURR_DIR=path;
    vector<string> list_of_dirs;
    //cout<<"Going to print"<<endl;
    list_of_dirs=list_files_directories(path);
    for(auto it:list_of_dirs)   {
        cout<<it<<endl;
    }
    cout<<endl;
    cout<<"EXIT"<<endl;
    //cout<<"Current directory: "<<CURR_DIR<<endl;
    // cout<<endl;
    // cout<<"--------------------------------------"<<endl;
    
    // cout<<"History: "<<endl;
    // for(auto it:history)    {
    //     cout<<it<<endl;
    // }

    x=1;
    y=1;
    gotoxy(x,y);
}

int check_dir_or_file(string path1) {
    bool file=false;
    string fname;
    for(ll i = 0; i<num_of_inodes; i++)
    {
        string this_inode_path=array_of_inodes[i].filepath+"/"+array_of_inodes[i].filename;
        if(this_inode_path==path1)
        {
            if(array_of_inodes[i].is_directory==false)  {
                fname=array_of_inodes[i].filename;
                file=true;
                break;
            }
        }
    }
    if(file)
        return 1;
    return 0;
}

void screen_loc_to_dir(int x,int y)  {
    int i;
    string this_dir="";
    vector<string> list_of_dirs;
    list_of_dirs=list_files_directories(CURR_DIR);
    //if(list_of_dirs.size()<=0)
    //    return;
    if(x==list_of_dirs.size()+6)    {
        EXIT=1;
        //system("clear");
        //cout<<"Thank You!!!"<<endl;
        return;
        //exit(0);
    }
    else{
    int flag=0;
    for(i=0;i<list_of_dirs.size();i++)   {
        cout<<list_of_dirs[i]<<endl;
        if(i==x-5)  {
            flag=1;
            this_dir=list_of_dirs[i];
            break;
        }
    }
    if(flag==0 || i==list_of_dirs.size()) {
        print_dirs(CURR_DIR);
    }
    else{
    if(this_dir=="")
        return;
    string curr_path=CURR_DIR+"/"+this_dir;
    int f=check_dir_or_file(curr_path);
    
    if(f==1)            //it is a file    
    {       
        //print_dirs(CURR_DIR);
        print_file(curr_path);
    }
    else                //it is a directory
    {
        int point;
        for(i=0;i<history.size();i++)   {
            if(history[i]==CURR_DIR)    {
                point=i;
                break;
            }
        }
        history.resize(point+1);
        // int count=0;
        // for(auto it=history.begin();it!=history.end();it++)    {
        //     if(count>point)
        //         history.erase(it);
        //     count++;
        // }

        history.push_back(curr_path);

        CURR_DIR=curr_path;
        
        print_dirs(CURR_DIR);
    }
    }
    }
}

void match_forward_dir()   {
    // int i,point;
    // for(i=0;i<history.size();i++)   {
    //     if(history[i]==CURR_DIR)    {
    //         //point=i;
    //         break;
    //     }
    // }
    // if((i+1)<history.size())
    //     CURR_DIR=history[i+1];
    // print_dirs(CURR_DIR);

    for(int i=0;i<history.size();i++)    {
        if(history[i]==CURR_DIR)    {
            if((i+1)<history.size())    {
                string curr_path=history[i+1];
                int f=check_dir_or_file(curr_path);
                if(f==1)
                    print_file(curr_path);
                else
                    print_dirs(history[i+1]);
                break;
            }
        }
    }
}

void match_back_dir()   {
    // if(history.size()<1)
    //     return;
    // CURR_DIR=history[history.size()-2];
    // print_dirs(CURR_DIR);

    for(int i=history.size()-1;i>=0;i--)    {
        if(history[i]==CURR_DIR)    {
            if((i-1)>=0)    {
                print_dirs(history[i-1]);
                break;
            }
        }
    }
}

void disable()	{
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&ttyorig);
}

int showgui()  {
    //vector<string> list_of_dirs;
    //list_of_dirs=list_files_directories("root");
    tcgetattr(STDIN_FILENO,&ttyorig);
    atexit(disable);
    struct termios raw=ttyorig;
    raw.c_lflag &=~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);
    
    
    history.push_back("root");
    //back_history.push_back("root");
    CURR_DIR="root";
    //cout<<"I am here"<<endl;
    print_dirs("root");
    //int x=1,y=1;
	//gotoxy(x,y);
	char ch,c;
	int flag=0;

    while(c=getchar())	{	
        if(EXIT==1) {
            EXIT=0;
            system("clear");
            cout<<"Thank You!!!"<<endl;
            tcsetattr(STDIN_FILENO,TCSAFLUSH,&ttyorig);
            return 1;
            //exit(0);
        }
		if(iscntrl(c))	{
			if((int)c==10)	{
                screen_loc_to_dir(x,y);
				//cout<<"\n";
				//break;
			}
		
			if((int)c==27)	{
				c=getchar();
				c=getchar();
                if(c=='A')  {   //up
                    if(x==1)    {
                        break;
                    }               
                    x-=1;
                    gotoxy(x,y);
                }
                if(c=='B')  {   //down
                    x+=1;
                    gotoxy(x,y);
                }
                if(c=='C')  {   //right
                    match_forward_dir();
                }
                if(c=='D')  {   //left
                    match_back_dir();
                }
                //cout<<"c: "<<c<<endl;
    			continue;
			}		
		}
		else	{
            continue;
			//cout<<c;
			//str1+=c;
		}
	}
    if(EXIT==1) {
        EXIT=0;
        tcsetattr(STDIN_FILENO,TCSAFLUSH,&ttyorig);
        //disable();
        return 1;
    }
        //exit(0);
    return 0;
}

#endif