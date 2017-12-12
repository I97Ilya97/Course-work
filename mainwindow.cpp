#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    PlanetList = NULL;
    timer = new QTimer(this);
    timer->stop();
    timer->setInterval(50);
    connect(ui->mnReadConfigFile, SIGNAL(triggered()), this, SLOT(stReadConfigFile()));
    connect(ui->mnStartStop, SIGNAL(triggered()), this, SLOT(stStartStopAnimation()));
    connect(this->timer,SIGNAL(timeout()),this,SLOT(stStepModelling()));

}

void MainWindow::stStepModelling(){
    listEl *tmp = PlanetList;
    while( tmp ){
        tmp->dPlanet->move();
        tmp = tmp->next;
    }
    tmp = PlanetList;
    QString db;
    while( tmp ){
        Planet *Px = (Planet*) (tmp->dPlanet);
        db = db + Px->getName() +
                  Px->getsPos() + "\n";
//                  Px->getsVel() + "\n";
        tmp = tmp->next;
    }
    ui->lbDebug->setText(db);
    repaint();
}


void MainWindow::stStartStopAnimation(){
    if( timer->isActive() ){
        timer->stop();
        ui->mnStartStop->setText( QString("Start"));
    }
    else{
        timer->start();
        ui->mnStartStop->setText( QString("Stop"));
    }
}


void MainWindow::stReadConfigFile(){
    int count = 0;

    QFile file("StarMap.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0,"Can't open file:",file.errorString());
        return;
    }

    QTextStream in(&file);

    QString db;
    while( PlanetList ){
        listEl *tmp = PlanetList;

        PlanetList = tmp->next;

        delete tmp->dPlanet;
        delete tmp;

    }


    while(!in.atEnd()) {

        QString line = in.readLine();
        int index_dublespace;
        while( (index_dublespace=line.indexOf("  ")) != -1 ){
            line.remove(index_dublespace, 1 );
        }

        QStringList  fields = line.split(" ");
        bool Ok;
        int clr = fields[13].toInt(&Ok, 16);
        listEl *tmp = new listEl;
        basePlanet *Px;
        switch( fields[0].toStdString()[0] ){
            case '+' : db = db+QString('R');
                       Px = new RotPlanet( fields[0],
                               fields[1].toFloat(),
                               fields[2].toFloat(),
                               fields[4].toFloat(),
                               fields[5].toFloat(),
                               fields[6].toFloat(),
                               fields[9].toFloat(),
                               fields[10].toFloat(),
                               fields[11].toFloat(),
                               fields[13].toInt(&Ok,16) );
                       break;
            case '-' : db = db+QString('L');
                       Px = new Planet( fields[0],
                            fields[1].toFloat(),
                            fields[2].toFloat(),
                            fields[4].toFloat(),
                            fields[5].toFloat(),
                            fields[6].toFloat(),
                            fields[9].toFloat(),
                            fields[10].toFloat(),
                            fields[11].toFloat(),
                            fields[13].toInt(&Ok,16) );

        }

        tmp->dPlanet = Px;
        tmp->next = PlanetList;
        PlanetList = tmp;

        QString  crd = QString( "  crd=( %1 %2 %3 )" ).arg(fields[4]).arg(fields[5]).arg(fields[6]);
        QString  vct = QString( "  vel=( %1 %2 %3 )" ).arg(fields[9]).arg(fields[10]).arg(fields[11]);
        QString  cl = QString( " RGB(%1)=%2").arg(fields[13]).arg(clr);
        db = db+fields[0]+ crd + vct + cl +"\n";

        count++;
    }
    ui->lbDebug->setText(db);
    QMessageBox::information(this, tr("Read file success!!"),
                        tr("Read file with %1 objects.").arg(count));

}

MainWindow::~MainWindow()
{
    delete ui;
}


/* Определяем виртуальный метод родительского класса
 * для отрисовки содержимого виджета
**/
void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this); // Создаём объект отрисовщика
    // Устанавливаем кисть абриса

