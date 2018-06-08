#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP
#include <cstring>
#include <fstream>
using namespace std;

//const int idxSize = 5, dataBlkSize = 5;
template<class T, class U,int idxSize,int dataBlkSize>
//template <class T, class U, int idxSize, int dataBlkSize>
class BplusTree {
private:
	//static const int idxSize=4096,dataBlkSize;
	friend class iterator;
	struct idxNode {//索引结点
		int type;//type = 0  下一层为索引结点   type = 1  下一层为数据结点
		T key[idxSize - 1];//存储关键字
		int idx[idxSize];//记录位置
		int len;
		idxNode() :len(1) {}
	};
	struct dataNode {//数据结点
		int len;//数据记录个数
		T record[dataBlkSize];//索引关键字项
		U value[dataBlkSize];//数据项
		int nex;
		dataNode() :len(1), nex(0) {}
	};
	
	fstream fidx, fdata;//fidx文件第一个int为结点数，第二个int记录root结点位置   fdata文件第一个int记录结点数
	char *idxname, *dataname;

public:
	class iterator {
		friend class BplusTree;
	private:
		struct dataNode {//数据结点
			int len;//数据记录个数
			T record[dataBlkSize];//索引关键字项
			U value[dataBlkSize];//数据项
			int nex;
			dataNode() :len(1), nex(0) {}
		};
		fstream ifdata;
		char *data;
		int dcur, dpos;
	public:
		iterator() {
			data = NULL;
			dcur = 0;
			dpos = 0;
		}
		iterator(char *d, int c, int p) {
			data = new char[strlen(d) + 1];
			strcpy(data,d);
			dcur = c;
			dpos = p;
		}
		iterator(const iterator &other) {
			data = new char[strlen(other.data) + 1];
			strcpy(data,other.data);
			dcur = other.dcur;
			dpos = other.dpos;
		}
		~iterator() {
			if (data!=NULL)	delete [] data;
		}
		iterator& operator=(const iterator &other) {
			if (data!=NULL)	delete [] data;
			data = new char[strlen(other.data) + 1];
			strcpy(data,other.data);
			dcur = other.dcur;
			dpos = other.dpos;
			return (*this);
			//return iterator(data, dcur, dpos);
		}
		iterator operator++(int) {
			iterator tmp = iterator(data, dcur, dpos);
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			ifdata.open(data, fstream::in | fstream::binary);
			ifdata.seekg(sizeof(int) + sizeof(dataNode) * (dcur - 1), ios::beg);
			ifdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			ifdata.close();
			if (dpos == d.len - 1) {
				dcur = d.nex;
				dpos = 0;
			}
			else
				dpos++;
			delete d1;
			return tmp;
		}
		iterator& operator++() {
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			ifdata.open(data, fstream::in | fstream::binary);
			ifdata.seekg(sizeof(int) + sizeof(dataNode) * (dcur - 1), ios::beg);
			ifdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			ifdata.close();
			if (dpos == d.len - 1) {
				dcur = d.nex;
				dpos = 0;
			}
			else
				dpos++;
			delete d1;
			return (*this);
			//return iterator(data, dcur, dpos);
		}
		U Value() {
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			ifdata.open(data, fstream::in | fstream::binary);
			ifdata.seekg(sizeof(int) + sizeof(dataNode) * (dcur - 1), ios::beg);
			ifdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			ifdata.close();
			U ans=d.value[dpos];
			delete d1;
			return ans;
		}

		T Record() {
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			ifdata.open(data, fstream::in | fstream::binary);
			ifdata.seekg(sizeof(int) + sizeof(dataNode) * (dcur - 1), ios::beg);
			ifdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			ifdata.close();
			T ans=d.record[dpos];
			delete d1;
			return ans;
		}
	};

	BplusTree() {
		idxname = NULL;
		dataname = NULL;
	}
	
	~BplusTree()
	{
		if (idxname!=NULL)	delete [] idxname;
		if (dataname!=NULL)	delete [] dataname;
	}

	void init(const char *i, const char *d) {
		if (idxname!=NULL)	delete [] idxname;
		if (dataname!=NULL)	delete [] dataname;
		idxname = new char[strlen(i) + 1];
		strcpy(idxname, i);
		dataname = new char[strlen(d) + 1];
		strcpy(dataname, d);
		fidx.open(idxname, fstream::in | fstream::binary);
		fdata.open(dataname, fstream::in | fstream::binary);
		if (!fidx) {//空文件
			fidx.open(idxname, fstream::out | fstream::trunc | fstream::binary);
			int num = 0, root = 0;
			fidx.write(reinterpret_cast<char*>(&num), sizeof(int));
			fidx.write(reinterpret_cast<char*>(&root), sizeof(int));
		}
		if (!fdata) {//空文件
			fdata.open(dataname, fstream::out | fstream::trunc | fstream::binary);
			int num = 0;
			fdata.write(reinterpret_cast<char*>(&num), sizeof(int));
		}
		fidx.close();
		fdata.close();
	}

