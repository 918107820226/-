#include "xiubi.h"
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>
#include <vector>
#include <string>
#include <QDebug>
#include <QString>
#include <random>
#include <map>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace  std;


//zobrist每次都与下一步要走的表的位置来置换，理解理解，实际上是个不断置换的code
//初始化black和white 二维zobrist数组
unsigned long long Xiubi::initZobristRand(){
    std::random_device rd;
    mt19937_64 m(rd());
    hashBlack.resize(CHESS_ROWS);
    for(int i=0;i<CHESS_ROWS;++i){
        hashBlack[i].resize(CHESS_COLUMNS);
        for(int j=0;j<CHESS_COLUMNS;++j){
            hashBlack[i][j]=m();
        }
    }
    hashWhite.resize(CHESS_ROWS);
    for(int i=0;i<CHESS_ROWS;++i){
        hashWhite[i].resize(CHESS_COLUMNS);
        for(int j=0;j<CHESS_COLUMNS;++j){
            hashWhite[i][j]=m();
        }
    }
    return m();
}
//初始化，封装了清空置换表的操作
void Xiubi::init(){
    TT.clear();
    //this->initZobristRand();
}




//最大最小函数
//board:当前棋局状况，给ai评估分数用
//depth:即minimax算法的决策树搜索深度d，显然，总的结点数有b^d
//alpha:Alpha-Beta Pruning算法的alpha参数，最简单的版本可将其初始化为INT_MIN
//beta:Alpha-Beta Pruning算法的beta参数，最简单的版本可将其初始化为INT_MAX
//gamer:黑子返回白子的max,白子返回黑子的min,或者到达搜索深度限制，返回当前的评估值
//hash:当前局面的Zobrist hash值
definedStruct Xiubi::miniMax(vector<vector<char>> &board,int depth,int alpha,int beta,bool gamer,unsigned long long &hash)
{
    //使用getScore获得当前位置的评估分数
    if(depth==0)return getScore(board,hash);
    //黑子，力求最大值
    if(gamer==true){
        vector<definedStruct> Vboard;
        int maxEval = std::numeric_limits<int>::min();
        for(int row=0;row<CHESS_ROWS;++row){
            for(int column=0;column<CHESS_COLUMNS;++column){
                //边界检测
                if(row<0||CHESS_ROWS<=row||column<0||CHESS_COLUMNS<=column||board[row][column]!='0')continue;
                board[row][column]='2';
                //第一步的指纹
                hash^=hashBlack[row][column];
                //qDebug() << hash << endl;
                //走第二步
                for(int row1=0;row1<CHESS_ROWS;++row1){
                    for(int column1=0;column1<CHESS_COLUMNS;++column1){
                        if(row1<0||CHESS_ROWS<=row1||column1<0||CHESS_COLUMNS<=column1||board[row1][column1]!='0')continue;
                        board[row1][column1]='2';
                        //两步作一步的指纹
                        hash^=hashBlack[row1][column1];
                        //qDebug() << hash << endl;
                        if(TT.count(hash)==0)TT[hash]=definedStruct((char)row,(char)column,(char)row1,(char)column1,hash);
                        Vboard.push_back(TT[hash]);
                        board[row1][column1]='0';
                        hash^=hashBlack[row1][column1];
                    }
                }
            hash^=hashBlack[row][column];
            board[row][column]='0';
            }
        }
        int size=Vboard.size();
        //sort(Vboard.begin(),Vboard.end());
        sort(Vboard.begin(),Vboard.end(),[](const definedStruct& a,const definedStruct& b)->bool{return a.score>b.score;});
        unsigned long long maxHash;
        definedStruct tboard;
        for(int i=0;i<size;++i){
            //maxEval=max(maxEval,eval);

            tboard=Vboard[i];
            //qDebug() << tboard.hash<<endl;
            board[tboard.x1][tboard.y1]='2';
            board[tboard.x2][tboard.y2]='2';
            int eval = miniMax(board,depth-1,alpha,beta,false,tboard.hash).score;
            board[tboard.x1][tboard.y1]='0';
            board[tboard.x2][tboard.y2]='0';
            TT[tboard.hash].score=eval;
            //++TT[tboard.hash].count;
            if(maxEval<eval){
                maxEval=eval;
                maxHash=tboard.hash;
//                px1=tboard.x1;
//                py1=tboard.y1;
//                px2=tboard.x2;
//                py2=tboard.y2;
                //hash=tboard.hash;
                //qDebug() << "求到勒目前最大值"<<maxEval;
                //qDebug() << "当前的hash值"<<hash;
                //qDebug() << "当前第一步xy" << px1 << " "<<py1;
                //qDebug() << "当前第二步xy" << px2 << " "<<py2;
            }
            alpha=max(alpha,eval);
            if(beta<=alpha)break;
        }
        return TT[maxHash];
    }else if(gamer==0){ //白子，力求最小值
        vector<definedStruct> Vboard;
        int minEval = std::numeric_limits<int>::max();
        for(int row=0;row<CHESS_ROWS;++row){
            for(int column=0;column<CHESS_COLUMNS;++column){
                //边界检测
                if(row<0||CHESS_ROWS<=row||column<0||CHESS_COLUMNS<=column||board[row][column]!='0')continue;
                board[row][column]='1';
                //第一步的指纹
                hash^=hashWhite[row][column];
                //走第二步
                for(int row1=0;row1<CHESS_ROWS;++row1){
                    for(int column1=0;column1<CHESS_COLUMNS;++column1){
                        if(row1<0||CHESS_ROWS<=row1||column1<0||CHESS_COLUMNS<=column1||board[row1][column1]!='0')continue;
                        board[row1][column1]='1';
                        //两步作一步的指纹
                        hash^=hashWhite[row1][column1];
                        if(TT.count(hash)==0)TT[hash]=definedStruct((char)row,(char)column,(char)row1,(char)column1,hash);
                        Vboard.push_back(TT[hash]);
                        board[row1][column1]='0';
                        hash^=hashWhite[row1][column1];
                    }
                }
            hash^=hashWhite[row][column];
            board[row][column]='0';
            }
        }
        int size=Vboard.size();
        //sort(Vboard.begin(),Vboard.end());
        sort(Vboard.begin(),Vboard.end(),[](const definedStruct& a,const definedStruct& b)->bool{return a.score<b.score;});
        unsigned long long minHash;
        definedStruct tboard;
        for(int i=0;i<size;++i){
            //maxEval=max(maxEval,eval);
            //qDebug() << tboard.hash<<endl;
            tboard=Vboard[i];
            board[tboard.x1][tboard.y1]='1';
            board[tboard.x2][tboard.y2]='1';

            int eval = miniMax(board,depth-1,alpha,beta,true,tboard.hash).score;
            
            board[tboard.x1][tboard.y1]='0';
            board[tboard.x2][tboard.y2]='0';
            TT[tboard.hash].score=eval;
            //++TT[tboard.hash].count;
            //qDebug() <<"得分:"<<eval;
            //qDebug() << "x1:"<<(int)tboard.x1 << "y1:"<<(int)tboard.y1;
           // qDebug() << "x2:"<<(int)tboard.x2 << "y2:"<<(int)tboard.y2;
            if(eval<minEval){
                minEval=eval;
                minHash=tboard.hash;
                //qDebug() <<"得分:"<<eval;
//                px1=tboard.x1;
//                py1=tboard.y1;
//                px2=tboard.x2;
//                py2=tboard.y2;
                //hash=tboard.hash;

                //qDebug() << "x1:"<<(int)tboard.x1 << "y1:"<<(int)tboard.y1;
                //qDebug() << "x2:"<<(int)tboard.x2 << "y2:"<<(int)tboard.y2;
            }
            beta=min(beta,eval);
            if(beta<=alpha)break;
        }
        return TT[minHash];
    }
}







