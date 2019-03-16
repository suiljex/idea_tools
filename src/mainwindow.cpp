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
    return;
  }

  std::string plain_text(ui->plainTextEdit_decrypted->toPlainText().toStdString());
  std::string encrypted_text;

  uint8_t data_block_in[4];
  uint8_t data_block_out[4];

  for (unsigned int i = 0; i < plain_text.length(); ++i)
  {
    data_block_in[i % 4] = plain_text[i];

    if (i % 4 == 3)
    {
      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      encrypted_text += std::string(reinterpret_cast<char*>(data_block_out));
    }
    else if (i == plain_text.length() - 1)
    {
      for (unsigned int j = (i % 4) + 1; j < 4; ++j)
      {
        data_block_in[j] = 0x00;
      }

      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      encrypted_text += std::string(reinterpret_cast<char*>(data_block_out));
    }
  }

  ui->plainTextEdit_encrypted->setPlainText(QString::fromStdString(encrypted_text));
}

void MainWindow::on_pushButton_text_decrypt_clicked()
{
  if (key_applied == false)
  {
    return;
  }

  std::string plain_text(ui->plainTextEdit_encrypted->toPlainText().toStdString());
  std::string decrypted_text;

  uint8_t data_block_in[4];
  uint8_t data_block_out[4];

  for (unsigned int i = 0; i < plain_text.length(); ++i)
  {
    data_block_in[i % 4] = plain_text[i];

    if (i % 4 == 3)
    {
      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      decrypted_text += std::string(reinterpret_cast<char*>(data_block_out));
    }
    else if (i == plain_text.length() - 1)
    {
      for (unsigned int j = (i % 4) + 1; j < 4; ++j)
      {
        data_block_in[j] = 0x00;
      }

      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      decrypted_text += std::string(reinterpret_cast<char*>(data_block_out));
    }
  }

  ui->plainTextEdit_decrypted->setPlainText(QString::fromStdString(decrypted_text));
}
