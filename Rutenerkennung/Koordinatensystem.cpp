#include "Koordinatensystem.h"
#include "ui_koordinatensystem.h"

Koordinatensystem::Koordinatensystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Koordinatensystem)
{
    ui->setupUi(this);
}

Koordinatensystem::~Koordinatensystem()
{
    delete ui;
}
