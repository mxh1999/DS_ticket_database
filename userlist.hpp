#include<iostream>
#include <fstream>
#include<cstring>


/*
Userlist(filename) 以filename为文件名构造一个Userlist
int Register(char *name,char *password,char *email,char *phone) 注册用户，成功返回id，失败返回-1 要求name数组长度>40，password email phone长度>20，下同
int Login(int id,char *password) 登录，成功返回1，失败返回0
int Query_profile(int id,char *name ,char *email,char *phone,char &privilege) privilege为char 查询用户信息，查询成功返回1，否则返回0
int Modify_profile(int id,char *name ,char *password,char *email,char *phone) 修改用户信息，成功返回1，否则返回0
int Modify_privilege(int id1,int id2,char privilege)	修改用户权限，成功返回1，否则返回0
int size() 返回用户总数
*/
class Userlist
{
private:
	const static int size_name=40;
	const static int size_password=20;
	const static int size_email=20;
	const static int size_phone=20;
	const static int size_privilege=1;
	const static int size_man=size_name+size_password+size_email+size_phone+size_privilege;
	std::fstream iofile;
	int usernum;
	int blocknum;
	char *filename;
	char buffer[4096];
	int nowuser;
	int nowblock;
	struct Block
	{
		char name[40];
		char password[20];
		char email[20];
		char phone[20];
		char privilege;
	}	bl[40];
	int change_block(int idb)
	{
		if (idb==nowblock)	return 0;
		if (nowblock==1)
		{
			memcpy(buffer,reinterpret_cast<char *> (&usernum),4);
			memcpy(buffer+4,bl,size_man*40);
			iofile.seekp(0);
			iofile.write(buffer,4096);
		}	else
		{
			memcpy(buffer,bl,size_man*40);
			iofile.seekp(nowblock*4096-4096);
			iofile.write(buffer,4096);
		}
		if (idb>blocknum)
		{
			iofile.seekp(4096*(idb-1));
			iofile.write(buffer,4096);
			if (iofile.rdstate()!=0)	return -1;
			blocknum++;
		}	else
		{
			iofile.seekp(4096*(idb-1));
			iofile.read(buffer,4096);
			if (idb==1)	memcpy(bl,buffer+4,size_man*40);
			else memcpy(bl,buffer,size_man*40);
		}
		nowblock=idb;
		return 0; 
	}
public:
	Userlist() = default;
	Userlist(const char *_filename){
		filename=new char[strlen(_filename)+1];
		strcpy(filename,_filename);
		
		iofile.open(filename,std::fstream::in);
		usernum=0;
		if(!iofile)
		{
			iofile.open(filename,std::fstream::out|std::fstream::binary);
			memcpy(buffer,&usernum,4);
			nowuser=1;
			iofile.write(buffer,4096);
		}
		iofile.close();
		iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
		iofile.read(buffer,4096);
		memcpy(reinterpret_cast<char *> (&usernum),buffer,4);
		memcpy(reinterpret_cast<char *> (bl),buffer+4,size_man*40);
		nowuser=1;
		nowblock=1;
		blocknum=(usernum-1)/40+1;
	}
	~Userlist()
	{
		change_block(1);
		memcpy(buffer,reinterpret_cast<char *> (&usernum),4);
		memcpy(buffer+4,bl,size_man*40);
		iofile.seekp(0);
		iofile.write(buffer,4096);
		iofile.close();
		if (filename!=NULL)	delete [] filename;
	}
	int Register(char *name,char *password,char *email,char *phone)
	{
		++usernum;
		if (change_block((usernum-1)/40+1)==-1)	return -1;
		int pos=usernum-nowblock*40+39;
		memcpy(bl[pos].name,name,size_name);
		memcpy(bl[pos].password,password,size_password);
		memcpy(bl[pos].email,email,size_email);
		memcpy(bl[pos].phone,phone,size_phone);
		if (usernum==1)	bl[pos].privilege=2;else bl[pos].privilege=1;
		//if (iofile.rdstate()!=0)	return -1;
		return usernum+2017;
	}
	int Login(const int id,const char *password)
	{
		if (id<2018 || id>2017+usernum)	return 0;
		change_block((id-2018)/40+1);
		int pos=id-2017-nowblock*40+39;
		char *s=new char [size_password+1];
		memcpy(s,bl[pos].password,size_password);
		s[size_password]=0;
		int ans=0;
		if (strcmp(password,s)==0)	ans=1;
		delete [] s;
		return ans;
	}	
	int Query_profile(const int id,char *name ,char *email,char *phone,char &privilege)
	{
		if (id<2018 || id>2017+usernum)	return 0;
		change_block((id-2018)/40+1);
		int pos=id-2017-nowblock*40+39;
		memcpy(name,bl[pos].name,size_name);
		name[size_name]=0;
		memcpy(email,bl[pos].email,size_email);
		email[size_email]=0;
		memcpy(phone,bl[pos].phone,size_phone);
		phone[size_phone]=0;
		privilege=bl[pos].privilege;
		return 1;
	}
    int Modify_profile(const int id,const char *name ,const char *password,const char *email,const char *phone)
    {
        if (id<2018 || id>2017+usernum)	return 0;
		change_block((id-2018)/40+1);
		int pos=id-2017-nowblock*40+39;
		memcpy(bl[pos].name,name,size_name);
		memcpy(bl[pos].password,password,size_password);
		memcpy(bl[pos].email,email,size_email);
		memcpy(bl[pos].phone,phone,size_phone);
        return 1;
    }
    int Modify_privilege(const int id1,const int id2,const char privilege)
    {
        if (id1<2018 || id1>2017+usernum)	return 0;
        if (id2<2018 || id2>2017+usernum)	return 0;
        if (id1==id2)	return 0;
        //if (privilege!= 2)	return 0;
		change_block((id1-2018)/40+1);
		int pos=id1-2017-nowblock*40+39;
        char privilege1=bl[pos].privilege;
		change_block((id2-2018)/40+1);
		pos=id2-2017-nowblock*40+39;
        char privilege2=bl[pos].privilege;
        if (privilege1!=2 || privilege<privilege2)	return 0;
		bl[pos].privilege=privilege;
        return 1;
    }
	int size()
	{
		return usernum;
	}
    void clear()
    {
        iofile.open(filename,std::fstream::out|std::fstream::binary);
        usernum=0;
		memcpy(buffer,reinterpret_cast<char *> (&usernum),4);
        iofile.write(buffer,4096);
        iofile.close();
    }
	bool isvalid(int id)
	{
		if (id<2018 || id>2017+usernum)	return 0;
		return 1;
	}
};
