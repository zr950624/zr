#include "mysyntaxhighlighter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    highlighter = new MySyntaxhighlighter(ui->textEdit->document());

}

MainWindow::~MainWindow()
{
    delete ui;
}





void MainWindow::on_open_clicked()
{
    QString fn = QFileDialog::getOpenFileName(this,tr("读取文件"),"C:",tr("c语言源文件(*c)"));
    if (!fn.isEmpty())
        {
            if(QFile::exists (fn))
            {
                QFile file(fn);
                if(file.open(QFile::ReadOnly))
                {
                        fileName=fn;
                        QByteArray data=file.readAll ();
                        ui->textEdit->setText (QObject::tr(data));
                        setWindowTitle (fileName+"-QNotepad");
                }
                else
                {
                    QMessageBox::warning(this,tr("失败"),tr("打开文件失败!"));
                }
            }
            else
            {
                QMessageBox::warning(this,tr("失败"),tr("文件不存在!"));
            }
        }

}

void MainWindow::on_save_clicked()
{
    if(fileName.isEmpty ())
        {
            on_save_clicked();
        }
    else
    {
        QFile file(fileName);
        file.open (QFile::WriteOnly);
        QTextStream out(&file);
        out<<ui->textEdit->toPlainText();
        setWindowTitle (fileName+"-QNotepad");
     }
}

void MainWindow::on_save_other_clicked()
{
    QString fn = QFileDialog::getSaveFileName (this, tr("另存为..."), "c:",tr("文本文件 (*.c)"));
    if (!fn.isEmpty())
    {
        if(!fn.endsWith(".c", Qt::CaseInsensitive))
        {
            fileName=fn+".c";
        }
        else
        {
            fileName=fn;
        }
        on_save_clicked();
    }
}
