#include "connector.h"
#include <QGraphicsItem>
#include <QGraphicsScene>

Connector::Connector(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), startItem(startItem), endItem(endItem)
{
    updateLine();
    setPen(QPen(Qt::black, 2));
}

void Connector::updateLine()
{
    if (startItem && endItem)
    {
        // Get the center points of the start and end items
        QPointF startPos = startItem->sceneBoundingRect().center();
        QPointF endPos = endItem->sceneBoundingRect().center();
        // Set the line between the two points
        setLine(QLineF(startPos, endPos));
    }
}
