#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <cassert>
#include <algorithm>

#define MIN -1000000000
#define MAX  1000000000
#define SIZE 8

struct Point {
    int x, y;
    int h;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
    Point(int x, int y, int h) : x(x), y(y),h(h) {}

	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

//Special corner
std :: array <Point, 4> corner = {Point(0, 0), Point(0, 7), Point(7, 0), Point(7, 7)};
std :: array <Point, 4> xq     = {Point(1, 1), Point(1, 6), Point(6, 1), Point(6, 6)};
std :: array <Point, 8> cq     = {Point(0, 1), Point(1, 0), Point(0, 6), Point(1, 7), 
                                  Point(6, 0), Point(7, 1), Point(6 ,7), Point(7, 6)};
//Edge                                  
std :: array <Point, 6> upedge   = {Point(0, 1), Point(0, 2), Point(0, 3), Point(0, 4),Point(0, 5), Point(0, 6)};
std :: array <Point, 6> leftedge = {Point(1, 0), Point(2, 0), Point(3, 0), Point(4, 0),Point(5, 0), Point(6, 0)};
std :: array <Point, 6> dwnedge = {Point(7, 1), Point(7, 2), Point(7, 3), Point(7, 4),Point(7, 5), Point(7, 6)};
std :: array <Point, 6> rghedge  = {Point(1, 7), Point(2, 7), Point(3, 7), Point(4, 7),Point(5, 7), Point(6, 7)};

struct tmp_board{
    std::array<std::array<int, SIZE>, SIZE> board;

    tmp_board(std::array<std::array<int, SIZE>, SIZE> &inboard){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] =  inboard[i][j];
            }
        }
    }
};

int player;
std::array<std::array<int, SIZE>, SIZE> board;
std::array<std::array<int, SIZE>, SIZE> scoreboard = { 70, -3,  7,  4,  4,  7, -3,  70,
                                                      -3, -30,  3, 1, 1, 3, -30, -3,
                                                       6, 3,  5,  3,  3,  5, 3,  6,
                                                       4, 1,  3,  1,  1,  3, 1,  4,
                                                       4, 1,  3,  1,  1,  3, 1,  4,
                                                       6, 3,  5,  3,  3,  5, 3,  6,
                                                      -3, -30, 3, 1, 1, 3, -30, -3,
                                                       70, -3, 7,  4,  4,  7, -3, 70};
std::vector<Point> next_valid_spots;
Point ourstep(0, 0, MIN); 

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back(Point(x, y));
    }
}

void write_output_spot(std::ofstream& fout) {
    fout << ourstep.x <<" "<< ourstep.y << std::endl;
    fout.flush();
}

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    
    void count_disc(){
        for(int i = 0; i < SIZE;i++){
            for(int j = 0;j < SIZE; j++)
            {
                if(board[i][j] == WHITE)
                    disc_count[WHITE]++;
                else if(board[i][j] == BLACK)
                    disc_count[BLACK]++; 
            }
        }
    }
