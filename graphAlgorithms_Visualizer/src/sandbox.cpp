#include "sandbox.h"
#include <QVBoxLayout>
#include <QPainter>

SandBox::SandBox(QWidget *parent)
{
    // Setup layout
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
}

void SandBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::blue);
    painter.drawEllipse(100,100,100,100);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, "Qt");
}
