#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class MySyntaxhighlighter;


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

    void on_open_clicked();

    void on_save_clicked();

    void on_save_other_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    MySyntaxhighlighter *highlighter;
};

#endif // MAINWINDOW_H