	void insert(const T &x, const U &v) {
		fidx.open(idxname, fstream::in | fstream::out | fstream::binary);
		fdata.open(dataname, fstream::in | fstream::out | fstream::binary);
		int num, root;
		fidx.seekg(0, ios::beg);
		fidx.read(reinterpret_cast<char*>(&num), sizeof(int));//读入结点个数num
		if (num == 0) {//文件为空
			//idxNode t=new idxNode;
			idxNode *t1 = new idxNode;//新建idxNode
			idxNode &t = *t1;
			t.type = 1;
			t.idx[0] = 1;
			num = 1;
			root = 1;
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			d.record[0] = x;
			d.value[0] = v;
			fidx.seekp(0, ios::beg);
			fidx.write(reinterpret_cast<char*>(&num), sizeof(int));//修改num值
			fidx.write(reinterpret_cast<char*>(&root), sizeof(int));//写入根结点位置
			fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));//写入idxNode内容
			fdata.seekp(0, ios::beg);
			fdata.write(reinterpret_cast<char*>(&num), sizeof(int));//写入记录数量
			fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));//写入dataNode内容
			delete d1;
			delete t1;
			fidx.close();
			fdata.close();
			return;
		}
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));//读取root
		int pos = insert(x, v, root);//调用私有insert函数，判断是否需要分裂根节点
		if (pos != 0) {//分裂根结点
			//idxNode t;
			idxNode *t1 = new idxNode;
			idxNode &t = *t1;
			t.type = 0;
			t.len = 2;
			t.idx[0] = root;
			t.idx[1] = pos;
			//idxNode tmp;
			idxNode *tmp1 = new idxNode;
			idxNode &tmp = *tmp1;
			fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
			fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
			while (tmp.type == 0) {//寻找第二块的最小值
				fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp.idx[0] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
			}
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (tmp.idx[0] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			t.key[0] = d.record[0];//修改关键字值
			fidx.seekg(0, ios::beg);
			fidx.read(reinterpret_cast<char*>(&num), sizeof(int));
			num++;
			root = num;
			fidx.seekp(0, ios::beg);
			fidx.write(reinterpret_cast<char*>(&num), sizeof(int));
			fidx.write(reinterpret_cast<char*>(&root), sizeof(int));
			fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (num - 1), ios::beg);
			fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
			delete t1;
			delete d1;
		}
		fidx.close();
		fdata.close();
	}

	void erase(const T &x) {
		fidx.open(idxname, fstream::in | fstream::out | fstream::binary);
		fdata.open(dataname, fstream::in | fstream::out | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));//读取root
		int p = erase(x, root);
		if (p != 0) {
			//idxNode t;
			idxNode *t1 = new idxNode;
			idxNode &t = *t1;
			fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
			fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
			if (t.len == 1) {
				root = t.idx[0];
				fidx.seekp(sizeof(int), ios::beg);
				fidx.write(reinterpret_cast<char*>(&root), sizeof(int));//更改root
				//idxNode tmp;
				idxNode *tmp1 = new idxNode;
				idxNode &tmp = *tmp1;
				fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[0] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
				if (tmp.type == 1 && tmp.len == 1) {//数据结点
					//dataNode d;
					dataNode *d1 = new dataNode;
					dataNode &d = *d1;
					fdata.seekg(sizeof(int) + sizeof(dataNode) * (tmp.idx[0] - 1), ios::beg);
					fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
					if (d.len == 0) {
						root = 0;
						fidx.seekp(sizeof(int), ios::beg);
						fidx.write(reinterpret_cast<char*>(&root), sizeof(int));
					}
					delete d1;
				}
				delete tmp1;
			}
			delete t1;
		}
		fidx.close();
		fdata.close();
	}

	iterator at(const T &x) {
		fidx.open(idxname, fstream::in | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));
		fidx.close();
		return at(x, root);
	}

	U* find(const T &x) {
		fidx.open(idxname, fstream::in | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));
		fidx.close();
		return find(x, root);
	}

	iterator lowerbound(const T &x) {
		fidx.open(idxname, fstream::in | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));
		fidx.close();
		return lowerbound(x, root);
	}

	iterator begin() {
		fidx.open(idxname, fstream::in | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		while (t.type == 0) {
			fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[0] - 1), ios::beg);
			fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		}
		//dataNode d;
		dataNode *d1 = new dataNode;
		dataNode &d = *d1;
		fdata.open(dataname, fstream::in | fstream::binary);
		fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[0] - 1), ios::beg);
		fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
		fidx.close();
		fdata.close();
		delete t1;
		delete d1;
		return iterator(dataname, t.idx[0], 0);
	}

	bool isValid(iterator &i) {
		return i.dcur != 0;
	}

	bool check(const T &x) {
		fidx.open(idxname, fstream::in | fstream::out | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));
		fidx.close();
		if (root == 0)
			return 0;
		else
			return check(x, root);
	}

	void modify(const T &x, const U &v) {
		fidx.open(idxname, fstream::in | fstream::out | fstream::binary);
		int root;
		fidx.seekg(sizeof(int), ios::beg);
		fidx.read(reinterpret_cast<char*>(&root), sizeof(int));
		modify(x, v, root);
		fidx.close();
	}

	void clear() {
		fidx.open(idxname, fstream::out | fstream::trunc | fstream::binary);
		int num = 0, root = 0;
		fidx.write(reinterpret_cast<char*>(&num), sizeof(int));
		fidx.write(reinterpret_cast<char*>(&root), sizeof(int));
		fidx.close();
		fdata.open(dataname, fstream::out | fstream::trunc | fstream::binary);
		num = 0;
		fdata.write(reinterpret_cast<char*>(&num), sizeof(int));
		fdata.close();
	}

