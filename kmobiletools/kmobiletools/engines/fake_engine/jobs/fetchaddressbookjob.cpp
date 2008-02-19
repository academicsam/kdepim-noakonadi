/***************************************************************************
   Copyright (C) 2007 by Matthias Lechner <matthias@lmme.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "fetchaddressbookjob.h"

#include <KDE/ThreadWeaver/Thread>
#include <KDE/KDebug>

FetchAddressbookJob::FetchAddressbookJob( QObject* parent )
 : KMobileTools::JobXP( JobXP::fetchAddressbook, parent )
{
}


FetchAddressbookJob::~FetchAddressbookJob()
{
}

void FetchAddressbookJob::run() {
    for( int i=1; i<=100; i++ ) {
        thread()->msleep( 50 );
        setProgress( i );
    }
}

void FetchAddressbookJob::requestAbort() {
	kDebug() << "========= job cancelled.";
}

#include "fetchaddressbookjob.moc"
