#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <QRegExpValidator>

#include <random>
#include <chrono>

#define STATUS_BAR_TIMEOUT 1800

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  key_applied(false)
{
  QRegExp reg_exp_key("^(1|0){128}$");

  ui->setupUi(this);
  ui->key_input->setValidator(new QRegExpValidator(reg_exp_key, this));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_genkey_clicked()
{
  QString new_key;
  std::mt19937_64 rand_engine(std::chrono::system_clock::now().time_since_epoch().count());

  for (int i = 0; i < 128; ++i)
  {
    new_key += QString::number(rand_engine() % 2);
  }

  ui->key_input->setText(new_key);
}

void MainWindow::on_pushButton_applykey_clicked()
{
  QString new_key(ui->key_input->text());

  if (new_key.length() == 128)
  {
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

    ui->pushButton_applykey->setEnabled(false);
    key_applied = true;
    ui->statusBar->showMessage("Ключ принят", STATUS_BAR_TIMEOUT);
  }
  else
  {
    ui->statusBar->showMessage("Короткий ключ", STATUS_BAR_TIMEOUT);
  }
}

void MainWindow::on_key_input_textChanged(const QString &/*arg1*/)
{
  ui->pushButton_applykey->setEnabled(true);
}

void MainWindow::on_exit_triggered()
{
  QApplication::quit();
}

void MainWindow::on_pushButton_text_encrypt_clicked()
{
  if (key_applied == false)
  {
    ui->statusBar->showMessage("Нет ключа", STATUS_BAR_TIMEOUT);
    return;
  }

  QByteArray input_data(ui->plainTextEdit_decrypted->toPlainText().toUtf8());
  QByteArray output_data;

  uint8_t data_block_in[8] = {0};
  uint8_t data_block_out[8] = {0};

  for (int i = 0; i < input_data.length(); ++i)
  {
    data_block_in[i % 8] = input_data[i];

    if (i % 8 == 7)
    {
      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      for (int k = 0; k < 8; ++k)
      {
        output_data.append(data_block_out[k]);
      }
    }
    else if (i == input_data.length() - 1)
    {
      for (unsigned int j = (i % 8) + 1; j < 8; ++j)
      {
        data_block_in[j] = 0x00;
      }

      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      for (int k = 0; k < 8; ++k)
      {
        output_data.append(data_block_out[k]);
      }
    }
  }

  ui->plainTextEdit_encrypted->setPlainText(output_data.toBase64());
  ui->statusBar->showMessage("Успешно зашифровано", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_pushButton_text_decrypt_clicked()
{
  if (key_applied == false)
  {
    ui->statusBar->showMessage("Нет ключа", STATUS_BAR_TIMEOUT);
    return;
  }

  QRegExp reg_exp_key("^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$");
  QRegExpValidator input_check(reg_exp_key, this);
  QString temp_input_string(ui->plainTextEdit_encrypted->toPlainText());
  int temp_pos = 0;

  if (input_check.validate(temp_input_string, temp_pos) != QValidator::Acceptable)
  {
    ui->statusBar->showMessage("Некорректный ввод", STATUS_BAR_TIMEOUT);
    return;
  }


  QByteArray input_data(QByteArray::fromBase64(temp_input_string.toUtf8()));
  QByteArray output_data;

  uint8_t data_block_in[8] = {0};
  uint8_t data_block_out[8] = {0};

  for (int i = 0; i < input_data.length(); ++i)
  {
    data_block_in[i % 8] = input_data[i];

    if (i % 8 == 7)
    {
      IdeaDecryptBlock(&idea_ctx, data_block_in, data_block_out);
      for (int k = 0; k < 8; ++k)
      {
        output_data.append(data_block_out[k]);
      }
    }
    else if (i == input_data.length() - 1)
    {
      for (unsigned int j = (i % 8) + 1; j < 8; ++j)
      {
        data_block_in[j] = 0x00;
      }

      IdeaDecryptBlock(&idea_ctx, data_block_in, data_block_out);
      for (int k = 0; k < 8; ++k)
      {
        output_data.append(data_block_out[k]);
      }
    }
  }

  ui->plainTextEdit_decrypted->setPlainText(QString::fromUtf8(output_data));
  ui->statusBar->showMessage("Успешно расшифровано", STATUS_BAR_TIMEOUT);
}
