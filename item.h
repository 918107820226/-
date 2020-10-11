#pragma once
#include <QPoint>

class Item
{
public:
    Item();
    Item(QPoint pt,bool bBlack);
    ~Item();
    //为了调用Qvector的contains函数，需要重载==运算符
    bool operator==(const Item &t1)const
    {
        return ((m_pt == t1.m_pt) && (m_bBlack == t1.m_bBlack));
    }

    QPoint m_pt;
    bool m_bBlack;


};

