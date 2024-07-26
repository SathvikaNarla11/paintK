#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QVector>
#include <QRect>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPushButton>
#include <QGraphicsEllipseItem>
#include "connector.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *event);// overrides;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;


    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;

    bool drawing;
    QPointF startPosition;
    QPointF endPosition;

    enum Shape{ None, Line, Rectangle, Select, RightAngleLine, Connector};
    Shape currentShape;

    QGraphicsScene *scene;
    QGraphicsRectItem *currentRectItem;
    QGraphicsRectItem *selectedRectItem;
    QGraphicsItem *selectItem;
    bool setSelected = false;
    QGraphicsRectItem *highlightedRectItem;
    QGraphicsEllipseItem *highlightedPoint;
    QVector<QGraphicsEllipseItem*> handles;
    QGraphicsPathItem *currentPathItem;

    QGraphicsEllipseItem *resizingHandle;
    bool resizing;
    QPointF resizeStartPos;

    QGraphicsLineItem *currentLineItem;

    QGraphicsItem *sourceShape;
    QGraphicsItem *destinationShape;
    QPointF getBluePointPos(QGraphicsItem *item);
    QPointF getRedPointPos(QGraphicsItem *item);
    void addPointsToItem(QGraphicsPixmapItem* item, int width, int height);

private slots:
    void startDragging();
    void on_pushButtonRectDraw_clicked();
    void on_pushButtonSelect_clicked();

    void addHandle(const QPointF &pos);
    void removeHighlightPoints();
    void highlightRectanglePoints();

    void updateHandlePositions(const QRectF &rect);

    void on_pushButtonLines_pressed();
    void on_pushButtonLines_released();
    void on_pushButtonShape_pressed();
    void on_pushButtonShape_released();
    void on_pushButtonDrawingTools_clicked();
    void on_pushButtonLineDrag_clicked();
    QPointF findNearestPoint(QPointF scenePos);
    void highlightNearestPoint(QPointF scenePos);
    void on_pushButtonRightConnect_clicked();
    void on_pushButtonRightAngleConnect_clicked();
    void on_pushButtonRoundRectangle_pressed();

    void onSelectionChanged();
    void on_pushButtonEllipse_pressed();
    void on_pushButtonRhombus_pressed();


//    void addPointsToItem(QGraphicsPixmapItem* item, int width, int height);

};
#endif // MAINWINDOW_H
