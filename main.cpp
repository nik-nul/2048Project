/* Project of Programme Design Basis ---- 2048 */
/* 231220103 lzc //\\ GitHub: nik_nul */
/* test platform: Debian 12(Bookworm); gcc version: 12.2.0 (Debian 12.2.0-14)*/
/* debug command: g++ -fdiagnostics-color=always -g ${file} -o ${fileDirname}/${fileBasenameNoExtension} */

// 137 & 23 are my lucky numbers and primes, and 20050523 is my birthday and a prime. they're chosen for no specific reason.
// in vector<vector<int> >, i leaves a wierd space intentionally, as i heard some bugs may occur if i don't do so.

/* 2023-11-16 */
/* construct init part, bulid grid and left merge */
/* submission id: 305851e1cc1a1ceaf9edb9ababce781ca19e6b41 */

/* 2023-11-17 19:30*/
/* i learnt oop in sicp today:) decide to reconstruct a grid class to make it more clear and use vector instead of silly int[][] */
/* implement the main game */
/* submission id: db6e2486ee6eb0ba3cff39800c5075a0c5713af6 */
/* submission id: 6b3b06b47df054d34da8585fb19bdfbb7e3767c9 end judge */
/* submission id: 710c644bdbf04f6f6bf17602ad9352ae672bccb1 non-square*/
/* submission id: 39eb69cf7e794d6aabd7f8e20ffee3b4c35392eb colourful-update */

/* 2023-11-18 14:50 */
/* change the rule to fit the description of pdf document, like step count and score will minus the step */
/* submission id: 58ca5cbbccd0b87192b8184cbdf9a98379b73a51*/

/* 2023-11-20 19:00 ~ 2023-11-22 20:20 */
/* finished save and load part using cereal, after fails with boost and mere fstream */
/* reformat the code */
/* revise menu jump logic */

/* 2023-12-07 20:00 ~ 2023-12-08 14:30 */
/* finish coin and delete. finish ranklist */
/* improve structure of the program */

#include <bits/stdc++.h>
#include <termio.h> // for instant input use
#include <dirent.h> // for dir scan use
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>
// #include <boost/serialization/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
using namespace std;
#define cls() system("clear")
#define cl_buf() while((xxx = getchar()) != 0x0a && xxx != 0 && xxx != -1) // clear the input buffer to avoid wierd bugs 
#define q() if(QUIT == 1) return 0
#define self (*this) // this is actually borrow from python, as i've already get used to python style in sicp~ qwq
#define is_exist(s) (access(s.c_str(), F_OK ) != -1 )

bool QUIT = 0, LOG = 0;
int ROW = 4, COLUMN = 4, GOAL = 2048, PROB = 10, wid = 6, xxx;
string LOAD_FLAG = "None";

// int** play_initialize();
// void play_save(int HASH); //
// void play_load(int HASH); //
// void left_merge(int** res, int r = ROW, int c = COLUMN);
// void grid_reverse(int** res, int r = ROW); //
// void display(int** res);
// bool winQ(int** res);
// int cnt_zeros(int** res);

class Grid{
    public:
        int score, step, goal;
        unsigned int HASH = (unsigned)time(nullptr);
        int r, c;
        int coin = 0;
        bool arch = 0;
        vector<vector<int> > res;
    private:
        // friend class boost::serialization::access;
        // template<class Archive> void serialize(Archive & ar, const unsigned int version){
        //     ar &score;
        //     ar &step;
        //     ar &HASH;
        //     ar &res; 
        // }
        // bool rev = 0, tr = 0;
        template<class Archive> friend void serialize(Archive &ar, Grid &grid);
    public:
        Grid();
        ~Grid();
        // Grid(vector<vector<int> > cur, int x, int y);
        void set_zero(int x, int y);
        void gen2(int zeros);
        bool left_merge();
        void reverse();
        bool right_merge();
        void transpose();
        bool down_merge();
        bool up_merge();
        void display();
        bool winQ();
        int cnt_zeros();
        bool deadQ();
        void step_increment();
};

void intro();
void ranklist(); //
void play(string fname); // unsigned int HASH = (unsigned)time(nullptr)
string colour_select(int n);
bool save(Grid grid);
Grid load(string fname);
vector<string> read_list();
void print_vec(vector<string> vec);
bool rank_save(int r);
void intro_l();
set<int, greater<int>> rank_load();

