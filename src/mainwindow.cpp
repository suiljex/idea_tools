#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QFileDialog>

#include <random>
#include <chrono>
#include <fstream>
#include <vector>

#define STATUS_BAR_TIMEOUT 1800

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  base_cryptotext(-1),
  base_key(2),
  key_applied(false)
{
  ui->setupUi(this);

  ui->comboBox_base_key->addItem("2");
  ui->comboBox_base_key->addItem("16");
  ui->comboBox_base_key->setCurrentIndex(1);

  ui->comboBox_base_crypt->addItem("base64");
  ui->comboBox_base_crypt->addItem("hex");
  ui->comboBox_base_crypt->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_genkey_clicked()
{
  //Генерация случайного ключа
  QString new_key;
  std::mt19937_64 rand_engine(std::chrono::system_clock::now().time_since_epoch().count());

  if (base_key == 2)
  {
    for (int i = 0; i < 128; ++i)
    {
      new_key += QString::number(rand_engine() % 2, base_key);
    }
  }
  else if(base_key == 16)
  {
    for (int i = 0; i < 32; ++i)
    {
      new_key += QString::number(rand_engine() % 16, base_key);
    }
  }

  ui->key_input->setText(new_key);
}

void MainWindow::on_pushButton_applykey_clicked()
{
  QString new_key(ui->key_input->text());

  bool success_conversion;
  int current_index = 0;

  key_string = new_key;

  //Проверка длины ключа
  if (base_key == 2 && new_key.length() == 128)
  {
    for (auto it = key_string.begin(); it != key_string.end(); it += 8)
    {
      QString temp_text_part(it, 8);
      key_data[current_index] = temp_text_part.toInt(&success_conversion, base_key);
      ++current_index;
    }
  }
  else if(base_key == 16 && new_key.length() == 32)
  {
    for (auto it = key_string.begin(); it != key_string.end(); it += 2)
    {
      QString temp_text_part(it, 2);
      key_data[current_index] = temp_text_part.toInt(&success_conversion, base_key);
      ++current_index;
    }
  }
  else
  {
    ui->statusBar->showMessage("Короткий ключ", STATUS_BAR_TIMEOUT);
    return;
  }

  IdeaInit(&idea_ctx, key_data, 16);

  ui->pushButton_applykey->setEnabled(false);
  key_applied = true;
  ui->statusBar->showMessage("Ключ принят", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_key_input_textChanged(const QString &/*arg1*/)
{
  //При изменении текста в строке появляется возможность применения текста
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

  ProcessText(input_data, output_data, true);

  if (base_cryptotext == 16)
  {
    ui->plainTextEdit_encrypted->setPlainText(output_data.toHex());
  }
  else if (base_cryptotext == 0)
  {
    ui->plainTextEdit_encrypted->setPlainText(output_data.toBase64());
  }

  ui->statusBar->showMessage("Успешно зашифровано", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_pushButton_text_decrypt_clicked()
{
  if (key_applied == false)
  {
    ui->statusBar->showMessage("Нет ключа", STATUS_BAR_TIMEOUT);
    return;
  }

  //Проверка на корректность ввода.
  //Входной текст должен быть закодирован в корректной кодировке
  QRegExpValidator input_check(reg_exp_crypt, this);
  QString temp_input_string(ui->plainTextEdit_encrypted->toPlainText());
  int temp_pos = 0;

  if (input_check.validate(temp_input_string, temp_pos) != QValidator::Acceptable)
  {
    ui->statusBar->showMessage("Некорректный ввод", STATUS_BAR_TIMEOUT);
    return;
  }

  QByteArray input_data;
  QByteArray output_data;

  if (base_cryptotext == 16)
  {
    input_data = QByteArray::fromHex(temp_input_string.toUtf8());
  }
  else if (base_cryptotext == 0)
  {
    input_data = QByteArray::fromBase64(temp_input_string.toUtf8());
  }

  ProcessText(input_data, output_data, false);

  ui->plainTextEdit_decrypted->setPlainText(QString::fromUtf8(output_data));
  ui->statusBar->showMessage("Успешно расшифровано", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_pushButton_open_file_srs_clicked()
{
  QString temp_filename;
  temp_filename = QFileDialog::getOpenFileName(this, tr("Открыть исходный файл"));
  ui->lineEdit_file_src->setText(temp_filename);
}

void MainWindow::on_pushButton_open_file_dst_clicked()
{
  QString temp_filename;
  temp_filename = QFileDialog::getSaveFileName(this, tr("Открыть целевой файл"));
  ui->lineEdit_file_dst->setText(temp_filename);
}

void MainWindow::on_pushButton_reset_clicked()
{
  ui->lineEdit_file_dst->clear();
  ui->lineEdit_file_src->clear();
}

void MainWindow::on_pushButton_file_encrypt_clicked()
{
  if (key_applied == false)
  {
    ui->statusBar->showMessage("Нет ключа", STATUS_BAR_TIMEOUT);
    return;
  }

  QFile fin(ui->lineEdit_file_src->text());
  QFile fout(ui->lineEdit_file_dst->text());

  if (fin.open(QIODevice::ReadOnly) == false)
  {
    ui->statusBar->showMessage("Не удалось открыть исходный файл", STATUS_BAR_TIMEOUT);
    fin.close();
    return;
  }

  if (fout.open(QIODevice::WriteOnly) == false)
  {
    ui->statusBar->showMessage("Не удалось открыть целевой файл", STATUS_BAR_TIMEOUT);
    fin.close();
    fout.close();
    return;
  }

  ProcessFile(fin, fout, true);

  fin.close();
  fout.close();
  ui->statusBar->showMessage("Файл успешно зашифрован", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_pushButton_file_decrypt_clicked()
{
  if (key_applied == false)
  {
    ui->statusBar->showMessage("Нет ключа", STATUS_BAR_TIMEOUT);
    return;
  }

  QFile fin(ui->lineEdit_file_src->text());
  QFile fout(ui->lineEdit_file_dst->text());

  if (fin.open(QIODevice::ReadOnly) == false)
  {
    ui->statusBar->showMessage("Не удалось открыть исходный файл", STATUS_BAR_TIMEOUT);
    fin.close();
    return;
  }

  if (fout.open(QIODevice::WriteOnly) == false)
  {
    ui->statusBar->showMessage("Не удалось открыть целевой файл", STATUS_BAR_TIMEOUT);
    fin.close();
    fout.close();
    return;
  }

  ProcessFile(fin, fout, false);

  fin.close();
  fout.close();
  ui->statusBar->showMessage("Файл успешно расшифрован", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_pushButton_files_switch_clicked()
{
  QString temp(ui->lineEdit_file_src->text());
  ui->lineEdit_file_src->setText(ui->lineEdit_file_dst->text());
  ui->lineEdit_file_dst->setText(temp);
}

void MainWindow::ProcessText(QByteArray &i_data, QByteArray &o_data, bool i_encrypt)
{
  uint8_t data_block_in[8] = {0};
  uint8_t data_block_out[8] = {0};

  //Пока не закончатся данные
  for (int i = 0; i < i_data.length(); ++i)
  {
    //Копирование данных в массив для шифрования
    data_block_in[i % 8] = i_data[i];

    //Если массив заполнился
    if (i % 8 == 7)
    {
      if (i_encrypt == true)
      {
        IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      }
      else
      {
        IdeaDecryptBlock(&idea_ctx, data_block_in, data_block_out);
      }

      //Данные дописываются в конец выходного массива
      for (int k = 0; k < 8; ++k)
      {
        o_data.append(data_block_out[k]);
      }
    }
    //Если дошли до конца
    else if (i == i_data.length() - 1)
    {
      //Заполнить конец входного массива нулями
      for (unsigned int j = (i % 8) + 1; j < 8; ++j)
      {
        data_block_in[j] = 0x00;
      }

      if (i_encrypt == true)
      {
        IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
      }
      else
      {
        IdeaDecryptBlock(&idea_ctx, data_block_in, data_block_out);
      }

      //Данные дописываются в конец выходного массива
      for (int k = 0; k < 8; ++k)
      {
        o_data.append(data_block_out[k]);
      }
    }
  }
}

void MainWindow::ProcessFile(QFile &i_fin, QFile &i_fout, bool i_encrypt)
{
  uint8_t data_block_in[8] = {0};
  uint8_t data_block_out[8] = {0};

  int file_size = i_fin.size();

  int temp_size_left;

  //Пока не дошли до конца файла
  while (i_fin.atEnd() == false)
  {
    //Вычисление оставшегося размера
    temp_size_left = file_size - i_fin.pos();

    //Если массив заполняется не полностью, в конец дописывабтся нули
    if (temp_size_left < 8)
    {
      i_fin.read(reinterpret_cast<char*>(data_block_in), temp_size_left);
      for (unsigned int j = temp_size_left; j < 8; ++j)
      {
        data_block_in[j] = 0x00;
      }
    }
    else
    {
      i_fin.read(reinterpret_cast<char*>(data_block_in), 8);
    }

    if (i_encrypt == true)
    {
      IdeaEncryptBlock(&idea_ctx, data_block_in, data_block_out);
    }
    else
    {
      IdeaDecryptBlock(&idea_ctx, data_block_in, data_block_out);
    }

    i_fout.write(reinterpret_cast<char*>(data_block_out), 8);
  }
}

void MainWindow::on_pushButton_key_choose_clicked()
{
  QString temp_filename;
  temp_filename = QFileDialog::getSaveFileName(this, tr("Открыть ключевой файл"));
  ui->lineEdit_key_file->setText(temp_filename);
}

void MainWindow::on_pushButton_key_load_clicked()
{
  QFile fin(ui->lineEdit_key_file->text());

  if (fin.open(QIODevice::ReadOnly) == false)
  {
    ui->statusBar->showMessage("Не удалось открыть ключевой файл", STATUS_BAR_TIMEOUT);
    fin.close();
    return;
  }

  int file_size = fin.size();
  if(file_size != 16)
  {
    ui->statusBar->showMessage("Некорректное содержимое ключевого файла", STATUS_BAR_TIMEOUT);
    return;
  }

  fin.read(reinterpret_cast<char*>(key_data), file_size);
  fin.close();

  QString new_key;

  for (int i = 0; i < 16; ++i)
  {
    if (base_key == 2)
    {
      new_key += QString("%1").arg(key_data[i], 8, base_key, QChar('0'));
    }
    else if(base_key == 16)
    {
      new_key += QString("%1").arg(key_data[i], 2, base_key, QChar('0'));
    }
  }

  ui->key_input->setText(new_key);

  ui->statusBar->showMessage("Ключ успешно загружен", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_pushButton_key_save_clicked()
{
  QFile fout(ui->lineEdit_key_file->text());

  if (fout.open(QIODevice::WriteOnly) == false)
  {
    ui->statusBar->showMessage("Не удалось открыть ключевой файл", STATUS_BAR_TIMEOUT);
    fout.close();
    return;
  }

  fout.write(reinterpret_cast<char*>(key_data), 16);

  fout.close();

  ui->statusBar->showMessage("Ключ успешно сохранен", STATUS_BAR_TIMEOUT);
}

void MainWindow::on_start_demo_triggered()
{
  form_demo = new Demo(this);
  form_demo->show();
}

void MainWindow::on_help_triggered()
{
  form_help = new Help(this);
  form_help->show();
}

void MainWindow::on_about_triggered()
{
  form_about = new About(this);
  form_about->show();
}

void MainWindow::on_comboBox_base_key_currentIndexChanged(int index)
{
  QRegExp reg_exp_key;

  ui->key_input->clear();

  if (index == 0)
  {
    //Регулярное выражение, описывающее последовательность из 0 и 1 длиной 128
    reg_exp_key.setPattern("^[0-1]{128}$");
    base_key = 2;
  }
  else if (index == 1)
  {
    //Регулярное выражение, описывающее последовательность из 0 - F длиной 32
    reg_exp_key.setPattern("^[0-9a-fA-F]{32}$");
    base_key = 16;
  }

  //Проверка ввода ключа на корректность
  ui->key_input->setValidator(new QRegExpValidator(reg_exp_key, this));
}

void MainWindow::on_comboBox_base_crypt_currentIndexChanged(int index)
{
  if (index == 0)
  {
    //Регулярное выражение, описывающее base64 строку
    reg_exp_crypt.setPattern("^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$");
    base_cryptotext = 0;
  }
  else if (index == 1)
  {
    //Регулярное выражение, описывающее последовательность из 0 - F длиной 8
    reg_exp_crypt.setPattern("^([0-9a-fA-F]{16})+$");
    base_cryptotext = 16;
  }
}
