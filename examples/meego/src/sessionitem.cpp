/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#include "sessionitem.h"
#include "sessionchilditem.h"
#include <IrcSession>
#include <IrcMessage>

SessionItem::SessionItem(IrcSession* session) : AbstractSessionItem(session)
{
    setIcon("icon-m-content-description");
    setTitle(session->host());
    setSubtitle(session->nickName());

    connect(session, SIGNAL(hostChanged(QString)), this, SLOT(setTitle(QString)));
    connect(session, SIGNAL(nickNameChanged(QString)), this, SLOT(setSubtitle(QString)));

    connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateBusy()));
    connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateBusy()));

    setSession(session);
    m_handler.setSession(session);
    m_handler.setDefaultReceiver(this);
    m_handler.setCurrentReceiver(this);
    connect(&m_handler, SIGNAL(receiverToBeAdded(QString)), SLOT(addChild(QString)));
    connect(&m_handler, SIGNAL(receiverToBeRenamed(QString,QString)), SLOT(renameChild(QString,QString)));
    connect(&m_handler, SIGNAL(receiverToBeRemoved(QString)), SLOT(removeChild(QString)));
    updateCurrent(this);
}

QObjectList SessionItem::childItems() const
{
    return m_children;
}

void SessionItem::updateCurrent(AbstractSessionItem* item)
{
    m_handler.setCurrentReceiver(item->isCurrent() ? item : this);
}

void SessionItem::addChild(const QString& name)
{
    SessionChildItem* child = new SessionChildItem(this);
    connect(child, SIGNAL(alert(QString)), SIGNAL(alert(QString)));
    child->setTitle(name);
    m_handler.addReceiver(name, child);
    m_children.append(child);
    emit childItemsChanged();
}

void SessionItem::renameChild(const QString& from, const QString& to)
{
    for (int i = 0; i < m_children.count(); ++i)
    {
        SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
        if (child->title() == from)
        {
            child->setTitle(to);
            emit childItemsChanged();
            break;
        }
    }
}

void SessionItem::removeChild(const QString& name)
{
    for (int i = 0; i < m_children.count(); ++i)
    {
        SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
        if (child->title() == name)
        {
            m_children.removeAt(i);
            emit childItemsChanged();
            break;
        }
    }
}

void SessionItem::updateBusy()
{
    const IrcSession* session = m_handler.session();
    setBusy(session && session->isActive() && !session->isConnected());
}