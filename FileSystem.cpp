#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <fstream>

using namespace std;

class BitMap
{
	public:
		int size;
		vector<char> Bmap;		
		void setSize(int s, string map) { size = s;Bmap.clear();for(int i=0;i<size;i++) Bmap.push_back(map[i]);}
    	int alloc() { for(int i=0;i<size;i++) if(Bmap[i] == '0') { Bmap[i] = '1';return i;} return -1; }
    	void free(int index) { if(Bmap[index] == '1') Bmap[index] = '0'; }	
    	void allocated(int index) { if (Bmap[index] == '0') Bmap[index] = '1'; }
    	string print() { string str = "";for(int i=0;i<size;i++) str += Bmap[i];return str;}
};

class Block 
{
	public:
	    string type, data;
	    int used,max;
	    map<string, int> Directories;
	    Block(string t) { type=t;used=0;max=32;data = "";}
	    string getType() { return type;}
	    string getData() { return data;}
	    void setType(string t) { if(type == "free") type = t;}
	    void addData(string d) { if(type == "f") data=d;}
	    int getEntries() { return used;}
	    int getFree() { return (max - used);}
	    void addDirectory(string name, int num) { Directories.insert(pair <string, int> (name, num));used += 1;assert (used <= max);}
		void delDirectory(string name)
	    {
	    	if(type == "d")
	    	{
		    	string token;std::vector<string> v;
		    	std::istringstream iss(name);
		    	while(std::getline(iss, token, '/')) v.push_back(token);
		    	for(map<string, int>::iterator it = Directories.begin(); it != Directories.end(); ++it)
		       	{
		       		if(it->first == v.back()) { Directories.erase(it);used -= 1;return;}	
		       	}
	    	}
	    }
	    bool search(string name)
	    {
	    	if (type == "d") { for(map<string,int>::iterator it = Directories.begin(); it != Directories.end(); ++it) if(name == it->first) return true;}
	        return false;
	    }
	    int getInodeofDirectory(string name)
	    {
	        for(map<string,int>::iterator it = Directories.begin(); it != Directories.end(); ++it) if(name == it->first) return Directories.find(it->first)->second;
	        return -1;
	    }
	    void free()
	    {
	    	if(type != "free")
	    	{
		        if(type == "d") used = 0;
		        data = "";type = "free";
		    }
	    }
		string print()
	    {
	        if(type == ("free")) return " ";
	        else if(type == ("d"))
	        {
	            string s= "";
	       		for(map<string,int>::iterator it = Directories.begin(); it != Directories.end(); ++it)
	            {
	            	std::string se; std::stringstream out;
					out << Directories.find(it->first)->second;
					se = out.str();
	                string str = it->first + "," + se;
	                if(s == ("")) s = str;
	                else s += " " + str;
	            }
	            return s;
	        }
	        else return data;
	    }
};

class Inode
{
	public:
		int ref, nbytes, noofblock;
	    string type;
	    std::vector<int> add;
	    Inode() { type="free"; ref = 1; nbytes = -1; } 
	    void setSize(int s) 
	    { 
	    	nbytes = s;
	    	if((nbytes%4096) == 0) noofblock = (nbytes/4096);
	    	else noofblock = (nbytes/4096) + 1;
	    	if(noofblock > 12) {cout<<"File System Corrupt !!!!\n-- You can't Store large data --"<<endl;exit(0);}
	    }
	    int size() { return nbytes; }
	    void setType(string t) {type=t;}
	    void setAdd(int a) { add.push_back(a); }
	    void setRef(int r) {ref=r;}
	    void addRef() {ref += 1;}
	    void delRef() {ref -= 1;}
	    int getRef() {return ref;}
	    std::vector<int> getAdd() {return add;}
	    string getType() {return type;}
	    void free() {type = "free";add.clear();nbytes=-1;ref=1;}
	    int getNoofBlock() { return noofblock;}
	    int getSize()
	    {
	        if(add.back() == -1) return 0;
	        else return 1;
	    }
	    string printAdd()
	    {
	    	string s = "";
	    	for(int j=0; j<add.size();j++)
	        {
	            char a[10];itoa(add[j], a, 10);s += string(a) + ",";
	        }
	        return s;
	    }
};

