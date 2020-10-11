#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include "item.h"
//#include "qmap.h"
#include "xiubi.h"
#include <QPushButton>
#include <QVector>
#include <vector>
namespace Ui {
class MainWindow;
}

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
private:
    void DrawChessboard();
    void DrawItemWithMouse(QPainter& painter);
    void initState();
    void initState1();
    void initState2();
    void DrawChessAtPoint(QPainter& painter,QPoint& pt);
    void SHIRO();
    void SHIRO1();
    
    int CountNearItem(Item item,QPoint ptDirection);	//统计某个方向(共8个方向)上的相连个数，用QPoint表示统计方向，如(1,1)表示右下方,（-1,0）表示向左
    Ui::MainWindow *ui;
    //判定是否达到获胜条件用
    int nLeft;
    int nLeftUp;
    int nUp;
    int nRightUp;
    int nRight;
    int nRightDown;
    int nDown;
    int nLeftDown;
    bool black_first_move;//黑子先手
    bool is_black_turn;   //当前该黑棋下
    bool is_win;          //某方获胜，游戏结束
    int counter;          //模四计数器，01白子，23黑子
    QPushButton *start;   //人人对战开始按钮
    QPushButton* start1;  //人机对战开始按钮
    QPushButton* start2;  //机机对战开始按钮
    enum mode{GAMER,XIUBI_GAMER,XIUBI}m; //枚举类型{人，人机，机}
    Xiubi xiubi;          //ai休比，负责ai函数的调用
    unsigned long long hash; //zobrist hashing，用来标识当前棋局
    QVector<Item> m_items;//用来存储已经下过的棋子的颜色以及位置
    vector<vector<char>> board; //0无，1白，2黑
};

#endif // MAINWINDOW_H
