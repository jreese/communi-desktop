/*
* Copyright (C) 2008-2013 Communi authors
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef SESSIONTABWIDGET_H
#define SESSIONTABWIDGET_H

#include <QHash>
#include <QStackedWidget>
#include "sessionmodel.h"

class Session;
class IrcMessage;
class MessageView;

class SessionTabWidget : public QStackedWidget
{
    Q_OBJECT

public:
    SessionTabWidget(Session* session, QWidget* parent = 0);

    Session* session() const;
    SessionModel* model() const;

    MessageView* currentView() const;
    MessageView* viewAt(int index) const;

    QByteArray saveSplitter() const;

public slots:
    void restoreSplitter(const QByteArray& state);

    MessageView* addView(const QString& receiver);
    void openView(const QString& receiver);
    void removeView(const QString& receiver);
    void closeView(int index);
    void sendMessage(const QString& receiver, const QString& message);

signals:
    void splitterChanged(const QByteArray& state);

    void viewAdded(MessageView* view);

private slots:
    void tabActivated(int index);
    MessageView* addView(SessionItem* item);

private:
    struct SessionTabWidgetData {
        SessionModel model;
        QHash<QString, MessageView*> views;
    } d;
};

#endif // SESSIONTABWIDGET_H