int main(void){
    // enable instant input
    termios tio;
    tcgetattr(STDIN_FILENO, &tio);
    tio.c_lflag &=(~ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    while(1){
        cls();
        intro();
        q();
        cls();
        play(LOAD_FLAG);
        q();
        //TODO
    }
}

void intro(){
    menu:
    cout<<"THIS IS 2048 GAME"<<endl<<" enter TO STRAT DIRECTLY\n :m FOR MODIFIED GAME\n :l TO LOAD\n :q TO QUIT\n :h FOR HELP\n :r FOR RANKLIST"<<endl<<"Please enter:"<<endl;
    err:
    if(getchar() == 0x3A){

        switch(getchar()){
            case 0x71:
                QUIT = 1;
                break;
            case 0x68:
                cls();
                cout<<"Use your arrow keys to move the tiles.\nTiles with the same number merge into one when they touch.\nAdd them up to reach 2048!"<<endl<<"Enter to Back to Menu"<<endl;
                cl_buf(); // wait for input
                cls();
                goto menu;
            case 0x6C:
                intro_l();
                goto err;
            case 0x6D:
                cout<<endl<<"ENTER LENGTH (default = 4):"; cin>>ROW;
                cout<<"ENTER WIDTH: (default = 4):"; cin>>COLUMN;
                cout<<"ENTER GOAL: (default = 2048):"; cin>>GOAL;
                wid = max(int(log10(GOAL)) + 3, 6);
                cout<<"ENTER PROB OF FOUR AT %: (default = 10):"; cin>>PROB;
                cout<<"LOG MODE ON? (default = 0 = false)):"; cin>>LOG;
                break;
            case 0x72:
                ranklist();
                goto menu;
            default:
                cout<<endl<<"INVALID PARAMETER. TRY AGAIN!"<<endl;
                goto err;
        }
        cl_buf();
    }
}

void play(string fname){
    Grid cur;
    if(fname != "None") cur = load(fname);
    int tmpcnt = 23; // chosen arbitrarily. that doesn't matter as long as it's positive.
    bool flag;
    while(1){
        cout<<"YOUR SCORE: "<<(cur.score-cur.step)<<endl;
        cout<<"YOUR STEP: "<<cur.step<<endl;
        cout<<"YOUR COIN: "<<cur.coin<<endl;
        cur.display();
        err:
        cout<<"NEXT MOVE? (:q FOR QUIT; :w FOR SAVE; :d FOR SET ZEROS WITH COINS)"<<endl;
        switch(getchar()){
            case 0x3A:
                switch(getchar()){
                    case 0x71:
                        QUIT = 1;
                        cout<<endl;
                        goto E; // i know i can use flag to jump out the loop, but i think it's uncessary!
                    case 0x77:
                        if(save(cur))
                            cout<<endl<<"SAVED"<<endl;
                        else
                            cout<<endl<<"FAILD"<<endl;
                        goto err;
                    case 0x64:
                        if(cur.coin < 15)
                            cout<<endl<<"Insufficient coins!"<<endl;
                        else{
                            errr:
                            cout<<endl<<"Please input x(ROW) and y(COLUMN) (begin from 1)"<<endl;
                            int x, y;
                            cin>>x>>y;
                            cl_buf();
                            if(y > cur.r || x > cur.c || x < 1 || y < 1){
                                cout<<endl<<"Invalid input, Try again!"<<endl;
                                goto errr;
                            }else
                                cur.set_zero(x-1, y-1);
                        }
                        cur.display();
                        goto err;
                    default:
                        cout<<endl<<"INVALID PARAMETER. TRY AGAIN!"<<endl;
                        goto err;
                        break;
                }
                break;
            case 0x1B:
                getchar();
                switch(getchar()){
                    case 0x43: flag = cur.right_merge(); break;
                    case 0x44: flag = cur.left_merge(); break;
                    case 0x41: flag = cur.up_merge(); break;
                    case 0x42: flag = cur.down_merge(); break;
                }
        }
        if(!flag){
            cout<<endl<<"INVALID MOVE! TRY AGAIN!"<<endl;
            goto err;
        }
        cur.step_increment();
        if(!LOG) cls();
        tmpcnt = cur.cnt_zeros();
        if(tmpcnt == 0) goto L;
        cur.gen2(tmpcnt);
        tmpcnt = cur.cnt_zeros();
        if(tmpcnt == 0 && cur.deadQ()){
            L:
            cout<<endl;
            cur.display();
            cout<<"You Lose~"<<endl<<"Total Score: "<<(cur.score-cur.step)<<endl<<"Enter m to menu";
            ini:
            while(getchar()!=0x6D);
            goto E;
        }
        if(cur.winQ()){
            cout<<endl;
            cur.display();
            cout<<"YOU WIN!!!"<<endl<<"Total Score: "<<(cur.score-cur.step)<<endl<<"Enter m to menu";
            goto ini;
        }
    }
    E:;
}

Grid::Grid(){
    step = 0; r = ROW; c = COLUMN; score = 0; goal = GOAL; LOG = 0;
    res.resize(max(r, c));
    for(int i = 0; i < max(r, c); i++)
        res[i].resize(max(r, c));
    // display(res);
    srand(137*(unsigned)time(nullptr));
    int tmpa, tmpb, tmpc, tmpd;
    do{
        tmpa = rand()%ROW;
        tmpc = rand()%ROW;
        tmpb = rand()%COLUMN;
        tmpd = rand()%COLUMN;
    } while(tmpa == tmpc && tmpb == tmpd);
    // int tmp1 = rand()%(ROW*COLUMN), tmp2 = (tmp1 + 20050523)%(ROW*COLUMN);
    // tmpa = tmp1 % ROW; tmpb = tmp1 % COLUMN; tmpc = tmp2 % ROW; tmpd = tmp2 % COLUMN;
    bool tmpv1 = (rand()%100 < PROB), tmpv2 = (rand()%100 < PROB);
    res[tmpa][tmpb] = tmpv1?4:2;
    res[tmpc][tmpd] = tmpv2?4:2;
}

Grid::~Grid(){
    if(not self.arch){
        rank_save(score - step);
        ROW = 4; COLUMN = 4; GOAL = 2048; PROB = 10; wid = 6;
    }
    cout<<endl<<"Done!"<<endl;
}

void Grid::set_zero(int x, int y){
    res[x][y] = 0;
    coin -= 15;
    cout<<"DELETED!"<<endl;
}

// Grid::Grid(vector<vector<int> > cur, int x, int y){
//     res = cur; r = x; c = y;
// }

vector<string> read_list(){
    vector<string> vec;
    DIR *dp;
	struct dirent *dirp;
    regex suffix_filter(".*\.save", regex::icase);
    dp = opendir(".");
	while((dirp = readdir(dp)) != NULL)
        if(regex_match((*dirp).d_name, suffix_filter))
            vec.push_back((*dirp).d_name);
    return vec;
}

void print_vec(vector<string> vec){
    int i = 0;
    for(vector<string>::iterator it = vec.begin(); it != vec.end(); it++){
        cout<<i<<"\t"<<*it<<endl;
        i++;
    }
}

void print_set(set<int, greater<int>> s){
    int i = 1;
    for(set<int, greater<int>>::iterator it = s.begin(); it != s.end(); it++){
        cout<<i<<"\t"<<*it<<endl;
        i++;
    }
}

// set kth 0 to 2
void Grid::gen2(int zeros){
    int tmp = rand()%zeros, k = 0;
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            if(res[i][j] == 0){
                if(tmp == k){
                    res[i][j] = 2;
                    goto E; // i too know i can use flag to jump out the loop here, but i think it's uncessary!
                }else k++;
            }
        }
    }
    E:;
}