//返回该方向的直线上的棋子类型
//board 2d数组行有限存储   横行竖列
//传入棋盘，和type,0水平，1左上至右下，2竖直，3右上至左下
//用2黑子，1白子
string Xiubi::gline(const vector<vector<char>> &board,int type,int x,int y){
    //构造string 用于之后匹配字符模式
    //构造长达九位的string
    string s;
    // '-' '\' '|' '/'
    //s+"3";else s.append(to_string(board[x][j]));
    if(type==0){
        int j=y-4;int loop=y+4;
        for(;j<=loop;++j){//3指越界，不能下
            if(j<0||CHESS_COLUMNS<=j)s.push_back('4');else s.push_back(board[x][j]);
        }
    }else if(type==1){
        int i=x-4;int j=y-4;
        for(int k=0;k<=8;++k){
                if(i<0||CHESS_ROWS<=i||j<0||CHESS_COLUMNS<=j)s.push_back('4');else s.push_back(board[i][j]);
                ++i;++j;
        }
    }else if(type==2){
        int i=x-4;int loop=x+4;
        for(;i<=loop;++i){
            if(i<0||CHESS_ROWS<=i)s.push_back('4');else s.push_back(board[i][y]);
        }
    }else if(type==3){
        int i=x-4;int j=y+4;
        for(int k=0;k<=8;++k){
            if(i<0||CHESS_ROWS<=i||j<0||CHESS_COLUMNS<=j)s.push_back('4');else s.push_back(board[i][j]);
            ++i;--j;
        }
    }
    s.push_back(0);
    //qDebug() << s.size();
    //qDebug(s.c_str());
    //qDebug() << 1111 << endl;
   // QString ss;
//    for(int i =0;i<s.length();++i){
//        ss.append(s[i]);
//    }
    //qDebug() << ss;
    return s;
}



