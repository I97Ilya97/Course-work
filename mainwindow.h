#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtGui>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <math.h>

class basePlanet{
private:
    QString name;
    int Color;

protected:
    float weight, radius;
    float Pos[3];

public:
    basePlanet( QString nm, float w, float r, float px, float py, float pz,
                int xColor );
    basePlanet();
    virtual ~basePlanet();

    QString getName();
    QString getsPos();

    void DrawMe( int &xColor, int &rad, int &px, int &py, int &pz );
    virtual void move();
};

class Planet:public basePlanet{
    float Vel[3],Accel[3];

public:
    Planet( QString nm, float w, float r, float px, float py, float pz,
                                          float vx, float vy, float vz,
                                          int xColor );
    Planet();
    virtual ~Planet();
//    QString getsVel();

    virtual void move();
};

class RotPlanet:public basePlanet{
    float RotR, omega, alpha;
    int ttime;

public:
    RotPlanet( QString nm, float w, float r, float px, float py, float pz,
                                          float RotRx, float omegax, float alphax,
                                          int xColor );
    RotPlanet();
    virtual ~RotPlanet();
//    QString getsVel();

    virtual void move();
};

typedef struct planetList{
   basePlanet *dPlanet;
   struct planetList *next;
} listEl;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    /* Определяем виртуальный метод родительского класса
     * для отрисовки содержимого виджета
     * */
    void paintEvent(QPaintEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer *timer;

private:
    Ui::MainWindow *ui;
    listEl * PlanetList;

private slots:
    void stReadConfigFile();
    void stStartStopAnimation();
    void stStepModelling();
};

#endif // MAINWINDOW_H
