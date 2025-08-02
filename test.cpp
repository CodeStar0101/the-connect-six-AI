//
//  main.cpp
//  cpp test
//
//  Created by 张凯玄 on 2024/3/26.
//
#include <iostream>
#include <vector>
#include<algorithm>
#include <climits>
#include <chrono>
using namespace std;

#define GRIDSIZE 15
#define grid_blank 0
#define grid_black 1
#define grid_white (-1)

int TopBoard = GRIDSIZE - 1;
int BottomBoard = 0;
int LeftBoard = GRIDSIZE - 1;
int RightBoard = 0;

const int SIX = 1000000;
const int HUO_FIVE = 10000;
const int MIAN_FIVE = 8000;
const int HUO_FOUR = 9000;
const int MIAN_FOUR = 5500;
const int HUO_THREE = 2400;
const int MENGLONG_THREE = 2100;
const int MIAN_THREE = 1300;
const int HUO_TWO = 600;
const int MIAN_TWO = 20;

const int MAXVAL = INT_MAX;//无穷大
const int OHUO_FIVE = 100000;
const int OMIAN_FIVE = 80000;
const int OHUO_FOUR = 9000;
const int OMIAN_FOUR = 5500;
const int OHUO_THREE = 2400;
const int OMENGLONG_THREE = 2100;
const int OMIAN_THREE = 1300;
const int OHUO_TWO = 600;
const int OMIAN_TWO = 20;
const int ddl = 750;
int flag = 0;

void UpdateBoardBoundaries(int x, int y) {
    TopBoard = min(TopBoard, y);
    BottomBoard = max(BottomBoard, y);
    LeftBoard = min(LeftBoard, x);
    RightBoard = max(RightBoard, x);
}

void UpdateBoardBoundariesForStep(int x0, int y0, int x1, int y1) {
    // 检测第一步是否为边界
    UpdateBoardBoundaries(x0, y0);
    // 检测第二步是否为边界
    if (x1 >= 0) {
        UpdateBoardBoundaries(x1, y1);
    }
}

void ExtendBoardBoundaries() {
    // 向上扩展
    if (TopBoard - 3 >= 0)
        TopBoard -= 3;
    else if (TopBoard - 2 >= 0) {
        TopBoard = TopBoard - 2;
    }
    else if (TopBoard - 1 >= 0) {
        TopBoard = TopBoard - 1;
    }
    // 向下扩展
    if (BottomBoard + 3 < GRIDSIZE)
        BottomBoard += 3;
    else if (BottomBoard + 2 < GRIDSIZE)
        BottomBoard += 2;
    else if (BottomBoard + 1 < GRIDSIZE) {
        BottomBoard = BottomBoard + 1;
    }
    // 向左扩展
    if (LeftBoard - 3 >= 0) {
        LeftBoard -= 3;
    }
    else if (LeftBoard - 2 >= 0) {
        LeftBoard -= 2;
    }
    else if (LeftBoard - 1 >= 0) {
        LeftBoard = LeftBoard - 1;
    }
    // 向右扩展
    if (RightBoard + 3 < GRIDSIZE) {
        RightBoard += 3;
    }
    else if (RightBoard + 2 < GRIDSIZE) {
        RightBoard += 2;
    }
    else if (RightBoard + 1 < GRIDSIZE) {
        RightBoard = RightBoard + 1;
    }
}

int MyColor; // 本方所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE + 1][GRIDSIZE + 1] = { 0 }; // 先x后y，记录棋盘状态

class Location {
public:
    int x;
    int y;

    Location() : x(0), y(0) {} // 默认构造函数
};

class LocationWithValue : public Location {
public:
    int value;

    LocationWithValue() : value(0) {} // 默认构造函数

    LocationWithValue& operator=(const Location& loc) {
        this->x = loc.x;
        this->y = loc.y;
        return *this;
    }
};

int EvaluateROAD(Location location, int color, int dx, int dy);
vector<LocationWithValue> PossibleLocation;
Location BestLocation[2];

