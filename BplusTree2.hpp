#include<map>
#include<cstdio>
using namespace std;

template<class U,class T>
class BplusTree
{
public:
	typedef typename map<U,T>::iterator ITER;
	map<U,T> data;
	char *filename;
	fstream iofile;
	BplusTree()
	{
		data.clear();
		filename=NULL;
	}
	void init(const char *_filename,const char *_ha)
	{
		data.clear();
		filename=new char[strlen(_filename)+1];
		strcpy(filename,_filename);
		
		iofile.open(filename,std::fstream::in);
		if(!iofile)
		{
			iofile.open(filename,std::fstream::out|std::fstream::binary);
			int num=0;
			iofile.write(reinterpret_cast<const char *> (&num),sizeof(num));
		}
		iofile.close();
		iofile.open(filename,std::fstream::in|std::fstream::out|std::fstream::binary);
		int num;
		iofile.read(reinterpret_cast<char *> (&num),sizeof(num));
		U a;
		T b;
		for (int i=1;i<=num;i++)
		{
			iofile.read(reinterpret_cast<char *>(&a),sizeof(U));
			iofile.read(reinterpret_cast<char *>(&b),sizeof(T));
			data[a]=b;
		}
		iofile.close();
	}
	~BplusTree()
	{
		if (filename!=NULL)	delete [] filename;
	}
	void insert(const U &a,const T &b)
	{
		data[a] = b;
	}
	void erase(const U &a)
	{
		data.erase(a);
	}
	void modify(const U &a,const T &b)
	{
		data[a]=b;
	}
	bool check(const U &a){
		return data.count(a);
	}
	T & find(const U &a)
	{
		return data[a];
	}
    size_t size(){
        return data.size();
    }
    void clear(){
        data.clear();
    }
	class iterator
	{
		public:
		ITER it;
		iterator(const ITER &it2)
		{
			it=it2;
		}
		iterator() = default;
		iterator(const iterator &other)
		{
			it=other.it;
		}
		iterator operator++(int)
		{
			ITER it2=it;
			it++;
			return iterator(it2);
		}
		iterator& operator++()
		{
			it++;
			return (*this);
		}
        bool operator !=(const iterator &other){
            return it != other.it;
        }
		U Record()
		{
			return it->first;
		}
		T &Value()
		{
			return it->second;
		}
	};
    iterator begin(){
        return iterator(data.begin());
    }
    iterator end(){
        return iterator(data.end());
    }
	iterator lowerbound(const U &a)
	{
		return iterator(data.lower_bound(a));
	}
    bool isValid(const iterator &t)
    {
        return t.it!= data.end();
    }
	void exit()
	{
		iofile.open(filename,std::fstream::out|std::fstream::binary);
		int num=data.size();
		iofile.write(reinterpret_cast<const char *> (&num),sizeof(num));
		U a;
		T b;
		for (auto i:data)
		{
			a=i.first;
			b=i.second;
			iofile.write(reinterpret_cast<const char *> (&a),sizeof(a));
			iofile.write(reinterpret_cast<const char *> (&b),sizeof(b));
		}
	}
};