public:
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE>& cur) {
        reset(cur);
    }

    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void reset(std::array<std::array<int, SIZE>, SIZE>& cur) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = cur[i][j];
            }
        }
        cur_player = player;
        disc_count[EMPTY] = 0;
        disc_count[BLACK] = 0;
        disc_count[WHITE] = 0;
        count_disc();
        disc_count[EMPTY] = 8*8-disc_count[WHITE] - disc_count[BLACK];
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }

    void copyboard(OthelloBoard & inboard){
         for(int i = 0; i < SIZE;i++){
            for(int j = 0;j < SIZE; j++)
            {
                board[i][j] = inboard.board[i][j];
            }
        }
        cur_player = inboard.cur_player;
        disc_count[BLACK] = inboard.disc_count[BLACK];
        disc_count[WHITE] = inboard.disc_count[WHITE];
        disc_count[EMPTY] = inboard.disc_count[EMPTY];
        done = inboard.done;
        winner = inboard.winner;
    }

    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }

    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }

    int get_h(int helping, std::ofstream& fout){
        //int h = 0;
        int m = 0; // mobility
        int p = 0; // point
        //int pc = 0;
        //int s = 0;


        //p = disc_count[player] - disc_count[3 - player];

        /*for(int i = 0; i < SIZE; i++)
        {
            for(int j = 0; j < SIZE;j++)
            {
                if(board[i][j] == helping){
                    h += scoreboard[i][j];
                }
                else if(board[i][j] == 3- helping){
                    h -= scoreboard[i][j];
                }
            }
        }*/
        //pc = p - v;

        if(cur_player == helping){
            m = next_valid_spots.size();
        }  
        else{
            m=  -next_valid_spots.size();
        }

        /*int j = 0;
        int sowner = 0;

        if(disc_count[EMPTY] < 50){
            for(int i = 0; i < 4; i++)
                {
                    if (j == 6){
                        j = 0;
                        continue;
                    }
                    Point cur = corner[i];
                    sowner = board[corner[i].x][corner[i].y];

                    for(j = 0; j < 6; j++)
                    {
                        int chk = board[cur.x][cur.y];

                        if(chk == EMPTY || chk != sowner){
                            break;
                        }

                        if(sowner == helping)
                            s++;
                        else 
                            s--;

                        if(i == 0)
                            cur = upedge[j];
                        else if(i == 1)
                            cur = upedge[6 - j];
                        else if(i == 2)
                            cur = dwnedge[j];
                        else 
                            cur = dwnedge[6 - j]; 
                    }
                }
            
            j = 0;
            for(int i:{0, 2, 1, 3})
                {
                    if (j == 6){
                        j = 0;
                        continue;
                    }
                    Point cur = corner[i];
                    sowner = board[corner[i].x][corner[i].y];

                    for(j = 0; j < 6; j++)
                    {
                        int chk = board[cur.x][cur.y];

                        if(chk == EMPTY || chk != sowner){
                            break;
                        }
                        
                        if(sowner == helping)
                            s++;
                        else 
                            s--;

                        if(i == 0)
                            cur = leftedge[j];
                        else if(i == 1)
                            cur = leftedge[6 - j];
                        else if(i == 2)
                            cur = rghedge[j];
                        else 
                            cur = rghedge[6 - j]; 
                    }
                }
        }*/

        for(auto pt: corner)
        {
            if(board[pt.x][pt.y] == helping)
                p += 2; 
        }
        
        p += disc_count[helping] - disc_count[3 - helping];
        /*if(disc_count[EMPTY] > 50)
            return h * 1 + m * 10 + s * 10 + p * -1;
        else if (disc_count[EMPTY] > 20)
            return h * 1 + m * 10 + s * 10;
        else 
            return h * 1 + m * 10 + s * 10 + p * 2;*/
        //fout << p <<"\n" ;
        //fout << "p:" << p << "\n"; 
        return player * p + (p / 10)  * m * player;
    }

};

int minmax(OthelloBoard &inboard, int depth,int& A, int& B,int cplayer,int helping,std::ofstream& fout){
    
    // fout <<"player :" << cplayer <<"\n";
    // fout <<"depth :" << depth << "\n";
    // fout <<inboard.disc_count[1] << " "<< inboard.disc_count[2] << "\n";

    // for(int i = 0; i < SIZE; i++){
    //     for(int j = 0; j < SIZE; j++)
    //         fout << inboard.board[i][j];
    //     fout << "\n";
    // }

    if(depth == 6)
    {   
        int re = inboard.get_h(helping, fout);
        //fout << re<<"\n";
        return re;
    }

    OthelloBoard malboard(inboard.board);
    malboard.copyboard(inboard);
    if(cplayer == helping){
        //fout << "helping\n";
        int value = MIN;
        for(auto p: inboard.next_valid_spots){
            malboard.put_disc(p);
            int hcom = minmax(malboard, depth+1, A , B,malboard.cur_player, helping,fout);
            value = std::max(value, hcom);
            A = std :: max(A , value);

            malboard.copyboard(inboard);
            if(A >= B){
                //fout <<"cut \n---------------------------------------------------------------------\n";
                break;
            }
        }
        return value;        
    }
    else {    
        //fout <<"nothelping\n"; 
        int value = MAX;
        for(auto p: inboard.next_valid_spots){
            
            for(auto pt : corner)
            {
                if(inboard.is_spot_valid(pt)){
                    B = MIN + 100;
                    //fout << "oops\n=============================================================";
                    return MIN + 100; 
                }  
            }

            malboard.put_disc(p);
            int hcom = minmax(malboard, depth+1, A , B,malboard.cur_player,helping ,fout);
            value = std::min(value,hcom);
            malboard.copyboard(inboard);
            B = std :: min(B, value);
            if(B <= A){
                //fout << "cut \n -------------------------------------------------------------------\n";
                break;
            }
        }
        return value;        
    }         
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    
    OthelloBoard currentboard(board);
    OthelloBoard inputboard(board);
    inputboard.copyboard(currentboard);

    //fout << player << "\n";
    ourstep = Point(next_valid_spots[0].x, next_valid_spots[0].y, MIN);

    for(auto p: next_valid_spots){
        inputboard.put_disc(p);
        int A = MIN;
        int B = MAX;
        int her = minmax(inputboard, 0, A , B, inputboard.cur_player, player,fout);
        // fout << "choosen :" <<her << "\n ////////////////////////////////////////////////////////";
         if(ourstep.h < her){
        //     fout << "ourh:"<< ourstep.h << " <-" << her <<" \n";
             ourstep = Point(p.x, p.y, her);
         }
        inputboard.copyboard(currentboard);
        write_output_spot(fout);
    }
    fin.close();
    fout.close();
    return 0;
}