#ifndef _OrganizerCONDUIT_H
#define _OrganizerCONDUIT_H

/* Organizer-conduit.h			KPilot
**
** Copyright (C) 1998-2001 Dan Pilone
** Copyright (C) 1998-2000 Preston Brown
** Copyright (C) 1998 Herwin-Jan Steehouwer
** Copyright (C) 1998 Reinhold Kainhofer
**
** This file is part of the Organizer conduit, a conduit for KPilot that
** synchronises the Pilot's Organizer application with the outside world,
** which currently means KOrganizer.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
** MA 02139, USA.
*/

/*
** Bug reports and questions can be sent to groot@kde.org
*/

#include "MultiDB-conduit.h"
#ifndef _KPILOT_PILOTOrganizerENTRY_H
#include "pilotOrganizerEntry.h"
#endif
#include <pilotSerialDatabase.h>
#include <pilotLocalDatabase.h>
#include <calendar.h>

#define MAX_HIER_LEVEL 10

using namespace KCal;
#define FLG_SORTED 1
#define FLG_NUMBERED 2

class OrganizerConduit : public MultiDBConduit {
	Q_OBJECT
public:
	OrganizerConduit(KPilotDeviceLink *, const char *n=0L, const QStringList &l=QStringList(), SyncTypeList_t *tps=NULL);
	virtual ~OrganizerConduit();
	virtual unsigned long flags() const { return FLG_SORTED; }

protected:
	virtual void updateLocalEntry(PilotRecord *rec, bool force=false);
	
// TODEL	virtual bool GetSyncType(DBInfo dbinfo, DBSyncInfo*syncinfo);
	virtual PilotOrganizerEntry*createOrganizerEntry(PilotRecord *rec=NULL)=0;
	virtual PilotOrganizerEntry*createOrganizerEntry(KCal::Todo *todo=NULL)=0;
	// let child classes read and set fields from the todo to the palm entry
	virtual void getCustomFields(Todo*, PilotOrganizerEntry*) {} ;
	virtual void setCustomFields(Todo*, PilotOrganizerEntry*) {} ;
	
	virtual const QString getSyncTypeEntry() { return "SyncAction";};
	virtual const QString getSyncFileEntry() { return "SyncFile";};
	virtual const QString settingsFileList() { return "Databases";};
	// merging the databases:
	virtual void insertRecordToPC(int pos, PilotRecord*rec);
	virtual void insertRecordToPalm(int pos, KCal::Todo*todo);
	virtual void updateRecords(int palmpos, PilotRecord*rec, int pcpos, KCal::Todo*todo);
	virtual void movePCRecord(int frompos, int topos);
	virtual void movePalmRecord(int frompos, int topos);
	virtual bool preSyncAction(DBSyncInfo*dbinfo);


protected slots:
// TODEL	virtual void syncNextDB();
	virtual void syncNextRecord();
	virtual void cleanup();
	virtual void cleanupDB();
	virtual void exec();
	
protected:
	KCal::Calendar *fCalendar;
	QString fCalendarFile;
	// these are used to count the current position on the palm and
	// in the PC database when merging the two together.
	int PCix, Palmix;
	// inserted determines if any entries have been moved/inserted before the current record in the palm db
	bool inserted;
	QString timezone;
	
private:
	class VCalPrivate;
	VCalPrivate *fP;
	Todo* levelparent[MAX_HIER_LEVEL];
	int hierlevel;
	unsigned long previd;
};


// $Log$
// Revision 1.1  2002/04/07 12:09:43  kainhofe
// Initial checkin of the conduit. The gui works mostly, but syncing crashes KPilot...
//
// Revision 1.1  2002/04/07 01:03:52  reinhold
// the list of possible actions is now created dynamically
//
// Revision 1.11  2002/04/06 00:51:08  reinhold
// Finally the conduit compiles again... Still have a lot of TODOS
//
// Revision 1.10  2002/04/05 21:17:00  reinhold
// *** empty log message ***
//
// Revision 1.9  2002/04/01 09:22:11  reinhold
// Implemented the syncNextRecord routine
//
// Revision 1.8  2002/03/28 13:47:54  reinhold
// Added the list of synctypes, aboutbox is now directly passed on to the setup dlg (instead of being a static var)
//
// Revision 1.6  2002/03/23 21:46:42  reinhold
// config  dlg works, but the last changes crash the plugin itself
//
// Revision 1.5  2002/03/23 18:21:14  reinhold
// Cleaned up the structure. Works with QTimer instead of loops.
//
// Revision 1.4  2002/03/15 20:43:17  reinhold
// Fixed the crash on loading (member function not defined)...
//
// Revision 1.3  2002/03/10 23:58:32  reinhold
// Made the conduit compile...
//
// Revision 1.2  2002/03/10 16:06:43  reinhold
// Cleaned up the class hierarchy, implemented some more features (should be quite finished now...)
//
// Revision 1.1.1.1  2002/03/09 15:38:45  reinhold
// Initial checin of the  project manager / List manager conduit.
//
//
//
#endif
