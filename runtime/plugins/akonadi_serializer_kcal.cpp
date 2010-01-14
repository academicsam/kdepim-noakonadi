/*
    Copyright (c) 2007 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "akonadi_serializer_kcal.h"

#include <QtCore/qplugin.h>

#include <akonadi/item.h>
#include <kdebug.h>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<KCal::Incidence> IncidencePtr;

using namespace Akonadi;

bool SerializerPluginKCal::deserialize(Item & item, const QByteArray & label, QIODevice & data, int version)
{
  Q_UNUSED( version );

  if ( label != Item::FullPayload )
    return false;

  KCal::Incidence* i = mFormat.fromString( QString::fromUtf8( data.readAll() ) );
  if ( !i ) {
    kWarning( 5263 ) << "Failed to parse incidence!";
    data.seek( 0 );
    kWarning( 5263 ) << QString::fromUtf8( data.readAll() );
    return false;
  }
  item.setPayload<IncidencePtr>( IncidencePtr( i ) );
  return true;
}

void SerializerPluginKCal::serialize(const Item & item, const QByteArray & label, QIODevice & data, int &version)
{
  Q_UNUSED( version );

  if ( label != Item::FullPayload || !item.hasPayload<IncidencePtr>() )
    return;
  IncidencePtr i = item.payload<IncidencePtr>();
  // ### I guess this can be done without hardcoding stuff
  data.write( "BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n" );
  data.write( mFormat.toString( i.get() ).toUtf8() );
  data.write( "\nEND:VCALENDAR" );
}

Q_EXPORT_PLUGIN2( akonadi_serializer_kcal, SerializerPluginKCal )

#include "akonadi_serializer_kcal.moc"