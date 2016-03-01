#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"
#include "time.h"
#include "dialog.h"
#include <QtScript/QScriptEngine>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int y=(unsigned)time(NULL);
    srand(y);
    rand();

    avg_inacc_x = 0;
    avg_inacc_y = 0;
    global_inacc_x = 0;
    global_inacc_y = 0;
    operator_index = 0; //contains index of the previously used operator
    count = 0; //contains number of uses of previous combination of the function and the operator in a row
    avg_fitness = 0;
    function_index = 0; //contains index of the previously used function
    operator1_state = false;
    operator2_state = false;
    operator3_state = false;
    operator4_state = false;
    operator5_state = false;
    operator6_state = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

double RandomNumber() {
    return (double)rand()/(RAND_MAX+1);
}

int RandomNumber2(int a, int b) {
    int result;
    result=a+int(RandomNumber()*(b-a));
    if (result==b) result=b-1;
    return result;
}

int BinaryNumber() {
    double x=RandomNumber();
    int y;
    if (x>=0.5) y=1;
    else y=0;
    return y;
}


void MainWindow::on_pushButton_clicked()
{
    bool operator1 = ui->radioButton->isChecked(); //choosing an operator of interbreeding
    bool operator2 = ui->radioButton_2->isChecked();
    bool operator3 = ui->radioButton_3->isChecked();
    bool operator4 = ui->radioButton_4->isChecked();
    bool operator5 = ui->radioButton_5->isChecked();
    bool operator6 = ui->radioButton_6->isChecked();
    bool operator_continue = false;
    if ((operator1 == true && operator1 == operator1_state) || (operator2 == true && operator2 == operator2_state) || (operator3 == true && operator3_state) || (operator4 == true && operator4 == operator4_state) || (operator5 == true && operator5_state) || (operator6 == true && operator6 == operator6_state)) operator_continue = true;
    else operator_continue = false;

    int current_function_index = ui->comboBox->currentIndex();
    if (current_function_index == function_index && operator_continue == true) count++;
    else {
        avg_inacc_x = 0;
        avg_inacc_y = 0;
        global_inacc_x = 0;
        global_inacc_y = 0;
        avg_fitness = 0;
        count = 1;
    }

    double left_lim = ui->doubleSpinBox->value();
    double right_lim = ui->doubleSpinBox_2->value(); // function linits
    int M = ui->spinBox->value(); //number of strings
    int N_X = ui->spinBox_2->value(); //number of bits for x arg
    int N_Y = ui->spinBox_5->value(); //number of bits for y arg
    int cycle = ui->spinBox_3->value(); // number of generations
    
    int **mat_x; // matrix initialization for x arg
    mat_x = new int*[M];
    for (int i = 0; i < M; i++) mat_x[i] = new int[N_X];
    
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N_X; j++) {
            mat_x[i][j] = BinaryNumber();
        }
    }
    
    int **mat_y; // matrix initialization for y arg
    mat_y = new int*[M];
    for (int i = 0; i < M; i++) mat_y[i] = new int[N_Y];
    
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N_Y; j++) {
            mat_y[i][j] = BinaryNumber();
        }
    }
    
    QString matrix, string;
    
    for (int i = 0; i < M; i++) {
        string = "";
        for (int j = 0; j < N_X; j++) string += QString::number(mat_x[i][j]) + " ";
        for (int j = 0; j < N_Y; j++) string += QString::number(mat_y[i][j]) + " ";
        matrix += string + "\n";      //matrix x+y output
    }
    ui->textEdit->insertPlainText(matrix);
    
    function_index = ui->comboBox->currentIndex();

    double *f = new double[M], *value_x = new double[M], *value_y = new double[M];
    for (int i = 0; i < M; i++) {
        f[i] = 0, value_x[i] = 0, value_y[i] = 0;
        for (int j = 0; j < N_X; j++) value_x[i] += pow(2, N_X-(j+1)) * mat_x[i][j];
        for (int j = 0; j < N_Y; j++) value_y[i] += pow(2, N_Y-(j+1)) * mat_y[i][j];
        value_x[i] = value_x[i] * (right_lim - left_lim) / double(pow(2, N_X) - 1) + left_lim;
        value_y[i] = value_y[i] * (right_lim - left_lim) / double(pow(2, N_Y) - 1) + left_lim; //conversion of x&y strings into real numbers
        if (function_index == 0) {
            f[i] = value_x[i] * value_x[i] + value_y[i] * value_y[i];      //fitness function declaration and count
            f[i] = -f[i]; //<=searching for min. value
        } else if (function_index == 1) {
            f[i] = 0.5 * (value_x[i] * value_x[i] + value_x[i] * value_y[i] + value_y[i] * value_y[i]) * (1 + 0.5 * cos(1.5 * value_x[i]) * cos(3.2 * value_x[i] * value_y[i]) * cos(3.14 * value_y[i]) + 0.5 * cos(2.2 * value_x[i]) * cos(4.8 * value_x[i] * value_y[i]) * cos(3.5 * value_y[i])); //fitness function declaration and count
            f[i] = -f[i]; //<=searching for min. value
        } else if (function_index == 2) {
            f[i] = value_x[i] * value_x[i] * fabs(sin(2 * value_x[i])) + value_y[i] * value_y[i] * fabs(sin(2 * value_y[i])) - 1/(5 * value_x[i] * value_x [i] + 5 * value_y[i] * value_y[i] + 0.2) + 5; //fitness function declaration and count
            f[i] = -f[i]; //<=searching for min. value
        }
    }
    
    int *best_smp = new int[N_X + N_Y]; //the fittest string
    double best_smpf = f[0], best_x = value_x[0], best_y = value_y[0];            //its value
    
    for (int i = 0; i < M; i++) {
        if (f[i] > best_smpf) {
            best_smpf = f[i];  //best fitness value
            best_x = value_x[i];
            best_y = value_y[i];
            for (int j = 0; j < N_X; j++) {
                best_smp[j] = mat_x[i][j];
            }
            for (int j = N_X; j < N_X + N_Y; j++) {
                best_smp[j] = mat_y[i][j - N_X];  //string with the best fitness value
            }
        }
    }
    for (int i = 0; i < M; i++) ui->textEdit->insertPlainText(QString::number(f[i]) + "\n");
    
    int *spec = new int[N_X + N_Y];    //declaration of new specimen
    int **newmat;                   //declaration of new matrix filled with new specimens
    newmat = new int*[M];
    for (int i = 0; i < M; i++) newmat[i] = new int[N_X + N_Y];
    int **newmat_x;
    newmat_x = new int*[M];
    for (int i = 0; i < M; i++) newmat_x[i] = new int[N_X];
    int **newmat_y;
    newmat_y = new int*[M];
    for (int i = 0; i < M; i++) newmat_y[i] = new int[N_Y];
    double *newf = new double[M], *newvalue_x = new double[M], *newvalue_y = new double[M]; //arrays for saving new values

    for (int z = 0; z < cycle; z++) {         //process begins
        
        for (int i = 0; i < M; i++) {;
            int par1, par1_1, par1_2; //first parent selection
            par1_1 = RandomNumber2(0, M);
            par1_2 = RandomNumber2(0, M);
            if (f[par1_1] > f[par1_2]) par1 = par1_1;
            else par1 = par1_2;
            int par2, par2_1, par2_2; //second parent selection
            par2_1 = RandomNumber2(0, M);
            par2_2 = RandomNumber2(0, M);
            if (f[par2_1] > f[par2_2]) par2 = par2_1;
            else par2 = par2_2;
            
            if (operator1 == true) {                  // first operator
                int BreakPoint = RandomNumber2(1, N_X + N_Y); //division point
                
                if (BreakPoint < N_X) {
                    for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint == N_X){
                    for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                }                                                      //new specimen
                
                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator1_state = true;
                operator2_state = false, operator3_state = false, operator4_state = false, operator5_state = false, operator6_state = false;
            } else if (operator2 == true) {             //second specimen
                int BreakPoint1 = RandomNumber2(1, N_X + N_Y); //division point
                int BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                
                if (BreakPoint1 == BreakPoint2) {     // in case first breakpoint == second one
                    while (BreakPoint1 == BreakPoint2) {
                        BreakPoint1 = RandomNumber2(1, N_X + N_Y);
                        BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                    }
                }
                else if (BreakPoint2 > BreakPoint1)  // in case second breakpoint > first one
                {
                    int BreakPointX = BreakPoint1;
                    BreakPoint1 = BreakPoint2;
                    BreakPoint2 = BreakPointX;
                }
                
                if (BreakPoint1 < N_X && BreakPoint2 < N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 == N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 == N_X && BreakPoint2 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 > N_X && BreakPoint2 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint1; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                }                          //new specimen
                
                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator2_state = true;
                operator1_state = false, operator3_state = false, operator4_state = false, operator5_state = false, operator6_state = false;
            } else if (operator3 == true) {             //third operator
                int BreakPoint1 = RandomNumber2(1, N_X + N_Y); //division point
                int BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                int BreakPoint3 = RandomNumber2(1, N_X + N_Y);
                
                if (BreakPoint1 == BreakPoint2 || BreakPoint1 == BreakPoint3 || BreakPoint2 == BreakPoint3) {
                    while (BreakPoint1 == BreakPoint2 || BreakPoint1 == BreakPoint3 || BreakPoint2 == BreakPoint3) {
                        BreakPoint1 = RandomNumber2(1, N_X + N_Y);
                        BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                        BreakPoint3 = RandomNumber2(1, N_X + N_Y);
                    }
                } else if (BreakPoint1 > BreakPoint2 && BreakPoint2 > BreakPoint3) { //in case the sequence is NOT bp3 > bp2 > bp1
                    int BreakPointX = BreakPoint1;
                    BreakPoint1 = BreakPoint3;
                    BreakPoint3 = BreakPointX;
                } else if (BreakPoint1 > BreakPoint3 && BreakPoint3 > BreakPoint2) {
                    int BreakPointX = BreakPoint3;
                    BreakPoint3 = BreakPoint1;
                    BreakPoint1 = BreakPoint2;
                    BreakPoint2 = BreakPointX;
                } else if (BreakPoint2 > BreakPoint1 && BreakPoint1 > BreakPoint3) {
                    int BreakPointX = BreakPoint3;
                    BreakPoint3 = BreakPoint2;
                    BreakPoint2 = BreakPoint1;
                    BreakPoint1 = BreakPointX;
                } else if (BreakPoint2 > BreakPoint3 && BreakPoint3 > BreakPoint1) {
                    int BreakPointX = BreakPoint2;
                    BreakPoint2 = BreakPoint3;
                    BreakPoint3 = BreakPointX;
                } else if (BreakPoint3 > BreakPoint1 && BreakPoint1 > BreakPoint2) {
                    int BreakPointX = BreakPoint1;
                    BreakPoint1 = BreakPoint2;
                    BreakPoint2 = BreakPointX;
                }
                
                if (BreakPoint1 < N_X && BreakPoint2 < N_X && BreakPoint3 < N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint3; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 < N_X && BreakPoint3 == N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 < N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 == N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 > N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 == N_X && BreakPoint2 > N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 > N_X && BreakPoint2 > N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint1; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                }           //new specimen
                
                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator3_state = true;
                operator1_state = false, operator2_state = false, operator4_state = false, operator5_state = false, operator6_state = false;
            } else if (operator4 == true) {     //fourth operator
                int BreakPoint = RandomNumber2(1, N_X + N_Y);

                if (f[par2] > f[par1]) {
                    int x = f[par1];
                    f[par1] = f[par2];
                    f[par2] = x;
                }
                if ((N_X + N_Y)% 2 == 0) {                      //in case the line has even number of bits
                    if (BreakPoint < (N_X + N_Y)/ 2) {         //in case first part of the line is shorter than the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par2][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        }
                    }
                    if (BreakPoint >= (N_X + N_Y)/ 2) {        //in case first part of the line is longer or equals the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par1][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        }
                    }
                } else { //in case the line has odd number of bits
                    if (BreakPoint <= (N_X + N_Y)/ 2) {     //in case first part of the line is shorter than the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par2][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        }
                    } else if (BreakPoint > (N_X + N_Y)/ 2){  //in case first part of the line is longer than the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par1][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        }
                    }
                }
                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator4_state = true;
                operator1_state = false, operator2_state = false, operator3_state = false, operator5_state = false, operator6_state = false;
            } else if (operator5 == true) {    //fifth operator
                int BreakPoint1 = RandomNumber2(1, N_X); //division point
                int BreakPoint2 = RandomNumber2(N_X + 1, N_X + N_Y);

                for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_x[par1][j - N_X];
                for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_x[par2][j - N_X];

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator5_state = true;
                operator1_state = false, operator2_state = false, operator3_state = false, operator4_state = false, operator6_state = false;
            } else if (operator6 == true) {
                int modifier;
                for (int j = 0; j < N_X; j++) {
                    modifier = BinaryNumber();
                    if (modifier == 1) spec[j] = mat_x[par1][j];
                    else spec[j] = mat_x[par2][j];
                }
                for (int j = N_X; j < N_X + N_Y; j++) {
                    modifier = BinaryNumber();
                    if (modifier == 1) spec[j] = mat_y[par1][j - N_X];
                    else spec[j] = mat_y[par2][j - N_X];
                }

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator6_state = true;
                operator1_state = false, operator2_state = false, operator3_state = false, operator4_state = false, operator5_state = false;
            }
            
        }
        
        QString newmatrix, newstring;
        for (int i = 0; i < M; i++) {
            newstring = "";
            for (int j = 0; j < N_X + N_Y; j++) newstring += QString::number(newmat[i][j])+" ";
            newmatrix += newstring + "\n"; //post-selection matrix output
        }
        ui->textEdit->insertPlainText(newmatrix + "\n");
        
        int mutation;
        int mutrate = ui->spinBox_4->value();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N_X + N_Y; j++) {
                mutation = RandomNumber2(1, 101);
                if (mutation <= mutrate) {
                    if (newmat[i][j] == 0) newmat[i][j] = 1;
                    else newmat[i][j] = 0;                        //mutation process
                }
            }
        }
        
        newmatrix = "";
        for (int i = 0; i < M; i++) {
            newstring = "";
            for (int j = 0; j < N_X + N_Y; j++) newstring += QString::number(newmat[i][j])+" ";
            newmatrix += newstring + "\n";   //post-mutation matrix output
        }
        ui->textEdit->insertPlainText(newmatrix + "\n");
        
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N_X; j++) {
                newmat_x[i][j] = newmat[i][j];
            }
            for (int j = N_X; j < N_X + N_Y; j++) {
                newmat_y[i][j - N_X] = newmat[i][j];  //division of new matrix into x&y
            }
        }
        
        for (int i = 0; i < M; i++) {
            newf[i] = 0, newvalue_x[i] = 0, newvalue_y[i] = 0;
            for (int j = 0; j < N_X; j++) newvalue_x[i] += pow(2, N_X-(j+1)) * newmat_x[i][j];
            for (int j = 0; j < N_Y; j++) newvalue_y[i] += pow(2, N_Y-(j+1)) * newmat_y[i][j];
            newvalue_x[i] = newvalue_x[i] * (right_lim - left_lim) / double(pow(2, N_X) - 1) + left_lim;
            newvalue_y[i] = newvalue_y[i] * (right_lim - left_lim) / double(pow(2, N_Y) - 1) + left_lim; //conversion of new x&y strings into real numbers
            if (function_index == 0) {
                newf[i] = newvalue_x[i] * newvalue_x[i] + newvalue_y[i] * newvalue_y[i];      //fitness function declaration and count
                newf[i] = -newf[i]; //<= searching for min. value
            } else if (function_index == 1) {
                newf[i] = 0.5 * (newvalue_x[i] * newvalue_x[i] + newvalue_x[i] * newvalue_y[i] + newvalue_y[i] * newvalue_y[i]) * (1 + 0.5 * cos(1.5 * newvalue_x[i]) * cos (3.2 * newvalue_x[i] * newvalue_y[i]) * cos(3.14 * newvalue_y[i]) + 0.5 * cos(2.2 * newvalue_x[i]) * cos(4.8 * newvalue_x[i] * newvalue_y[i]) * cos(3.5 * newvalue_y[i])); //fitness function declaration and count
                newf[i] = -newf[i]; //<= searching for min. value
            } else if (function_index == 2) {
                newf[i] = newvalue_x[i] * newvalue_x[i] * fabs(sin(2 * newvalue_x[i])) + newvalue_y[i] * newvalue_y[i] * fabs(sin(2 * newvalue_y[i])) - 1/(5 * newvalue_x[i] * newvalue_x [i] + 5 * newvalue_y[i] * newvalue_y[i] + 0.2) + 5; //fitness function declaration and count
                newf[i] = -newf[i]; //<=searching for min. value
            }
        }
        
        for (int i = 0; i < M; i++) {
            if (newf[i] > best_smpf) {
                best_smpf = newf[i];  //best fitness value
                best_x = newvalue_x[i];
                best_y = newvalue_y[i];
                for (int j = 0; j < N_X; j++) {
                    best_smp[j] = newmat_x[i][j];
                }
                for (int j = N_X; j < N_X + N_Y; j++) {
                    best_smp[j] = newmat_y[i][j - N_X];  //string with the best fitness value
                }
            }
        }
        for (int i = 0; i < M; i++) ui->textEdit->insertPlainText(QString::number(newf[i]) + "\n");
        
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N_X; j++) {
                mat_x[i][j] = newmat_x[i][j];
            }
            for (int j = 0; j < N_Y; j++) {
                mat_y[i][j] = newmat_y[i][j]; // assignment of new values for further development
            }
            f[i] = newf[i];
        }
    }
    
    matrix = "";
    for (int i = 0; i < M; i++) {
        string = "";
        for (int j = 0; j < N_X; j++) string += QString::number(mat_x[i][j]) + " ";
        for (int j = 0; j < N_Y; j++) string += QString::number(mat_y[i][j]) + " ";
        matrix += string + "\n";                      //final matrix output
    }
    ui->textEdit->insertPlainText(matrix);
    
    for (int i = 0; i < M; i++) ui->textEdit->insertPlainText(QString::number(f[i]) + "\n");  //fitness value for final matrix
    
    double min_x = 0, min_y = 0;
    double inaccuracy_x, inaccuracy_y;
    inaccuracy_x = fabs(min_x - best_x);
    inaccuracy_y = fabs(min_y - best_y); //inaccuracy calculation for x&y
    
    ui->textEdit_2->insertPlainText(QString::number(best_x) + "\n");
    ui->textEdit_3->insertPlainText(QString::number(best_y) + "\n");
    ui->textEdit_4->insertPlainText(QString::number(best_smpf) + "\n");
    for (int i = 0; i < N_X + N_Y; i++) ui->textEdit_5->insertPlainText(QString::number(best_smp[i]) + " ");
    ui->textEdit_5->insertPlainText("\n");
    ui->textEdit_6->insertPlainText(QString::number(inaccuracy_x) + "\n");
    ui->textEdit_7->insertPlainText(QString::number(inaccuracy_y) + "\n");

    global_inacc_x = global_inacc_x + inaccuracy_x;
    global_inacc_y = global_inacc_y + inaccuracy_y;
    avg_inacc_x = global_inacc_x / count;
    avg_inacc_y = global_inacc_y / count;
    ui->textEdit_9->clear();
    ui->textEdit_9->insertPlainText("Лучшие значения погрешности: \nдля x = " + QString::number(inaccuracy_x) + "; \nдля y = " + QString::number(inaccuracy_y) + "; \n");
    ui->textEdit_9->insertPlainText("Средние значения погрешности за "+ QString::number(count) +" цикл(а/ов): \nдля x = " + QString::number(avg_inacc_x) + "; \nдля y = " + QString::number(avg_inacc_y));

    for (int i=0;i<M;i++) delete [] mat_x[i];
    delete [] mat_x;
    for (int i=0;i<M;i++) delete [] mat_y[i];
    delete [] mat_y;
    delete [] value_x;
    delete [] value_y;
    delete [] best_smp;
    delete [] spec;
    for (int i = 0; i < M; i++) delete [] newmat[i];
    delete [] newmat;
    for (int i = 0; i < M; i++) delete [] newmat_x[i];
    delete [] newmat_x;
    for (int i = 0; i < M; i++) delete [] newmat_x[i];
    delete [] newmat_x;
    delete [] newf;
    delete [] newvalue_x;
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit->clear();
    ui->textEdit_2->clear();
    ui->textEdit_3->clear();
    ui->textEdit_4->clear();
    ui->textEdit_5->clear();
    ui->textEdit_6->clear();
    ui->textEdit_7->clear();
    ui->textEdit_9->clear();
}