class System
{
	public:
		int noofinode, noofdata;
	    BitMap inodeMap, dataMap;
	    vector<Inode> inodes;
	    vector<Block> data;
	    vector<string> files, dir;
	    map<string, int> nameInode;
	    System(int ninode, int ndata, BitMap imap, vector<Inode> in, BitMap dmap, vector<Block> da, vector<string> fi, vector<string> di, map<string, int> ni)
	    {
	    	noofdata=ndata; noofinode=ninode; inodeMap = imap; inodes = in; data = da; dataMap = dmap; files = fi; dir = di; nameInode = ni;
	    }
	    int inodeAlloc() {return inodeMap.alloc();}
	    int dataAlloc() {return dataMap.alloc();}
	    void inodeFree(int index) { inodeMap.free(index); inodes[index].free(); }
	    void dataFree(int index) { dataMap.free(index); data[index].free(); }
	    int doPrintInodeMap() { cout<<"\n"<<inodeMap.print()<<endl;return 0;}
	    int doPrintDataMap() { cout<<"\n"<<dataMap.print()<<endl;return 0;}
	    int doPrintFiles() { cout<<endl;for(std::vector<string>::iterator it=files.begin(); it!=files.end(); ++it) cout<<*it<<", ";cout<<endl;return 0;}
	    string getParent(string name)
	    {
	    	string token, parent = ""; std::vector<string> v;
	    	std::istringstream iss(name);
	    	while(std::getline(iss, token, '/')) v.push_back(token);
	    	if(v.size() == 2) return "/";
	        for(int i=1;i<v.size()-1;i++) parent += "/" + v[i];
	        return parent;
	    }
	    int createFile(string parent, string newFile, string type, int nbytes)
	    {
	    	int inodeParent = nameInode.find(parent)->second;
	        int parentblock = (inodes[inodeParent].getAdd()).front();
	        if(data[parentblock].getFree() <= 0) return -1;
	        if(data[parentblock].search(newFile) == true) {cout<<"Change the name of file (This name is already exist)"<<endl;return -1;}
	        int inodenum = inodeAlloc();
	        if(inodenum == -1) {cout<<"No space in inode mapping"<<endl;return -1;}
	        int ref, f = -1, flag = 0;
	        inodes[inodenum].setSize(nbytes);
	        if(type == "d")
	        {
	            ref = 2;f = dataAlloc();
	            if(f == -1) {inodeFree(inodenum);cout<<"No space in Data mapping"<<endl;return -1;}
	            else
	            {
	                data[f].setType("d");
	                data[f].addDirectory(".", inodenum);
	                data[f].addDirectory("..",inodeParent);
	            }
	        }
	        else ref = 1;
	        inodes[inodenum].setType(type);
	        if(type == "d") inodes[inodenum].setAdd(f);
	        inodes[inodenum].setRef(ref);
	        inodes[inodeParent].addRef();
	        data[parentblock].addDirectory(newFile, inodenum);
	        return inodenum;
	    }
	    int writeFile(string file, string Data)
	    {
	        int inodenum = nameInode.find(file)->second;
	        int nob = inodes[inodenum].getNoofBlock();
	        for(int i=0; i<nob; i++)
	        {
		        int f = dataAlloc();
		        if(f == -1) {cout<<"No space (No space in data mapping)"<<endl;return -1;}
		        else 
		        {
		            data[f].setType("f");
		            data[f].addData(Data);
		        }
		        inodes[inodenum].setAdd(f);
		    }
	        return 0;
	    }
	    int deleteFile(string file)
	    {
	        int inodenum = nameInode.find(file)->second;
	        string type = inodes[inodenum].getType();
	        if(inodes[inodenum].getRef() == 1)
	        {
	            std::vector<int> deleteBlock = inodes[inodenum].getAdd();
	            for(int i=0; i<deleteBlock.size(); i++) if(deleteBlock[i] != -1) dataFree(deleteBlock[i]);
	            inodeFree(inodenum);
	        }
	        else inodes[inodenum].delRef();
	        string parent = getParent(file);
	        int inodeparent = nameInode.find(parent)->second;
	        int parentblock = (inodes[inodeparent].getAdd()).back();
	        inodes[inodeparent].delRef();
	        data[parentblock].delDirectory(file);
	        if(type == "f")
	        {
	        	for(std::vector<string>::iterator it=files.begin(); it!=files.end(); ++it) { if(*it == file) { files.erase(it); break; }}
			}
			else 
			{
				int block = (inodes[inodenum].getAdd()).back();
				dataFree(block);
				for(std::vector<string>::iterator it=dir.begin(); it!=dir.end(); ++it) { if(*it == file) { dir.erase(it); break; }}	
			}
	        return 0;
	    }
	    int doCreate(string type)
	    {
	    	int nbytes = -1;
	    	string file, parent, name="";
	    	cout<<"Create "<<type<<endl;
	   		cout<<"Enter the name which you want to create : ";
	   		cin>>file;
	   		if(type == "f") {cout<<"Enter the size of file in bytes : ";cin>>nbytes;}
	        cout<<"Enter the directory in which you want to create (Full path of Directory from root) : ";cin>>parent;
	        if(parent == "/") name = parent + file;
	        else name = parent + "/" + file;
	        int inodenum = createFile(parent, file, type, nbytes);
	        if(inodenum >= 0)
	        {
	        	if(type == "d") dir.push_back(name);
	        	else files.push_back(name);
	        	nameInode.insert(pair <string, int> (name, inodenum));
	            if(type == "f") if(doAppend(name) == -1) cout<<"You cannot store data into the file !!"<<endl;
	            return 0;
	        }
	        return -1;
	    }
	    int doAppend(string file)
	    {
	       	string data;
	        cout<<"Enter the data which you want to store !!"<<endl;cin>>data;
	        int te = writeFile(file, data);
	        return te;
	    }
	    int doDelete()
	    {
	    	string file;cout<<"Delete"<<endl;
	        if(files.size() == 0) {cout<<"There is no file to delete"<<endl;return -1;}
	        cout<<"Enter the name of the file which you want to delete (Enter the Full path of the file from Root) !"<<endl;cin>>file;
	        return deleteFile(file);
	    }
	    int doPrint()
	    {
	    	string file;int flag = 0;cout<<"Print"<<endl;
	    	cout<<"Enter the file name(full path of file from root) : ";cin>>file;
	    	for(std::vector<string>::iterator it=files.begin(); it!=files.end(); ++it) { if(*it == file) { flag = 1;break;}}
			if(flag == 0) {cout<<"File doesnot exist !!";return -1;}
			int inodenum = nameInode.find(file)->second;
			std::vector<int> blocknum = inodes[inodenum].getAdd();
			for(int i=0; i<blocknum.size(); i++)
			{
				if(blocknum[i] < 0) {cout<<"File is empty";return -1;}
				cout<<endl;
				cout<<data[blocknum[i]].getData()<<endl;
			}
	    }
	    string print()
	    {
	    	string res="";
	    	char cc[10], dd[10];
	    	itoa(noofinode, cc, 10);
	        itoa(noofdata, dd, 10);
	    	res += string(cc) + " " + string(dd) + "\n";
	    	res += inodeMap.print() + "\n";
	        for(int i=0;i<noofinode;i++)
	        {
	            string t = inodes[i].getType();
	            if(t == ("free")) res += " \n";
	            else
	            {
	            	char b[10], c[10], d[10];string s="";
	            	std::vector<int> v = inodes[i].getAdd();
	            	for(int j=0; j<v.size();j++)
	            	{
	            		char a[10];itoa(v[j], a, 10);s += string(a) + ",";
	            	}
	            	itoa(inodes[i].getRef(), b ,10);
	            	itoa(inodes[i].size(), c ,10);
	            	itoa(inodes[i].getNoofBlock(), d, 10);
	            	res += t + " " + s + " " + string(b) + " " + string(c) + " " + string(d) + "\n";
	        	}
	        }
	        res += dataMap.print() + "\n";
	        for(int i=0;i<noofdata;i++) res += data[i].print() + "\n";
	    	return res;
	    }
	    string run()
	    {
	    	string result = ""; vector <string> :: iterator it;
	        int ch, flag = 0,te = -1;
	        while (te == -1)
	        {
	           	cout<<"Enter your choice !\n1. Create Directory\n2. Create File\n3. Delete File\n4. Print the data of file\n5. Print Inode BitMap\n6. Print Data BitMap\n7. Print all the Files in the File system\n8. Exit"<<endl;
	           	cin>>ch;cout<<endl;
	           	switch(ch)
	           	{
	           		case 1: te = doCreate("d"); break;
	           		case 2: te = doCreate("f"); break;
	           		case 3: te = doDelete(); break;
	           		case 4: te = doPrint(); break;
	           		case 5: te = doPrintInodeMap(); break;
	           		case 6: te = doPrintDataMap(); break;
	           		case 7: te = doPrintFiles(); break;
	           		case 8: te = 0; break;
	           		default: cout<<"Invalid Choice !!"<<endl; break;
	           	}
	           	cout<<endl;
	        }
	        result += print();
	        for (it = files.begin(); it != files.end(); ++it) {	flag=1;result += *it + ",";}
	        if(flag == 0) result += " ";
	        result += "\n";
	        flag = 0;
	        for (it = dir.begin(); it != dir.end(); ++it) {	flag = 1;result += *it + ",";}
	        if(flag == 0) result += " ";
	        result += "\n";
	    	return result;
		}	
};

