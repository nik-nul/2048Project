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
#include <bits/stdc++.h>
#include <termio.h> // for instant input use
using namespace std;

#define cls() system("clear")
#define cl_buf() while((xxx = getchar()) != 0x0a && xxx != 0 && xxx != -1) // clear the input buffer to avoid wierd bugs 
#define q() if(QUIT == 1) return 0
#define self (*this) // this is actually borrow from python, as i've already get used to python style in sicp~ qwq
#define eswap(a, b) a^=b^=a^=b // c++ has akready got a swap func~


bool QUIT = 0, LOG = 1;
int ROW = 4, COLUMN = 4, GOAL = 2048, PROB = 10, SCORE = 0, wid = 6, xxx;

void intro();
void ranklist(); //
void play(); // unsigned int HASH = (unsigned)time(nullptr)
// int** play_initialize();
// void play_save(int HASH); //
// void play_load(int HASH); //
// void left_merge(int** res, int r = ROW, int c = COLUMN);
// void grid_reverse(int** res, int r = ROW); //
// void display(int** res);
// bool winQ(int** res);
// int cnt_zeros(int** res);

class Grid{
    private:
    vector<vector<int> > res;
    int r = ROW; int c = COLUMN;
    // bool rev = 0, tr = 0;
    unsigned int HASH = (unsigned)time(nullptr);
    public:
        Grid();
        Grid(vector<vector<int> > cur);
        void save();
        void load();
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
        void reset();
};

int main(void){
    
    // enable instant input
    termios tio;
    tcgetattr(STDIN_FILENO, &tio);
    tio.c_lflag &=(~ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    // reference: https://www.zhihu.com/question/598876426

    while(1){
        cls();
        intro();
        q();
        cls();
        play();
        q();
        //TODO
    }
}

void intro(){
    cout<<"THIS IS 2048 GAME"<<endl<<" enter TO STRAT DIRECTLY\n :m FOR MODIFIED GAME\n :q TO QUIT\n :h FOR HELP\n :r FOR RANKLIST"<<endl<<"Please enter:"<<endl;
    err:
    SCORE = 0;
    if(getchar() == 0x3A){
        switch(getchar()){
            case 0x71: QUIT = 1; break;
            case 0x68: cout<<endl<<"help here"<<endl<<"Please enter:"<<endl; cl_buf();  goto err; break;
            case 0x6D:
                cout<<endl<<"ENTER LENGTH (default = 4):"; cin>>ROW;
                cout<<"ENTER WIDTH: (default = 4):"; cin>>COLUMN;
                cout<<"ENTER GOAL: (default = 2048):"; cin>>GOAL;
                wid = max(int(log10(GOAL)) + 3, 6);
                cout<<"ENTER PROB OF FOUR AT %: (default = 10):"; cin>>PROB;
                cout<<"LOG MODE ON? (default = 0 = false)):"; cin>>LOG;
                break;
            case 0x72: ranklist(); goto err; break;
            default:
                cout<<endl<<"INVALID PARAMETER. TRY AGAIN!"<<endl;
                goto err;
                break;
        }
        cl_buf();
    }
}

void play(){
    Grid cur;
    int tmpcnt = 23; // chosen arbitrarily. that doesn't matter
    bool flag;
    while(!cur.winQ()){
        cout<<endl<<"YOUR SCORE: "<<SCORE<<endl;
        cur.display();
        err:
        cout<<"NEXT MOVE?(:wq FOR SAVE AND QUIT; :q FOR QUIT; :w FOR SAVE)"<<endl;
        switch(getchar()){
            case 0x3A:
                switch(getchar()){
                    case 0x71: QUIT = 1; cout<<endl; goto E; // i know i can use flag to jump out the loop, but i think it's uncessary!
                    case 0x77: //TODO: wq & w
                    default:
                        cout<<endl<<"INVALID PARAMETER. TRY AGAIN!"<<endl;
                        goto err;
                        break;
                }
                break;
            case 0x1B:
                // test only
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
        if(!LOG) cls();
        tmpcnt = cur.cnt_zeros();
        if(tmpcnt == 0){
            if(cur.deadQ()){
                cout<<"You Lose~"<<endl<<"Total Score: "<<SCORE<<endl<<"Enter m to menu";
                ROW = 4; COLUMN = 4; GOAL = 2048; PROB = 10; SCORE = 0; wid = 6;
                cur.reset();
                while(getchar()!=0x6D);
                break;
            }
        }
        cur.gen2(tmpcnt);
    }
    E:;
}

Grid::Grid(){
    res.resize(r);
    for(int i = 0; i < ROW; i++) res[i].resize(COLUMN);
    // display(res);
    srand(137*(unsigned)time(nullptr));
    int tmpa, tmpb, tmpc, tmpd;
    do{tmpa = rand()%ROW, tmpc = rand()%ROW, tmpb = rand()%COLUMN, tmpd = rand()%COLUMN;} while(tmpa == tmpc && tmpb == tmpd);
    // int tmp1 = rand()%(ROW*COLUMN), tmp2 = (tmp1 + 20050523)%(ROW*COLUMN);
    // tmpa = tmp1 % ROW; tmpb = tmp1 % COLUMN; tmpc = tmp2 % ROW; tmpd = tmp2 % COLUMN;
    bool tmpv1 = (rand()%100 < PROB), tmpv2 = (rand()%100 < PROB);
    res[tmpa][tmpb] = tmpv1?4:2; res[tmpc][tmpd] = tmpv2?4:2;
}

Grid::Grid(vector<vector<int> > cur){
    res = cur;
}

void Grid::save(){
    //TODO
}

void Grid::load(){
    //TODO
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
                    res[i][j] = res[i][j+1]; res[i][j+1] = 0; flag = 1;
                    j = max(0, j - 2);
                }
            }
            for(int j = 0; j < c - 1; j++){
                if(res[i][j] == res[i][j+1] && res[i][j] != 0){
                    SCORE += res[i][j]; res[i][j] *= 2; res[i][j+1] = 0; flag = 1;
                }
            }
        }
    }
    return flag;
}