void MainWindow::on_pushButton_3_clicked()
{
    bool operator1 = ui->radioButton_7->isChecked(); //choosing an operator of interbreeding
    bool operator2 = ui->radioButton_8->isChecked();
    bool operator3 = ui->radioButton_9->isChecked();
    bool operator4 = ui->radioButton_10->isChecked();
    bool operator5 = ui->radioButton_11->isChecked();
    bool operator6 = ui->radioButton_12->isChecked();
    bool operator_continue = false;
    if ((operator1 == true && operator1 == operator1_state) || (operator2 == true && operator2 == operator2_state) || (operator3 == true && operator3_state) || (operator4 == true && operator4 == operator4_state) || (operator5 == true && operator5_state) || (operator6 == true && operator6 == operator4_state)) operator_continue = true;
    else operator_continue = false;

    int current_function_index = ui->comboBox->currentIndex();
    if (current_function_index == function_index && operator_continue == true) count++;
    else {
        avg_inacc_x = 0;
        avg_inacc_y = 0;
        global_inacc_x = 0;
        global_inacc_y = 0;
        avg_fitness = 0;
        count = 1;
    }

    double left_lim_x = ui->doubleSpinBox_3->value();
    double right_lim_x = ui->doubleSpinBox_4->value(); // function linits
    double left_lim_y = ui->doubleSpinBox_5->value();
    double right_lim_y = ui->doubleSpinBox_6->value(); // function linits
    int M = ui->spinBox_6->value(); //number of strings
    int N_X = ui->spinBox_7->value(); //number of bits for x arg
    int N_Y = ui->spinBox_8->value(); //number of bits for y arg
    int cycle = ui->spinBox_9->value(); // number of generations
    bool task_min = ui->radioButton_13->isChecked();

    int **mat_x; // matrix initialization for x arg
    mat_x = new int*[M];
    for (int i = 0; i < M; i++) mat_x[i] = new int[N_X];

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N_X; j++) {
            mat_x[i][j] = BinaryNumber();
        }
    }

    int **mat_y; // matrix initialization for y arg
    mat_y = new int*[M];
    for (int i = 0; i < M; i++) mat_y[i] = new int[N_Y];

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N_Y; j++) {
            mat_y[i][j] = BinaryNumber();
        }
    }

    QString matrix, string;

    for (int i = 0; i < M; i++) {
        string = "";
        for (int j = 0; j < N_X; j++) string += QString::number(mat_x[i][j]) + " ";
        for (int j = 0; j < N_Y; j++) string += QString::number(mat_y[i][j]) + " ";
        matrix += string + "\n";      //matrix x+y output
    }
    ui->textEdit_8->insertPlainText(matrix);

    double *f = new double[M], *value_x = new double[M], *value_y = new double[M];
    for (int i = 0; i < M; i++) {
        f[i] = 0, value_x[i] = 0, value_y[i] = 0;
        for (int j = 0; j < N_X; j++) value_x[i] += pow(2, N_X-(j+1)) * mat_x[i][j];
        for (int j = 0; j < N_Y; j++) value_y[i] += pow(2, N_Y-(j+1)) * mat_y[i][j];
        value_x[i] = value_x[i] * (right_lim_x - left_lim_x) / double(pow(2, N_X) - 1) + left_lim_x;
        value_y[i] = value_y[i] * (right_lim_y - left_lim_y) / double(pow(2, N_Y) - 1) + left_lim_y; //conversion of x&y strings into real numbers

        QString Function = ui->lineEdit->text();                      //function
        QString CodeFunction="RESULT =" + Function + ";";

        QScriptEngine engine;
        QScriptValue scriptFun;

        engine.evaluate("function fun(x,y)\n {\n var RESULT=0;\n"+CodeFunction+"\n return RESULT;\n}\n");
        scriptFun = engine.globalObject().property("fun");

        f[i] = scriptFun.call(QScriptValue(), QScriptValueList() << value_x[i] << value_y[i]).toNumber(); //fitness function declaration and count
        if (task_min == true) f[i] = -f[i];
    }

    int *best_smp = new int[N_X + N_Y]; //the fittest string
    double best_smpf = f[0], best_x = value_x[0], best_y = value_y[0];            //its value

    for (int i = 0; i < M; i++) {
        if (f[i] > best_smpf) {
            best_smpf = f[i];  //best fitness value
            best_x = value_x[i];
            best_y = value_y[i];
            for (int j = 0; j < N_X; j++) {
                best_smp[j] = mat_x[i][j];
            }
            for (int j = N_X; j < N_X + N_Y; j++) {
                best_smp[j] = mat_y[i][j - N_X];  //string with the best fitness value
            }
        }
    }
    for (int i = 0; i < M; i++) ui->textEdit_8->insertPlainText(QString::number(f[i]) + "\n");

    int *spec = new int[N_X + N_Y];    //declaration of new specimen
    int **newmat;                   //declaration of new matrix filled with new specimens
    newmat = new int*[M];
    for (int i = 0; i < M; i++) newmat[i] = new int[N_X + N_Y];
    int **newmat_x;
    newmat_x = new int*[M];
    for (int i = 0; i < M; i++) newmat_x[i] = new int[N_X];
    int **newmat_y;
    newmat_y = new int*[M];
    for (int i = 0; i < M; i++) newmat_y[i] = new int[N_Y];
    double *newf = new double[M], *newvalue_x = new double[M], *newvalue_y = new double[M]; //arrays for saving new values

    for (int z = 0; z < cycle; z++) {         //process begins

        for (int i = 0; i < M; i++) {;
            int par1, par1_1, par1_2; //first parent selection
            par1_1 = RandomNumber2(0, M);
            par1_2 = RandomNumber2(0, M);
            if (f[par1_1] > f[par1_2]) par1 = par1_1;
            else par1 = par1_2;
            int par2, par2_1, par2_2; //second parent selection
            par2_1 = RandomNumber2(0, M);
            par2_2 = RandomNumber2(0, M);
            if (f[par2_1] > f[par2_2]) par2 = par2_1;
            else par2 = par2_2;

            if (operator1 == true) {                  // first operator
                int BreakPoint = RandomNumber2(1, N_X + N_Y); //division point

                if (BreakPoint < N_X) {
                    for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint == N_X){
                    for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                }                                                      //new specimen

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator1_state = true;
                operator2_state = false, operator3_state = false, operator4_state = false, operator5_state = false, operator6_state = false;
            } else if (operator2 == true) {             //second specimen
                int BreakPoint1 = RandomNumber2(1, N_X + N_Y); //division point
                int BreakPoint2 = RandomNumber2(1, N_X + N_Y);

                if (BreakPoint1 == BreakPoint2) {     // in case first breakpoint == second one
                    while (BreakPoint1 == BreakPoint2) {
                        BreakPoint1 = RandomNumber2(1, N_X + N_Y);
                        BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                    }
                }
                else if (BreakPoint2 > BreakPoint1)  // in case second breakpoint > first one
                {
                    int BreakPointX = BreakPoint1;
                    BreakPoint1 = BreakPoint2;
                    BreakPoint2 = BreakPointX;
                }

                if (BreakPoint1 < N_X && BreakPoint2 < N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 == N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 == N_X && BreakPoint2 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                } else if (BreakPoint1 > N_X && BreakPoint2 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint1; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                }                          //new specimen

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator2_state = true;
                operator1_state = false, operator3_state = false, operator4_state = false, operator5_state = false, operator6_state = false;
            } else if (operator3 == true) {             //third operator
                int BreakPoint1 = RandomNumber2(1, N_X + N_Y); //division point
                int BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                int BreakPoint3 = RandomNumber2(1, N_X + N_Y);

                if (BreakPoint1 == BreakPoint2 || BreakPoint1 == BreakPoint3 || BreakPoint2 == BreakPoint3) {
                    while (BreakPoint1 == BreakPoint2 || BreakPoint1 == BreakPoint3 || BreakPoint2 == BreakPoint3) {
                        BreakPoint1 = RandomNumber2(1, N_X + N_Y);
                        BreakPoint2 = RandomNumber2(1, N_X + N_Y);
                        BreakPoint3 = RandomNumber2(1, N_X + N_Y);
                    }
                } else if (BreakPoint1 > BreakPoint2 && BreakPoint2 > BreakPoint3) { //in case the sequence is NOT bp3 > bp2 > bp1
                    int BreakPointX = BreakPoint1;
                    BreakPoint1 = BreakPoint3;
                    BreakPoint3 = BreakPointX;
                } else if (BreakPoint1 > BreakPoint3 && BreakPoint3 > BreakPoint2) {
                    int BreakPointX = BreakPoint3;
                    BreakPoint3 = BreakPoint1;
                    BreakPoint1 = BreakPoint2;
                    BreakPoint2 = BreakPointX;
                } else if (BreakPoint2 > BreakPoint1 && BreakPoint1 > BreakPoint3) {
                    int BreakPointX = BreakPoint3;
                    BreakPoint3 = BreakPoint2;
                    BreakPoint2 = BreakPoint1;
                    BreakPoint1 = BreakPointX;
                } else if (BreakPoint2 > BreakPoint3 && BreakPoint3 > BreakPoint1) {
                    int BreakPointX = BreakPoint2;
                    BreakPoint2 = BreakPoint3;
                    BreakPoint3 = BreakPointX;
                } else if (BreakPoint3 > BreakPoint1 && BreakPoint1 > BreakPoint2) {
                    int BreakPointX = BreakPoint1;
                    BreakPoint1 = BreakPoint2;
                    BreakPoint2 = BreakPointX;
                }

                if (BreakPoint1 < N_X && BreakPoint2 < N_X && BreakPoint3 < N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint3; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 < N_X && BreakPoint3 == N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 < N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_x[par2][j];
                    for (int j = BreakPoint2; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 == N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 < N_X && BreakPoint2 > N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                    for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 == N_X && BreakPoint2 > N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                } else if (BreakPoint1 > N_X && BreakPoint2 > N_X && BreakPoint3 > N_X) {
                    for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                    for (int j = N_X; j < BreakPoint1; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint1; j < BreakPoint2; j++) spec[j] = mat_y[par2][j - N_X];
                    for (int j = BreakPoint2; j < BreakPoint3; j++) spec[j] = mat_y[par1][j - N_X];
                    for (int j = BreakPoint3; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                }           //new specimen

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator3_state = true;
                operator1_state = false, operator2_state = false, operator4_state = false, operator5_state = false, operator6_state = false;
            } else if (operator4 == true) {     //fourth operator
                int BreakPoint = RandomNumber2(1, N_X + N_Y);

                if (f[par2] > f[par1]) {
                    int x = f[par1];
                    f[par1] = f[par2];
                    f[par2] = x;
                }
                if ((N_X + N_Y)% 2 == 0) {                      //in case the line has even number of bits
                    if (BreakPoint < (N_X + N_Y)/ 2) {         //in case first part of the line is shorter than the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par2][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        }
                    }
                    if (BreakPoint >= (N_X + N_Y)/ 2) {        //in case first part of the line is longer or equals the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par1][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        }
                    }
                } else { //in case the line has odd number of bits
                    if (BreakPoint <= (N_X + N_Y)/ 2) {     //in case first part of the line is shorter than the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par2][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par2][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par1][j - N_X];
                        }
                    } else if (BreakPoint > (N_X + N_Y)/ 2){  //in case first part of the line is longer than the second
                        if (BreakPoint < N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X; j++) spec[j] = mat_x[par2][j];
                            for (int j = N_X; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint == N_X) {
                            for (int j = 0; j < BreakPoint; j++) spec[j] = mat_x[par1][j];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        } else if (BreakPoint > N_X) {
                            for (int j = 0; j < N_X; j++) spec[j] = mat_x[par1][j];
                            for (int j = N_X; j < BreakPoint; j++) spec[j] = mat_y[par1][j - N_X];
                            for (int j = BreakPoint; j < N_X + N_Y; j++) spec[j] = mat_y[par2][j - N_X];
                        }
                    }
                }
                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator4_state = true;
                operator1_state = false, operator2_state = false, operator3_state = false, operator5_state = false, operator6_state = false;
            } else if (operator5 == true) {    //fifth operator
                int BreakPoint1 = RandomNumber2(1, N_X); //division point
                int BreakPoint2 = RandomNumber2(N_X + 1, N_X + N_Y);

                for (int j = 0; j < BreakPoint1; j++) spec[j] = mat_x[par1][j];
                for (int j = BreakPoint1; j < N_X; j++) spec[j] = mat_x[par2][j];
                for (int j = N_X; j < BreakPoint2; j++) spec[j] = mat_x[par1][j - N_X];
                for (int j = BreakPoint2; j < N_X + N_Y; j++) spec[j] = mat_x[par2][j - N_X];

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator5_state = true;
                operator1_state = false, operator2_state = false, operator3_state = false, operator4_state = false, operator6_state = false;
            } else if (operator6 == true) {
                int modifier;
                for (int j = 0; j < N_X; j++) {
                    modifier = BinaryNumber();
                    if (modifier == 1) spec[j] = mat_x[par1][j];
                    else spec[j] = mat_x[par2][j];
                }
                for (int j = N_X; j < N_X + N_Y; j++) {
                    modifier = BinaryNumber();
                    if (modifier == 1) spec[j] = mat_y[par1][j - N_X];
                    else spec[j] = mat_y[par2][j - N_X];
                }

                for (int k = 0; k < N_X + N_Y; k++) {
                    newmat[i][k] = spec[k];   //new matrix with new specimens
                }

                operator6_state = true;
                operator1_state = false, operator2_state = false, operator3_state = false, operator4_state = false, operator5_state = false;
            }
        }

        QString newmatrix, newstring;
        for (int i = 0; i < M; i++) {
            newstring = "";
            for (int j = 0; j < N_X + N_Y; j++) newstring += QString::number(newmat[i][j])+" ";
            newmatrix += newstring + "\n"; //post-selection matrix output
        }
        ui->textEdit_8->insertPlainText(newmatrix + "\n");

        int mutation;
        int mutrate = ui->spinBox_10->value();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N_X + N_Y; j++) {
                mutation = RandomNumber2(1, 101);
                if (mutation <= mutrate) {
                    if (newmat[i][j] == 0) newmat[i][j] = 1;
                    else newmat[i][j] = 0;                        //mutation process
                }
            }
        }

        newmatrix = "";
        for (int i = 0; i < M; i++) {
            newstring = "";
            for (int j = 0; j < N_X + N_Y; j++) newstring += QString::number(newmat[i][j])+" ";
            newmatrix += newstring + "\n";   //post-mutation matrix output
        }
        ui->textEdit_8->insertPlainText(newmatrix + "\n");

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N_X; j++) {
                newmat_x[i][j] = newmat[i][j];
            }
            for (int j = N_X; j < N_X + N_Y; j++) {
                newmat_y[i][j - N_X] = newmat[i][j];  //division of new matrix into x&y
            }
        }

        for (int i = 0; i < M; i++) {
            newf[i] = 0, newvalue_x[i] = 0, newvalue_y[i] = 0;
            for (int j = 0; j < N_X; j++) newvalue_x[i] += pow(2, N_X-(j+1)) * newmat_x[i][j];
            for (int j = 0; j < N_Y; j++) newvalue_y[i] += pow(2, N_Y-(j+1)) * newmat_y[i][j];
            newvalue_x[i] = newvalue_x[i] * (right_lim_x - left_lim_x) / double(pow(2, N_X) - 1) + left_lim_x;
            newvalue_y[i] = newvalue_y[i] * (right_lim_y - left_lim_y) / double(pow(2, N_Y) - 1) + left_lim_y; //conversion of new x&y strings into real numbers

            QString Function = ui->lineEdit->text();                      //function
            QString CodeFunction="RESULT =" + Function + ";";

            QScriptEngine engine;
            QScriptValue scriptFun;

            engine.evaluate("function fun(x,y)\n {\n var RESULT=0;\n"+CodeFunction+"\n return RESULT;\n}\n");
            scriptFun = engine.globalObject().property("fun");

            newf[i] = scriptFun.call(QScriptValue(), QScriptValueList() << newvalue_x[i] << newvalue_y[i]).toNumber(); //fitness function declaration and count
            if (task_min == true) newf[i] = -newf[i];
        }

        for (int i = 0; i < M; i++) {
            if (newf[i] > best_smpf) {
                best_smpf = newf[i];  //best fitness value
                best_x = newvalue_x[i];
                best_y = newvalue_y[i];
                for (int j = 0; j < N_X; j++) {
                    best_smp[j] = newmat_x[i][j];
                }
                for (int j = N_X; j < N_X + N_Y; j++) {
                    best_smp[j] = newmat_y[i][j - N_X];  //string with the best fitness value
                }
            }
        }
        for (int i = 0; i < M; i++) ui->textEdit_8->insertPlainText(QString::number(newf[i]) + "\n");

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N_X; j++) {
                mat_x[i][j] = newmat_x[i][j];
            }
            for (int j = 0; j < N_Y; j++) {
                mat_y[i][j] = newmat_y[i][j]; // assignment of new values for further development
            }
            f[i] = newf[i];
        }
    }

    matrix = "";
    for (int i = 0; i < M; i++) {
        string = "";
        for (int j = 0; j < N_X; j++) string += QString::number(mat_x[i][j]) + " ";
        for (int j = 0; j < N_Y; j++) string += QString::number(mat_y[i][j]) + " ";
        matrix += string + "\n";                      //final matrix output
    }
    ui->textEdit_8->insertPlainText(matrix);

    for (int i = 0; i < M; i++) ui->textEdit_8->insertPlainText(QString::number(f[i]) + "\n");  //fitness value for final matrix

    ui->textEdit_10->insertPlainText(QString::number(best_x) + "\n");
    ui->textEdit_11->insertPlainText(QString::number(best_y) + "\n");
    for (int i = 0; i < N_X + N_Y; i++) ui->textEdit_12->insertPlainText(QString::number(best_smp[i]) + " ");
    ui->textEdit_12->insertPlainText("\n");
    ui->textEdit_13->insertPlainText(QString::number(best_smpf) + "\n");

    //--------------------------------------------------------//
    /*double x = 3, y = 5;
    QString Function = ui->lineEdit->text();
    QString CodeFunction="RESULT =" + Function + ";";
    
    double Result;
    QScriptEngine engine;
    QScriptValue scriptFun;
    
    engine.evaluate("function fun(x,y)\n {\n var RESULT=0;\n"+CodeFunction+"\n return RESULT;\n}\n");
    scriptFun = engine.globalObject().property("fun");
    
    Result = scriptFun.call(QScriptValue(), QScriptValueList() << x << y).toNumber();
    
    ui->textEdit_8->insertPlainText(QString::number(Result)+"\n");*/
}

void MainWindow::on_pushButton_4_clicked()
{
    Dialog m;
    m.exec();
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->textEdit_8->clear();
    ui->textEdit_10->clear();
    ui->textEdit_11->clear();
    ui->textEdit_12->clear();
    ui->textEdit_13->clear();
}
