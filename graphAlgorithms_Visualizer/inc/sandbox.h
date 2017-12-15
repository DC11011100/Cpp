#ifndef SANDBOX_H
#define SANDBOX_H

#include <QWidget>

class SandBox : public QWidget
{
    public:
        SandBox(QWidget*);

    protected:
        void paintEvent(QPaintEvent *);

    private:

};

#endif // SANDBOX_H
