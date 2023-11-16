/* Project of Programme Design Basis ---- 2048 */
/* 231220103 lzc //\\  */

/* 2023-11-16 */
/* construct init part, bulid grid and left merge */
#include <bits/stdc++.h>
#include <termio.h> // for instant input use
using namespace std;

#define cls() system("clear")
#define cl_buf() while((xxx = getchar()) != 0x0a && xxx != 0 && xxx != -1) // clear the input buffer to avoid wierd bugs 
#define q() if(QUIT == 1) return 0


bool QUIT = 0, LOG = 0;
int ROW = 4, COLUMN = 4, GOAL = 2048, PROB = 10, SCORE = 0, wid = 6, xxx;

void initialize();
void ranklist(); //
void play(unsigned int HASH = (unsigned)time(nullptr));
int** play_initialize();
void play_save(int HASH); //
void play_load(int HASH); //
void left_merge(int** res, int r = ROW, int c = COLUMN);
void grid_reverse(int** res, int r = ROW); //
void display(int** res);
bool winQ(int** res);
int cnt_zeros(int** res);

int main(void){
    
    // enable instant input
    termios tio;
    tcgetattr(STDIN_FILENO, &tio);
    tio.c_lflag &=(~ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    // reference: https://www.zhihu.com/question/598876426


    while(1){
        cls();
        initialize();
        q();
        cls();
        play();
        q();
        //TODO
    }
}

void initialize(){
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

void play(unsigned int HASH){
    int** res = play_initialize();
    while(! (winQ(res) || (cnt_zeros(res) == 0))){
        cout<<"YOUR SCORE: "<<SCORE<<endl;
        display(res);
        err:
        cout<<"NEXT MOVE?(:wq FOR SAVE AND QUIT; :q FOR QUIT; :w FOR SAVE)"<<endl;
        switch(getchar()){
            case 0x3A:
                switch(getchar()){
                    case 0x71: QUIT = 1; cout<<endl; goto E;
                    case 0x77: //TODO: wq & w
                    default:
                        cout<<endl<<"INVALID PARAMETER. TRY AGAIN!"<<endl;
                        goto err;
                        break;
                }
                break;
            default:
                // test only
                left_merge(res);
        }
        if(!LOG) cls();
    }
    E:;
}

int** play_initialize(){
    int** res = new int*[ROW];
    for(int i = 0; i < COLUMN; i++){
        res[i] = new int[COLUMN];
        memset(res[i], 0, COLUMN*sizeof(int));
    }
    // display(res);
    srand(137*(unsigned)time(nullptr));
    int tmpa, tmpb, tmpc, tmpd; do{tmpa = rand()%ROW, tmpc = rand()%ROW, tmpb = rand()%COLUMN, tmpd = rand()%COLUMN;} while(tmpa == tmpc && tmpb == tmpd);
    bool tmpv1 = (rand()%100 < PROB), tmpv2 = (rand()%100 < PROB);
    res[tmpa][tmpb] = tmpv1?4:2; res[tmpc][tmpd] = tmpv2?4:2;
    return res;
}

void play_save(int HASH){
    //TODO
}

void play_load(int HASH){
    //TODO
}

void left_merge(int** res, int r, int c){
    for(int i = 0; i < r; i++){
        for(int k = 0; k < c - 1; k++){
            for(int j = 0; j < c - 1; j++){
                if(res[i][j] == 0 && res[i][j+1] != 0){
                    res[i][j] = res[i][j+1]; res[i][j+1] = 0;
                    j = max(0, j - 2);
                }
            }
            for(int j = 0; j < c - 1; j++){
                if(res[i][j] == res[i][j+1] && res[i][j] != 0){
                    SCORE += res[i][j]; res[i][j] *= 2; res[i][j+1] = 0;
                }
            }
        }
    }
}

void grid_reverse(int** res, int r){
    //TODO
}

void display(int** res){
    bool flag = 0;
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COLUMN; j++) cout<<setw(wid)<<res[i][j];
        cout<<endl<<endl;
    }
}

bool winQ(int** res){
    for(int i = 0; i < ROW; i++) for(int j = 0; j < COLUMN; j++) if(res[i][j] >= GOAL) return 1;
    return 0;
}

int cnt_zeros(int** res){
    int cnt = 0;
    for(int i = 0; i < ROW; i++) for(int j = 0; j < COLUMN; j++) if(res[i][j] == 0) cnt++;
    return cnt;
}

void ranklist(){
    //TODO
}
