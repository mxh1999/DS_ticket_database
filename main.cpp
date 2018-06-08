#include <iostream>
#include <cstdio>
#include <iomanip>
#include "userlist.hpp"
#include "BplusTree.hpp"
#include <cstring>
#include <iomanip>
using namespace std;
struct USER {
    char name[41], password[21], mail[21], phone_num[21];
    char privilege;
};

struct user_order_key {
    int user_id;
    char date[11], train_kind, train_id[21];
    char loc1[41], loc2[41];

    user_order_key() {
        memset(loc1, 0, sizeof(loc1));
        memset(loc2, 0, sizeof(loc2));
        memset(date, 0, sizeof(date));
        user_id = 0;
        memset(train_id, 0, sizeof(train_id));
    }

    bool operator<(const user_order_key &other) const {
        if (user_id !=
            other.user_id)//|| (strcmp(date, other.date) > 0) || (strcmp(train_kind, other.train_kind) > 0) || (strcmp(train_id, other.train_id) > 0) || (strcmp(loc1, other.loc1) > 0) || (strcmp(loc2, other.loc2) > 0))
            return user_id < other.user_id;
        if (strcmp(date, other.date) != 0)
            return (strcmp(date, other.date) < 0);
        if (train_kind != other.train_kind)
            return (train_kind < other.train_kind);
        if (strcmp(train_id, other.train_id) != 0)
            return (strcmp(train_id, other.train_id) < 0);
        if (strcmp(loc1, other.loc1) != 0)
            return (strcmp(loc1, other.loc1) < 0);
        else return (strcmp(loc2, other.loc2) < 0);
    }

    bool operator==(const user_order_key &other) const {
        if (user_id == other.user_id && (strcmp(date, other.date) == 0) &&
            (train_kind == other.train_kind) && (strcmp(train_id, other.train_id) == 0) &&
            (strcmp(loc1, other.loc1) == 0) && (strcmp(loc2, other.loc2) == 0))
            return 1;
        else return 0;
    }
};

struct tk_order {
    user_order_key ky;
    char tm_st[6], tm_arrive[6];
    double price[5];
    char tk_name[5][21];
    int num_tk_kind;
    int tk_order_num[5] = {0};

    tk_order() {
        memset(tm_st, 0, sizeof(tm_st));
        memset(tm_arrive, 0, sizeof(tm_arrive));
        for (int i = 0; i < 5; ++i)
            memset(tk_name[i], 0, sizeof(tk_name[i]));
        num_tk_kind = 0;
    }
};

struct tk_key {
    char loc1[41], loc2[41], train_kind;
    char train_id[21];

    tk_key() {
        memset(loc1, 0, sizeof(loc1));
        memset(loc2, 0, sizeof(loc2));
        memset(train_id, 0, sizeof(train_id));
    }

    bool operator<(const tk_key &other) const {
        if (strcmp(loc1, other.loc1) != 0) return (strcmp(loc1, other.loc1) < 0);
        if (train_kind != other.train_kind) return (train_kind < other.train_kind);
        if (strcmp(loc2, other.loc2) != 0) return (strcmp(loc2, other.loc2) < 0);
        return (strcmp(train_id, other.train_id) < 0);
    }

    bool operator==(const tk_key &other) const {
        if ((strcmp(loc1, other.loc1) == 0) && strcmp(loc2, other.loc2) == 0 &&
            train_kind == other.train_kind && strcmp(train_id, other.train_id) == 0)
            return 1;
        else return 0;
    }
//    bool operator <(const tk_key &other) const {
//        if(strcmp(loc1, other.loc1) < 0)//|| (strcmp(train_kind, other.train_kind) > 0 && strcmp(loc1, other.loc1) == 0) || (strcmp(loc2, other.loc2) > 0 && ) || (strcmp(train_id, other.train_id) > 0))
//            return 1;
//        else if(strcmp(loc1, other.loc2) > 0)
//            return 0;
//        else if(strcmp(train_kind, other.train_kind) < 0)
//            return 1;
//        else if(strcmp(train_kind, other.train_kind) > 0)
//            return 0;
//        else if(strcmp(loc2, other.loc2) < 0)
//            return 1;
//        else if(strcmp(loc2, other.loc2) > 0)
//            return 0;
//        else if(strcmp(train_id, other.train_id) < 0)
//            return 1;
//        else return 0;
//    }

};

struct tk {
    tk_key ky;
    bool flag;//1代表顺序的车票，0代表是反着的车票;
    char tm_arrive[6];
    char tm_st[6];

    tk() {
        flag = 1;
        memset(tm_arrive, 0, sizeof(tm_arrive));
        memset(tm_st, 0, sizeof(tm_st));
    }
};

