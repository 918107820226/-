#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qpainter.h"
#include "qevent.h"
#include "qpoint.h"
#include "qmessagebox.h"
#include <QTime>
#include <QDebug>
#include <vector>
#include <QTimer>
#include <QCoreApplication>
#include <QPushButton>
#include <QMouseEvent>
#include <iostream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
    ui->menuBar->hide();
    //设置主界面的大小，为按钮留下空间
    resize((CHESS_COLUMNS + 10)*RECT_WIDTH  ,(CHESS_ROWS + 1)*RECT_HEIGHT);
    
    m_items.clear();
    board.resize(CHESS_ROWS);
    for(int i=0;i<CHESS_ROWS;++i){
        board[i].resize(CHESS_COLUMNS);
        for(int j=0;j<CHESS_COLUMNS;++j){
            board[i][j]='0';
        }
    }
    black_first_move = true;
    counter=0;
    is_win=0;
    m=mode::GAMER;
    hash=xiubi.initZobristRand();
    //init一次置换表就行，虽然还想再弄个开局库，但是受限于时间...
    xiubi.init();
    start=new QPushButton("人人对战",this);
    start1=new QPushButton("人机对战",this);
    start2=new QPushButton("机机对战",this);
    connect(this->start,&QAbstractButton::clicked,this,&MainWindow::initState);
    connect(this->start1,&QAbstractButton::clicked,this,&MainWindow::initState1);
    connect(this->start2,&QAbstractButton::clicked,this,&MainWindow::initState2);
    start->show();
    start->move(1000,203);
    start1->show();
    start1->move(1000,403);
    start2->show();
    start2->move(1000,603);


}

MainWindow::~MainWindow()
{
    delete ui;
}
//人人对战
void MainWindow::initState(){
    //memset(state,0,sizeof(state));
    m=mode::GAMER;
    m_items.clear();
    for(int i=0;i<CHESS_ROWS;++i){
        for(int j=0;j<CHESS_COLUMNS;++j){
            board[i][j]='0';
        }
    }
    black_first_move = true;
    counter=0;
    is_win=false;
}
//人机对战
void MainWindow::initState1(){
    m=mode::XIUBI_GAMER;
    m_items.clear();
    for(int i=0;i<CHESS_ROWS;++i){
        for(int j=0;j<CHESS_COLUMNS;++j){
            board[i][j]='0';
        }
    }
    black_first_move = true;
    counter=0;
    is_win=false;

}
//机机对战
void MainWindow::initState2(){
    m=mode::XIUBI;
    m_items.clear();
    for(int i=0;i<CHESS_ROWS;++i){
        for(int j=0;j<CHESS_COLUMNS;++j){
            board[i][j]='0';
        }
    }
    black_first_move = true;
    counter=0;
    is_win=false;
}
//重写绘图事件
void MainWindow::paintEvent(QPaintEvent *e)
{
    
    SHIRO();                 //人机对战
    SHIRO1();                //机机对战
    DrawChessboard();		 //画棋盘和棋子
    
    update();
}
//绘制棋盘和棋子
void MainWindow::DrawChessboard()
{
    QPainter painter(this);
    //Indicates that the engine should antialias edges of primitives if possible.
    painter.setRenderHint(QPainter::Antialiasing, true);
    //Dark yellow (#808000)
    painter.setBrush(Qt::darkYellow);
    //Constructs a solid line pen with 1 width and the given color.
    painter.setPen(QPen(QColor(Qt::black),2));

    for(int i = 0;i<CHESS_COLUMNS; ++i)
    {
        for (int j = 0; j<CHESS_ROWS; ++j)
        {
           //void QPainter::drawRect(int x, int y, int width, int height)
           //Draws a rectangle with upper left corner at (x, y) and with the given width and height.
           painter.drawRect( (i+0.5)*RECT_WIDTH,(j+0.5)*RECT_HEIGHT,RECT_WIDTH,RECT_HEIGHT);
        }
    }

    //绘制落子后的棋盘（绘制落子）
    for (int i = 0; i<m_items.size(); i++)
    {
        Item item = m_items[i];
        if (item.m_bBlack)
        {
            painter.setBrush(Qt::black);
        }
        else
        {

            painter.setBrush(Qt::white);
        }
        DrawChessAtPoint(painter,item.m_pt);
    }
    
    DrawItemWithMouse(painter);	//画鼠标（当前方的棋子形状）

}

