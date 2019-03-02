#ifndef KOORDINATENSYSTEM_H
#define KOORDINATENSYSTEM_H

#include <QWidget>

namespace Ui {
class Koordinatensystem;
}

class Koordinatensystem : public QWidget
{
    Q_OBJECT

public:
    explicit Koordinatensystem(QWidget *parent = 0);
    ~Koordinatensystem();

private:
    Ui::Koordinatensystem *ui;
};

#endif // KOORDINATENSYSTEM_H
