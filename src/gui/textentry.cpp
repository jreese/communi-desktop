/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "textentry.h"
#include "commandparser.h"
#include <QStringListModel>
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcCompleter>
#include <IrcBuffer>
#include <QShortcut>

TextEntry::TextEntry(QWidget* parent) : LineEditor(parent)
{
    d.buffer = 0;

    d.parser = new CommandParser(this);
    d.parser->setTriggers(QStringList("/"));
    d.parser->setTolerant(true);

    d.completer = new IrcCompleter(this);
    connect(d.completer, SIGNAL(completed(QString,int)), this, SLOT(complete(QString,int)));
    d.completer->setParser(d.parser);

    setAttribute(Qt::WA_MacShowFocusRect, false);

    QShortcut* shortcut = new QShortcut(Qt::Key_Tab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(tryComplete()));

    setButtonVisible(Left, true);
    setAutoHideButton(Left, true);
    setButtonPixmap(Left, QPixmap(":/icons/buttons/tab.png"));
    connect(this, SIGNAL(leftButtonClicked()), this, SLOT(tryComplete()));

    setButtonVisible(Right, true);
    setAutoHideButton(Right, true);
    setButtonPixmap(Right, QPixmap(":/icons/buttons/return.png"));
    connect(this, SIGNAL(rightButtonClicked()), this, SLOT(sendInput()));

    connect(this, SIGNAL(returnPressed()), this, SLOT(sendInput()));
}

IrcBuffer* TextEntry::buffer() const
{
    return d.buffer;
}

void TextEntry::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            d.histories.insert(d.buffer, history());

            IrcBufferModel* model = d.buffer->model();
            disconnect(model, SIGNAL(channelsChanged(QStringList)), d.parser, SLOT(setChannels(QStringList)));
            disconnect(buffer, SIGNAL(titleChanged(QString)), d.parser, SLOT(setTarget(QString)));
        }

        d.buffer = buffer;
        setHistory(d.histories.value(buffer));

        if (buffer) {
            IrcBufferModel* model = buffer->model();
            connect(model, SIGNAL(channelsChanged(QStringList)), d.parser, SLOT(setChannels(QStringList)));
            connect(buffer, SIGNAL(titleChanged(QString)), d.parser, SLOT(setTarget(QString)));
            connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup(IrcBuffer*)), Qt::UniqueConnection);

            d.parser->setTarget(buffer->title());
            d.parser->setChannels(buffer->model()->channels());
            d.completer->setBuffer(buffer);
        } else {
            d.parser->reset();
            d.completer->setBuffer(0);
        }
    }
}

void TextEntry::sendInput()
{
    if (!d.buffer)
        return;

    bool error = false;
    const QStringList lines = text().split(QRegExp("[\\r\\n]"), QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        if (!line.trimmed().isEmpty()) {
            if (!d.buffer->sendCommand(d.parser->parse(line)))
                error = true;
        }
    }
    if (!error)
        clear();
}

void TextEntry::cleanup(IrcBuffer* buffer)
{
    d.histories.remove(buffer);
}

void TextEntry::tryComplete()
{
    d.completer->complete(text(), cursorPosition());
}

void TextEntry::complete(const QString& text, int cursor)
{
    setText(text);
    setCursorPosition(cursor);
}