//调用qt api绘制棋子
void MainWindow::DrawChessAtPoint(QPainter& painter,QPoint& pt)
{
    //painter.drawRect( (pt.x()+0.5)*RECT_WIDTH,(pt.y()+0.5)*RECT_HEIGHT,RECT_WIDTH,RECT_HEIGHT);

    QPoint ptCenter((pt.x()+0.5)*RECT_WIDTH,(pt.y()+0.5)*RECT_HEIGHT);
    //Draws the ellipse positioned at center with radii rx and ry.
    painter.drawEllipse(ptCenter,RECT_WIDTH / 2,RECT_HEIGHT / 2);
}
//人机对战时的ai
void MainWindow::SHIRO(){
    if(m==mode::XIUBI_GAMER && !is_win){
    if(black_first_move){
        return;
    }else{
        if (counter<2)
        {
            is_black_turn=false;
            counter+=2;
            int alpha=std::numeric_limits<int>::min();
            int beta=std::numeric_limits<int>::max();
            //https://stackoverflow.com/questions/33622292/why-is-stdbad-alloc-thrown-when-enough-memory-is-available
            //32位程序的局限性，故最大搜索深度只有1了
            //
            definedStruct bestMove = xiubi.miniMax(board,1,alpha,beta,0,hash);
            //qDebug() << xiubi.count;
            //xiubi.miniMax(board,2,std::numeric_limits<long long>::min(),std::numeric_limits<long long>::max(),1,x,y);
            //m_items.append(Item(QPoint(ar[0],ar[1]),0));
            board[bestMove.x1][bestMove.y1]='1';
            board[bestMove.x2][bestMove.y2]='1';
            Item item1(QPoint(bestMove.y1,bestMove.x1),0);
            Item item2(QPoint(bestMove.y2,bestMove.x2),0);
            m_items.append(item1);
            m_items.append(item2);
            nLeft =			CountNearItem(item1,QPoint(-1,0));
            nLeftUp =		CountNearItem(item1,QPoint(-1,-1));
            nUp =			CountNearItem(item1,QPoint(0,-1));
            nRightUp =		CountNearItem(item1,QPoint(1,-1));
            nRight =		CountNearItem(item1,QPoint(1,0));
            nRightDown =	CountNearItem(item1,QPoint(1,1));
            nDown =			CountNearItem(item1,QPoint(0,1));
            nLeftDown =		CountNearItem(item1,QPoint(-1,1));
            if ( (nLeft + nRight) >= 5 ||
                 (nLeftUp + nRightDown) >= 5 ||
                 (nUp + nDown) >= 5 ||
                 (nRightUp + nLeftDown) >= 5 )
            {
                QString str = is_black_turn?"Black":"White";
                is_win=true;
                QMessageBox::information(nullptr,  "GAME OVER",str, QMessageBox::Yes , QMessageBox::Yes);

                //NewGame();
                //initState1();
                
                return;

            }
    }
}
    }
}
//机机对战时的ai
void MainWindow::SHIRO1(){
    if(m==mode::XIUBI && !is_win){
        if(black_first_move){
            //随机下一子
            srand(time(NULL));
            int x=rand()%CHESS_ROWS;
            int y=rand()%CHESS_COLUMNS;
            Item item(QPoint(y,x),1);
            m_items.append(item);
            black_first_move=false;
        }else if(counter<2){
            is_black_turn=false;
            counter+=2;
            int alpha=std::numeric_limits<int>::min();
            int beta=std::numeric_limits<int>::max();
            definedStruct bestMove = xiubi.miniMax(board,1,alpha,beta,is_black_turn,hash);
            board[bestMove.x1][bestMove.y1]='1';
            board[bestMove.x2][bestMove.y2]='1';
            Item item1(QPoint(bestMove.y1,bestMove.x1),0);
            Item item2(QPoint(bestMove.y2,bestMove.x2),0);
            m_items.append(item1);
            m_items.append(item2);
            nLeft =			CountNearItem(item1,QPoint(-1,0));
            nLeftUp =		CountNearItem(item1,QPoint(-1,-1));
            nUp =			CountNearItem(item1,QPoint(0,-1));
            nRightUp =		CountNearItem(item1,QPoint(1,-1));
            nRight =		CountNearItem(item1,QPoint(1,0));
            nRightDown =	CountNearItem(item1,QPoint(1,1));
            nDown =			CountNearItem(item1,QPoint(0,1));
            nLeftDown =		CountNearItem(item1,QPoint(-1,1));
            if ( (nLeft + nRight) >= 5 ||
                 (nLeftUp + nRightDown) >= 5 ||
                 (nUp + nDown) >= 5 ||
                 (nRightUp + nLeftDown) >= 5 )
            {
                QString str = is_black_turn?"Black":"White";
                is_win=true;
                QMessageBox::information(nullptr,  "GAME OVER",str, QMessageBox::Yes , QMessageBox::Yes);

                //NewGame();
                //initState1();
                
                return;

            }
        }else {
            is_black_turn=true;
            counter+=2;
            int alpha=std::numeric_limits<int>::min();
            int beta=std::numeric_limits<int>::max();
            definedStruct bestMove = xiubi.miniMax(board,1,alpha,beta,is_black_turn,hash);
            board[bestMove.x1][bestMove.y1]='2';
            board[bestMove.x2][bestMove.y2]='2';
            Item item1(QPoint(bestMove.y1,bestMove.x1),1);
            Item item2(QPoint(bestMove.y2,bestMove.x2),1);
            m_items.append(item1);
            m_items.append(item2);
            nLeft =			CountNearItem(item1,QPoint(-1,0));
            nLeftUp =		CountNearItem(item1,QPoint(-1,-1));
            nUp =			CountNearItem(item1,QPoint(0,-1));
            nRightUp =		CountNearItem(item1,QPoint(1,-1));
            nRight =		CountNearItem(item1,QPoint(1,0));
            nRightDown =	CountNearItem(item1,QPoint(1,1));
            nDown =			CountNearItem(item1,QPoint(0,1));
            nLeftDown =		CountNearItem(item1,QPoint(-1,1));
            if ( (nLeft + nRight) >= 5 ||
                 (nLeftUp + nRightDown) >= 5 ||
                 (nUp + nDown) >= 5 ||
                 (nRightUp + nLeftDown) >= 5 )
            {
                QString str = is_black_turn?"Black":"White";
                is_win=true;
                QMessageBox::information(nullptr,  "GAME OVER",str, QMessageBox::Yes , QMessageBox::Yes);

                //NewGame();
                //initState2();
                
                return;
            }
        }
    }
}

