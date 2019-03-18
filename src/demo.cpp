#include "demo.h"
#include "ui_demo.h"

#include <random>
#include <chrono>

#define STATUS_BAR_TIMEOUT 1800

Demo::Demo(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::Demo)
{
  QRegExp reg_exp_key("^(1|0){128}$");

  ui->setupUi(this);
  ui->edit_key->setValidator(new QRegExpValidator(reg_exp_key, this));

  QString new_key;
  std::mt19937_64 rand_engine(std::chrono::system_clock::now().time_since_epoch().count());

  for (int i = 0; i < 128; ++i)
  {
    new_key += QString::number(rand_engine() % 2);
  }

  ui->edit_key->setText(new_key);

  ui->pushButton_next->setEnabled(false);
  ui->pushButton_prev->setEnabled(false);
  ui->comboBox_stages->setEnabled(false);

  ui->groupBox_keys->setEnabled(false);
  ui->groupBox_keys->setVisible(true);

  ui->groupBox_round->setEnabled(false);
  ui->groupBox_round->setVisible(false);

  ui->groupBox_fin->setEnabled(false);
  ui->groupBox_fin->setVisible(false);
}

Demo::~Demo()
{
  delete ui;
}

void Demo::on_pushButton_start_clicked()
{
  QString new_key(ui->edit_key->text());

  if (new_key.length() != 128)
  {
    ui->statusbar->showMessage("Короткий ключ", STATUS_BAR_TIMEOUT);
    return;
  }

  bool success_conversion;
  int current_index = 0;

  key_string = new_key;

  for (auto it = key_string.begin(); it != key_string.end(); it += 8)
  {
    QString temp_text_part(it, 8);
    key_data[current_index] = temp_text_part.toInt(&success_conversion, 2);
    ++current_index;
  }

  IdeaInit(&idea_ctx, key_data, 16);

  ui->comboBox_stages->addItem("Генерация Ключей раундов шифрования");
  ui->comboBox_stages->addItem("Раунд шифрования 1");
  ui->comboBox_stages->addItem("Раунд шифрования 2");
  ui->comboBox_stages->addItem("Раунд шифрования 3");
  ui->comboBox_stages->addItem("Раунд шифрования 4");
  ui->comboBox_stages->addItem("Раунд шифрования 5");
  ui->comboBox_stages->addItem("Раунд шифрования 6");
  ui->comboBox_stages->addItem("Раунд шифрования 7");
  ui->comboBox_stages->addItem("Раунд шифрования 8");
  ui->comboBox_stages->addItem("Генерация Ключей раундов дешифрования");
  ui->comboBox_stages->addItem("Раунд дешифрования 1");
  ui->comboBox_stages->addItem("Раунд дешифрования 2");
  ui->comboBox_stages->addItem("Раунд дешифрования 3");
  ui->comboBox_stages->addItem("Раунд дешифрования 4");
  ui->comboBox_stages->addItem("Раунд дешифрования 5");
  ui->comboBox_stages->addItem("Раунд дешифрования 6");
  ui->comboBox_stages->addItem("Раунд дешифрования 7");
  ui->comboBox_stages->addItem("Раунд дешифрования 8");

  ui->comboBox_stages->setCurrentIndex(-1);

  ui->groupBox_round->setEnabled(true);
  //ui->groupBox_keys->setVisible(false);
  //ui->groupBox_round->setVisible(true);

  ui->pushButton_next->setEnabled(true);
  ui->pushButton_prev->setEnabled(true);
  ui->comboBox_stages->setEnabled(true);

  ui->pushButton_next->click();
}

void Demo::on_pushButton_next_clicked()
{
  ui->comboBox_stages->setCurrentIndex(ui->comboBox_stages->currentIndex() + 1);

  ui->pushButton_next->setEnabled(true);
  ui->pushButton_prev->setEnabled(true);

  if (ui->comboBox_stages->currentIndex() == 0)
  {
    ui->pushButton_prev->setEnabled(false);
  }

  if (ui->comboBox_stages->currentIndex() == ui->comboBox_stages->count() - 1)
  {
    ui->pushButton_next->setEnabled(false);
  }
}

void Demo::on_pushButton_prev_clicked()
{
  ui->comboBox_stages->setCurrentIndex(ui->comboBox_stages->currentIndex() - 1);

  ui->pushButton_next->setEnabled(true);
  ui->pushButton_prev->setEnabled(true);

  if (ui->comboBox_stages->currentIndex() == 0)
  {
    ui->pushButton_prev->setEnabled(false);
  }

  if (ui->comboBox_stages->currentIndex() == ui->comboBox_stages->count() - 1)
  {
    ui->pushButton_next->setEnabled(false);
  }
}

void Demo::on_comboBox_stages_currentIndexChanged(int index)
{
  QString temp_mess("Changed ");
  temp_mess += QString::number(index);
  ui->statusbar->showMessage(temp_mess, STATUS_BAR_TIMEOUT);
}
