#include <unistd.h> //usleep
#include <iostream> //std::cout, std::cin
#include <thread>   //std::thread
#include <limits>   //std::numeric_limits

const  uint  SEC = 1000000;
static short SPEED = 15;
const  short FRAME_RATE = 20;
const  short R_MAX = 20;
const  short C_MAX = 35;
const  short R_INIT = R_MAX/2;
const  short C_INIT = C_MAX/2;

enum direction{ UP, RIGHT, DOWN, LEFT };
const short DIRNUM = 4;

void createApple(short[][C_MAX], short);
void print(const short[][C_MAX], const short*, const short*);
int main(int argc, char *argv[])
{
        if (argc != 1)
                SPEED = std::atoi(argv[1]);
        if (SPEED < 1)
                return 1;

        //declaring length, time, and direction
        short length(5), *timer(new short(0)), *score(new short(0));
        direction *dir(new direction(RIGHT));

        //declaring and initializing board and front ptr
        short board[R_MAX][C_MAX], *front;
        {
                for (short r = 1; r < R_MAX-1; ++r)
                        for (short c = 1; c < C_MAX-1; ++c)
                                board[r][c] = -1;
                for (short r = 0; r < R_MAX; r += R_MAX-1)
                        for (short c = 0; c < C_MAX; ++c)
                                board[r][c] = std::numeric_limits<short>::max();
                for (short c = 0; c < C_MAX; c += C_MAX-1)
                        for (short r = 0; r < R_MAX; ++r)
                                board[r][c] = std::numeric_limits<short>::max();

                board[R_INIT][C_INIT] = *timer + length;

                front = &board[R_INIT][C_INIT];
        }

        //declaring and initializing apple
        {
                createApple(board, *timer);
        }

        //accepting raw input
        {
                system ("/bin/stty raw");
        }

        //declaring lock (allowing one dir change per move)
        bool *lock = new bool(false);


        //controlling snake thread
        std::thread controlling([](direction *dir, bool *lock)
        {
                while(1)
                {                        
                        while(!*lock)
                        {
                                //reading key order
                                char ch;
                                std::cin >> ch;

                                direction tempDir;
                                switch(ch){
                                        case 'w': tempDir = UP;   break;
                                        case 's': tempDir = DOWN; break;
                                        case 'a': tempDir = LEFT; break;
                                        case 'd': tempDir = RIGHT;break;
                                        case 'q': system ("/bin/stty cooked");                                                
                                                exit(0);                                        
                                }

                                //ensuring feasible change
                                if (tempDir != (*dir + 2) % DIRNUM){
                                        *dir = tempDir;
                                        *lock = true;
                                }
                                
                        }
                }
        }, dir, lock);

        //moving snake thread
        std::thread moving([](short *timer, short board[][C_MAX], 
                              short *front, short length, const volatile direction *dir, volatile bool *lock, short *score)
        {
                while(1){
                        //incrementing timer
                        {
                                ++*timer;
                        }
                        
                        //moving and checking
                        {
                                *dir==RIGHT?++front:*dir==LEFT?--front:*dir==UP?front-=C_MAX:front+=C_MAX;
                                *lock = false; //unlocking
                                if (*front >= *timer){
                                        system("/bin/stty cooked");
                                        print(board, score, timer);
                                        score = NULL;
                                        std::cout << "You Lost\n";
                                        exit(0);
                                }
                                else if (*front == 0){
                                        createApple(board, *timer);
                                        *score += SPEED;
                                        ++length;
                                }
                                *front = *timer + length;
                        }
                        
                        //sleeping
                        {
                                usleep(SEC/SPEED);
                        }
                }
        }, timer, board, front, length, dir, lock, score);
        


        //printing board thread
        std::thread printing([](short board[][C_MAX], short *score, short *timer)
        {
                        //clearing & printing
                        while(1)
                        {
                                system ("/bin/stty cooked");
                                print(board, score, timer);
                                usleep(SEC/FRAME_RATE);
                                system ("/bin/stty raw");
                        }
        }, board, score, timer);
        moving.join();
        controlling.join();
        printing.join();
}

void createApple(short board[][C_MAX], short timer)
{
appleGen:
        short r = rand() % (R_MAX-1) + 1;
        short c = rand() % (C_MAX-1) + 1;
        if (board[r][c] > timer)
                goto appleGen;
        board[r][c] = 0;

}

void print(const short board[][C_MAX], const short *score, const short *timer)
{
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                for (short i = 0; i < C_MAX/2; ++i)
                        std::cout << "  ";
                std::cout << "SCORE : " << *score << "!\n";
                for (short r = 0; r < R_MAX; ++r)
                        for (short c = 0; c < C_MAX; ++c)
                                if (!board[r][c])
                                        std::cout << "o ";
                                else
                                        std::cout << " k"[board[r][c] >= *timer]
                                                  << " \n"[c == C_MAX-1];
}

