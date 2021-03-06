/*
    This file is part of kdepim.

    Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#ifndef WEBDAVHANDLER_H
#define WEBDAVHANDLER_H


#include <QString>
#include <QDateTime>
#include <qdom.h>

#include <kdatetime.h>

#include <kabc/addressee.h>

#include "slox_export.h"

namespace KCal {
class Incidence;
}

class SloxBase;

class KSLOX_EXPORT SloxItem
{
  public:
    enum Status { Invalid, Delete, Create, New };

    SloxItem();

    QDomNode domNode;
    QString sloxId;
    QString clientId;
    Status status;
    QString response;
    QString responseDescription;
    QString lastModified;
};

class KSLOX_EXPORT WebdavHandler
{
  public:
    WebdavHandler();

    void setUserId( const QString & );
    QString userId() const;
    void setResource( SloxBase *res ) { mRes = res; }

    void log( const QString & );

    static QDomElement addElement( QDomDocument &, QDomNode &,
                                   const QString &tag );
    static QDomElement addDavElement( QDomDocument &, QDomNode &,
                                      const QString &tag );
    static QDomElement addSloxElement( SloxBase *res,
                                       QDomDocument &, QDomNode &,
                                       const QString &tag,
                                       const QString &text = QString() );

    static KDateTime sloxToKDateTime( const QString &str );
    static KDateTime sloxToKDateTime( const QString &str,
                                      const KDateTime::Spec &timeSpec );
    static KDE_DEPRECATED QDateTime sloxToQDateTime( const QString &str );
    static QString kDateTimeToSlox( const KDateTime &dt );
    static KDE_DEPRECATED QString qDateTimeToSlox( const QDateTime &dt );

    static QList<SloxItem> getSloxItems( SloxBase *res, const QDomDocument &doc );

    void clearSloxAttributeStatus();
    void parseSloxAttribute( const QDomElement & );
    void setSloxAttributes( KCal::Incidence * );
    void setSloxAttributes( KABC::Addressee & );

  private:
    QString mLogFile;
    int mLogCount;
    SloxBase *mRes;

    QString mUserId;

    bool mWritable;
};

#endif
