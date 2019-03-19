#include "demo.h"
#include "ui_demo.h"

#include <random>
#include <chrono>
#include <vector>

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
  ui->groupBox_keys->setVisible(false);

  ui->groupBox_round->setEnabled(false);
  ui->groupBox_round->setVisible(false);

  ui->groupBox_placeholder->setEnabled(true);
  ui->groupBox_placeholder->setVisible(true);
}

Demo::~Demo()
{
  delete ui;
}

void Demo::on_pushButton_start_clicked()
{
  QString new_key(ui->edit_key->text());
  QString block_str(ui->edit_data->text());

  if (new_key.length() != 128)
  {
    ui->statusbar->showMessage("Короткий ключ", STATUS_BAR_TIMEOUT);
    return;
  }

  if (block_str.length() > 8)
  {
    ui->statusbar->showMessage("Слишком длинный блок", STATUS_BAR_TIMEOUT);
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

  uint8_t block_plain[8];
  uint8_t block_cipher[8];

  const char* temp_data = block_str.toStdString().c_str();
  unsigned int i, j;
  for (i = 0; i < strlen(temp_data) && i < 8; ++i)
  {
    block_plain[i] = temp_data[i];
  }
  for (j = i; j < 8; ++j)
  {
    block_plain[j] = 0x00;
  }

  IdeaDemoInit(&idea_ctx, key_data, 16);
  rounds_data_encryption = IdeaDemoEncryptBlock(&idea_ctx, block_plain, block_cipher);
  rounds_data_decryption = IdeaDemoDecryptBlock(&idea_ctx, block_cipher, block_plain);

  ui->comboBox_stages->addItem("Генерация Ключей раундов шифрования");
  ui->comboBox_stages->addItem("Раунд шифрования 1");
  ui->comboBox_stages->addItem("Раунд шифрования 2");
  ui->comboBox_stages->addItem("Раунд шифрования 3");
  ui->comboBox_stages->addItem("Раунд шифрования 4");
  ui->comboBox_stages->addItem("Раунд шифрования 5");
  ui->comboBox_stages->addItem("Раунд шифрования 6");
  ui->comboBox_stages->addItem("Раунд шифрования 7");
  ui->comboBox_stages->addItem("Раунд шифрования 8");
  ui->comboBox_stages->addItem("Финальный раунд шифрования");
  ui->comboBox_stages->addItem("Генерация Ключей раундов дешифрования");
  ui->comboBox_stages->addItem("Раунд дешифрования 1");
  ui->comboBox_stages->addItem("Раунд дешифрования 2");
  ui->comboBox_stages->addItem("Раунд дешифрования 3");
  ui->comboBox_stages->addItem("Раунд дешифрования 4");
  ui->comboBox_stages->addItem("Раунд дешифрования 5");
  ui->comboBox_stages->addItem("Раунд дешифрования 6");
  ui->comboBox_stages->addItem("Раунд дешифрования 7");
  ui->comboBox_stages->addItem("Раунд дешифрования 8");
  ui->comboBox_stages->addItem("Финальный раунд дешифрования");

  ui->comboBox_stages->setCurrentIndex(-1);

  ui->groupBox_placeholder->setEnabled(false);
  ui->groupBox_placeholder->setVisible(false);

  ui->pushButton_next->setEnabled(true);
  ui->pushButton_prev->setEnabled(true);
  ui->comboBox_stages->setEnabled(true);

  ui->pushButton_next->click();
}

void Demo::on_pushButton_next_clicked()
{
  ui->comboBox_stages->setCurrentIndex(ui->comboBox_stages->currentIndex() + 1);
}

void Demo::on_pushButton_prev_clicked()
{
  ui->comboBox_stages->setCurrentIndex(ui->comboBox_stages->currentIndex() - 1);
}

void Demo::on_comboBox_stages_currentIndexChanged(int index)
{
  QString temp_mess("Changed ");
  temp_mess += QString::number(index);

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

  ui->groupBox_keys->setEnabled(false);
  ui->groupBox_keys->setVisible(false);
  ui->groupBox_round->setEnabled(false);
  ui->groupBox_round->setVisible(false);

  if (index == 0)
  {
    ui->groupBox_keys->setEnabled(true);
    ui->groupBox_keys->setVisible(true);

    FillInfoKeysEnc(idea_ctx.keys_encryption);

    ui->key_main->setText(key_string);
  }
  else if (index == 10)
  {
    ui->groupBox_keys->setEnabled(true);
    ui->groupBox_keys->setVisible(true);

    FillInfoKeysDec(idea_ctx.keys_decryption);

    ui->key_main->setText(key_string);
  }

  ui->statusbar->showMessage(temp_mess, STATUS_BAR_TIMEOUT);
}

void Demo::FillInfoKeysEnc(const uint16_t i_keys[52])
{
  if (i_keys == nullptr)
  {
    return;
  }

  ui->key_title_1->setText("Сдвиг на 0");
  ui->key_title_2->setText("Сдвиг на 25");
  ui->key_title_3->setText("Сдвиг на 50");
  ui->key_title_4->setText("Сдвиг на 75");
  ui->key_title_5->setText("Сдвиг на 100");
  ui->key_title_6->setText("Сдвиг на 125");
  ui->key_title_7->setText("Сдвиг на 150");

  ui->key_1->setText(QString("EK1: ") + QString::number(i_keys[0], 2));
  ui->key_2->setText(QString("EK2: ") + QString::number(i_keys[1], 2));
  ui->key_3->setText(QString("EK3: ") + QString::number(i_keys[2], 2));
  ui->key_4->setText(QString("EK4: ") + QString::number(i_keys[3], 2));
  ui->key_5->setText(QString("EK5: ") + QString::number(i_keys[4], 2));
  ui->key_6->setText(QString("EK6: ") + QString::number(i_keys[5], 2));
  ui->key_7->setText(QString("EK7: ") + QString::number(i_keys[6], 2));
  ui->key_8->setText(QString("EK8: ") + QString::number(i_keys[7], 2));
  ui->key_9->setText(QString("EK9: ") + QString::number(i_keys[8], 2));
  ui->key_10->setText(QString("EK10: ") + QString::number(i_keys[9], 2));
  ui->key_11->setText(QString("EK11: ") + QString::number(i_keys[10], 2));
  ui->key_12->setText(QString("EK12: ") + QString::number(i_keys[11], 2));
  ui->key_13->setText(QString("EK13: ") + QString::number(i_keys[12], 2));
  ui->key_14->setText(QString("EK14: ") + QString::number(i_keys[13], 2));
  ui->key_15->setText(QString("EK15: ") + QString::number(i_keys[14], 2));
  ui->key_16->setText(QString("EK16: ") + QString::number(i_keys[15], 2));
  ui->key_17->setText(QString("EK17: ") + QString::number(i_keys[16], 2));
  ui->key_18->setText(QString("EK18: ") + QString::number(i_keys[17], 2));
  ui->key_19->setText(QString("EK19: ") + QString::number(i_keys[18], 2));
  ui->key_20->setText(QString("EK20: ") + QString::number(i_keys[19], 2));
  ui->key_21->setText(QString("EK21: ") + QString::number(i_keys[20], 2));
  ui->key_22->setText(QString("EK22: ") + QString::number(i_keys[21], 2));
  ui->key_23->setText(QString("EK23: ") + QString::number(i_keys[22], 2));
  ui->key_24->setText(QString("EK24: ") + QString::number(i_keys[23], 2));
  ui->key_25->setText(QString("EK25: ") + QString::number(i_keys[24], 2));
  ui->key_26->setText(QString("EK26: ") + QString::number(i_keys[25], 2));
  ui->key_27->setText(QString("EK27: ") + QString::number(i_keys[26], 2));
  ui->key_28->setText(QString("EK28: ") + QString::number(i_keys[27], 2));
  ui->key_29->setText(QString("EK29: ") + QString::number(i_keys[28], 2));
  ui->key_30->setText(QString("EK30: ") + QString::number(i_keys[29], 2));
  ui->key_31->setText(QString("EK31: ") + QString::number(i_keys[30], 2));
  ui->key_32->setText(QString("EK32: ") + QString::number(i_keys[31], 2));
  ui->key_33->setText(QString("EK33: ") + QString::number(i_keys[32], 2));
  ui->key_34->setText(QString("EK34: ") + QString::number(i_keys[33], 2));
  ui->key_35->setText(QString("EK35: ") + QString::number(i_keys[34], 2));
  ui->key_36->setText(QString("EK36: ") + QString::number(i_keys[35], 2));
  ui->key_37->setText(QString("EK37: ") + QString::number(i_keys[36], 2));
  ui->key_38->setText(QString("EK38: ") + QString::number(i_keys[37], 2));
  ui->key_39->setText(QString("EK39: ") + QString::number(i_keys[38], 2));
  ui->key_40->setText(QString("EK40: ") + QString::number(i_keys[39], 2));
  ui->key_41->setText(QString("EK41: ") + QString::number(i_keys[40], 2));
  ui->key_42->setText(QString("EK42: ") + QString::number(i_keys[41], 2));
  ui->key_43->setText(QString("EK43: ") + QString::number(i_keys[42], 2));
  ui->key_44->setText(QString("EK44: ") + QString::number(i_keys[43], 2));
  ui->key_45->setText(QString("EK45: ") + QString::number(i_keys[44], 2));
  ui->key_46->setText(QString("EK46: ") + QString::number(i_keys[45], 2));
  ui->key_47->setText(QString("EK47: ") + QString::number(i_keys[46], 2));
  ui->key_48->setText(QString("EK48: ") + QString::number(i_keys[47], 2));
  ui->key_49->setText(QString("EK49: ") + QString::number(i_keys[48], 2));
  ui->key_50->setText(QString("EK50: ") + QString::number(i_keys[49], 2));
  ui->key_51->setText(QString("EK51: ") + QString::number(i_keys[50], 2));
  ui->key_52->setText(QString("EK52: ") + QString::number(i_keys[51], 2));
  ui->key_53_d->clear();
  ui->key_54_d->clear();
  ui->key_55_d->clear();
  ui->key_56_d->clear();
}

void Demo::FillInfoKeysDec(const uint16_t i_keys[52])
{
  if (i_keys == nullptr)
  {
    return;
  }

  ui->key_title_1->clear();
  ui->key_title_2->clear();
  ui->key_title_3->clear();
  ui->key_title_4->clear();
  ui->key_title_5->clear();
  ui->key_title_6->clear();
  ui->key_title_7->clear();

  ui->key_1->setText(QString("DK1 = EK49^(-1): ") + QString::number(i_keys[0], 2));
  ui->key_2->setText(QString("DK2 = -EK50: ") + QString::number(i_keys[1], 2));
  ui->key_3->setText(QString("DK3 = -EK51: ") + QString::number(i_keys[2], 2));
  ui->key_4->setText(QString("DK4 = EK52^(-1): ") + QString::number(i_keys[3], 2));
  ui->key_5->setText(QString("DK5 = EK47: ") + QString::number(i_keys[4], 2));
  ui->key_6->setText(QString("DK6 = EK48: ") + QString::number(i_keys[5], 2));
  ui->key_7->setText(QString("DK7 = EK43^(-1): ") + QString::number(i_keys[6], 2));
  ui->key_8->setText(QString("DK8 = -EK44: ") + QString::number(i_keys[7], 2));
  ui->key_9->setText(QString("DK9 = -EK45:  ") + QString::number(i_keys[8], 2));
  ui->key_10->setText(QString("DK10 = EK46^(-1): ") + QString::number(i_keys[9], 2));
  ui->key_11->setText(QString("DK11 = EK41: ") + QString::number(i_keys[10], 2));
  ui->key_12->setText(QString("DK12 = EK42: ") + QString::number(i_keys[11], 2));
  ui->key_13->setText(QString("DK13 = EK37^(-1): ") + QString::number(i_keys[12], 2));
  ui->key_14->setText(QString("DK14 = -EK38: ") + QString::number(i_keys[13], 2));
  ui->key_15->setText(QString("DK15 = -EK39: ") + QString::number(i_keys[14], 2));
  ui->key_16->setText(QString("DK16 = EK40^(-1): ") + QString::number(i_keys[15], 2));
  ui->key_17->setText(QString("DK17 = EK35: ") + QString::number(i_keys[16], 2));
  ui->key_18->setText(QString("DK18 = EK36: ") + QString::number(i_keys[17], 2));
  ui->key_19->setText(QString("DK19 = EK31^(-1): ") + QString::number(i_keys[18], 2));
  ui->key_20->setText(QString("DK20 = -EK32: ") + QString::number(i_keys[19], 2));
  ui->key_21->setText(QString("DK21 = -EK33: ") + QString::number(i_keys[20], 2));
  ui->key_22->setText(QString("DK22 = EK34^(-1): ") + QString::number(i_keys[21], 2));
  ui->key_23->setText(QString("DK23 = EK29: ") + QString::number(i_keys[22], 2));
  ui->key_24->setText(QString("DK24 = EK30: ") + QString::number(i_keys[23], 2));
  ui->key_25->setText(QString("DK25 = EK25^(-1): ") + QString::number(i_keys[24], 2));
  ui->key_26->setText(QString("DK26 = -EK26: ") + QString::number(i_keys[25], 2));
  ui->key_27->setText(QString("DK27 = -EK27: ") + QString::number(i_keys[26], 2));
  ui->key_28->setText(QString("DK28 = EK28^(-1): ") + QString::number(i_keys[27], 2));
  ui->key_29->setText(QString("DK29 = EK23: ") + QString::number(i_keys[28], 2));
  ui->key_30->setText(QString("DK30 = EK24: ") + QString::number(i_keys[29], 2));
  ui->key_31->setText(QString("DK31 = EK19^(-1): ") + QString::number(i_keys[30], 2));
  ui->key_32->setText(QString("DK32 = -EK20: ") + QString::number(i_keys[31], 2));
  ui->key_33->setText(QString("DK33 = -EK21: ") + QString::number(i_keys[32], 2));
  ui->key_34->setText(QString("DK34 = EK22^(-1): ") + QString::number(i_keys[33], 2));
  ui->key_35->setText(QString("DK35 = EK17: ") + QString::number(i_keys[34], 2));
  ui->key_36->setText(QString("DK36 = EK18: ") + QString::number(i_keys[35], 2));
  ui->key_37->setText(QString("DK37 = EK13^(-1): ") + QString::number(i_keys[36], 2));
  ui->key_38->setText(QString("DK38 = -EK14: ") + QString::number(i_keys[37], 2));
  ui->key_39->setText(QString("DK39 = -EK15: ") + QString::number(i_keys[38], 2));
  ui->key_40->setText(QString("DK40 = EK16^(-1): ") + QString::number(i_keys[39], 2));
  ui->key_41->setText(QString("DK41 = EK11: ") + QString::number(i_keys[40], 2));
  ui->key_42->setText(QString("DK42 = EK12: ") + QString::number(i_keys[41], 2));
  ui->key_43->setText(QString("DK43 = EK7^(-1): ") + QString::number(i_keys[42], 2));
  ui->key_44->setText(QString("DK44 = -EK8: ") + QString::number(i_keys[43], 2));
  ui->key_45->setText(QString("DK45 = -EK9: ") + QString::number(i_keys[44], 2));
  ui->key_46->setText(QString("DK46 = EK10^(-1): ") + QString::number(i_keys[45], 2));
  ui->key_47->setText(QString("DK47 = EK5: ") + QString::number(i_keys[46], 2));
  ui->key_48->setText(QString("DK48 = EK6: ") + QString::number(i_keys[47], 2));
  ui->key_49->setText(QString("DK49 = EK1^(-1): ") + QString::number(i_keys[48], 2));
  ui->key_50->setText(QString("DK50 = -EK2: ") + QString::number(i_keys[49], 2));
  ui->key_51->setText(QString("DK51 = -EK3: ") + QString::number(i_keys[50], 2));
  ui->key_52->setText(QString("DK52 = EK4^(-1): ") + QString::number(i_keys[51], 2));
  ui->key_53_d->clear();
  ui->key_54_d->clear();
  ui->key_55_d->clear();
  ui->key_56_d->clear();
}
