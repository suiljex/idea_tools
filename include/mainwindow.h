#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

#include <fstream>

#include "idea.hpp"
#include "demo.h"
#include "about.h"
#include "help.h"

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

  void on_pushButton_open_file_srs_clicked();

  void on_pushButton_open_file_dst_clicked();

  void on_pushButton_reset_clicked();

  void on_pushButton_file_encrypt_clicked();

  void on_pushButton_file_decrypt_clicked();

  void on_pushButton_files_switch_clicked();

  void ProcessText(QByteArray& i_data, QByteArray& o_data, bool i_encrypt);

  void ProcessFile(QFile &i_fin, QFile &i_fout, bool i_encrypt);

  void on_pushButton_key_choose_clicked();

  void on_pushButton_key_load_clicked();

  void on_pushButton_key_save_clicked();

  void on_start_demo_triggered();

  void on_help_triggered();

  void on_about_triggered();

private:
  Ui::MainWindow *ui;

  Demo *form_demo;
  About *form_about;
  Help* form_help;

  bool key_applied;
  QString key_string;
  uint8_t key_data[16];
  IdeaContext idea_ctx;
};

#endif // MAINWINDOW_H