struct STATION {
    char loc[41];
    char time_arrive[6], time_start[6], time_stop[6];//arrive到达时间，start出发时间，stop停靠时间
    float price[5];

    STATION() {
        memset(time_arrive, 0, sizeof(time_arrive));
        memset(time_start, 0, sizeof(time_start));
        memset(time_stop, 0, sizeof(time_stop));
        memset(loc, 0, sizeof(loc));
        for (int i = 0; i < 5; ++i)
            price[i] = 0;
    }
};

struct train_id_key {
    char train_id[21];

    bool operator<(const train_id_key &b) const {
        if (strcmp(train_id, b.train_id) < 0)
            return 1;
        else return 0;
    }

    bool operator==(const train_id_key &b) const {
        if (strcmp(train_id, b.train_id) == 0)
            return 1;
        else return 0;
    }
};

struct TRAIN {
    char train_id[21];
    char train_name[41], train_kind;
    short num_station, num_ticket_kind;
    char ticket_kind[5][21];
    bool isSale = 0;
    STATION station[60];
    short tk_remain[31][60][5];

    TRAIN() {
        memset(train_id, 0, sizeof(train_id));
        memset(train_name, 0, sizeof(train_name));
        for (int i = 0; i < 31; ++i)
            for (int j = 0; j < 60; ++j)
                for (int p = 0; p < 5; ++p)
                    tk_remain[i][j][p] = 2000;
        isSale = 0;
        num_ticket_kind = 0;
        num_station = 0;
    }
};

struct tk_query {
    tk_key ky;
    char tm_st[6], tm_arrive[6];
    double price[5];
    int tk_kind_num;
    char tk_name[5][21];
    int tk_remain[5];
    int dt1 = 0;
    int dt2 = 0;
    tk_query() {
        memset(tm_st, 0, sizeof(tm_st));
        memset(tm_arrive, 0, sizeof(tm_arrive));
        for (int i = 0; i < 5; ++i)
            memset(tk_name[i], 0, sizeof(tk_name[i]));
        tk_kind_num = 0;
        for (int i = 0; i < 5; ++i)
            price[i] = 0;
    }
};

Userlist user("test1.txt");
BplusTree<train_id_key, TRAIN,160,1> train;
BplusTree<tk_key, tk,30,10> ticket;
BplusTree<user_order_key, tk_order,35,30> User;