// 判断是否在棋盘内
inline bool inMap(int x, int y) {
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return false;
    return true;
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int grid_color, bool check_only) {
    if (x1 == -1 || y1 == -1) {
        if (!inMap(x0, y0) || gridInfo[x0][y0] != grid_blank)
            return false;
        if (!check_only) {
            gridInfo[x0][y0] = grid_color;
        }
        return true;
    }
    else {
        if ((!inMap(x0, y0)) || (!inMap(x1, y1)))
            return false;
        if (gridInfo[x0][y0] != grid_blank || gridInfo[x1][y1] != grid_blank)
            return false;
        if (!check_only) {
            gridInfo[x0][y0] = grid_color;
            gridInfo[x1][y1] = grid_color;
        }
        return true;
    }
}


//分情况的getValue
/**
 * 根据棋型，计算该点下一个棋子的价值
 * @param chessCount1    该空位置下一个棋子后同种颜色棋子连续的个数
 * @param spaceCount1  连续棋子一端的连续空位数
 * @param chessCount2  如果spaceCount1为1，继续连续同种颜色棋子的个数
 * @param spaceCount2  继chessCount2之后，连续空位数
 * @param spaceCount3  连续棋子另一端的连续空位数
 * @param chessCount3  如果spaceCount3为1，继续连续同种颜色棋子的个数
 * @param spaceCount4  继chessCount3之后，连续空位数
 * @param color   棋子的颜色 1：黑子，2：白子
 * @return  该点放color棋子给color方带来的价值
 */
int getValue(int chessCount1, int chessCount2, int chessCount3, int spaceCount1, int spaceCount2, int spaceCount3, int spaceCount4, int color)
{
    int value = 0;
    // 将六子棋棋型分为连六、活五、眠五、活四、眠四、活三、朦胧三、眠三、活二、眠二
    switch (chessCount1)
    {
        case 6: // 如果已经可以连成6子，则赢棋
            value = SIX;
            return value;
        case 5:
            if ((spaceCount1 > 0) && (spaceCount3 > 0))
            { // 活五
                if (color == MyColor)
                    value = HUO_FIVE;
                else
                    value = OHUO_FIVE;
            }
            else if (((spaceCount1 == 0) && (spaceCount3 > 0)) || ((spaceCount1 > 0) && (spaceCount3 == 0)))
            { // 眠五
                if (color == MyColor)
                    value = MIAN_FIVE;
                else
                    value = OMIAN_FIVE;
            }
            break;

        case 4:
            if ((spaceCount1 > 1) && (spaceCount3 > 1))
            { // 活四
                if (color == MyColor)
                    value = HUO_FOUR;
                else
                    value = OHUO_FOUR;
            }
            else if (((spaceCount1 > 1) && (spaceCount3 == 0)) || ((spaceCount1 == 0) && (spaceCount3 > 1)))
            { // 眠四
                if (color == MyColor)
                    value = MIAN_FOUR;
                else
                    value = OMIAN_FOUR;
            }
            break;
        case 3:
            if ((spaceCount1 > 2) && (spaceCount3 > 2))
            { // OOOAAAOOO
                //活三
                if (color == MyColor)
                    value = HUO_THREE;
                else
                    value = OHUO_THREE;

            }
            else if (((spaceCount1 == 0) && (spaceCount3 > 3)) || ((spaceCount3 > 3) && (chessCount3 == 0)))
            { // AAAOOO
                if (color == MyColor)
                    value = MIAN_THREE;
                else
                    value = OMIAN_THREE;
            }
            break;
        case 2:
            if ((spaceCount1 > 3) && (spaceCount3 > 3))
            { // 活二
                if (color == MyColor)
                    value = HUO_TWO;
                else
                    value = OHUO_TWO;
            }
            else if (((spaceCount1 > 3) && (spaceCount3 == 0)) || ((spaceCount1 == 0) && (spaceCount3 > 3)))
            { // 眠二
                if (color == MyColor)
                    value = MIAN_TWO;
                else
                    value = OMIAN_TWO;

            }
            else if (((spaceCount1 == 1) && (chessCount2 == 1) && (spaceCount2 == 2) && (spaceCount3 == 1)) || ((spaceCount1 == 1) && (chessCount3 == 1) && (spaceCount3 == 1) && (spaceCount4 == 2)))
            { // BOOAOAAOB
                if (color == MyColor)
                    value = MENGLONG_THREE;
                else
                    value = OMENGLONG_THREE;
            }
            break;
        case 1:
            if (((spaceCount1 == 2) && (spaceCount3 == 1) && (chessCount3 == 2) && (spaceCount4 == 1)) || ((spaceCount1 == 1) && (spaceCount2 == 1) && (chessCount2 == 2) && (spaceCount3 == 2)))
            { // BOOAOAAOB
                if (color == MyColor)
                    value = MENGLONG_THREE;
                else
                    value = OMENGLONG_THREE;
            }
            break;
        default:
            value = 0;
            break;
    }
    return value;
}