bool Grid::left_merge(){
    bool flag = 0;
    for(int i = 0; i < r; i++){
        for(int k = 0; k < c - 1; k++){
            for(int j = 0; j < c - 1; j++){
                if(res[i][j] == 0 && res[i][j+1] != 0){
                    res[i][j] = res[i][j+1];
                    res[i][j+1] = 0;
                    flag = 1;
                    j = max(0, j - 2);
                }
            }

            for(int j = 0; j < c - 1; j++){
                if(res[i][j] == res[i][j+1] && res[i][j] != 0){
                    score += res[i][j];
                    res[i][j] *= 2;
                    res[i][j+1] = 0;
                    flag = 1;
                    coin++;
                }
            }
        }
    }
    return flag;
}

void Grid::reverse(){
    for(int i = 0; i < r; i++) 
        ::reverse(&res[i][0], &res[i][c-1]+1);
}

bool Grid::right_merge(){
    self.reverse();
    bool tmp = self.left_merge();
    self.reverse();
    return tmp;
}

void Grid::transpose(){
    for(int i = 1; i < max(r, c); i++)
        for(int j = 0; j < i; j++)
            swap(res[i][j], res[j][i]);
    swap(r, c);
}

bool Grid::up_merge(){
    self.transpose();
    bool tmp = self.left_merge();
    self.transpose();
    return tmp;
}

