/***************************************************************************
 *   Copyright (C) 2004 by Stanislav Karchebny                             *
 *   Stanislav.Karchebny@kdemail.net                                       *
 *                                                                         *
 *   Licensed under GPL.                                                   *
 ***************************************************************************/
#include "feedgroup.h"
#include "feedscollection.h"
#include <qlistview.h>
#include <qdom.h>

namespace Akregator {

FeedGroup::FeedGroup(QListViewItem *i, FeedsCollection *coll)
    : QObject(0, "some_random_name")
    , m_title()
    , m_item(i)
    , m_collection(coll)
{
}

FeedGroup::~FeedGroup()
{
}

void FeedGroup::destroy()
{
   if (m_collection)
    m_collection->remove(m_item);
   delete this;
}

bool FeedGroup::isGroup()
{
    return true;
}

QString FeedGroup::title()
{
    return m_title;
}

void FeedGroup::setTitle(const QString &title)
{
    m_title = title;
    if (m_item)
        m_item->setText(0, title);
}

void FeedGroup::setItem(QListViewItem *i)
{
    m_item=i;
}
    
void FeedGroup::setCollection(FeedsCollection *c)
{
    m_collection=c;
}

QDomElement FeedGroup::toXml( QDomElement parent, QDomDocument document )
{
    QDomElement el = document.createElement( "outline" );
    el.setAttribute( "text", title() );
    parent.appendChild( el );
    return el;
}

};

#include "feedgroup.moc"
