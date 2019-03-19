#ifndef DEMO_H
#define DEMO_H

#include <QMainWindow>

#include "idea.hpp"

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

private:

  void FillInfoKeysEnc(const uint16_t i_keys[52]);

  Ui::Demo *ui;

  bool key_applied;
  QString key_string;
  uint8_t key_data[16];
  IdeaContext idea_ctx;
};

#endif // DEMO_H
