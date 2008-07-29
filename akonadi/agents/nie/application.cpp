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
#include "application.h"


Nepomuk::Application::Application()
  : Software( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Application") )
{
}


Nepomuk::Application::Application( const Application& res )
  : Software( res )
{
}


Nepomuk::Application::Application( const Nepomuk::Resource& res )
  : Software( res )
{
}


Nepomuk::Application::Application( const QString& uri )
  : Software( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Application") )
{
}

Nepomuk::Application::Application( const QUrl& uri )
  : Software( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Application") )
{
}

Nepomuk::Application::Application( const QString& uri, const QUrl& type )
  : Software( uri, type )
{
}

Nepomuk::Application::Application( const QUrl& uri, const QUrl& type )
  : Software( uri, type )
{
}

Nepomuk::Application::~Application()
{
}


Nepomuk::Application& Nepomuk::Application::operator=( const Application& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Application::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Application";
}

QList<Nepomuk::Application> Nepomuk::Application::allApplications()
{
    return Nepomuk::convertResourceList<Application>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Application") ) );
}