//    painter.setBrush(QBrush(QColor( 0,0,255,255 ), Qt::SolidPattern));
//    painter.drawEllipse(100, 50, 150, 150);

    listEl *tmp = PlanetList;
    int col,rad,r,g,b,x,y,z,xp,yp, width, height, i,i1,rr1,ix,iy;

    QRect rct = this->geometry();


    width = rct.width()/2;
    height = rct.height()/2;
    QString db = QString( "width=%1\nheight=%2" ).arg(width).arg(height);
    ui->winsize->setText(db);

    float deepth_gr[3];
    deepth_gr[0] = 1;
    deepth_gr[1] = exp( log(0.3)*( 100/500.0));
    deepth_gr[2] = exp( log(0.3)*( 200/500.0));

    i = 75;
    while( i< width ){
        for( i1=0; i1<3; i1++){
          rr1 = i*deepth_gr[i1];
          painter.drawLine(rr1+width,0,rr1+width, height*2);
          painter.drawLine(-rr1+width,0,-rr1+width, height*2);
        }
        i += 150;
    }
    i = 75;
    while( i< height ){
        for( i1=0; i1<3; i1++){
          rr1 = i*deepth_gr[i1];
          painter.drawLine(0, height+rr1, width*2, height+rr1);
          painter.drawLine(0, height-rr1, width*2, height-rr1);
        }
        i += 150;
    }
    ix = 75;
    while( ix< width ){
        iy = 75;
        while( iy< height ){
            int x1, y1;

            x1 = ix;
            y1 = iy;
            for( i1=1; i1<3; i1++){
                int x2,y2;
                x2 = ix*deepth_gr[i1];
                y2 = iy*deepth_gr[i1];

                painter.drawLine(width+x1, height+y1, width+x2, height+y2);
                painter.drawLine(width-x1, height+y1, width-x2, height+y2);
                painter.drawLine(width+x1, height-y1, width+x2, height-y2);
                painter.drawLine(width-x1, height-y1, width-x2, height-y2);

                x1 = x2;
                y1 = y2;
            }
            iy += 150;
        }
        ix += 150;
    }



    while( tmp ){
        tmp->dPlanet->DrawMe(col,rad,x,y,z);
        r = col & 0xFF;
        g = (col>>8)& 0xFF;
        b = (col>>16)& 0xFF;

        float deepth = exp( log(0.3)*(z /500.0));
        rad = rad*deepth;

        xp = x - width;
        xp = x*deepth + width;
        yp = y - height;
        yp = y*deepth + height;

//        yp = y;

        painter.setBrush(QBrush(QColor( r,g,b,255 ), Qt::SolidPattern));
        painter.drawEllipse(xp-rad, yp-rad, 2*rad, 2*rad);

        tmp = tmp->next;
    }

}

basePlanet::basePlanet( QString nm, float w, float r, float px, float py, float pz,
                        int xColor ){

    name = nm;
    weight = w;
    radius = r;
    Pos[0] = px;
    Pos[1] = py;
    Pos[2] = pz;

    Color = xColor;
}

basePlanet::basePlanet(){

    name = QString("prototype");
    weight = 10;
    radius = 10;
    Pos[0] = 0;
    Pos[1] = 0;
    Pos[2] = 0;

    Color = 0x0000FF;
}

basePlanet::~basePlanet(){
}

void basePlanet::move(){
}

QString basePlanet::getName(){
    return name;
}

void basePlanet::DrawMe( int &xColor, int &rad, int &px, int &py, int &pz ){
    xColor = Color;
    rad = radius;
    px = Pos[0];
    py = Pos[1];
    pz = Pos[2];
}

QString basePlanet::getsPos(){
    QString  crd = QString( "  crd=( %1 %2 %3 )" ).arg(Pos[0])
                                                  .arg(Pos[1])
                                                  .arg(Pos[2]);
    return crd;
}


Planet::Planet( QString nm, float w, float r,
                float px, float py, float pz,
                float vx, float vy, float vz,
                int xColor ) :
    basePlanet( nm, w, r, px, py, pz, xColor ) {

    Vel[0] = vx;
    Vel[1] = vy;
    Vel[2] = vz;

}

Planet::Planet():basePlanet(){
    Vel[0] = 0;
    Vel[1] = 0;
    Vel[2] = 0;
}

Planet::~Planet(){
}

/*
QString Planet::getsVel(){
    QString  vct = QString( "  vel=( %1 %2 %3 )" ).arg(Vel[0])
                                                  .arg(Vel[1])
                                                  .arg(Vel[2]);
    return vct;
}
*/
void Planet::move(){
    int i;
    for( i=0; i<3; i++ )
        Pos[i] += Vel[i];
}


RotPlanet::RotPlanet( QString nm, float w, float r,
                      float px, float py, float pz,
                      float RotRx, float omegax, float alphax,
                      int xColor ) :
    basePlanet( nm, w, r, px, py, pz, xColor ) {

    RotR = RotRx;
    omega = omegax;
    alpha = alphax;

    ttime = 0;
}

RotPlanet::RotPlanet():basePlanet(){
    RotR = 300;
    omega = 0.3;
    alpha = 1;
    ttime = 0;
}

RotPlanet::~RotPlanet(){
}

void RotPlanet::move(){
    Pos[1] = 200;
    Pos[0] = RotR*sin(omega*ttime + alpha );
    Pos[2] = RotR*cos(omega*ttime + alpha );

    ttime++;
}
