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
	char *filename;
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
			iofile.write(reinterpret_cast<const char *> (&usernum),sizeof(usernum));
		}
		iofile.close();
		iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
		iofile.read(reinterpret_cast<char *> (&usernum),sizeof(usernum));
		iofile.close();
	}
	~Userlist()
	{
		delete [] filename;
	}
	int Register(char *name,char *password,char *email,char *phone)
	{
		iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
		++usernum;
		iofile.write(reinterpret_cast<const char *> (&usernum),sizeof(usernum));
		iofile.seekp(sizeof(int)+size_man*(usernum-1));
		iofile.write(reinterpret_cast<const char *> (name),size_name);
		iofile.write(reinterpret_cast<const char *> (password),size_password);
		iofile.write(reinterpret_cast<const char *> (email),size_email);
		iofile.write(reinterpret_cast<const char *> (phone),size_phone);
		char privilege=0;
		if (usernum==1)	privilege=2;else privilege=1;
		iofile.write(reinterpret_cast<const char *> (&privilege),sizeof(char));
		iofile.close();
		if (iofile.rdstate()!=0)	return -1;
		return usernum+2017;
	}
	int Login(const int id,const char *password)
	{
		if (id<2018 || id>2017+usernum)	return 0;
		iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
		iofile.seekg(sizeof(int)+size_man*(id-2018)+size_name);
		char *s=new char [size_password+1];
		iofile.read(reinterpret_cast<char *> (s),size_password);
		s[size_password]=0;
		int ans=0;
		if (strcmp(password,s)==0)	ans=1;
		iofile.close();
		delete [] s;
		return ans;
	}	
	int Query_profile(const int id,char *name ,char *email,char *phone,char &privilege)
	{
		if (id<2018 || id>2017+usernum)	return 0;
		iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
		iofile.seekg(sizeof(int)+size_man*(id-2018));
		iofile.read(reinterpret_cast<char *> (name),size_name);
		name[size_name]=0;
		iofile.seekg(size_password,iofile.cur);
		iofile.read(reinterpret_cast<char *> (email),size_email);
		email[size_email]=0;
		iofile.read(reinterpret_cast<char *> (phone),size_phone);
		phone[size_phone]=0;
		iofile.read(reinterpret_cast<char *> (&privilege),sizeof(char));
		iofile.close();
		return 1;
	}
    int Modify_profile(const int id,const char *name ,const char *password,const char *email,const char *phone)
    {
        if (id<2018 || id>2017+usernum)	return 0;
        iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
        iofile.seekg(sizeof(int)+size_man*(id-2018));
        iofile.write(reinterpret_cast<const char *> (name),size_name);
        iofile.write(reinterpret_cast<const char *> (password),size_password);
        iofile.write(reinterpret_cast<const char *> (email),size_email);
        iofile.write(reinterpret_cast<const char *> (phone),size_phone);
        iofile.close();
        return 1;
    }
    int Modify_privilege(const int id1,const int id2,const char privilege)
    {
        if (id1<2018 || id1>2017+usernum)	return 0;
        if (id2<2018 || id2>2017+usernum)	return 0;
        if (id1==id2)	return 0;
        //if (privilege!= 2)	return 0;
        iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
        char privilege1;
        iofile.seekg(sizeof(int)+size_man*(id1-2018)+size_name+size_password+size_email+size_phone);
        iofile.read(reinterpret_cast<char *> (&privilege1),sizeof(char));
        char privilege2;
        iofile.seekg(sizeof(int)+size_man*(id2-2018)+size_name+size_password+size_email+size_phone);
        iofile.read(reinterpret_cast<char *> (&privilege2),sizeof(char));
        if (privilege1!=2 || privilege<privilege2)
        {
            iofile.close();
            return 0;
        }
        iofile.seekp(sizeof(int)+size_man*(id2-2018)+size_name+size_password+size_email+size_phone);
        iofile.write(reinterpret_cast<const char *> (&privilege),sizeof(char));
        iofile.close();
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
        iofile.write(reinterpret_cast<const char *> (&usernum),sizeof(usernum));
        iofile.close();
    }
	bool isvalid(int id)
	{
		if (id<2018 || id>2017+usernum)	return 0;
		return 1;
	}
};
