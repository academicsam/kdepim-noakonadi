/*
 *
 * $Id: $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

/*
 * This file has been generated by the Nepomuk Resource class generator.
 * DO NOT EDIT THIS FILE.
 * ANY CHANGES WILL BE LOST.
 */

#include <nepomuk/tools.h>
#include <nepomuk/variant.h>
#include <nepomuk/resourcemanager.h>
#include "mailboxdataobject.h"


Nepomuk::MailboxDataObject::MailboxDataObject()
  : DataObject( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject") )
{
}


Nepomuk::MailboxDataObject::MailboxDataObject( const MailboxDataObject& res )
  : DataObject( res )
{
}


Nepomuk::MailboxDataObject::MailboxDataObject( const Nepomuk::Resource& res )
  : DataObject( res )
{
}


Nepomuk::MailboxDataObject::MailboxDataObject( const QString& uri )
  : DataObject( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject") )
{
}

Nepomuk::MailboxDataObject::MailboxDataObject( const QUrl& uri )
  : DataObject( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject") )
{
}

Nepomuk::MailboxDataObject::MailboxDataObject( const QString& uri, const QUrl& type )
  : DataObject( uri, type )
{
}

Nepomuk::MailboxDataObject::MailboxDataObject( const QUrl& uri, const QUrl& type )
  : DataObject( uri, type )
{
}

Nepomuk::MailboxDataObject::~MailboxDataObject()
{
}


Nepomuk::MailboxDataObject& Nepomuk::MailboxDataObject::operator=( const MailboxDataObject& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::MailboxDataObject::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject";
}

QList<bool> Nepomuk::MailboxDataObject::isReads() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead") ).toBoolList());
}

void Nepomuk::MailboxDataObject::setIsReads( const QList<bool>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead"), Variant( value ) );
}

void Nepomuk::MailboxDataObject::addIsRead( const bool& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead"), v );
}

QUrl Nepomuk::MailboxDataObject::isReadUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead");
}

QList<Nepomuk::MailboxDataObject> Nepomuk::MailboxDataObject::allMailboxDataObjects()
{
    return Nepomuk::convertResourceList<MailboxDataObject>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject") ) );
}


