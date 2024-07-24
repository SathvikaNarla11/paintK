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
#include <QGraphicsSceneMouseEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,drawing(false)
    ,currentShape(None)
    ,currentRectItem(nullptr)
    ,selectedRectItem(nullptr)
    ,highlightedRectItem(nullptr)
    ,resizingHandle(nullptr)
    ,resizing(false)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    ui->graphicsView->setFixedSize(700, 400);
    ui->graphicsView->setAcceptDrops(true);
    ui->graphicsView->viewport()->installEventFilter(this);
    ui->pushButtonRect->installEventFilter(this);

    ui->toolBar->addWidget(ui->pushButtonDrawingTools);
    ui->groupBoxShapesToDraw->hide();

    scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());

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
    QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
    QGraphicsItem *item = scene->itemAt(scenePos, QTransform());

    if (currentShape == Rectangle && event->button() == Qt::LeftButton)
    {
        drawing = true;
        startPosition = scenePos;
        endPosition = scenePos;
        qDebug() << "Started drawing at"<<startPosition;
    }
    else if (currentShape == Line && event->button() == Qt::LeftButton)
    {
        drawing = true;
        startPosition = scenePos;
        endPosition = scenePos;
        currentLineItem = scene->addLine(QLineF(startPosition, endPosition), QPen(Qt::black, 2));
    }
    else if (currentShape == Select && event->button() == Qt::LeftButton)
    {
        QGraphicsRectItem *rectItem = dynamic_cast<QGraphicsRectItem *>(item);
        if (rectItem)
        {
            selectedRectItem = rectItem;
            qDebug()<<"selectedRectItem "<<selectedRectItem;
            qDebug()<<"highlightedRectItem "<<highlightedRectItem;
            if (highlightedRectItem)
            {
                scene->removeItem(highlightedRectItem);
                removeHighlightPoints();
                delete highlightedRectItem;
                highlightedRectItem = nullptr;
            }
            QRectF rect = rectItem->rect();
            highlightedRectItem = scene->addRect(rect, QPen(Qt::blue, 2, Qt::DotLine));
            highlightedRectItem->setPos(rectItem->pos());
            qDebug() << "Selected rectangle at" << rectItem->rect();
            highlightRectanglePoints();
        }
    }
    //QGraphicsItem *item = scene->itemAt(event->scenePos(), QTransform());
    QGraphicsEllipseItem *handle = dynamic_cast<QGraphicsEllipseItem *>(item);
    if (handle && handles.contains(handle))
    {
        resizingHandle = handle;
        resizingHandle->setBrush(Qt::green);  // Change to green when selected
        resizing = true;
    }
}

void MainWindow:: mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << " mouseMoveEvent ";
    if (drawing && currentShape == Rectangle)
    {
        qDebug() << "move draw rectangle ";
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
        qDebug() << " drawing line";
    }
    else if (drawing && currentShape == Line)
    {
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
        endPosition = scenePos;
        currentLineItem->setLine(QLineF(startPosition, endPosition));
        highlightNearestPoint(scenePos);
    }
    if (resizing && resizingHandle)
    {
        QPointF handlePos = ui->graphicsView->mapToScene(event->pos());//event->scenePos();
        QRectF rect = selectedRectItem->rect();
        QRectF newRect = rect;

        if (resizingHandle == handles[0]) {
            newRect.setTopLeft(handlePos);

        } else if (resizingHandle == handles[1]) {
            newRect.setTopRight(handlePos);
        } else if (resizingHandle == handles[2]) {
            newRect.setBottomLeft(handlePos);
        } else if (resizingHandle == handles[3]) {
            newRect.setBottomRight(handlePos);
        } else if (resizingHandle == handles[4]) {
            newRect.setTop(handlePos.y());
        } else if (resizingHandle == handles[5]) {
            newRect.setBottom(handlePos.y());
        } else if (resizingHandle == handles[6]) {
            newRect.setLeft(handlePos.x());
        } else if (resizingHandle == handles[7]) {
            newRect.setRight(handlePos.x());
        }

        selectedRectItem->setRect(newRect);
        highlightedRectItem->setRect(newRect);
        updateHandlePositions(newRect);
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
    else if (drawing && currentShape == Line)
    {
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());
        QPointF nearestPoint = findNearestPoint(scenePos);

        //currentLineItem->setLine(QLineF(startPosition, endPosition));
        if (nearestPoint != scenePos)
        {
            currentLineItem->setLine(QLineF(startPosition, nearestPoint));
        }

        if (highlightedPoint)
        {
            scene->removeItem(highlightedPoint);
            delete highlightedPoint;
            highlightedPoint = nullptr;
        }

        currentLineItem = nullptr;
        drawing = false;
    }
    if (resizing && resizingHandle)
    {
        resizingHandle->setBrush(Qt::red);  // Change back to red when resizing ends
        resizingHandle = nullptr;
        resizing = false;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
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
        QPointF viewPos = event->pos();
        QPointF scenePos = ui->graphicsView->mapToScene(viewPos.toPoint());

        QRectF rect( scenePos, QSizeF(100, 100));
        scene->addRect(rect);
    }
    else
    {
        qDebug() << "Dropped with incorrect format";
    }

}

void MainWindow::on_pushButtonRectDraw_clicked()
{
    currentShape = Rectangle;
    ui->groupBoxShapesToDraw->hide();
}

