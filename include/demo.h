#ifndef DEMO_H
#define DEMO_H

#include <QMainWindow>

#include "idea_demo.hpp"

namespace Ui {
  class Demo;
}

class Demo : public QMainWindow
{
  Q_OBJECT

public:
  explicit Demo(QWidget *parent = 0);
  ~Demo();

private slots:
  void on_pushButton_start_clicked();

  void on_pushButton_next_clicked();

  void on_pushButton_prev_clicked();

  void on_comboBox_stages_currentIndexChanged(int index);

  void on_comboBox_base_data_currentIndexChanged(int index);

  void on_comboBox_base_key_currentIndexChanged(int index);

  void on_pushButton_genkey_clicked();

private:

  void FillInfoKeysEnc(const uint16_t i_keys[52]);

  void FillInfoKeysDec(const uint16_t i_keys[52]);

  void FillInfoRoundEnc(IdeaDemoRoundData i_round_data);

  void FillInfoRoundDec(IdeaDemoRoundData i_round_data);

  void FillInfoRoundFinEnc(IdeaDemoRoundData i_round_data);

  void FillInfoRoundFinDec(IdeaDemoRoundData i_round_data);

  Ui::Demo *ui;

  bool key_applied;
  QString key_string;
  QString block_string;
  uint8_t key_data[16];
  uint8_t block_data[8];
  uint8_t cipher_data[8];
  IdeaDemoContext idea_ctx;

  unsigned int base_number;
  unsigned int base_data;

  std::vector<IdeaDemoRoundData> rounds_data_encryption;
  std::vector<IdeaDemoRoundData> rounds_data_decryption;
};

#endif // DEMO_H