int main() {
//	printf("%d %d\n",sizeof(train_id_key),sizeof(TRAIN));
//	printf("%d %d\n",sizeof(tk_key),sizeof(tk));
//	printf("%d %d\n",sizeof(user_order_key),sizeof(tk_order));
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    train.init("train_in", "train_out");
    User.init("User_in", "User_out");
    ticket.init("ticket_in", "ticket_out");
    char a[20];
    while (cin >> a) {
        if (strcmp(a, "register") == 0) {//注册用户
            USER now;
            cin >> now.name >> now.password >> now.mail >> now.phone_num;
            cout << user.Register(now.name, now.password, now.mail, now.phone_num) << endl;
        }
        else if (strcmp(a, "login") == 0) {//用户登陆
            int user_id;
            char user_pw[21];
            cin >> user_id >> user_pw;
            cout << user.Login(user_id, user_pw) << endl;
        }
        else if (strcmp(a, "query_profile") == 0) {//查询用户信息
            USER now;
            int user_id;
            cin >> user_id;
            if (user.Query_profile(user_id, now.name, now.mail, now.phone_num, now.privilege)) {
                cout << now.name << ' ' << now.mail << ' ' << now.phone_num << ' ' << (int) now.privilege << endl;
            } else cout << 0 << '\n';
        }
        else if (strcmp(a, "modify_profile") == 0) {//修改用户信息
            USER now;
            int user_id;
            cin >> user_id >> now.name >> now.password >> now.mail >> now.phone_num;
            if (user.Modify_profile(user_id, now.name, now.password, now.mail, now.phone_num))
                cout << 1 << endl;
            else cout << 0 << endl;
        }
        else if (strcmp(a, "modify_privilege") == 0) {//修改用户权限
            int user1_id, user2_id;
            int pvl;
            cin >> user1_id >> user2_id >> pvl;
            cout << user.Modify_privilege(user1_id, user2_id, (char) pvl) << endl;
        }
        else if (strcmp(a, "add_train") == 0) {//添加车次
            /*新建车次，车次编号，车次名称，车次类型，车站数，车票数，车票信息
             * 更新车票内容*/
            TRAIN now;
            cin >> now.train_id >> now.train_name >> now.train_kind >> now.num_station >> now.num_ticket_kind;
            train_id_key ID;
            strcpy(ID.train_id, now.train_id);
            for (int i = 0; i < now.num_ticket_kind; ++i)
                cin >> now.ticket_kind[i];
            for (int i = 0; i < now.num_station; ++i) {//插入车站信息
                cin >> now.station[i].loc >> now.station[i].time_arrive >> now.station[i].time_start
                    >> now.station[i].time_stop;
                for (int j = 0; j < now.num_ticket_kind; ++j) {
                    char tmp;
                    int ans = 0;
                    double dis = 0.1;
                    for (tmp = cin.get(); tmp < '0' || tmp > '9'; tmp = cin.get());
                    ans = tmp - '0';
                    for (tmp = cin.get(); tmp >= '0' && tmp <= '9'; tmp = cin.get())
                        ans = ans * 10 + (tmp - '0');
                    now.station[i].price[j] = ans;
                    for (tmp = cin.get(); tmp >= '0' && tmp <= '9'; tmp = cin.get()) {
                        now.station[i].price[j] += (tmp - '0') * dis;
                        dis *= 0.1;
                    }
                    if(i != 0)
                        now.station[i].price[j] += now.station[i - 1].price[j];
                }
            }
            for (int i = 0; i < now.num_station - 1; ++i) {//tk_remain[dt][i][j]中i表示第i个车站和第i + 1个车站之间的余票数
                for (int j = 0; j < now.num_ticket_kind; ++j) {
                    for (int dt = 0; dt <= 30; ++dt) {
                        now.tk_remain[dt][i][j] = 2000;
                    }
                }
            }
            now.isSale = 0;
//            //test:map
//            bool insert_success;
//            map<int, TRAIN>::iterator test;
//            train.insert(make_pair(ID, now));
            //test:map
            train.insert(ID, now);//还差判断和输出
            cout << 1 << endl;
        }
        else if (strcmp(a, "sale_train") == 0) {
            train_id_key K;
            //TRAIN now;
            cin >> K.train_id;
            if (!train.check(K))
                cout << 0 << endl;
            else {
                TRAIN *_now = train.find(K);
				TRAIN &now=(*_now);
                if (now.isSale == 1)
                    cout << 0 << endl;
                else {
                    now.isSale = 1;
                    train.modify(K, now);
                    for (int i = 0; i < now.num_station; ++i) {
                        for (int j = i + 1; j < now.num_station; ++j) {
                            tk_key K;
                            tk T;
                            strcpy(K.loc1, now.station[i].loc);
                            strcpy(K.loc2, now.station[j].loc);
                            strcpy(K.train_id, now.train_id);
                            K.train_kind = now.train_kind;
                            T.ky = K;
                            strcpy(T.tm_arrive, now.station[j].time_arrive);
                            strcpy(T.tm_st, now.station[i].time_start);
                            T.flag = true;
                            ticket.insert(K, T);
                            strcpy(K.loc1, now.station[j].loc);
                            strcpy(K.loc2, now.station[i].loc);
                            T.ky = K;
                            T.flag = false;
                            strcpy(T.tm_arrive, now.station[i].time_start);
                            strcpy(T.tm_st, now.station[j].time_arrive);
                            ticket.insert(K, T);
//                        cout << "ticket中的票数:" << ticket.size() << '\n';
                        }
                    }
//                cout << "ticket中的票数:" << ticket.size() << '\n';
                    cout << 1 << endl;
                }
				delete _now;
            }
        }
        else if (strcmp(a, "query_train") == 0) {
            train_id_key K;
            //TRAIN now;
            cin >> K.train_id;
            if (train.check(K) == 0)
                cout << 0 << endl;
            else {
                TRAIN *_now = train.find(K);
				TRAIN &now=(*_now);
                if(now.isSale == 0)
                    cout << 0 << endl;
                else{
                    cout << now.train_id << ' ' << now.train_name << ' ' << now.train_kind << ' ' << now.num_station << ' '
                         << now.num_ticket_kind;
                    for (int i = 0; i < now.num_ticket_kind; ++i)
                        cout << ' ' << now.ticket_kind[i];
                    cout << endl;
                    for (int i = 0; i < now.num_station; ++i) {
                        cout << now.station[i].loc << ' ' << now.station[i].time_arrive << ' ' << now.station[i].time_start
                             << ' ' << now.station[i].time_stop;
                        for (int j = 0; j < now.num_ticket_kind; ++j){
                            if(i == 0)
                                cout << ' ' << "￥" << fixed << setprecision(6) << now.station[i].price[j];
                            else cout << ' ' << "￥" << fixed << setprecision(6) << now.station[i].price[j] - now.station[i - 1].price[j];
                        }
                        //要记得用printf
                        cout << endl;
                    }
                }
				delete _now;
            }
        }
        else if (strcmp(a, "delete_train") == 0) {
            train_id_key K;
            //TRAIN now;
            cin >> K.train_id;
            if (train.check(K) == 0)
                cout << 0 << endl;
            else {
                TRAIN *_now = train.find(K);
				TRAIN &now = (*_now);
                if (now.isSale == 1)
                    cout << 0 << endl;
                else {
                    train.erase(K);
                    cout << 1 << endl;
                }
				delete (_now);
            }
        }
        else if (strcmp(a, "modify_train") == 0) {
            train_id_key K;
            //TRAIN now;
            cin >> K.train_id;
            if (train.check(K) == 0)
                cout << 0 << endl;
            else {
				TRAIN *_now = train.find(K);
                TRAIN &now = (*_now);
                if (now.isSale == 1)
                    cout << 0 << endl;
                else {
                    cin >> now.train_name >> now.train_kind >> now.num_station >> now.num_ticket_kind;
                    for (int i = 0; i < now.num_ticket_kind; ++i)
                        cin >> now.ticket_kind[i];
                    for (int i = 0; i < now.num_station; ++i) {
                        cin >> now.station[i].loc >> now.station[i].time_arrive >> now.station[i].time_start
                            >> now.station[i].time_stop;
                        for (int j = 0; j < now.num_ticket_kind; ++j) {
                            char tmp;
                            int ans = 0;
                            double dis = 0.1;
                            for (tmp = cin.get(); tmp < '0' || tmp > '9'; tmp = cin.get());
                            ans = tmp - '0';
                            for (tmp = cin.get(); tmp >= '0' && tmp <= '9'; tmp = cin.get())
                                ans = ans * 10 + (tmp - '0');
                            now.station[i].price[j] = ans;
                            for (tmp = cin.get(); tmp >= '0' && tmp <= '9'; tmp = cin.get()) {
                                now.station[i].price[j] += (tmp - '0') * dis;
                                dis *= 0.1;
                            }
                            if(i != 0)
                                now.station[i].price[j] += now.station[i - 1].price[j];
                        }
                    }
                    train.modify(K, now);
                    cout << 1 << endl;
                }
				delete (_now);
            }
        }
        else if (strcmp(a, "query_ticket") == 0) {
            /*查询两地的票，根据loc1, catalog, loc2查询出第一个符合条件的车次，
             * 然后用迭代器迭代
             * 日期影响输出*/
//            BplusTree<tk_key, tk>::iterator it1;
//            int cc = 0;
//            for(it1 = ticket.begin();ticket.isValid(it1); cc++){
//                cout << it1.Record().loc1 << it1.Record().loc2 << ' ';
//                it1++;
//            }
            tk_key K;
            tk_query T[8000];
            BplusTree<tk_key, tk,30,10>::iterator it;
            char date[12];
            char catalog[11];
            int dt;
            cin >> K.loc1 >> K.loc2 >> date >> catalog;
            dt = (date[8] - '0') * 10 + (date[9] - '0');
            int cnt_train = 0;
            for (int cnt = 0; catalog[cnt] != '\0'; ++cnt) {
                K.train_kind = catalog[cnt];
                it = ticket.lowerbound(K);
                while (true) {
                    tk_key KK = it.Record();
                    if (KK.train_kind == K.train_kind && strcmp(KK.loc1, K.loc1) == 0 &&
                        strcmp(KK.loc2, K.loc2) == 0 && ticket.isValid(it)) {
//                        train_id_key train_K;
//                        strcpy(train_K.train_id, KK.train_id);
//                        tk TT = it.Value();//或者改成it.Value();
                        train_id_key train_K;
                        strcpy(train_K.train_id, KK.train_id);
                        tk TT = it.Value();//或者改成it.Value();
                        if (TT.flag) {
                            T[cnt_train].ky = KK;
                            T[cnt_train].dt1 = dt;
                            T[cnt_train].dt2 = dt;
                            TRAIN *_now = train.find(train_K);
							TRAIN &now = (*_now);
                            int i = 0, min_ticket_remain[5];
                            for (int p = 0; p < now.num_ticket_kind; ++p)
                                min_ticket_remain[p] = 2010;
                            for (i = 0; i != now.num_station - 1 && strcmp(now.station[i].loc, KK.loc1) != 0; ++i){
                                if(strcmp(now.station[i].time_start, now.station[i + 1].time_start) > 0)
                                    T[cnt_train].dt1++;
                            };
                            T[cnt_train].dt2 = T[cnt_train].dt1;
                            strcpy(T[cnt_train].tm_st, now.station[i].time_start);
                            int j;
                            for (j = i; j != now.num_station - 1 && strcmp(now.station[j].loc, KK.loc2) != 0; ++j) {
                                if(strcmp(now.station[j].time_start, now.station[j + 1].time_start) > 0)
                                    T[cnt_train].dt2++;
                                for(int p = 0; p < now.num_ticket_kind; ++p){
                                    if(now.tk_remain[dt][j][p] < min_ticket_remain[p])
                                        min_ticket_remain[p] = now.tk_remain[dt][j][p];
                                }
                            }
                            for (int p = 0; p < now.num_ticket_kind; ++p)
                                T[cnt_train].tk_remain[p] = min_ticket_remain[p];
                            strcpy(T[cnt_train].tm_arrive, now.station[j].time_arrive);
                            for (int p = 0; p < now.num_ticket_kind; ++p) {
                                T[cnt_train].price[p] = now.station[j].price[p] - now.station[i].price[p];
                                strcpy(T[cnt_train].tk_name[p], now.ticket_kind[p]);
                            }
                            T[cnt_train].tk_kind_num = now.num_ticket_kind;
                            cnt_train++;
							delete (_now);
                        }
                        it++;
                    }
                    else break;
                }
            }
            if (cnt_train == 0)
                cout << 0 << endl;
            else {
                cout << cnt_train << endl;
                for (int i = 0; i < cnt_train; ++i) {
                    cout << T[i].ky.train_id << ' ' << T[i].ky.loc1 << ' ' << "2018-06-" << setw(2) << setfill('0') << T[i].dt1 << ' ' << T[i].tm_st << ' ' << T[i].ky.loc2 << ' ' << "2018-06-" << setw(2) << setfill('0') << T[i].dt2;
                    cout << ' ' << T[i].tm_arrive;
                    for (int p = 0; p < T[i].tk_kind_num; ++p)
                        cout << ' ' << T[i].tk_name[p] << ' ' << T[i].tk_remain[p] << ' ' << fixed << setprecision(6) << T[i].price[p];//要记得用printf
                    cout << endl;
                }
            }
        }
        else if (strcmp(a, "query_transfer") == 0) {//查找A到C地，中转B站的车票;
            BplusTree<tk_key, tk,30,10>::iterator it1; //A到B；
            BplusTree<tk_key, tk,30,10>::iterator it2; //C到B；
            tk_key K1;
            tk_key K2;
            char date[12];
            int dt;
            char train_kind;
            char catalog[11];
            cin >> K1.loc1 >> K2.loc1 >> date >> catalog;
            dt = (date[8] - '0') * 10 + (date[9] - '0');
            int cnt = 0;
            tk T_1[5000], T_2[5000];//用来记录每个确定中转站的最短时间的tk的信息
            tk_query TK1, TK2;
//            BplusTree<tk_key, tk>::iterator lyt;
//            int cc = 0;
//            for(lyt = ticket.begin();ticket.isValid(lyt); cc++){
//                cout << lyt.Record().loc1 << lyt.Record().loc2 << ' ';
//                lyt++;
//            }
            for (int b = 0; catalog[b] != '\0'; ++b) {
                train_kind = catalog[b];
                K1.train_kind = train_kind;
                K2.train_kind =train_kind;
                it1 = ticket.lowerbound(K1);
                it2 = ticket.lowerbound(K2);
                while (true) {
                    tk_key KK1 = it1.Record();
                    tk_key KK2 = it2.Record();
                    //接下来的while循环是为了找到一个相同的中转站， KK1指的是A到某个中转站中第一个train_id, KK2指的是C到某个中转站中的第一个train_id;
                    while ((strcmp(KK1.loc1, K1.loc1) == 0) && (strcmp(KK2.loc1, K2.loc1) == 0) &&
                           (KK1.train_kind == KK2.train_kind) && (strcmp(KK1.loc2, KK2.loc2) != 0)) {
                        if (strcmp(KK1.loc2, KK2.loc2) < 0) {
                            tk_key tmp = KK1;
                            while ((strcmp(tmp.loc2, KK1.loc2)) == 0 && tmp.train_kind == KK1.train_kind) {
                                it1++;
                                tmp = it1.Record();
                            }
                            KK1 = tmp;
                        } else {
                            tk_key tmp = KK2;
                            while ((strcmp(tmp.loc2, KK2.loc2)) == 0 && tmp.train_kind == KK2.train_kind) {
                                it2++;
                                tmp = it2.Record();
                            }
                            KK2 = tmp;
                        }
                    }
                    tk_key ky1 = KK1;
                    tk_key ky2 = KK2;
                    tk T1[2000], T2[2000];//用来记录某个特定中转站中的tk信息
                    int cnt_t1 = 0, cnt_t2 = 0;
                    if((strcmp(KK1.loc1, K1.loc1) == 0) && (strcmp(KK2.loc1, K2.loc1) == 0) &&
                       (KK1.train_kind == KK2.train_kind)){
                        while((strcmp(ky1.loc1, KK1.loc1) == 0)  && ky1.train_kind == KK1.train_kind && strcmp(ky1.loc2, KK1.loc2) == 0 &&
                              ticket.isValid(it1)) {
                            tk TT1 = it1.Value();
                            if (TT1.flag) {
                                T1[cnt_t1] = TT1;
                                cnt_t1++;
                            }//把符合条件的train_id存起来；
                            it1++;
                            ky1 = it1.Record();
                        }
                        while (strcmp(ky2.loc1, KK2.loc1) == 0 && ky2.train_kind == KK2.train_kind && strcmp(ky2.loc2, KK2.loc2) == 0 && ticket.isValid(it2)) {
                            tk TT2 = it2.Value();
                            if (!TT2.flag) {
                                T2[cnt_t2] = TT2;
                                cnt_t2++;
                            }//把符合条件的train_id存起来；
                            it2++;
                            ky2 = it2.Record();
                        }
                    }
                    else break;
                    if (cnt_t1 == 0 || cnt_t2 == 0)
                        continue;
                    else {
                        int min_tm = 1440, I, J;
                        for (int i = 0; i < cnt_t1; ++i) {
                            for (int j = 0; j < cnt_t2; ++j) {
                                if (strcmp(T1[i].tm_arrive, T2[j].tm_arrive) < 0) {
                                    int tmp_tm;
                                    tmp_tm = (T2[j].tm_st[0] - T1[i].tm_st[0]) * 10 * 60 +
                                             (T2[j].tm_st[1] - T1[i].tm_st[1]) * 60 +
                                             (T2[j].tm_st[3] - T1[i].tm_st[3]) * 10 + (T2[j].tm_st[4] - T1[i].tm_st[4]);
                                    if(tmp_tm < 0)
                                        tmp_tm += 1440;
                                    if (tmp_tm < min_tm) {
                                        min_tm = tmp_tm;
                                        I = i;
                                        J = j;
                                    }
                                }//比较时间；
                            }
                        }
                        T_1[cnt] = T1[I];
                        T_2[cnt] = T2[J];
                        cnt++;
                    }
                }
            }
            if (cnt == 0) {
                cout << -1 << '\n';
            } else {
                int min_tm = 1440, I;
                for (int i = 0; i < cnt; ++i) {
                    int tmp_tm;
                    tmp_tm = (T_2[i].tm_st[0] - T_1[i].tm_st[0]) * 10 * 60 + (T_2[i].tm_st[1] - T_1[i].tm_st[1]) * 60 +
                             (T_2[i].tm_st[3] - T_1[i].tm_st[3]) * 10 + (T_2[i].tm_st[4] - T_1[i].tm_st[4]);
                    if(tmp_tm < 0)
                        tmp_tm += 1440;
                    if (tmp_tm < min_tm) {
                        min_tm = tmp_tm;
                        I = i;
                    }
                }
                //比较T_1和T_2的时间；
                TK1.ky = T_1[I].ky;//TK1记录的是A到B的时间最短的票信息
                strcpy(TK1.tm_st, T_1[I].tm_st);
                strcpy(TK1.tm_arrive, T_1[I].tm_arrive);
                train_id_key train_K1, train_K2;
                strcpy(train_K1.train_id, TK1.ky.train_id);
                TRAIN *_now = train.find(train_K1);
				TRAIN &now =(*_now);
                int i = 0, min_ticket_remain[5];
                for (int p = 0; p < now.num_ticket_kind; ++p)
                    min_ticket_remain[p] = 2010;
                for (i = 0; strcmp(now.station[i].loc, TK1.ky.loc1) != 0; ++i);
                int j;
                for (j = i; strcmp(now.station[j].loc, TK1.ky.loc2) != 0; ++j) {
                    for (int p = 0; p < now.num_ticket_kind; ++p) {
                        if (now.tk_remain[dt][j][p] < min_ticket_remain[p])
                            min_ticket_remain[p] = now.tk_remain[dt][j][p];
                    }
                }
                TK1.tk_kind_num = now.num_ticket_kind;
                for (int p = 0; p < now.num_ticket_kind; ++p) {
                    strcpy(TK1.tk_name[p], now.ticket_kind[p]);
                    TK1.tk_remain[p] = min_ticket_remain[p];
                    TK1.price[p] = now.station[j].price[p] - now.station[i].price[p];
                }
                TK2.ky = T_2[I].ky;//重复对TK2进行操作
                strcpy(train_K2.train_id, TK2.ky.train_id);
                strcpy(TK2.ky.loc2, T_2[I].ky.loc1);
                strcpy(TK2.ky.loc1, T_2[I].ky.loc2);
                strcpy(TK2.tm_st, T_2[I].tm_arrive);
                strcpy(TK2.tm_arrive, T_2[I].tm_st);
                int min_ticket_remain1[5];
                TRAIN *_now1 = train.find(train_K2);
				TRAIN &now1 = (*_now1);
                for (int p = 0; p < now1.num_ticket_kind; ++p)
                    min_ticket_remain1[p] = 2010;
                for (i = 0; strcmp(now1.station[i].loc, TK2.ky.loc1) != 0; ++i);
                for (j = i; strcmp(now1.station[j].loc, TK2.ky.loc2) != 0; ++j) {
                    for (int p = 0; p < now.num_ticket_kind; ++p) {
                        if (now.tk_remain[dt][j][p] < min_ticket_remain1[p])
                            min_ticket_remain1[p] = now.tk_remain[dt][j][p];
                    }
                }
                TK2.tk_kind_num = now1.num_ticket_kind;
                for (int p = 0; p < now1.num_ticket_kind; ++p) {
                    strcpy(TK2.tk_name[p], now1.ticket_kind[p]);
                    TK2.tk_remain[p] = min_ticket_remain1[p];
                    TK2.price[p] = now1.station[j].price[p] - now1.station[i].price[p];
                }
                cout << TK1.ky.train_id << ' ' << TK1.ky.loc1 << ' ' << "2018-06-" << setw(2) << setfill('0') << dt << ' ' << TK1.tm_st << ' ' << TK1.ky.loc2 << ' ' << "2018-06-" << setw(2) << setfill('0') << dt << ' ' << TK1.tm_arrive;
                for (int p = 0; p < TK1.tk_kind_num; ++p)
                    cout << ' ' << TK1.tk_name[p] << ' ' << TK1.tk_remain[p] << ' ' << fixed << setprecision(6) << TK1.price[p];//要记得用printf;
                cout << endl;
                cout << TK2.ky.train_id << ' ' << TK2.ky.loc1 << ' ' << "2018-06-" << setw(2) << setfill('0') << dt << ' ' << TK2.tm_st << ' ' << TK2.ky.loc2 << ' ' << "2018-06-" << setw(2) << setfill('0') << dt << ' ' << TK2.tm_arrive;
                for (int p = 0; p < TK2.tk_kind_num; ++p)
                    cout << ' ' << TK2.tk_name[p] << ' ' << TK2.tk_remain[p] << ' ' << fixed << setprecision(6) << TK2.price[p];//要记得用printf
                cout << endl;
				delete (_now);
				delete (_now1);
            }

        }
        else if (strcmp(a, "buy_ticket") == 0) {
            user_order_key U;
            int num, dt;
            char tk_kind[21];
            bool order_succeed = true;
            train_id_key K;
            cin >> U.user_id >> num >> U.train_id >> U.loc1 >> U.loc2 >> U.date >> tk_kind;
            if(!user.isvalid(U.user_id))
                cout << 0 << endl;
            else{
                strcpy(K.train_id, U.train_id);
                dt = (U.date[8] - '0') * 10 + (U.date[9] - '0');
                if(!train.check(K))
                    cout << 0 << endl;
                else{
                    TRAIN *_now = train.find(K);
					TRAIN &now = (*_now);
                    int p;
                    for (p = 0; p < now.num_ticket_kind; ++p)
                        if (strcmp(tk_kind, now.ticket_kind[p]) == 0)
                            break;
                    if(p == now.num_ticket_kind)
                        cout << 0 << endl;
                    else{
                        int i;
                        for (i = 0; i != now.num_station - 1 && strcmp(now.station[i].loc, U.loc1) != 0; ++i);
                        int j;
                        tk_order Tk;
                        for (j = i; j != now.num_station - 1 && strcmp(now.station[j].loc, U.loc2) != 0; ++j) {
                            if(now.tk_remain[dt][j][p] < num)
                                order_succeed = false;
                        }
                        if(strcmp(now.station[i].loc, U.loc1) != 0 || strcmp(now.station[j].loc, U.loc2) != 0)
                            order_succeed = false;
                        if (order_succeed) {
                            for (j = i; j != now.num_station - 1 && strcmp(now.station[j].loc, U.loc2) != 0; ++j) {
                                now.tk_remain[dt][j][p] -= num;
                            }
                            train.modify(K, now);
                            Tk.ky = U;
                            U.train_kind = now.train_kind;
                            Tk.num_tk_kind = now.num_ticket_kind;
                            for (int q = 0; q < now.num_ticket_kind; ++q)
                                strcpy(Tk.tk_name[q], now.ticket_kind[q]);
                            if (User.check(U) == 0) {
                                Tk.tk_order_num[p] += num;
                                strcpy(Tk.tm_st, now.station[i].time_start);
                                strcpy(Tk.tm_arrive, now.station[j].time_arrive);
                                for (int q = 0; q < now.num_station; ++q)
                                    Tk.price[q] = now.station[j].price[q] - now.station[i].price[q];
                                User.insert(U, Tk);
                            } else {
                                Tk.tk_order_num[p] += num;
                                for (int q = 0; q < now.num_station; ++q)
                                    Tk.price[q] = now.station[j].price[q] - now.station[i].price[q];
                                User.modify(U, Tk);
                            }
                            cout << 1 << endl;
                        }
                        else cout << 0 << endl;
                    }
					delete (_now);
                }
            }
        }
        else if (strcmp(a, "query_order") == 0) {
            user_order_key K;
            int dt;
            char catalog[11];
            cin >> K.user_id >> K.date >> catalog;
            dt = (K.date[8] - '0') * 10 + (K.date[9] - '0');
            tk_order t[8000];
            int cnt = 0;
            if(!user.isvalid(K.user_id))
                cout << -1 << endl;
            else{
                BplusTree<user_order_key, tk_order,35,30>::iterator it;
                for (int b = 0; catalog[b] != '\0'; ++b) {
                    K.train_kind = catalog[b];
                    it = User.lowerbound(K);
                    while (true) {
                        user_order_key k = it.Record();
                        tk_order tmp = it.Value();
                        if (k.train_kind == K.train_kind && User.isValid(it)) {
                            t[cnt] = tmp;
                            cnt++;
                            it++;
                        } else break;
                    }
                }
                if (cnt == 0)
                    cout << -1 << endl;
                else {
                    cout << cnt << '\n';
                    for (int i = 0; i < cnt; ++i) {
                        cout << t[i].ky.train_id << ' ' << t[i].ky.loc1 << ' ' << "2018-06-" << setw(2) << setfill('0') << dt << ' ' << t[i].tm_st;
                        cout << ' ' << t[i].ky.loc2 << ' ' << "2018-06-" << setw(2) << setfill('0') << dt << ' ' << t[i].tm_arrive;
                        for (int p = 0; p < t[i].num_tk_kind; ++p) {
                            cout << ' ' << t[i].tk_name[p] << ' ' << t[i].tk_order_num[p] << ' '
                                 << fixed << setprecision(6) << t[i].price[p];//要记得用printf
                        }
                        cout << endl;
                    }
                }
            }
        }
        else if (strcmp(a, "refund_ticket") == 0) {
            user_order_key K;
            int dt, num;
            char tk_kind[21];
            cin >> K.user_id >> num >> K.train_id >> K.loc1 >> K.loc2 >> K.date >> tk_kind;
            dt = (K.date[8] - '0') * 10 + (K.date[9] - '0');
            train_id_key train_K;
            strcpy(train_K.train_id, K.train_id);
            if (train.check(train_K) == 0)
                cout << 0 << endl;
            else {
                TRAIN *_now = train.find(train_K);
				TRAIN &now = (*_now);
                K.train_kind = now.train_kind;
                if (!User.check(K))
                    cout << 0 << endl;
                else {
                    tk_order *_Tk = User.find(K);
					tk_order &Tk = (*_Tk);
                    int p;
                    for (p = 0; p < Tk.num_tk_kind; ++p)
                        if (strcmp(tk_kind, Tk.tk_name[p]) == 0)
                            break;
                    if (Tk.tk_order_num[p] < num)
                        cout << 0 << endl;
                    else {
                        Tk.tk_order_num[p] -= num;
                        User.modify(K, Tk);
                        int i;
                        for (i = 0; strcmp(now.station[i].loc, Tk.ky.loc1) != 0; ++i);
                        int j;
                        for (j = i; strcmp(now.station[j].loc, Tk.ky.loc2) != 0; ++j)
                            now.tk_remain[dt][j][p] += num;
                        train.modify(train_K, now);
                        cout << 1 << endl;
                    }
					delete (_Tk);
                }
				delete (_now);
            }
        }
        else if (strcmp(a, "exit") == 0) {
            cout << "BYE" << endl;
//            train.exit();
//            ticket.exit();
//            User.exit();
            return 0;
        }
        else if (strcmp(a, "clean") == 0) {
            user.clear();
            train.clear();
            ticket.clear();
            User.clear();
            cout << 1 << endl;
        }
    }
    return 0;
}
//如果查询操作过多的话建议把余票信息存在票的信息里
