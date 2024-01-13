#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "command-packet.pb.h"
#include "publisher.h"
#include "logger.h"
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
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    Logger *logger;
    Publisher *publisher;
};

#endif // MAINWINDOW_H