//绘制鼠标（白子或黑子）
void MainWindow::DrawItemWithMouse(QPainter& painter)
{
    //QPainter painter(this);
    painter.setPen(QPen(QColor(Qt::transparent)));
    if(black_first_move){
        painter.setBrush(Qt::black);
        //Draws the ellipse positioned at center with radii rx and ry.
        //Translates the global screen coordinate pos to widget coordinates.
        painter.drawEllipse(mapFromGlobal(QCursor::pos()),RECT_WIDTH / 2,RECT_HEIGHT / 2);
        return;
    }else{
        counter%=4;
        if (counter<2)
        {
            //休比落子
            painter.setBrush(Qt::white);
        }
        else
        {
            //玩家落子
            painter.setBrush(Qt::black);
        }
    }

    //Translates the global screen coordinate pos to widget coordinates.
    painter.drawEllipse(mapFromGlobal(QCursor::pos()),RECT_WIDTH / 2,RECT_HEIGHT / 2);
}

//鼠标按下事件
void MainWindow::mousePressEvent(QMouseEvent * e)
{
    //人人对战
    if(m==mode::GAMER &&!is_win){
        //通过[0,40)/40==0的方法求得点击之后的坐标
        QPoint pt;
        QPainter painter(this);
        pt.setX( (e->pos().x() ) / RECT_WIDTH);//求鼠标点击处的棋子pt
        pt.setY( (e->pos().y() ) / RECT_HEIGHT);
        //已有棋子&&边界检测
        if((CHESS_COLUMNS*RECT_WIDTH)<e->x()||
                (CHESS_ROWS*RECT_HEIGHT)<e->y())return;
        for (int i = 0; i<m_items.size(); i++){
            Item item = m_items[i];
            if (item.m_pt == pt){
                return;}
        }
        //不存在棋子，就下一个
        //counter%=4;
        if(black_first_move){is_black_turn=true;black_first_move=false;}
        else{
            if(counter<2){
                is_black_turn=false;
                ++counter;
            }else {
                is_black_turn=true;
                ++counter;
            }
        }

        Item item(pt,is_black_turn);
        m_items.append(item);
        //qDebug()<<board.size() << " "<<board[0].size()<<endl;
        //统计4个方向是否六子连
        nLeft =			CountNearItem(item,QPoint(-1,0));
        nLeftUp =		CountNearItem(item,QPoint(-1,-1));
        nUp =			CountNearItem(item,QPoint(0,-1));
        nRightUp =		CountNearItem(item,QPoint(1,-1));
        nRight =		CountNearItem(item,QPoint(1,0));
        nRightDown =	CountNearItem(item,QPoint(1,1));
        nDown =			CountNearItem(item,QPoint(0,1));
        nLeftDown =		CountNearItem(item,QPoint(-1,1));
        if ( (nLeft + nRight) >= 5 ||
             (nLeftUp + nRightDown) >= 5 ||
             (nUp + nDown) >= 5 ||
             (nRightUp + nLeftDown) >= 5 )
        {
            QString str = is_black_turn?"Black":"White";
            QMessageBox::information(nullptr,  "GAME OVER",str, QMessageBox::Yes , QMessageBox::Yes);

            //NewGame();
            //initState();
            is_win=true;
            return;
        }
    }else if(m==mode::XIUBI_GAMER && !is_win){//人机对战
        QPoint pt;
        QPainter painter(this);
        //通过[0,40)/40==0的方法求得点击之后的坐标
        pt.setX( (e->pos().x() ) / RECT_WIDTH);//求鼠标点击处的棋子pt
        pt.setY( (e->pos().y() ) / RECT_HEIGHT);
        //已有棋子&&边界检测
        if((CHESS_COLUMNS*RECT_WIDTH)<e->x()||
                (CHESS_ROWS*RECT_HEIGHT)<e->y())return;
        for (int i = 0; i<m_items.size(); i++){
            Item item = m_items[i];
            if (item.m_pt == pt){
                return;}
        }
        //先判断是XIUBI还是玩家在控制棋局
        if(black_first_move){is_black_turn=true;black_first_move=false;}
        else{
            if(counter<2){
                is_black_turn=false;
            }else {
                is_black_turn=true;
            }
            ++counter;
        }
        //玩家在落子
        if(is_black_turn){
            
            
            Item item(pt,is_black_turn);
            m_items.append(item);
            board[pt.ry()][pt.rx()]='2';
            hash^=xiubi.hashBlack[pt.ry()][pt.rx()];
            nLeft =			CountNearItem(item,QPoint(-1,0));
            nLeftUp =		CountNearItem(item,QPoint(-1,-1));
            nUp =			CountNearItem(item,QPoint(0,-1));
            nRightUp =		CountNearItem(item,QPoint(1,-1));
            nRight =		CountNearItem(item,QPoint(1,0));
            nRightDown =	CountNearItem(item,QPoint(1,1));
            nDown =			CountNearItem(item,QPoint(0,1));
            nLeftDown =		CountNearItem(item,QPoint(-1,1));
            if ( (nLeft + nRight) >= 5 ||
                 (nLeftUp + nRightDown) >= 5 ||
                 (nUp + nDown) >= 5 ||
                 (nRightUp + nLeftDown) >= 5 )
            {
                QString str = is_black_turn?"Black":"White";
                is_win=true;
                QMessageBox::information(nullptr,  "GAME OVER",str, QMessageBox::Yes , QMessageBox::Yes);

                //NewGame();
                //initState1();
                
                return;
        }
        }else{//是休比
            return;
        }
    }else if(m==mode::XIUBI && !is_win){//机机对战
        return;
    }
}

//计算上下左右八个方向的棋子个数
//做个全局函数
int MainWindow::CountNearItem(Item item,QPoint ptDirection)
{
    int nCount = 0;
    item.m_pt += ptDirection;

    while (m_items.contains(item))
    {
        nCount++;
        item.m_pt += ptDirection;
    }
    return nCount;
}

