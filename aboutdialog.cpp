#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QPixmap>
#include <QDebug>


AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->logoLabel->setPixmap(QPixmap(":/logo.png").scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));


}

AboutDialog::~AboutDialog()
{
    delete ui;
}
