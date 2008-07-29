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
#include "internationaldeliveryaddress.h"


Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress()
  : PostalAddress( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#InternationalDeliveryAddress") )
{
}


Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress( const InternationalDeliveryAddress& res )
  : PostalAddress( res )
{
}


Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress( const Nepomuk::Resource& res )
  : PostalAddress( res )
{
}


Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress( const QString& uri )
  : PostalAddress( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#InternationalDeliveryAddress") )
{
}

Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress( const QUrl& uri )
  : PostalAddress( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#InternationalDeliveryAddress") )
{
}

Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress( const QString& uri, const QUrl& type )
  : PostalAddress( uri, type )
{
}

Nepomuk::InternationalDeliveryAddress::InternationalDeliveryAddress( const QUrl& uri, const QUrl& type )
  : PostalAddress( uri, type )
{
}

Nepomuk::InternationalDeliveryAddress::~InternationalDeliveryAddress()
{
}


Nepomuk::InternationalDeliveryAddress& Nepomuk::InternationalDeliveryAddress::operator=( const InternationalDeliveryAddress& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::InternationalDeliveryAddress::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#InternationalDeliveryAddress";
}

QList<Nepomuk::InternationalDeliveryAddress> Nepomuk::InternationalDeliveryAddress::allInternationalDeliveryAddresss()
{
    return Nepomuk::convertResourceList<InternationalDeliveryAddress>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#InternationalDeliveryAddress") ) );
}


