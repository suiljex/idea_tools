#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "idea.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_pushButton_genkey_clicked();

  void on_pushButton_applykey_clicked();

  void on_key_input_textChanged(const QString &arg1);

  void on_exit_triggered();

  void on_pushButton_text_encrypt_clicked();

  void on_pushButton_text_decrypt_clicked();

private:
  Ui::MainWindow *ui;

  bool key_applied;
  QString key_string;
  uint8_t key_data[16];
  IdeaContext idea_ctx;
};

#endif // MAINWINDOW_H