void MainWindow::startDragging()
{
    qDebug() << "Dragging started";
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-rect-item", QByteArray());

    QDrag *drag = new QDrag(ui->pushButtonRect);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void MainWindow:: paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::on_pushButtonSelect_clicked()
{
    currentShape = Select;
    ui->groupBoxShapesToDraw->hide();
}


void MainWindow::highlightRectanglePoints()
{
    if (selectedRectItem)
    {
        QRectF rect = selectedRectItem->rect();
        QPointF topLeft = rect.topLeft();
        QPointF topRight = rect.topRight();
        QPointF bottomLeft = rect.bottomLeft();
        QPointF bottomRight = rect.bottomRight();

        handles.append(scene->addEllipse(QRectF(topLeft - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
        handles.append(scene->addEllipse(QRectF(topRight - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
        handles.append(scene->addEllipse(QRectF(bottomLeft - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
        handles.append(scene->addEllipse(QRectF(bottomRight - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));

        QPointF topMid((topLeft + topRight) / 2);
        QPointF bottomMid((bottomLeft + bottomRight) / 2);
        QPointF leftMid((topLeft + bottomLeft) / 2);
        QPointF rightMid((topRight + bottomRight) / 2);

        handles.append(scene->addEllipse(QRectF(topMid - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
        handles.append(scene->addEllipse(QRectF(bottomMid - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
        handles.append(scene->addEllipse(QRectF(leftMid - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
        handles.append(scene->addEllipse(QRectF(rightMid - QPointF(5, 5), QSizeF(10, 10)), QPen(Qt::red), QBrush(Qt::red)));
    }
}


void MainWindow::addHandle(const QPointF &pos)
{
    QGraphicsEllipseItem *handle = new QGraphicsEllipseItem(QRectF(pos - QPointF(5, 5), QSizeF(10, 10)));
    handle->setPen(QPen(Qt::red));
    scene->addItem(handle);
    handles.append(handle);
    qDebug() << "Handle added at position:" << pos;
}

void MainWindow::removeHighlightPoints()
{
    for (auto handle : handles) {
        scene->removeItem(handle);
        delete handle;
    }
    handles.clear();
}


void MainWindow::updateHandlePositions(const QRectF &rect)
{
    handles[0]->setRect(QRectF(rect.topLeft() - QPointF(5, 5), QSizeF(10, 10)));
    handles[1]->setRect(QRectF(rect.topRight() - QPointF(5, 5), QSizeF(10, 10)));
    handles[2]->setRect(QRectF(rect.bottomLeft() - QPointF(5, 5), QSizeF(10, 10)));
    handles[3]->setRect(QRectF(rect.bottomRight() - QPointF(5, 5), QSizeF(10, 10)));

    QPointF topMid((rect.topLeft() + rect.topRight()) / 2);
    QPointF bottomMid((rect.bottomLeft() + rect.bottomRight()) / 2);
    QPointF leftMid((rect.topLeft() + rect.bottomLeft()) / 2);
    QPointF rightMid((rect.topRight() + rect.bottomRight()) / 2);

    handles[4]->setRect(QRectF(topMid - QPointF(5, 5), QSizeF(10, 10)));
    handles[5]->setRect(QRectF(bottomMid - QPointF(5, 5), QSizeF(10, 10)));
    handles[6]->setRect(QRectF(leftMid - QPointF(5, 5), QSizeF(10, 10)));
    handles[7]->setRect(QRectF(rightMid - QPointF(5, 5), QSizeF(10, 10)));
}

QPointF MainWindow::findNearestPoint(QPointF scenePos)
{
    QPointF nearestPoint = scenePos;
    qreal minDistance = std::numeric_limits<qreal>::max();

    for (QGraphicsItem *item : scene->items())
    {
        QGraphicsRectItem *rectItem = dynamic_cast<QGraphicsRectItem *>(item);
        if (rectItem)
        {
            QRectF rect = rectItem->rect();
            QPointF points[8] = {
                rect.topLeft(), rect.topRight(), rect.bottomLeft(), rect.bottomRight(),
                (rect.topLeft() + rect.topRight()) / 2,
                (rect.bottomLeft() + rect.bottomRight()) / 2,
                (rect.topLeft() + rect.bottomLeft()) / 2,
                (rect.topRight() + rect.bottomRight()) / 2
            };

            for (const QPointF &point : points)
            {
                qreal distance = QLineF(scenePos, point).length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestPoint = point;
                }
            }
        }
    }

    return nearestPoint;
}

void MainWindow::highlightNearestPoint(QPointF scenePos)
{
    QPointF nearestPoint = findNearestPoint(scenePos);

    if (highlightedPoint)
    {
        scene->removeItem(highlightedPoint);
        delete highlightedPoint;
        highlightedPoint = nullptr;
    }

    if (nearestPoint != scenePos)
    {
        highlightedPoint = scene->addEllipse(nearestPoint.x() - 4, nearestPoint.y() - 4, 8, 8, QPen(Qt::blue), QBrush(Qt::blue));
    }
}
void MainWindow::on_pushButtonLines_pressed()
{
    ui->groupBoxShapesToDrag->hide();
}


void MainWindow::on_pushButtonLines_released()
{
    ui->groupBoxLine->show();
}


void MainWindow::on_pushButtonShape_pressed()
{
    ui->groupBoxLine->hide();
}


void MainWindow::on_pushButtonShape_released()
{
    ui->groupBoxShapesToDrag->show();
}


void MainWindow::on_pushButtonDrawingTools_clicked()
{
    ui->groupBoxShapesToDraw->show();
}

void MainWindow::on_pushButtonLineDrag_clicked()
{
    currentShape = Line;
}

