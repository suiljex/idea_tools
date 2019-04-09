#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::About)
{
  ui->setupUi(this);
}

About::~About()
{
  delete ui;
}

void About::on_pushButton_egg_pressed()
{
  ui->plainTextEdit_logo->setPlainText("SLX");
}

void About::on_pushButton_egg_released()
{
  ui->plainTextEdit_logo->setPlainText("#<(");
}