void Grid::reverse(){
    for(int i = 0; i < r; i++) ::reverse(res[i].begin(), res[i].end());
}

bool Grid::right_merge(){
    self.reverse(); bool tmp = self.left_merge(); self.reverse();
    return tmp;
}

void Grid::transpose(){
    for(int i = 1; i < r; i++) for(int j = 0; j < i; j++) eswap(res[i][j], res[j][i]);
}

bool Grid::up_merge(){
    self.transpose(); bool tmp = self.left_merge(); self.transpose();
    return tmp;
}

bool Grid::down_merge(){
    self.transpose(); bool tmp = self.right_merge(); self.transpose();
    return tmp;
}

void Grid::display(){
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COLUMN; j++) cout<<setw(wid)<<res[i][j];
        cout<<endl<<endl;
    }
}

bool Grid::winQ(){
    for(int i = 0; i < ROW; i++) for(int j = 0; j < COLUMN; j++) if(res[i][j] >= GOAL) return 1;
    return 0;
}

int Grid::cnt_zeros(){
    int cnt = 0;
    for(int i = 0; i < ROW; i++) for(int j = 0; j < COLUMN; j++) if(res[i][j] == 0) cnt++;
    return cnt;
}

bool Grid::deadQ(){
    for(int i = 0; i < ROW; i++) for(int j = 0; j < COLUMN - 1; j++) if(res[i][j] == res[i][j+1]) return 0;
    for(int i = 0; i < ROW - 1; i++) for(int j = 0; j < COLUMN; j++) if(res[i][j] == res[i+1][j]) return 0;
    return 1;
}

void Grid::reset(){
    res.clear();
}

void ranklist(){
    //TODO
}
