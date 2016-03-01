#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    double global_inacc_x;
    double global_inacc_y;
    int operator_index;
    double avg_fitness;
    double avg_inacc_x;
    double avg_inacc_y;
    int function_index;
    int count;
    bool operator1_state;
    bool operator2_state;
    bool operator3_state;
    bool operator4_state;
    bool operator5_state;
    bool operator6_state;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
};

int BinaryNumber();

int RandomNumber2(int a, int b);

double RandomNumber();

void CountOperator(int, int, int, double, double, double);

#endif // MAINWINDOW_H