int noofdata, noofinodes;
BitMap Imap, Dmap;
vector<Inode> inodes;
vector<Block> data;
vector<string> files, dir;
map<string, int> nameInode;

void setFilMapping()
{
	vector <string> :: iterator it; string token;
	for (it = files.begin(); it != files.end(); ++it)
    {
    	int inum=0, add=0;string s = *it;
    	std::vector<string> v;
    	std::istringstream iss(s);
    	while(std::getline(iss, token, '/')) v.push_back(token);
    	for(int i=1;i<v.size();i++)
    	{
    		inum = data[add].getInodeofDirectory(v[i]);
    		add = (inodes[inum].getAdd()).front();
    	}
    	v.clear();
    	nameInode.insert(pair <string, int> (s,inum));
    }
}
void setDirMapping()
{
	vector <string> :: iterator it; string token;
    for (it = dir.begin(); it != dir.end(); ++it)
    {
    	int inum=0, add=0;
    	string str = *it;std::vector<string> v;
    	std::istringstream ss(str);
    	while(std::getline(ss, token, '/')) v.push_back(token);
    	for(int i=1; i<v.size();i++)
    	{
    		inum = data[add].getInodeofDirectory(v[i]);
    		add = (inodes[inum].getAdd()).front();
    	}
    	v.clear();
    	nameInode.insert(pair <string, int> (str,inum));
    }
}
void read(char filename[])
{
	int i=0, p=0; char buffer[1024];
	string token, a, b, t, u, c, d;
	ifstream in(filename);
	in.getline(buffer, 1024);
	while(buffer[i] != ' ') {p=p*10 + (buffer[i] - '0'); ++i;}
	noofinodes = p; p=0; ++i;
	while(buffer[i] != ' ' && buffer[i] != '\0' && buffer[i] != '\n') {p=p*10 + (buffer[i] - '0'); ++i;}
	noofdata = p; p=0;
	for(i=0;i<noofinodes;i++) inodes.push_back(Inode());
	for (i=0;i<noofdata;i++) data.push_back(Block("free"));
	in.getline(buffer, 1024);
	Imap.setSize(noofinodes, buffer);
	in.getline(buffer, 1024);i=0;
	for(int j=0;j<noofinodes;j++)
	{
    	std::vector<string> v;
    	std::istringstream iss(buffer);
    	while(std::getline(iss, token, ' ')) v.push_back(token);
    	if(v[0] != " " && v[0] != "\n" && v[0] != "\0") 
    	{
    		inodes[j].setType(v[0]);i=0;p=0;
    		a = v[1];
    		if(a[0] == '-') inodes[j].setAdd(-1);
    		else
    		{
    			std::vector<string> add;
    			std::istringstream Ss(a);
    			while(std::getline(Ss, token, ',')) add.push_back(token);
    			for(int k=0; k<add.size(); k++)
    			{
    				string ad = add[k];
    				while(ad[i] != ' ' && ad[i] != '\0' && ad[i] != '\n') {p=p*10 + (ad[i] - '0'); ++i;}
    				inodes[j].setAdd(p);i=0;p=0;
    			}
    		}
    		b = v[2];
    		while(b[i] != ' ' && b[i] != '\0' && b[i] != '\n') {p=p*10 + (b[i] - '0'); ++i;}
    		inodes[j].setRef(p);i=0;p=0;
    		c = v[3];
    		if(c[0] == '-') inodes[j].setSize(-1);
    		else 
    		{
    			while(c[i] != ' ' && c[i] != '\0' && c[i] != '\n') {p=p*10 + (c[i] - '0'); ++i;}
    			inodes[j].setSize(p);i=0;p=0;	
    		}
    	}
    	v.clear();
    	in.getline(buffer, 1024);
	}
	Dmap.setSize(noofdata, buffer);
	in.getline(buffer, 1024);
	for(int j=0;j<noofdata;j++)
	{
    	std::vector<string> v;
    	std::istringstream iss(buffer);
    	while(std::getline(iss, token, ' ')) v.push_back(token);
    	for(i=0;i<v.size();i++)
    	{
    		if(v[0] != " " && v[0] != "\n" && v[0] != "\0")
    		{
    			std::vector<string> v2;
    			std::istringstream ss(v[i]);
    			while(std::getline(ss, t, ',')) v2.push_back(t);
    			p=0;int k=0;int p = 0;
    			if(i==0 && v2.size()==1) data[j].setType("f");
    			else if(i==0) data[j].setType("d"); 
    			if(data[j].getType() == "d") {u = v2[1];while(u[k] != '\0' && u[k] != '\n') {p=p*10 + (u[k] - '0'); ++k;}data[j].addDirectory(v2[0], p);}
    			else data[j].addData(v2[0]);
    		}
    	}
    	v.clear();
    	in.getline(buffer, 1024);
	}
	if(buffer[0] != ' ' && buffer[0] != '\n' && buffer[0] != '\0')
	{	
    	std::istringstream iss(buffer);
   		while(std::getline(iss, token, ',')) files.push_back(token);
   	}
   	in.getline(buffer, 1024);
   	if(buffer[0] != ' ' && buffer[0] != '\n' && buffer[0] != '\0')
   	{	
   		std::istringstream ss(buffer);
   		while(std::getline(ss, token, ',')) dir.push_back(token);
	}
	setDirMapping();
	setFilMapping();
	in.close();
}
void writefile(char filename[], string str) { std::ofstream outfile (filename); outfile<<str; outfile.close();}
void createFile(char filename[], int nbytes)
{
	cout<<"\nSystem created of 256 KB, 64 Blocks, 80 Inodes, 56 Disk Blocks for USER data, Each Block of 4 KB, You can store max 48 KB data file\n"<<endl;
	noofdata = 56; noofinodes = 80;
	std::ofstream outfile (filename);
	outfile<<noofinodes<<" "<<noofdata<<" "<<nbytes<<std::endl;
	string s = "", str="";
	for(int i=0;i<noofinodes;i++) 
	{	
		if(i == 0) s += "1";
		else s += "0";
	}
	outfile<<s<<endl;
	for(int i=0;i<noofinodes;i++) 
	{
		if(i==0) outfile<<"d"<<" "<<"0"<<" "<<"2"<<" "<<"-1"<<endl;
		else outfile<<" "<<endl;
	}
	for(int i=0;i<noofdata;i++) 
	{	
		if(i == 0) str += "1";
		else str += "0";
	}
	outfile<<str<<endl;
	for(int i=0;i<noofdata;i++) 
	{
		if(i==0) outfile<<".,0"<<" "<<"..,0"<<endl;
		else outfile<<" "<<endl;
	}
	outfile<<" "<<endl;outfile<<"/"<<endl;
	outfile.close();
}
void print()
{
    cout<<"Inode bitMap : "<<Imap.print()<<endl;
    cout<<"Inodes : ";
    for(int i=0;i<noofinodes;i++)
    {
        string t = inodes[i].getType();
        if(t == ("free")) cout<<"[]";
        else cout<<"["<<t<<" a:"<<inodes[i].printAdd()<<" r:"<<inodes[i].getRef()<<" Size:"<<inodes[i].size()<<" NOB:"<<inodes[i].getNoofBlock()<<"]";
    }
    cout<<endl;
    cout<<"Data Bitmap : "<<Dmap.print()<<endl;
    cout<<"Data : ";
    for(int i=0;i<noofdata;i++) cout<<"["<<data[i].print()<<"]";
    cout<<endl;
    cout<<"Files are : ";
    for (vector <string> :: iterator it2 = files.begin(); it2 != files.end(); ++it2) { if(*it2 == " " || *it2 == "\n") cout<<"@"<<" ";else cout<<*it2<<",";}
    cout<<endl;cout<<"Directories are : ";
    for (vector <string> :: iterator it2 = dir.begin(); it2 != dir.end(); ++it2) { if(*it2 == " " || *it2 == "\n") cout<<"@"<<" ";else cout<<*it2<<",";}
    cout<<endl;
    for (map <string, int> :: iterator itr = nameInode.begin(); itr != nameInode.end(); ++itr) cout<<itr->first<<" "<< itr->second<<endl;
}
int main()
{
	int ch;
	char filename[] = "MyFileSystem.txt";
	srand(time(NULL));
	if(!std::ifstream(filename)) createFile(filename,262144);
	while(1)
	{
		read(filename);print();
		System system(noofinodes, noofdata, Imap, inodes, Dmap, data, files, dir, nameInode);
    	string result = system.run(); 
    	writefile(filename, result);
    	cout<<"Do you want to do more operation ? (Enter 0(NO) and 1(Yes)) : ";
    	cin>>ch;
    	inodes.clear();data.clear();files.clear();dir.clear();nameInode.clear();
    	if(ch == 0) break;
    }
	return 0;
}