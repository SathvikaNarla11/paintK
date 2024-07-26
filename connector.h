#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QGraphicsLineItem>

class Connector : public QGraphicsLineItem
{
public:
    Connector(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent = nullptr);

    void updateLine();

private:
    QGraphicsItem *startItem;
    QGraphicsItem *endItem;
};

#endif // CONNECTOR_H
