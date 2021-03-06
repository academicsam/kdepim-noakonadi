/*
  This file is part of KOrganizer.

  Copyright (c) 2002,2003 Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

#include "koglobals.h"
#include "koprefs.h"
#include "korganizer_part.h"

#include <libkdepim/reminderclient.h>

#include <kholidays/holidays.h>
using namespace KHolidays;

#include <k3staticdeleter.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <kcalendarsystem.h>

#include <QApplication>
#include <QPixmap>
#include <QIcon>

KOGlobals *KOGlobals::mSelf = 0;

static K3StaticDeleter<KOGlobals> koGlobalsDeleter;

KOGlobals *KOGlobals::self()
{
  if ( !mSelf ) {
    koGlobalsDeleter.setObject( mSelf, new KOGlobals );
  }

  return mSelf;
}

KOGlobals::KOGlobals()
  : mOwnInstance( "korganizer" ), mHolidays( 0 )
{
  KIconLoader::global()->addAppDir( "kdepim" );

  mReminderClient = new KPIM::ReminderClient;
}

KConfig *KOGlobals::config() const
{
  KSharedConfig::Ptr c = mOwnInstance.config();
  return c.data();
}

KOGlobals::~KOGlobals()
{
  delete mReminderClient;
  delete mHolidays;
}

const KCalendarSystem *KOGlobals::calendarSystem() const
{
  return KGlobal::locale()->calendar();
}

KPIM::ReminderClient *KOGlobals::reminderClient() const
{
  return mReminderClient;
}

void KOGlobals::fitDialogToScreen( QWidget *wid, bool force )
{
  bool resized = false;

  int w = wid->frameSize().width();
  int h = wid->frameSize().height();

  QRect desk = KGlobalSettings::desktopGeometry( wid );
  if ( w > desk.width() ) {
    w = desk.width();
    resized = true;
  }
  // FIXME: ugly hack.  Is the -30 really to circumvent the size of kicker?!
  if ( h > desk.height() - 30 ) {
    h = desk.height() - 30;
    resized = true;
  }

  if ( resized || force ) {
    wid->resize( w, h );
    wid->move( desk.x(), desk.y()+15 );
    if ( force ) {
      wid->setFixedSize( w, h );
    }
  }
}

bool KOGlobals::reverseLayout()
{
  return QApplication::isRightToLeft();
}

QPixmap KOGlobals::smallIcon( const QString &name ) const
{
  return SmallIcon( name );
}

QStringList KOGlobals::holiday( const QDate &date ) const
{
  QStringList hdays;

  if ( !mHolidays ) {
    return hdays;
  }
  const Holiday::List list = mHolidays->holidays( date );
  for ( int i = 0; i < list.count(); ++i ) {
    hdays.append( list.at( i ).text() );
  }
  return hdays;
}

bool KOGlobals::isWorkDay( const QDate &date ) const
{
  int mask( ~( KOPrefs::instance()->mWorkWeekMask ) );

  bool nonWorkDay = ( mask & ( 1 << ( date.dayOfWeek() - 1 ) ) );
  if ( KOPrefs::instance()->mExcludeHolidays && mHolidays ) {
    const Holiday::List list = mHolidays->holidays( date );
    for ( int i = 0; i < list.count(); ++i ) {
      nonWorkDay = nonWorkDay || ( list.at( i ).dayType() == Holiday::NonWorkday );
    }
  }
  return !nonWorkDay;
}

int KOGlobals::getWorkWeekMask()
{
  return KOPrefs::instance()->mWorkWeekMask;
}

void KOGlobals::setHolidays( HolidayRegion *h )
{
  delete mHolidays;
  mHolidays = h;
}

HolidayRegion *KOGlobals::holidays() const
{
  return mHolidays;
}