bool Grid::down_merge(){
    self.transpose();
    bool tmp = self.right_merge();
    self.transpose();
    return tmp;
}

void Grid::display(){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++)
            cout<<colour_select(res[i][j])<<setw(int(wid/1.8))<<res[i][j]<<setw(wid)<<"\033[0m";
        cout<<"|"<<endl;
    }
}

bool Grid::winQ(){
    for(int i = 0; i < r; i++)
        for(int j = 0; j < c; j++)
            if(res[i][j] >= GOAL)
                return 1;
    return 0;
}

int Grid::cnt_zeros(){
    int cnt = 0;
    for(int i = 0; i < r; i++)
        for(int j = 0; j < c; j++)
            if(res[i][j] == 0)
                cnt++;
    return cnt;
}

bool Grid::deadQ(){
    for(int i = 0; i < r; i++)
        for(int j = 0; j < c - 1; j++)
            if(res[i][j] == res[i][j+1])
                return 0;

    for(int i = 0; i < r - 1; i++)
        for(int j = 0; j < c; j++)
            if(res[i][j] == res[i+1][j])
                return 0;
    return 1;
}

void Grid::step_increment(){
    step++;
}

template<class Archive> void serialize(Archive &ar, Grid &grid){
    ar(grid.res, grid.HASH, grid.r, grid.c, grid.score, grid.step, grid.goal, grid.coin);
}


bool save(Grid grid){
    // if(access(to_string(res.HASH).c_str(), F_OK ) == -1 ) system("touch "+to_string(res.HASH));
    ofstream ofs(to_string(grid.HASH)+".save", ios::binary);
    if(!ofs)
        return 0;
    // boost::archive::text_oarchive oa(ofs);
    // oa<<res;
    cereal::BinaryOutputArchive oarchive(ofs);
    grid.arch = 1;
    oarchive(grid);
    grid.arch = 0;
    return 1;
}

Grid load(string fname){
    ifstream ifs(fname, ios::binary);
    Grid tmp;
    // boost::archive::text_iarchive ia(ifs);
    // ia>>tmp;
    cereal::BinaryInputArchive archive(ifs);
    archive(tmp);
    return tmp;
}

void ranklist(){
    cls();
    cout<<"ranklist as follow"<<endl;
    print_set(rank_load());
    cout<<"Enter to Back to Menu";
    getchar();
    cls();
}

void intro_l(){
    cls();
    vector<string> lst = read_list();
    int tmp;
    if(lst.empty()){
        cout<<"NO SAVE!"<<endl<<"Enter to Back to Menu";
        getchar();
        cls();
    }else{
        cout<<"saves list as follow:"<<endl;
        print_vec(lst);
        cout<<"Please Choose the Number:  ";
        cin>>tmp;
        LOAD_FLAG = lst[tmp];
    }
}

set<int, greater<int>> rank_load(){
    ifstream ifs("ranklist", ios::binary);
    if(!ifs)
        cout<<"err in open";
    set<int, greater<int>> tmp;
    try{
        cereal::BinaryInputArchive archive(ifs);
        archive(tmp);
    }catch(cereal::Exception& err){
        // cerr<<endl<<err.what()<<endl;
    }
    return tmp;
}

bool rank_save(int r){
    ofstream ofs;
    ofs.open("ranklist", ios::binary | ios::app);
    if(!ofs)
        return 0;
    ofs.close();
    set<int, greater<int>> tmp = rank_load();
    tmp.insert(r);
    ofs.open("ranklist", ios::binary);
    cereal::BinaryOutputArchive oarchive(ofs);
    oarchive(tmp);
    return 1;
}

string colour_select(int n){
    switch(n){
        case 0: return "\033[30m\033[7m ";
        case 2: return "\033[31m\033[7m ";
        case 4: return "\033[32m\033[7m ";
        case 8: return "\033[33m\033[7m ";
        case 16: return "\033[34m\033[7m ";
        case 32: return "\033[35m\033[7m ";
        case 64: return "\033[36m\033[7m ";
        case 128: return "\033[37m\033[7m ";
        case 256: return "\033[100m\033[7m ";
        case 512: return "\033[101m\033[7m ";
        case 1024: return "\033[102m\033[7m";
        case 2048: return "\033[103m\033[7m";
        case 4096: return "\033[104m\033[7m";
        case 8192: return "\033[105m\033[7m";
        case 16384: return "\033[106m\033[7m";
        default: return "\00[2m \00[6m";
    }
}
