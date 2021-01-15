/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "view.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include <QtGui/QMouseEvent>
#include <QChartView>
#include <QDebug>
#include "callout.h"
#include <QValueAxis>
#include <iostream>
#include <thread>
#include <QHeaderView>
#include <QGraphicsProxyWidget>

View::View(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      m_coordX(0),
      m_coordY(0),
      m_chart(0),
      m_tooltip(0),nDataIndex(0)
{
    // chart
    m_chart = new QChart;
    m_chart->setMinimumSize(640, 480);
    m_chart->legend()->hide();

    QLineSeries *series = new QLineSeries;
    m_chart->addSeries(series);

    m_chart->createDefaultAxes();
    QValueAxis* m_axis = new QValueAxis();
    m_chart->setAxisX(m_axis, series);
    m_axis->setTickCount(5);
    m_chart->setAcceptHoverEvents(true);

    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(m_chart);

    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height());
    m_coordX->setText("X: ");
    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height());
    m_coordY->setText("Y: ");

    connect(series, &QLineSeries::hovered, this, &View::tooltip);
    this->setMouseTracking(true);
}

void View::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        m_chart->resize(event->size());
        m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height() - 20);
        m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height() - 20);
        if(m_calloutsList.count() > 0)
        {
            const auto callouts =  m_calloutsList[nDataIndex];
            for (Callout *callout : callouts)
                callout->updateGeometry();
        }
    }

    viewport()->update();
    QGraphicsView::resizeEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    m_coordX->setText(QString("X: %1").arg(m_chart->mapToValue(event->pos()).x()));
    m_coordY->setText(QString("Y: %1").arg(m_chart->mapToValue(event->pos()).y()));

    QGraphicsView::mouseMoveEvent(event);
}

void View::mousePressEvent(QMouseEvent *event)
{
    if(m_tooltip != nullptr)
    {
        if(m_tooltip->isVisible())
        {
            QList<Callout *>& tmp = m_calloutsList[nDataIndex];
            m_tooltip->selectedPoint();
            tmp.append(m_tooltip);
            m_tooltip = nullptr;
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void View::keepCallout(const QPointF &point)
{
    QList<Callout *>& tmp = m_calloutsList[nDataIndex];
    m_tooltip->selectedPoint();
    tmp.append(m_tooltip);
    m_tooltip = nullptr;
}

void View::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart,nWavfs);

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setTime(point.x()*1000);
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void View::slotsdataStatusClear()
{
    //    resize(geometry().width()-1,geometry().height());
    //    resize(geometry().width()+1,geometry().height());

    for(auto& item : m_calloutsList)
    {
        for(auto& subitem : item)
        {
            if(subitem->scene() != nullptr)
            {
                scene()->removeItem(subitem);
            }
            subitem->deleteLater();
            subitem = nullptr;
        }
    }

    for(auto& toolTipsitemList : m_calloutsList)
    {
        //qDeleteAll(toolTipsitemList.begin(),toolTipsitemList.end());
        toolTipsitemList.clear();
    }
    viewport()->update();
}

void View::dataFileChang(QModelIndex index,const QVector<QPointF>& dataVec,int fs)
{
    if(m_calloutsList.count() <=0)
    {
        return;
    }

    m_chart->setTitle(index.data().toString());
    qDebug()<<"dataFileChang is called,"<<index.data();
    int nindex = index.row();

    QLineSeries * tmp = static_cast<QLineSeries *>(m_chart->series()[0]);

    QList<QPointF> dataPointList;
    qreal yMin = dataVec[0].y(),yMax=dataVec[0].y(),y=dataVec[0].y();
    for(int i = 0 ;i<dataVec.size();i++)
    {
        QPointF tmp;
        if(i<dataVec.size())
        {
            if(dataVec[i].y()> yMax)
                yMax = dataVec[i].y();
            tmp = dataVec[i];
        }
        else
        {
            tmp = QPointF(i,0);
        }
        dataPointList.append(tmp);
    }

    m_chart->axisX()->setRange(0,dataVec[dataVec.size()-1].x());
    m_chart->axisY()->setRange(-qAbs(yMax)*1.2f, qAbs(yMax)*1.2f);
    tmp->replace(dataPointList);

    for(auto& item : m_calloutsList[nDataIndex])
    {
        scene()->removeItem(item);
    }

    nDataIndex = nindex;
    nWavfs = fs;

    for(auto& item2 : m_calloutsList[nDataIndex])
    {
        scene()->addItem(item2);
    }
}

void View::dataFileCount(int count)
{
    m_calloutsList.clear();
    for(int i = 0 ;i<count;i++)
    {
        m_calloutsList.append(QList<Callout *>());
    }
}
