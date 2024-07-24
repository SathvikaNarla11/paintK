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
#include <QGraphicsEllipseItem>


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

    enum Shape{ None, Line, Rectangle, Select, RightAngleLine};
    Shape currentShape;

    QGraphicsScene *scene;
    QGraphicsRectItem *currentRectItem;
    QGraphicsRectItem *selectedRectItem;
    QGraphicsRectItem *highlightedRectItem;
    QGraphicsEllipseItem *highlightedPoint;
    QVector<QGraphicsEllipseItem*> handles;

    QGraphicsEllipseItem *resizingHandle;
    bool resizing;
    QPointF resizeStartPos;

    QGraphicsLineItem *currentLineItem;

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
};
#endif // MAINWINDOW_H