//enum Direction {HORIZONTAL, VERTICAL, DIAGONAL_NE, DIAGONAL_SE};

int EvaluateValue(int MyNum, int OpNum, int value[], int danger[]) {
    if (MyNum != 0 && OpNum == 0) {
        return value[MyNum - 1];
    }
    if (MyNum == 0 && OpNum != 0) {
        return danger[OpNum - 1];
    }
    return 0;
}

int EvaluateDirection(int color, vector<int>& directionValues, int value[], int danger[]) {
    while (directionValues.size() < 6) {
        directionValues.push_back(2);
    }
    int directionVal = 0;
    for (size_t i = 0; i < directionValues.size() - 5; i++) {
        int MyNum = 0, OpNum = 0;
        for (size_t j = i; j < i + 6; j++) {
            if (directionValues[j] == color) MyNum++;
            else if (directionValues[j] == -color) OpNum++;
        }
        directionVal += EvaluateValue(MyNum, OpNum, value, danger);
    }
    return directionVal;
}

/*
 通用的评估函数，评估单个方向上的值，合并了先前四个方向的pre评估
 while 循环确保 directionValues 的长度至少为 6。如果小于 6，会在向量末尾添加值为 2 的元素，用来填充空缺。
 通过 for 循环迭代 directionValues 中的每个窗口，每次取 6 个元素进行评估。
 在这个窗口内，统计当前玩家（color）和对手的棋子数量
 总体来说，这个函数是在评估一个给定方向上的局势，通过计算每个窗口内当前玩家和对手的棋子数量，并结合预定义的评估规则，来得出这个方向上的整体分值。
 */

int W_E(int color, Location location, int leftt, int rightt, int gridInfo[][GRIDSIZE + 1], int value[], int danger[]) {
    vector<int> WE;
    for (int i = leftt; i <= rightt; i++) {
        if (abs(location.x - i) < 6) {
            WE.push_back(gridInfo[i][location.y]);
        }
    }
    int WEval = EvaluateDirection(color, WE, value, danger);
    return WEval;
}

int N_S(int color, Location location, int top, int bottom, int gridInfo[][GRIDSIZE + 1], int value[], int danger[]) {
    vector<int> NS;
    for (int i = top; i <= bottom; i++) {
        if (abs(location.y - i) < 6) {
            NS.push_back(gridInfo[location.x][i]);
        }
    }
    int NSval = EvaluateDirection(color, NS, value, danger);
    return NSval;
}

int WS_EN(int color, Location location, int top, int bottom, int leftt, int rightt, int gridInfo[][GRIDSIZE + 1], int value[], int danger[]) {
    vector<int> WSEN;
    int left_down = max(bottom - location.y, location.x - leftt);
    int right_up = max(location.y - top, rightt - location.x);
    for (int i = -left_down; i <= right_up; i++) {
        if (abs(i) < 6) {
            int x = location.x + i;
            int y = location.y - i;
            if (y >= top && y <= bottom && x >= leftt && x <= rightt) {
                WSEN.push_back(gridInfo[x][y]);
            }
        }
    }
    int WSENval = EvaluateDirection(color, WSEN, value, danger);
    return WSENval;
}

int WN_ES(int color, Location location, int top, int bottom, int leftt, int rightt, int gridInfo[][GRIDSIZE + 1], int value[], int danger[]) {
    vector<int> WNES;
    int left_up = max(location.y - top, location.x - leftt);
    int right_down = max(bottom - location.y, rightt - location.x);
    for (int i = -left_up; i <= right_down; i++) {
        if (abs(i) < 6) {
            int x = location.x + i;
            int y = location.y + i;
            if (y >= top && y <= bottom && x >= leftt && x <= rightt) {
                WNES.push_back(gridInfo[x][y]);
            }
        }
    }
    int WNESval = EvaluateDirection(color, WNES, value, danger);
    return WNESval;
}