private:
	//void makeEmpty(idxNode *t) {}
	int insert(const T &x, const U &v, int pos) {//返回值为0，不用分裂根节点，否则返回位置
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i;
		for (i = 0; i < t.len - 1; ++i) {//查找x所在的子树
			if (x < t.key[i])
				break;
		}
		int newNode;
		if (t.type == 0)//仍为索引结点，继续寻找
			newNode = insert(x, v, t.idx[i]);
		else//数据结点，插入数据
			newNode = insertData(x, v, t.idx[i]);
		if (newNode == 0)
		{
			delete t1;
			return 0;
		}
		else {
			if (t.type == 0)//增加索引结点
			{
				delete t1;
				return addIdxBlk(newNode, pos);
			}
			else//增加数据结点
			{
				delete t1;
				return addDataBlk(newNode, pos);
			}
		}
	}

	int insertData(const T &x, const U &v, const int pos) {//pos为插入结点位置，返回0代表不需要插入新结点，否则返回新插入结点位置
		//dataNode d;
		dataNode *d1 = new dataNode;
		dataNode &d = *d1;
		fdata.seekg(sizeof(int) + sizeof(dataNode) * (pos - 1), ios::beg);
		fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
		if (d.len < dataBlkSize) {//x可以插入到当前块中,此时没有新建数据结点，不用更新nex值
			int i;
			for (i = d.len; i > 0 && x < d.record[i - 1]; --i) {
				d.record[i] = d.record[i - 1];
				d.value[i] = d.value[i - 1];
			}
			d.record[i] = x;
			d.value[i] = v;
			d.len++;
			fdata.seekp(sizeof(int) + sizeof(dataNode) * (pos - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
			delete d1;
			return 0;
		}
		//当前块已满，新建数据结点，注意更改nex值
		//dataNode newNode;
		dataNode *newNode1 = new dataNode;
		dataNode &newNode = *newNode1;
		int i, j;
		int max = dataBlkSize / 2;
		newNode.len = max + 1;
		for (i = max, j = dataBlkSize - 1; i >= 0 && x < d.record[j]; --i) {//分裂为两个数据结点
			newNode.value[i] = d.value[j];
			newNode.record[i] = d.record[j--];
		}
		if (i >= 0) {//x在新数据结点中
			newNode.record[i] = x;
			newNode.value[i--] = v;
		}
		for (; i >= 0; --i) {
			newNode.value[i] = d.value[j];
			newNode.record[i] = d.record[j--];
		}
		//newNode.nex = d.nex;
		int num;
		fdata.seekg(0, ios::beg);
		fdata.read(reinterpret_cast<char*>(&num), sizeof(int));
		num++;
		//d.nex = num;
		d.len = dataBlkSize - max;
		if (j < d.len - 1) {//x没有插入到新的块中
			for (; j >= 0 && x < d.record[j]; --j) {
				d.record[j + 1] = d.record[j];
				d.value[j + 1] = d.value[j];
			}
			d.record[j + 1] = x;
			d.value[j + 1] = v;
		}
		fdata.seekp(0, ios::beg);
		fdata.write(reinterpret_cast<char*>(&num), sizeof(int));//修改结点数
		fdata.seekp(sizeof(int) + sizeof(dataNode) * (num - 1), ios::beg);
		fdata.write(reinterpret_cast<char*>(&newNode), sizeof(dataNode));
		fdata.seekp(sizeof(int) + sizeof(dataNode) * (pos - 1), ios::beg);
		fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
		delete d1;
		delete newNode1;
		return num;
	}

	int addIdxBlk(const int newnode, const int tmp) {
		//idxNode p, t;
		idxNode *p1 = new idxNode;
		idxNode &p = *p1;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (newnode - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&p), sizeof(idxNode));
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		while (p.type == 0) {//找新插入块最小值存入min
			fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (p.idx[0] - 1), ios::beg);
			fidx.read(reinterpret_cast<char*>(&p), sizeof(idxNode));
		}
		//dataNode d;
		dataNode *d1 = new dataNode;
		dataNode &d = *d1;
		fdata.seekg(sizeof(int) + sizeof(dataNode) * (p.idx[0] - 1), ios::beg);
		fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
		T min = d.record[0];
		if (t.len < idxSize) {//索引块里没满，直接加入
			int i;
			for (i = t.len - 1; i > 0 && min < t.key[i - 1]; --i) {
				t.key[i] = t.key[i - 1];
				t.idx[i + 1] = t.idx[i];
			}
			t.idx[i + 1] = newnode;
			t.key[i] = min;
			t.len++;
			fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp - 1), ios::beg);
			fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
			delete p1;
			delete t1;
			delete d1;
			return 0;
		}
		//分裂当前结点
		//idxNode newIdx;
		idxNode *newIdx1 = new idxNode;
		idxNode &newIdx = *newIdx1;
		newIdx.type = 0;
		int max = idxSize / 2;
		newIdx.len = max + 1;
		int i, j;
		if (t.key[idxSize - 2] < min) {//新插入项是最大的，移到新索引块
			newIdx.key[max - 1] = min;
			newIdx.idx[max] = newnode;
		}
		else {//最大项移到新索引块，将新插入项插入到原索引块
			newIdx.key[max - 1] = t.key[idxSize - 2];
			newIdx.idx[max] = t.idx[idxSize - 1];
			int i;
			for (i = t.len - 2; i > 0 && min < t.key[i - 1]; --i) {
				t.key[i] = t.key[i - 1];
				t.idx[i + 1] = t.idx[i];
			}
			t.key[i] = min;
			t.idx[i + 1] = newnode;
		}
		//分裂一半索引项到新增索引结点
		for (i = max - 1, j = idxSize - 1; i > 0; --i, --j) {
			newIdx.key[i - 1] = t.key[j - 1];
			newIdx.idx[i] = t.idx[j];
		}
		newIdx.idx[0] = t.idx[j];
		t.len = idxSize - max;
		int num;
		fidx.seekg(0, ios::beg);
		fidx.read(reinterpret_cast<char*>(&num), sizeof(int));
		fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (tmp - 1), ios::beg);
		fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
		fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * num, ios::beg);
		fidx.write(reinterpret_cast<char*>(&newIdx), sizeof(idxNode));
		num++;
		fidx.seekp(0, ios::beg);
		fidx.write(reinterpret_cast<char*>(&num), sizeof(int));
		delete t1;
		delete p1;
		delete d1;
		delete newIdx1;
		return num;
	}

	int addDataBlk(const int newnode, const int tmp) {
		//dataNode newNode;
		dataNode *newNode1 = new dataNode;
		dataNode &newNode = *newNode1;
		fdata.seekg(sizeof(int) + sizeof(dataNode) * (newnode - 1), ios::beg);
		fdata.read(reinterpret_cast<char*>(&newNode), sizeof(dataNode));
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		if (t.len < idxSize) {//当前块没有满，直接插入
			int i;
			for (i = t.len - 1; i > 0 && newNode.record[0] < t.key[i - 1]; --i) {
				t.key[i] = t.key[i - 1];
				t.idx[i + 1] = t.idx[i];
			}
			t.key[i] = newNode.record[0];
			t.idx[i + 1] = newnode;
			t.len++;
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			newNode.nex = d.nex;
			d.nex = newnode;
			fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
			fdata.seekp(sizeof(int) + sizeof(dataNode) * (newnode - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&newNode), sizeof(dataNode));
			fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp - 1), ios::beg);
			fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
			delete newNode1;
			delete t1;
			delete d1;
			return 0;
		}
		//分裂结点
		//idxNode newIdx;
		idxNode *newIdx1 = new idxNode;
		idxNode &newIdx = *newIdx1;
		newIdx.type = 1;
		int max = idxSize / 2;
		newIdx.len = max + 1;
		int i, j;
		if (t.key[idxSize - 2] < newNode.record[0]) {//新增加的数据块是最大的
			newIdx.key[max - 1] = newNode.record[0];
			newIdx.idx[max] = newnode;
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[t.len - 1] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			newNode.nex = d.nex;
			d.nex = newnode;
			fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[t.len - 1] - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
			fdata.seekp(sizeof(int) + sizeof(dataNode) * (newnode - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&newNode), sizeof(dataNode));
			delete d1;
		}
		else {//将原索引块的最大项移到新索引块，新增加的数据插入原索引块
			newIdx.key[max - 1] = t.key[idxSize - 2];
			newIdx.idx[max] = t.idx[idxSize - 1];
			int i;
			for (i = t.len - 2; i > 0 && newNode.record[0] < t.key[i - 1]; --i) {
				t.key[i] = t.key[i - 1];
				t.idx[i + 1] = t.idx[i];
			}
			t.key[i] = newNode.record[0];
			t.idx[i + 1] = newnode;
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			newNode.nex = d.nex;
			d.nex = newnode;
			fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (newnode - 1), ios::beg);
			fdata.write(reinterpret_cast<char*>(&newNode), sizeof(dataNode));
			delete d1;
		}
		//将一半索引项移到新索引块
		for (i = max - 1, j = idxSize - 1; i > 0; --i, --j) {
			newIdx.idx[i] = t.idx[j];
			newIdx.key[i - 1] = t.key[j - 1];
		}
		newIdx.idx[0] = t.idx[j];
		t.len = idxSize - max;
		int num;
		fidx.seekg(0, ios::beg);
		fidx.read(reinterpret_cast<char*>(&num), sizeof(int));
		fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * num, ios::beg);
		fidx.write(reinterpret_cast<char*>(&newIdx), sizeof(idxNode));
		fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (tmp - 1), ios::beg);
		fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
		num++;
		fidx.seekp(0, ios::beg);
		fidx.write(reinterpret_cast<char*>(&num), sizeof(int));
		delete t1;
		delete newIdx1;
		delete newNode1;
		return num;
	}

	int erase(const T &x, const int pos) {
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i;
		for (i = 0; i < t.len - 1; ++i)
			if (x < t.key[i])
				break;//查找x所在子树 
		if (t.type == 1)
		{
			int ans=eraseData(t, i, x, pos);
			delete t1;
			return ans;
		}
		bool p = erase(x, t.idx[i]);
		if (p)
		{
			int ans=mergeIdxBlk(t, i, pos);
			delete t1;
			return ans;
		}
		delete t1;
		return 0;
	}

	int eraseData(idxNode &t, const int i, const T x, const int pos) {
		//dataNode d;
		dataNode *d1 = new dataNode;
		dataNode &d = *d1;
		fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
		fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
		int min = (dataBlkSize + 1) / 2;
		int j;
		for (j = 0; j < d.len; ++j)
			if (!(d.record[j] < x) && !(x < d.record[j]))
				break;
		for (; j < d.len - 1; ++j) {
			d.record[j] = d.record[j + 1];
			d.value[j] = d.value[j + 1];
		}
		d.len--;
		fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
		fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
		delete d1;
		return mergeDataBlk(t, i, pos);
	}

	int mergeDataBlk(idxNode &t, const int i, const int pos) {
		//dataNode d;
		dataNode *d1 = new dataNode;
		dataNode &d = *d1;
		int min = (dataBlkSize + 1) / 2;
		fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
		fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
		if (d.len < min) {//不到半满
			if (i > 0) {//有前邻居
				//dataNode pre;
				dataNode *pre1 = new dataNode;
				dataNode &pre = *pre1;
				fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i - 1] - 1), ios::beg);
				fdata.read(reinterpret_cast<char*>(&pre), sizeof(dataNode));
				if (pre.len > min) {//前邻居超过半满，借一个结点
					int j;
					for (j = d.len; j > 0; --j) {
						d.record[j] = d.record[j - 1];
						d.value[j] = d.value[j - 1];
					}
					d.record[0] = pre.record[pre.len - 1];
					d.value[0] = pre.value[pre.len - 1];
					pre.len--;
					d.len++;
					t.key[i - 1] = d.record[0];
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i - 1] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&pre), sizeof(dataNode));
					delete pre1;
					delete d1;
					return 0;
				}
				else if (pre.len == min) {//合并
					int j;
					for (j = 0; j < d.len; ++j) {
						pre.record[j + pre.len] = d.record[j];
						pre.value[j + pre.len] = d.value[j];
					}
					for (j = i; j < t.len - 1; ++j)
						t.idx[j] = t.idx[j + 1];
					for (j = i; j < t.len - 2; ++j)
						t.key[j] = t.key[j + 1];
					pre.len += d.len;
					t.len--;
					pre.nex = d.nex;
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					//fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
					//fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i - 1] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&pre), sizeof(dataNode));
					delete d1;
					delete pre1;
					return 1;
				}
				delete pre1;
			}
			if (i < t.len - 1) {//有后邻居
				//dataNode next;
				dataNode *next1 = new dataNode;
				dataNode &next = *next1;
				fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i + 1] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&next), sizeof(idxNode));
				if (next.len > min) {
					t.key[i] = d.record[0];
					int j;
					d.record[d.len] = next.record[0];
					d.value[d.len] = next.value[0];
					for (j = 0; j < next.len; ++j) {
						next.record[j] = next.record[j + 1];
						next.value[j] = next.value[j + 1];
					}
					next.len--;
					d.len++;
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i + 1] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&next), sizeof(dataNode));
					delete next1;
					delete d1;
					return 0;
				}
				else if (next.len == min) {//后邻居半满，合并
					int j;
					for (j = 0; j < next.len; ++j) {
						d.record[j + d.len] = next.record[j];
						d.value[j + d.len] = next.value[j];
					}
					for (j = i; j < t.len - 1; ++j)
						t.idx[j] = t.idx[j + 1];
					for (j = i; j < t.len - 2; ++j)
						t.key[j] = t.key[j + 1];
					d.len += next.len;
					t.len--;
					d.nex = next.nex;
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
					//fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i + 1] - 1), ios::beg);
					//fdata.write(reinterpret_cast<char*>(&next), sizeof(dataNode));
					delete d1;
					delete next1;
					return 1;
				}
				delete next1;
			}
			delete d1;
			return 1;
		}
		delete d1;
		return 0;
	}
	int mergeIdxBlk(idxNode &t, int i, int pos) {
		int min = (idxSize + 1) / 2;
		//idxNode t2;
		idxNode *t1 = new idxNode;
		idxNode &t2 = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i] - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t2), sizeof(idxNode));
		if (t2.len < min) {
			if (i > 0) {
				//idxNode pre;
				idxNode *pre1 = new idxNode;
				idxNode &pre = *pre1;
				fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i - 1] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&pre), sizeof(idxNode));
				if (pre.len > min) {
					t.key[i - 1] = pre.key[pre.len - 2];
					for (int j = t2.len; j > 0; --j)
						t2.idx[j] = t2.idx[j - 1];
					for (int j = t2.len - 1; j > 0; --j)
						t2.key[j] = t2.key[j - 1];
					t2.idx[0] = pre.idx[pre.len - 1];
					pre.len--;
					t2.len++;
					//dataNode d;
					dataNode *d1 = new dataNode;
					dataNode &d = *d1;
					if (t2.type == 0) {
						//idxNode tmp;
						idxNode *tmp1 = new idxNode;
						idxNode &tmp = *tmp1;
						fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t2.idx[1] - 1), ios::beg);
						fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
						while (tmp.type == 0) {
							fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp.idx[0] - 1), ios::beg);
							fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
						}
						fdata.seekg(sizeof(int) + sizeof(dataNode) * (tmp.idx[0] - 1), ios::beg);
						fdata.read(reinterpret_cast<char*>(&d1), sizeof(dataNode));
						delete tmp1;
					}
					else {
						fdata.seekg(sizeof(int) + sizeof(dataNode) * (t2.idx[1] - 1), ios::beg);
						fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
					}
					t2.key[0] = d.record[0];
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i - 1] - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&pre), sizeof(idxNode));
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i] - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t2), sizeof(idxNode));
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					delete d1;
					delete pre1;
					delete t1;
					return 0;
				}
				delete pre1;
			}
			if (i < t.len - 1) {
				//idxNode next;
				idxNode *next1 = new idxNode;
				idxNode &next = *next1;
				fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i + 1] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&next), sizeof(idxNode));
				if (next.len > min) {
					t.key[i] = next.key[0];
					t2.idx[t2.len] = next.idx[0];
					for (int j = 0; j < next.len - 1; ++j)
						next.idx[j] = next.idx[j + 1];
					for (int j = 0; j < next.len - 2; ++j)
						next.key[j] = next.key[j + 1];
					next.len--;
					t2.len++;
					//dataNode d;
					dataNode *d1 = new dataNode;
					dataNode &d = *d1;
					if (t2.type == 0) {
						//idxNode tmp;
						idxNode *tmp1 = new idxNode;
						idxNode &tmp = *tmp1;
						fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t2.idx[t2.len - 1] - 1), ios::beg);
						fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
						while (tmp.type == 0) {
							fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp.idx[0] - 1), ios::beg);
							fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
						}
						fdata.seekg(sizeof(int) + sizeof(dataNode) * (tmp.idx[0] - 1), ios::beg);
						fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
						delete tmp1;
					}
					else {
						fdata.seekg(sizeof(int) + sizeof(dataNode) * (t2.idx[t2.len - 1] - 1), ios::beg);
						fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
					}
					t2.key[t2.len - 2] = d.record[0];
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i + 1] - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&next), sizeof(idxNode));
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i] - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t2), sizeof(idxNode));
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					delete next1;
					delete d1;
					delete t1;
					return 0;
				}
				delete next1;
			}
			if (i > 0) {
				//idxNode pre;
				idxNode *pre1 = new idxNode;
				idxNode &pre = *pre1;
				fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i - 1] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&pre), sizeof(idxNode));
				if (pre.len == min) {
					//dataNode d;
					dataNode *d1 = new dataNode;
					dataNode &d = *d1;
					if (t2.type == 0) {
						//idxNode tmp;
						idxNode *tmp1 = new idxNode;
						idxNode &tmp = *tmp1;
						fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t2.idx[0] - 1), ios::beg);
						fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
						while (tmp.type == 0) {
							fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp.idx[0] - 1), ios::beg);
							fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
						}
						fdata.seekg(sizeof(int) + sizeof(dataNode) * (tmp.idx[0] - 1), ios::beg);
						fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
						delete tmp1;
					}
					else {
						fdata.seekg(sizeof(int) + sizeof(dataNode) * (t2.idx[0] - 1), ios::beg);
						fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
					}
					pre.key[pre.len - 1] = d.record[0];
					for (int j = 0; j < t2.len; ++j)
						pre.idx[j + pre.len] = t2.idx[j];
					for (int j = 0; j < t2.len - 1; ++j)
						pre.key[j + pre.len] = t2.key[j];
					for (int j = i; j < t.len - 1; ++j)
						t.idx[j] = t.idx[j + 1];
					for (int j = i - 1; j < t.len - 2; ++j)
						t.key[j] = t.key[j + 1];
					t.len--;
					pre.len += t2.len;
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
					fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i - 1] - 1), ios::beg);
					fidx.write(reinterpret_cast<char*>(&pre), sizeof(idxNode));
					delete d1;
					delete pre1;
					delete t1;
					return 1;
				}
				delete pre1;
			}
			if (i < t.len - 1) {
				//idxNode next;
				idxNode *next1 = new idxNode;
				idxNode &next = *next1;
				fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i + 1] - 1), ios::beg);
				fidx.read(reinterpret_cast<char*>(&next), sizeof(idxNode));
				//dataNode d;
				dataNode *d1 = new dataNode;
				dataNode &d = *d1;
				if (next.type == 0) {
					//idxNode tmp;
					idxNode *tmp1 = new idxNode;
					idxNode &tmp = *tmp1;
					fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (next.idx[0] - 1), ios::beg);
					fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
					while (tmp.type == 0) {
						fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (tmp.idx[0] - 1), ios::beg);
						fidx.read(reinterpret_cast<char*>(&tmp), sizeof(idxNode));
					}
					fdata.seekg(sizeof(int) + sizeof(dataNode) * (tmp.idx[0] - 1), ios::beg);
					fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
					delete tmp1;
				}
				else {
					fdata.seekg(sizeof(int) + sizeof(dataNode) * (next.idx[0] - 1), ios::beg);
					fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
				}
				t2.key[t2.len - 1] = d.record[0];
				for (int j = 0; j < next.len; ++j)
					t2.idx[j + t2.len] = next.idx[j];
				for (int j = 0; j < next.len - 1; ++j)
					t2.key[j + t2.len] = next.key[j];
				for (int j = i + 1; j < t.len - 1; ++j)
					t.idx[j] = t.idx[j + 1];
				for (int j = i; j < t.len - 2; ++j)
					t.key[j] = t.key[j + 1];
				t.len--;
				t2.len += next.len;
				fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (pos - 1), ios::beg);
				fidx.write(reinterpret_cast<char*>(&t), sizeof(idxNode));
				fidx.seekp(sizeof(int) * 2 + sizeof(idxNode) * (t.idx[i] - 1), ios::beg);
				fidx.write(reinterpret_cast<char*>(&t2), sizeof(idxNode));
				delete d1;
				delete next1;
				delete t1;
				return 1;
			}
			delete t1;
			return 1;
		}
		delete t1;
	}

	void modify(const T &x, const U &v, const int root) {
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i;
		for (i = 0; i < t.len - 1; ++i)
			if (x < t.key[i])
				break;
		if (t.type == 1) {
			fdata.open(dataname, fstream::in | fstream::out | fstream::binary);
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			for (int j = 0; j < d.len; ++j)
				if (!(d.record[j] < x) && !(x < d.record[j])) {
					d.value[j] = v;
					fdata.seekp(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
					fdata.write(reinterpret_cast<char*>(&d), sizeof(dataNode));
					fdata.close();
					delete t1;
					delete d1;
					return;
				}
			delete d1;
		}
		else	modify(x, v, t.idx[i]);
		delete t1;
	}

	iterator at(const T &x, const int root) {
		fidx.open(idxname, fstream::in | fstream::binary);
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i, j;
		for (i = 0; i < t.len - 1; ++i) {//查找x所在的子树
			if (x < t.key[i])
				break;
		}
		if (t.type == 1) {//找到数据结点
			fdata.open(dataname, fstream::in | fstream::binary);
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			for (j = 0; j < d.len; ++j) {
				if (d.record[j] == x) {
					fidx.close();
					fdata.close();
					int ha=t.idx[i];
					delete t1;
					delete d1;
					return iterator(dataname, ha, j);
				}
			}
			delete d1;
			fdata.close();
		}
		fidx.close();
		int ha=t.idx[i];
		delete t1;
		return at(x, ha);//若不是数据结点，递归调用私有find函数
	}

	U* find(const T &x, const int root) {
		fidx.open(idxname, fstream::in | fstream::binary);
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i, j;
		for (i = 0; i < t.len - 1; ++i) {//查找x所在的子树
			if (x < t.key[i])
				break;
		}
		if (t.type == 1) {//找到数据结点
			fdata.open(dataname, fstream::in | fstream::binary);
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			for (j = 0; j < d.len; ++j) {
				if (!(d.record[j] < x) && !(x < d.record[j])) {
					fidx.close();
					fdata.close();
					U *ans=new U(d.value[j]);
					delete d1;
					delete t1;
					return ans;
				}
			}
			delete d1;
			fdata.close();
		}
		fidx.close();
		int ha=t.idx[i];
		delete t1;
		return find(x, ha);//若不是数据结点，递归调用私有find函数
	}

	iterator lowerbound(const T &x, const int root) {
		fidx.open(idxname, fstream::in | fstream::binary);
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i, j;
		for (i = 0; i < t.len - 1; ++i) {//查找x所在的子树
			if (x < t.key[i])
				break;
		}
		if (t.type == 1) {//找到数据结点
			fdata.open(dataname, fstream::in | fstream::binary);
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			for (j = 0; j<d.len;j++)
			{
				if (x <  d.record[j]  || (x==d.record[j]))
					break;
			}
			int q=t.idx[i];
			if (j==d.len)
			{
				q=d.nex;
				if (q!=0)
				{
					fdata.seekg(sizeof(int) + sizeof(dataNode) * (q - 1), ios::beg);
					fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
					j=0;
				}
			}
			//for (j = d.len - 1; j >= 0; --j) {
			//	if (d.record[j] < x  || (x==d.record[j]))
			//		break;
		//	}
			fdata.close();
			fidx.close();
			delete t1;
			return iterator(dataname, q, j);
		}
		fidx.close();
		int ha=t.idx[i];
		delete t1;
		return lowerbound(x, ha);
	}

	bool check(const T &x, const int root) {
		fidx.open(idxname, fstream::in | fstream::binary);
		//idxNode t;
		idxNode *t1 = new idxNode;
		idxNode &t = *t1;
		fidx.seekg(sizeof(int) * 2 + sizeof(idxNode) * (root - 1), ios::beg);
		fidx.read(reinterpret_cast<char*>(&t), sizeof(idxNode));
		int i, j;
		for (i = 0; i < t.len - 1; ++i)
			if (x < t.key[i])
				break;
		if (t.type == 1) {
			fdata.open(dataname, fstream::in | fstream::binary);
			fdata.seekg(sizeof(int) + sizeof(dataNode) * (t.idx[i] - 1), ios::beg);
			//dataNode d;
			dataNode *d1 = new dataNode;
			dataNode &d = *d1;
			fdata.read(reinterpret_cast<char*>(&d), sizeof(dataNode));
			for (j = 0; j < d.len; ++j) {
				if (!(d.record[j] < x) && !(x < d.record[j])) {
					fidx.close();
					fdata.close();
					delete d1;
					delete t1;
					return 1;
				}
			}
			fidx.close();
			fdata.close();
			delete d1;
			delete t1;
			return 0;
		}
		fidx.close();
		int ha=t.idx[i];
		delete t1;
		return check(x, ha);
	}
};

#endif
