#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>
#include <QRectF>
#include <QRect>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,currentShape(None)
    ,selectedShape(nullptr)
    ,currentRectItem(nullptr)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    ui->graphicsView->setFixedSize(800, 600);

    connect(ui->pushButtonRect, &QPushButton::clicked, this, &MainWindow::on_pushButtonRect_clicked);

    ui->graphicsView->setAcceptDrops(true);
    //ui->pushButtonRect->setAcceptDrops(true);

    ui->graphicsView->viewport()->installEventFilter(this);
    ui->pushButtonRect->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->pushButtonRect && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            startDragging();
            return true;
        }
    }

    if (watched == ui->graphicsView->viewport())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            mousePressEvent(mouseEvent);
            return true;
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            mouseMoveEvent(mouseEvent);
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            mouseReleaseEvent(mouseEvent);
            return true;
        }

        else if (event->type() == QEvent::DragEnter)
        {
            qDebug() << "Event type enter:" << event->type();

            QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
            dragEnterEvent(dragEvent);
            return true;
        }
        else if(event->type() == QEvent::DragMove)
        {
            qDebug()<<"Enter move: "<<event->type();
            QDragMoveEvent *dragMove = static_cast<QDragMoveEvent*>(event);
            dragMoveEvent(dragMove);
            return true;
        }
        else if (event->type() == QEvent::Drop)
        {
            qDebug() << "Event type:" << event->type();

            QDropEvent *dropEv = static_cast<QDropEvent*>(event);
            dropEvent(dropEv);
            return true;
        }

    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{

    if (currentShape == Rectangle && event->button() == Qt::LeftButton)
    {
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
        drawing = true;
        startPosition = scenePos;
        endPosition = scenePos;
        //currentRectItem = nullptr; //scene->addRect(QRectF(startPosition, QSizeF()));
        qDebug() << "Started drawing at"<<startPosition;
    }
}

void MainWindow:: mouseMoveEvent(QMouseEvent *event)
{
    if (drawing && currentShape == Rectangle)
    {
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
        endPosition = scenePos;
        QRectF newRect(startPosition, endPosition);

        if (currentRectItem == nullptr)
        {
            currentRectItem = scene->addRect(newRect);
        }
        else
        {
            currentRectItem->setRect(newRect);
        }
        qDebug() << " drawing ";
    }
}

void MainWindow:: mouseReleaseEvent(QMouseEvent *event)
{

    if (drawing && currentShape == Rectangle && event->button() == Qt::LeftButton)
    {
        drawing = false;
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
        endPosition = scenePos;
        QRectF finalRect(startPosition, endPosition);
        if (currentRectItem != nullptr)
        {
            currentRectItem->setRect(finalRect);
        }

        currentRectItem = nullptr;
        qDebug() << "drawn";
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent called";
    if (event->mimeData()->hasFormat("application/x-rect-item"))
    {
        event->acceptProposedAction();
        qDebug() << "Drag entered with correct format";
    }
    else
    {
        qDebug() << "Drag entered with incorrect format";
    }

}
void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug() << "Dropped";
    if (event->mimeData()->hasFormat("application/x-rect-item"))
    {
        event->acceptProposedAction();
        //        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());


        QPointF viewPos = event->pos();
        QPointF scenePos = ui->graphicsView->mapToScene(viewPos.toPoint());

        qDebug() << "Scene Position:" << scenePos;
        QRectF rect( scenePos, QSizeF(100, 100));
        scene->addRect(rect);
        qDebug() << "Dropped rectangle at" << scenePos;
    }
    else
    {
        qDebug() << "Dropped with incorrect format";
    }

}

void MainWindow::on_pushButtonRect_clicked()
{
    currentShape = Rectangle;
}

void MainWindow:: paintEvent(QPaintEvent *event)
{

}

void MainWindow::startDragging()
{
    qDebug() << "Dragging started";
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-rect-item", QByteArray());

    QDrag *drag = new QDrag(ui->pushButtonRect);
    drag->setMimeData(mimeData);
    qDebug() << "Dragging started...";

    drag->exec(Qt::CopyAction);
    //    Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
}
