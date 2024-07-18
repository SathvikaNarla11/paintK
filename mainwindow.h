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

    bool isDragging;
    QGraphicsItem *draggedItem;

    bool drawing;
    bool movingShape;
    QPointF startPosition;
    QPointF endPosition;

    enum Shape{ None, Rectangle, Select};
    Shape currentShape;

    struct ShapeInfo
    {
        QGraphicsRectItem *item;
        Shape shape;
        QRectF rect;
        bool isSelected;
        QPointF start;
    };

    QVector<ShapeInfo> shapes;
    ShapeInfo* selectedShape;

    QGraphicsScene *scene;
    QGraphicsRectItem *currentRectItem;

private slots:
    void on_pushButtonRect_clicked();
    void startDragging();

};
#endif // MAINWINDOW_H