int First_Eva_BasedNUM(Location location, int color) {
    int value[6] = { 1, 4000,6000,10000,20,1000000 };
    int danger[6] = { 1, 2000, 3000, 600000, 600000, 1000000 };
    int NSval = N_S(color, location, TopBoard, BottomBoard, gridInfo, value, danger);
    int WEval = W_E(color, location, LeftBoard, RightBoard, gridInfo, value, danger);
    int WNESval = WN_ES(color, location, TopBoard, BottomBoard, LeftBoard, RightBoard, gridInfo, value, danger);
    int WSENval = WS_EN(color, location, TopBoard, BottomBoard, LeftBoard, RightBoard, gridInfo, value, danger);
    return NSval + WEval + WNESval + WSENval;
}

bool Compare(const LocationWithValue& a, const LocationWithValue& b) {
    return a.value > b.value;
}

void Chess(int color) {
    for (int y = TopBoard; y <= BottomBoard; y++) {
        for (int x = LeftBoard; x <= RightBoard; x++) {
            if (gridInfo[x][y] == grid_blank) {
                Location location{};
                location.x = x;
                location.y = y;
                int value = First_Eva_BasedNUM(location, color);
                LocationWithValue locationVal{};
                locationVal.x = location.x;
                locationVal.y = location.y;

                locationVal.value = value;
                PossibleLocation.push_back(locationVal);
            }
        }
    }
    sort(PossibleLocation.begin(), PossibleLocation.end(), Compare);
}