//count
//辅助函数，用来返回一个字符串蕴含某种形式的串的个数
int countOccur(const string & s,const string &pattern){
    int occurrences =0;
    int pos = 0;
       while ((pos = s.find(pattern, pos)) != std::string::npos) {
              ++occurrences;
              pos += pattern.length();
       }
       return occurrences;
}
//给定某种字符串，返回分数
//白方黑方对于某种一定形式的串都返回一样的分数
//评估函数还有一些问题，但根据Amdahl定律，也只能先这样了
//对Threat处理的还不够好
int Xiubi::evaluate(const string &s,char control){
    //白方
    if(control==1){
    vector<int> situation(27,0);
    if(s.find("111111")!=std::string::npos){
         situation[0]=10000000+741071;
    }if(s.find("0111110")!=std::string::npos){   //
         situation[1]=471125+100621;
    }if(s.find("010111010  ")!=std::string::npos){
         situation[2]=178408+2+10321+2;
    }if(s.find("100111010")!=std::string::npos){
         situation[3]=968445+2+10321+2;
    }if (s.find("100111001")!=std::string::npos) {
         situation[4]=136975+2+10321+2;
    }if(s.find("00111100")!=std::string::npos){
         situation[5]=958631+65651;
    } if(s.find("111110")!=std::string::npos){
         situation[6]=224804+100621;
    } if(s.find("111101")!=std::string::npos){
         situation[7]=745729+65651+2;
    } if(s.find("111011")!=std::string::npos){
         situation[8]=146434+10321+139;
    } if(s.find("1111001")!=std::string::npos){
         situation[9]=653420+65651+2;
    } if(s.find("1110011")!=std::string::npos){
         situation[10]=406632+10321+139;
    } if(s.find("1110101")!=std::string::npos){
         situation[11]=752847+10321+2+2;
    } if (s.find("1101101")!=std::string::npos) {
         situation[12]=835834+139+139+2;
    } if(s.find("1011101")!=std::string::npos){
         situation[13]=180915+2+10321+2;
    } if(s.find("1101011")!=std::string::npos){
         situation[14]=170667+139+2+139;
    } if(s.find("111100")!=std::string::npos){
         situation[15]=965425+65651;
    } if(s.find("111001")!=std::string::npos){
         situation[16]=294247+10321+2;
    } if(s.find("110011")!=std::string::npos){
         situation[17]=308961+139+139;
    } if(s.find("110101")!=std::string::npos){
         situation[18]=296802+139+2+2;
    } if(s.find("101101")!=std::string::npos){
         situation[19]=919855+2+139+2;
    } if(s.find("00111000")!=std::string::npos){
         situation[20]=916206+10321;
    } if (s.find("00110100")!=std::string::npos) {
         situation[21]=378982+139+2;
    } if(s.find("010110000")!=std::string::npos){
         situation[22]=262479+2+139;
    } if(s.find("100110000")!=std::string::npos){
         situation[23]=771643+2+139;
    } if(s.find("010011000")!=std::string::npos){
         situation[24]=28720+2+139;
    } if(s.find("010101000")!=std::string::npos){
         situation[25]=437413+2+2+2;
    } if(s.find("111000")!=std::string::npos){
         situation[26]=625778+10321;
    }
    int w6 = countOccur(s,"111111");
    int w5=countOccur(s,"11111");
    int w4=countOccur(s,"1111");
    int w3=countOccur(s,"111");
    int w2=countOccur(s,"11");
    int w1=countOccur(s,"1");
    int score=w6*741071+w5*100621+w4*65651+w3*10321+w2*2+w1*1;
    for(auto x:situation)score+=x;
    //qDebug()<<"评估白方分值"<<score;
    return score;
//    if(s.find("111111")!=std::string::npos){        //连六
//        return 741071;
//    }else if(s.find("11111")!=std::string::npos){    //连五
//        return 100621;
//    }else if(s.find("1111")!=std::string::npos){     //连四
//        return 65651;
//    }else if(s.find("111")!=std::string::npos){      //连三
//        return 10321;
//    }else if(s.find("11")!=std::string::npos){        //连二
//        return 2;
//    }

    
       
    }else{
    //黑方
    vector<int> situation(27,0);
    if(s.find("222222")!=std::string::npos){
         situation[0]=10000000+741071;
    } if(s.find("0222220")!=std::string::npos){   //
         situation[1]=471125+100621;
    } if(s.find("020222020  ")!=std::string::npos){
         situation[2]=178408+2+10321+2;
    } if(s.find("200222020")!=std::string::npos){
         situation[3]=968445+2+10321+2;
    } if (s.find("200222002")!=std::string::npos) {
         situation[4]=136975+2+10321+2;
    } if(s.find("00222200")!=std::string::npos){
         situation[5]=958631+65651;
    } if(s.find("222220")!=std::string::npos){
         situation[6]=224804+100621;
    } if(s.find("222202")!=std::string::npos){
         situation[7]=745729+65651+2;
    } if(s.find("222022")!=std::string::npos){
         situation[8]=146434+10321+139;
    } if(s.find("2222002")!=std::string::npos){
         situation[9]=653420+65651+2;
    } if(s.find("2220022")!=std::string::npos){
         situation[10]=406632+10321+139;
    } if(s.find("2220202")!=std::string::npos){
         situation[11]=752847+10321+2+2;
    } if (s.find("2202202")!=std::string::npos) {
         situation[12]=835834+139+139+2;
    } if(s.find("2022202")!=std::string::npos){
         situation[13]=180915+2+10321+2;
    } if(s.find("2202022")!=std::string::npos){
         situation[14]=170667+139+2+139;
    } if(s.find("222200")!=std::string::npos){
         situation[15]=965425+65651;
    } if(s.find("222002")!=std::string::npos){
         situation[16]=294247+10321+2;
    } if(s.find("220022")!=std::string::npos){
         situation[17]=308961+139+139;
    } if(s.find("220202")!=std::string::npos){
         situation[18]=296802+139+2+2;
    } if(s.find("202202")!=std::string::npos){
         situation[19]=919855+2+139+2;
    } if(s.find("00222000")!=std::string::npos){
         situation[20]=916206+10321;
    } if (s.find("00220200")!=std::string::npos) {
         situation[21]=378982+139+2;
    } if(s.find("020220000")!=std::string::npos){
         situation[22]=262479+2+139;
    } if(s.find("200220000")!=std::string::npos){
         situation[23]=771643+2+139;
    } if(s.find("020022000")!=std::string::npos){
         situation[24]=28720+2+139;
    } if(s.find("020202000")!=std::string::npos){
         situation[25]=437413+2+2+2;
    } if(s.find("222000")!=std::string::npos){
         situation[26]=625778+10321;
    }
    int b6=countOccur(s,"222222");
    int b5=countOccur(s,"22222");
    int b4=countOccur(s,"2222");
    int b3=countOccur(s,"222");
    int b2=countOccur(s,"22");
    int b1=countOccur(s,"2");
    int score=b6*741071+b5*100621+b4*65651+b3*10321+b2*2+b1*1;
    for(auto x:situation)score+=x;
    //qDebug()<<"评估黑方分值"<<score;
    return score;

}
}
//获取某一局面的股份
definedStruct Xiubi::getScore(const vector<vector<char>> &board,unsigned long long &hash){
    //score可正可负，但是都为black-white
    //返回当前情况的评估分数
    //qDebug() << "当前的hash值:"<<hash;
    //Lazy Evaluation
    if(TT.count(hash)&&TT[hash].score!=0){
        //++TT[hash].count;
        return TT[hash];
    }
    int score=0;
    for(int i=0;i<CHESS_ROWS;++i){
        for(int j=0;j<CHESS_COLUMNS;++j){
            //对于每个棋子，获取它的四种情况，然后调用函数获得每种情况的分数
            //黑方执子，目标获取最大分数
            if(board[i][j]=='2'){
                for(int k=0;k<=3;++k){
                    score+=evaluate(gline(board,k,i,j),2);
                }
            }else if(board[i][j]=='1'){  //白方执子，目标获取最小分数。
                for(int k=0;k<=3;++k){
                    score-=evaluate(gline(board,k,i,j),1);
                }
            }
        }
    }
    //qDebug()<< "估分值:" <<score;
    TT[hash].score=score;
    //++TT[hash].count;
    //qDebug() << "这局的分数:" << score;
    return TT[hash];
}
















