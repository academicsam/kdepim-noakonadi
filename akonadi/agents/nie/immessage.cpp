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
#include "immessage.h"


Nepomuk::IMMessage::IMMessage()
  : Message( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage") )
{
}


Nepomuk::IMMessage::IMMessage( const IMMessage& res )
  : Message( res )
{
}


Nepomuk::IMMessage::IMMessage( const Nepomuk::Resource& res )
  : Message( res )
{
}


Nepomuk::IMMessage::IMMessage( const QString& uri )
  : Message( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage") )
{
}

Nepomuk::IMMessage::IMMessage( const QUrl& uri )
  : Message( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage") )
{
}

Nepomuk::IMMessage::IMMessage( const QString& uri, const QUrl& type )
  : Message( uri, type )
{
}

Nepomuk::IMMessage::IMMessage( const QUrl& uri, const QUrl& type )
  : Message( uri, type )
{
}

Nepomuk::IMMessage::~IMMessage()
{
}


Nepomuk::IMMessage& Nepomuk::IMMessage::operator=( const IMMessage& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::IMMessage::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage";
}

QList<Nepomuk::IMMessage> Nepomuk::IMMessage::allIMMessages()
{
    return Nepomuk::convertResourceList<IMMessage>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage") ) );
}


