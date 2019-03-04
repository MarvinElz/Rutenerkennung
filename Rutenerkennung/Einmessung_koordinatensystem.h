#ifndef EINMESSUNG_KOORDINATENSYSTEM_H
#define EINMESSUNG_KOORDINATENSYSTEM_H

#include <QWidget>
#include <iostream>
#include <QInputEvent>

using namespace std;

namespace Ui {
class Einmessung_Koordinatensystem;
}

class Einmessung_Koordinatensystem : public QWidget
{
    Q_OBJECT

public:
    explicit Einmessung_Koordinatensystem(QWidget *parent = 0);
    ~Einmessung_Koordinatensystem();

private slots:
    void on_label_Clicked(QMouseEvent *);

private:
    Ui::Einmessung_Koordinatensystem *ui;
};

#endif // EINMESSUNG_KOORDINATENSYSTEM_H
