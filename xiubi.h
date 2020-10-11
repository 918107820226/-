#ifndef XIUBI_H
#define XIUBI_H
//to cause the current source file to be included only once in a single compilation.
#pragma once
#include <QVector>
#include <array>
#include <vector>
#include <unordered_map>
using namespace std;

const  int CHESS_ROWS=20;
const  int CHESS_COLUMNS=20;
const  int RECT_WIDTH=40;
const  int RECT_HEIGHT=40;
struct definedStruct{
    //vector<vector<int>> v;
    char x1,y1,x2,y2;
    unsigned long long hash;
    int score;
    //int count;

    definedStruct(char x1_,char y1_,char x2_,char y2_,unsigned long long hash_):x1(x1_),y1(y1_),x2(x2_),y2(y2_),hash(hash_){
        score=0;
        //count=0;
    }

    //降序排列重载<号
//    bool operator<(const definedStruct&b)const{
//        return this->count>b.count;
//    }
    definedStruct(){
        x1=y1=x2=y2==0;
        score=0;
        //count=0;
    }
};

struct Position{
    int x;//0到20，行
    int y;//0到20，列
};



class Xiubi {
public:
    //根据hash值，查找置换表（transposition table）
    //以空间换时间
    unordered_map<unsigned long long,definedStruct> TT;
    //白子的Zobrist hash
    vector<vector<unsigned long long>> hashBlack;
    //黑子的Zobrist hash
    vector<vector<unsigned long long>> hashWhite;
    //获取某行棋子的类型
    string gline(const vector<vector<char>> &board,int type,int x,int y);
    //评估根据类型评估某行棋子的分数
    int evaluate(const string &s,char control);
    //根据当前局面获得当前棋局的分数
    definedStruct getScore(const vector<vector<char>> &board,unsigned long long &hash);
    //最大最小&&alpha-beat剪枝算法，对完全信息的局面有效
    definedStruct miniMax(vector<vector<char>> &board,int depth,int alpha,int beta,bool gamer,unsigned long long &hash);
    //初始化Zobrist hash
    unsigned long long initZobristRand();
    //清空置换表
    void init();
};

#endif // XIUBI_H
