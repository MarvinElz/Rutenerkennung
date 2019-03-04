#include "Einmessung_koordinatensystem.h"
#include "ui_einmessung_koordinatensystem.h"

Einmessung_Koordinatensystem::Einmessung_Koordinatensystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Einmessung_Koordinatensystem)
{
    ui->setupUi(this);
}

Einmessung_Koordinatensystem::~Einmessung_Koordinatensystem()
{
    delete ui;
}

void Einmessung_Koordinatensystem::on_label_Clicked(QMouseEvent *event)
{
    cout << "LabelClicked " << event->pos().x() << " , " << event->pos().y() << endl;
}