int Viewline(vector<int>& Road, int color, Location location) {
    int TotalValue = 0;
    int HUO2LINE = 20000;
    int HUO3LINE = 100000;
    int HUO4LINE = 1000000;
    int HUO5LINE = 0;
    int HUO6LINE = 100000000;
    int MyValue = 0;
    int OPHUO1LINE = 100;
    int OPHUO2LINE = 1000;
    int OPHUO3LINE = 50000;
    int OPHUO4LINE = 10000000;
    int OPHUO5LINE = 10000000;
    int OpDanger = 0;
    if ((color == -MyColor && flag == 0)) {
        HUO2LINE = 1000;
        HUO3LINE = 50000;
        HUO4LINE = 100000;
        HUO5LINE = 0;
        HUO6LINE = 0;
        OPHUO1LINE = 1;
        OPHUO2LINE = 1;
        OPHUO4LINE = 1;
        OPHUO3LINE = 1;
        OPHUO5LINE = 1;
    }
    if (color == -MyColor && flag == 1) {
        HUO2LINE = 10000;
        HUO3LINE = 60000;
        HUO4LINE = 150000;
        HUO5LINE = 0;
        HUO6LINE = 0;
        OPHUO1LINE = 10;
        OPHUO2LINE = 500;
        OPHUO4LINE = 0;
        OPHUO3LINE = 25000;
        OPHUO5LINE = 0;
    }
    if (color == MyColor && flag == 2) {
        HUO2LINE = 200;
        HUO3LINE = 6000;
        HUO4LINE = 15000;
        HUO5LINE = 0;
        HUO6LINE = 0;
        OPHUO1LINE = 1;
        OPHUO2LINE = 1;
        OPHUO4LINE = 1;
        OPHUO3LINE = 1;
        OPHUO5LINE = 1;
    }
    int num = static_cast<int>(Road.size());
    while (num < 6) {
        Road.push_back(2);
        num++;
    }
    for (int i = 0; i <= num - 6; i++) {
        int MyNUM = 0;
        int OpNUM = 0;
        for (int j = i; j < i + 6; j++) {
            if (Road[j] == color) {
                MyNUM++;
            }
            else if (Road[j] == -color) {
                OpNUM++;
            }
        }
        if (OpNUM == 0) {
            if (MyNUM > 1) {
                MyValue += HUO2LINE * (MyNUM == 2) + HUO3LINE * (MyNUM == 3) +
                           HUO4LINE * (MyNUM == 4) + HUO5LINE * (MyNUM == 5) +
                           HUO6LINE * (MyNUM == 6);
            }
        }
        else {
            if (MyNUM == 1) {
                OpDanger += OPHUO1LINE * (OpNUM == 1) +
                            OPHUO2LINE * (OpNUM == 2) +
                            OPHUO3LINE * (OpNUM == 3) +
                            OPHUO4LINE * (OpNUM == 4) +
                            OPHUO5LINE * (OpNUM == 5);
            }
        }
    }
    TotalValue += MyValue + OpDanger;

    
    for (int dir = 1; dir <= GRIDSIZE - 15; dir++) {
        int k = 0, m;
        int col = location.x;
        int row = location.y;
        int chessCount1 = 1;
        int chessCount2 = 0;
        int chessCount3 = 0;
        int spaceCount1 = 0;
        int spaceCount2 = 0;
        int spaceCount3 = 0;
        int spaceCount4 = 0;

        switch (dir) {
            case 1: // 水平方向

                for (int k = col + 1; k <= GRIDSIZE; k++) {
                    if (gridInfo[k][row] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                while ((k <= GRIDSIZE) && (gridInfo[k][row] == 0)) {
                    spaceCount1++;
                    k++;
                }
                if (spaceCount1 == 1) {
                    while ((k <= GRIDSIZE) && (gridInfo[k][row] == color)) {
                        chessCount2++;
                        k++;
                    }
                    while ((k <= GRIDSIZE) && (gridInfo[k][row] == 0)) {
                        spaceCount2++;
                        k++;
                    }
                }
                for (int k = col - 1; k >= 0; k--) {
                    if (gridInfo[k][row] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                while (k >= 0 && (gridInfo[k][row] == 0)) {
                    spaceCount3++;
                    k--;
                }
                if (spaceCount3 == 1) {
                    while ((k >= 0) && (gridInfo[k][row] == color)) {
                        chessCount3++;
                        k--;
                    }
                    while ((k >= 0) && (gridInfo[k][row] == 0)) {
                        spaceCount4++;
                        k--;
                    }
                }
                break;
            case 2:  //  垂直方向
                //向增加的方向查找相同颜色连续的棋子
                for (k = row + 1; k <= GRIDSIZE; k++) {
                    if (gridInfo[col][k] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                //在棋子尽头查找连续的空格数
                while ((k <= GRIDSIZE) && (gridInfo[col][k] == 0)) {
                    spaceCount1++;
                    k++;
                }
                if (spaceCount1 == 1) {
                    while ((k <= GRIDSIZE) && (gridInfo[col][k] == color)) {
                        chessCount2++;
                        k++;
                    }
                    while ((k <= GRIDSIZE) && (gridInfo[col][k] == 0)) {
                        spaceCount2++;
                        k++;
                    }
                }

                //向相反方向查找相同颜色连续的棋子
                for (k = row - 1; k >= 0; k--) {
                    if (gridInfo[col][k] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                //在相反方向的棋子尽头查找连续的空格数
                while (k >= 0 && (gridInfo[col][k] == 0)) {
                    spaceCount3++;
                    k--;
                }
                if (spaceCount3 == 1) {
                    while ((k >= 0) && (gridInfo[col][k] == color)) {
                        chessCount3++;
                        k--;
                    }
                    while ((k >= 0) && (gridInfo[col][k] == 0)) {
                        spaceCount4++;
                        k--;
                    }
                }
                break;
            case 3:  //  左上到右下
                //向增加的方向查找相同颜色连续的棋子
                for (k = col + 1, m = row + 1; (k <= GRIDSIZE) && (m <= GRIDSIZE); k++, m++) {
                    if (gridInfo[k][m] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                //在棋子尽头查找连续的空格数
                while ((k <= GRIDSIZE) && (m <= GRIDSIZE) && (gridInfo[k][m] == 0)) {
                    spaceCount1++;
                    k++;
                    m++;
                }
                if (spaceCount1 == 1) {
                    while ((k <= GRIDSIZE) && (m <= GRIDSIZE) && (gridInfo[k][m] == color)) {
                        chessCount2++;
                        k++;
                        m++;
                    }
                    while ((k <= GRIDSIZE) && (m <= GRIDSIZE) && (gridInfo[k][m] == 0)) {
                        spaceCount2++;
                        k++;
                        m++;
                    }
                }

                //向相反方向查找相同颜色连续的棋子
                for (k = col - 1, m = row - 1; (k >= 0) && (m >= 0); k--, m--) {
                    if (gridInfo[k][m] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                //在相反方向的棋子尽头查找连续的空格数
                while ((k >= 0) && (m >= 0) && (gridInfo[k][m] == 0)) {
                    spaceCount3++;
                    k--;
                    m--;
                }
                if (spaceCount3 == 1) {
                    while ((k >= 0) && (m >= 0) && (gridInfo[k][m] == color)) {
                        chessCount3++;
                        k--;
                        m--;
                    }
                    while ((k >= 0) && (m >= 0) && (gridInfo[k][m] == 0)) {
                        spaceCount4++;
                        k--;
                        m--;
                    }
                }
                break;

            case 4:  //  右上到左下
                for (k = col + 1, m = row - 1; k <= GRIDSIZE && m >= 0; k++, m--) {  //查找连续的同色棋子
                    if (gridInfo[k][m] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                while (k <= GRIDSIZE && m >= 0 && (gridInfo[k][m] == 0)) { //统计空位数
                    spaceCount1++;
                    k++;
                    m--;
                }
                if (spaceCount1 == 1) {
                    while ((k <= GRIDSIZE) && (m >= 0) && (gridInfo[k][m] == color)) {
                        chessCount2++;
                        k++;
                        m--;
                    }
                    while ((k <= GRIDSIZE) && (m >= 0) && (gridInfo[k][m] == 0)) {
                        spaceCount2++;
                        k++;
                        m--;
                    }
                }

                for (k = col - 1, m = row + 1; k >= 0 && m <= GRIDSIZE; k--, m++) {  //查找连续的同色棋子
                    if (gridInfo[k][m] == color) {
                        chessCount1++;
                    }
                    else {
                        break;
                    }
                }
                while (k >= 0 && m <= GRIDSIZE && (gridInfo[k][m] == 0)) { // 统计空位数
                    spaceCount3++;
                    k--;
                    m++;
                }
                if (spaceCount3 == 1) {
                    while ((k >= 0) && (m <= GRIDSIZE) && (gridInfo[k][m] == color)) {
                        chessCount3++;
                        k--;
                        m++;
                    }
                    while ((k >= 0) && (m <= GRIDSIZE) && (gridInfo[k][m] == 0)) {
                        spaceCount4++;
                        k--;
                        m++;
                    }
                }
                break;
        }
    }
    // total_value += getValue(chessCount1, chessCount2, chessCount3, spaceCount1, spaceCount2, spaceCount3, spaceCount4, player);
    
    return TotalValue;
}

/// 基于路的评估函数
int EvaluateROAD(Location location, int color, int dx, int dy) {
    vector<int> line;
    int startX = location.x - dx * 5;
    int startY = location.y - dy * 5;
    for (int i = 0; i < 11; i++) {
        int x = startX + i * dx;
        int y = startY + i * dy;
        if (x >= LeftBoard && x <= RightBoard && y >= TopBoard && y <= BottomBoard) {
            line.push_back(gridInfo[x][y]);
        }
    }
    return Viewline(line, color, location);
}
/*
 当 dx 和 dy 分别取不同的值时，影响了在棋盘上沿着哪个方向移动以评估路线。这个函数可以合并先前的四个基于路的扫描评估
 当 dx = 0 而 dy = 1：
 这种情况下，函数沿着垂直于 y 轴的方向进行移动。dy 的正负值决定了是向上还是向下移动。因此，路线将会是一条垂直于 y 轴的直线。
 当 dx = 1 而 dy = 0：
 这种情况下，函数沿着垂直于 x 轴的方向进行移动。dx 的正负值决定了是向左还是向右移动。因此，路线将会是一条垂直于 x 轴的直线。
 当 dx 和 dy 分别取 1 和 1 （ 1 和 -1 ）：
 这种情况下，函数沿着斜线方向进行移动。dx 和 dy 的正负值决定了移动的方向。因此，路线将会是一条斜线。实现了左上右下和右上左下。
 
 */
vector<Location> MySimuLocation;//我方模拟落子的棋步
vector<Location> OpSimuLocation;//敌方模拟落子的棋步

//评估函数
int SimuEvaluate1(int color) {
    flag = 0;
    int result = 0;
    Location Myloc1 = MySimuLocation[0];
    Location Myloc2 = MySimuLocation[1];
    gridInfo[Myloc2.x][Myloc2.y] = 0;//我方撤回第二颗
    result +=
            EvaluateROAD(Myloc1,color,0,1)
            +EvaluateROAD(Myloc1, color, 1, 0)
            +EvaluateROAD(Myloc1, color, 1, 1)
            +EvaluateROAD(Myloc1, color, 1, -1);

    gridInfo[Myloc1.x][Myloc1.y] = 0;//我方撤回第一颗
    gridInfo[Myloc1.x][Myloc1.y] = -color;

    result +=
            EvaluateROAD(Myloc1,-color,0,1)
            +EvaluateROAD(Myloc1, -color, 1, 0)
            +EvaluateROAD(Myloc1, -color, 1, 1)
            +EvaluateROAD(Myloc1, -color, 1, -1);

    gridInfo[Myloc1.x][Myloc1.y] = color;


    gridInfo[Myloc2.x][Myloc2.y] = color;//假设我方下回第二步落子
    result +=
            EvaluateROAD(Myloc2,color,0,1)
            +EvaluateROAD(Myloc2, color, 1, 0)
            +EvaluateROAD(Myloc2, color, 1, 1)
            +EvaluateROAD(Myloc2, color, 1, -1);

    gridInfo[Myloc1.x][Myloc1.y] = 0;
    gridInfo[Myloc2.x][Myloc2.y] = 0;
    gridInfo[Myloc1.x][Myloc1.y] = -color;
    gridInfo[Myloc2.x][Myloc2.y] = -color;

    result +=
            EvaluateROAD(Myloc2, -color,0,1)
            +EvaluateROAD(Myloc2, -color, 1, 0)
            +EvaluateROAD(Myloc2, -color, 1, 1)
            +EvaluateROAD(Myloc2, -color, 1, -1);

    gridInfo[Myloc1.x][Myloc1.y] = color;
    gridInfo[Myloc2.x][Myloc2.y] = color;
    return result;
}

int SimuEvaluate(int color, int result) {
    Location Oploc1 = OpSimuLocation[0];
    Location Oploc2 = OpSimuLocation[1];
    gridInfo[Oploc2.x][Oploc2.y] = 0;  //敌方撤回第二颗
    flag = 1;
    result -=
            EvaluateROAD(Oploc1, -color,0,1)
            + EvaluateROAD(Oploc1, -color,1,0)
            + EvaluateROAD(Oploc1, -color,1,1)
            + EvaluateROAD(Oploc1, -color,1,-1);
    gridInfo[Oploc1.x][Oploc1.y] = 0;  //敌方撤回第一颗
    flag = 0;
    gridInfo[Oploc1.x][Oploc1.y] = color;
    flag = 2;
    result -=
            EvaluateROAD(Oploc1, color,0,1)
            + EvaluateROAD(Oploc1, color,1,0)
            + EvaluateROAD(Oploc1, color,1,1)
            + EvaluateROAD(Oploc1, color,1,-1);           //我方下第一颗
    gridInfo[Oploc2.x][Oploc2.y] = color;

    result -=
            EvaluateROAD(Oploc2, color,0,1)
            + EvaluateROAD(Oploc2, color,1,0)
            + EvaluateROAD(Oploc2, color,1,1)
            + EvaluateROAD(Oploc2, color,1,-1);
    gridInfo[Oploc1.x][Oploc1.y] = -color;
    gridInfo[Oploc2.x][Oploc2.y] = -color;
    gridInfo[Oploc2.x][Oploc2.y] = -color;//敌方下回第二颗
    flag = 1;
    result -=
            EvaluateROAD(Oploc2, -color,0,1)
            + EvaluateROAD(Oploc2, -color,1,0)
            + EvaluateROAD(Oploc2, -color,1,1)
            + EvaluateROAD(Oploc2, -color,1,-1);

    return result;
}

auto start_time = std::chrono::high_resolution_clock::now();

bool LimitTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return time_elapsed >= ddl;
}

int Alpha_Beta(int alpha, int beta, int depth, int color, int val) {
    if (LimitTime()) {
        return 0;
    }
    if (depth == 0) {
        return  SimuEvaluate(color, val);
    }
    int NUM = (int)PossibleLocation.size();
    int min = MAXVAL;
    int result = val;
    for (int i = 0; i < NUM; i++) {
        LocationWithValue location1 = PossibleLocation[i];
        if (gridInfo[location1.x][location1.y] != grid_blank) {
            continue;
        }
        gridInfo[location1.x][location1.y] = color;
        if (MyColor == color)
            MySimuLocation.push_back(location1);
        else
            OpSimuLocation.push_back(location1);
        for (int j = i; j < NUM; j++) {
 
            LocationWithValue location2 = PossibleLocation[j];

            if (gridInfo[location2.x][location2.y] != grid_blank) {
                continue;
            }
            gridInfo[location2.x][location2.y] = color;
            if (MyColor == color)
                MySimuLocation.push_back(location2);
            else
                OpSimuLocation.push_back(location2);
          
            if (MyColor == color)
                result = SimuEvaluate1(color);
            int value = -Alpha_Beta(-beta, -alpha, depth - 1, -color, result);
            int valuee = -value;
            if (min > valuee)
                min = valuee;
           
            gridInfo[location2.x][location2.y] = 0;
            if (MyColor == color)
                MySimuLocation.pop_back();
            else
                OpSimuLocation.pop_back();
      
            if (value >= beta) {     //这个函数对我方来说没用，留给敌方就好
                //撤回第一步落子
                gridInfo[location1.x][location1.y] = 0;
                OpSimuLocation.pop_back();
                return beta;                  //其实是返回了我方获取最大价值，也就是之前的最大价值，代表了不走这两步落子，只要有一种情况使得
            }                                   //在我方这两步落子的情况下敌方有情况可以使得我们比之前别的落子价值小，我们直接舍弃
            if (value > alpha && MyColor == color) {               //但是如果敌方怎么下我们都比之前情况好，那么我们就将落子更新为这两步落子
                alpha = value;
                if (depth == 2) {
                    BestLocation[0] = location1;
                    BestLocation[1] = location2;
                }
            }
        }
     
        gridInfo[location1.x][location1.y] = 0;
        if (MyColor == color)
            MySimuLocation.pop_back();
        else
            OpSimuLocation.pop_back();
    }

    return -min;
}

int main() {
    /// 恢复棋盘状态,并找出边界和记录最近4步棋
    int x0, y0, x1, y1;
    // 分析自己收到的输入和自己过往的输出，并恢复棋盘状态
    int turnID;
    cin >> turnID;
    MyColor = grid_white; // 先假设自己是白方
    // 根据这些输入输出逐渐恢复状态到当前回合，并找出边界和记录最近4步棋
    for (int i = 0; i < turnID; i++) {
        cin >> x0 >> y0 >> x1 >> y1;
        // 确定棋色
        if (x0 == -1) {
            MyColor = grid_black; // 第一回合收到坐标是-1, -1，说明我是黑方
        }
        // 模拟对方落子，并查找边界和记录对方最近两步棋
        if (x0 >= 0) {
            // 模拟落子
            ProcStep(x0, y0, x1, y1, -MyColor, false);
            UpdateBoardBoundariesForStep(x0, y0, x1, y1);
        }
        // 模拟己方落子，并查找边界和记录自己最近两步棋
        if (i < turnID - 1) {
            cin >> x0 >> y0 >> x1 >> y1;
            if (x0 >= 0) {
                // 模拟落子
                ProcStep(x0, y0, x1, y1, MyColor, false);
                UpdateBoardBoundariesForStep(x0, y0, x1, y1);
            }
        }
    }
    if (turnID == 1 && MyColor == grid_black) {
        BestLocation[0].x = 7;
        BestLocation[0].y = 8;
        BestLocation[1].x = -1;
        BestLocation[1].y = -1;
        cout << BestLocation[0].x << ' ' << BestLocation[0].y << ' ' << BestLocation[1].x << ' ' << BestLocation[1].y << endl;
        return 0;
    }
    else {
        ExtendBoardBoundaries();
    }
    Chess(MyColor);
    Alpha_Beta(-MAXVAL, MAXVAL, 2, MyColor, 0);
    //如果到了最后一个回合
    if (turnID == 57) {
        BestLocation[0] = PossibleLocation[0];
        BestLocation[1] = PossibleLocation[1];
        cout << BestLocation[0].x << ' ' << BestLocation[0].y << ' ' << BestLocation[1].x << ' ' << BestLocation[1].y << endl;
        return 0;
    }
    /// 决策结束，向平台输出决策结果
    cout << BestLocation[0].x << ' ' << BestLocation[0].y << ' ' << BestLocation[1].x << ' ' << BestLocation[1].y << endl;

    return 0;

}