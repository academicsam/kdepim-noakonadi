/* pilotDateEntry.h	-*- C++ -*-	KPilot
**
** Copyright (C) 1998-2001 by Dan Pilone
**
** See the .cc file for an explanation of what this file is for.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, 
** MA 02139, USA.
*/

/*
** Bug reports and questions can be sent to groot@kde.org
*/
#ifndef _KPILOT_PILOTDATEENTRY_H
#define _KPILOT_PILOTDATEENTRY_H

#include <time.h>
#include <string.h>

#ifndef QBITARRAY_H
#include <qbitarray.h>
#endif

#ifndef _PILOT_MACROS_H_
#include <pi-macros.h>
#endif

#ifndef _PILOT_DATEBOOK_H_
#include <pi-datebook.h>
#endif

#ifndef _KPILOT_PILOTAPPCATEGORY_H
#include "pilotAppCategory.h"
#endif

#ifndef _KPILOT_PILOTRECORD_H
#include "pilotRecord.h"
#endif



class PilotDateEntry : public PilotAppCategory
{
public:
  PilotDateEntry(void);
  PilotDateEntry(PilotRecord* rec);
  ~PilotDateEntry() { free_Appointment(&fAppointmentInfo); }

  PilotDateEntry(const PilotDateEntry &e);

  PilotDateEntry& operator=(const PilotDateEntry &e);
  
  PilotRecord* pack() { return PilotAppCategory::pack(); }
  
  bool isEvent() const { return fAppointmentInfo.event; }
  int getEvent() const { return fAppointmentInfo.event; }
  void setEvent(int event) { fAppointmentInfo.event = event; }
  
  struct tm getEventStart() const { return fAppointmentInfo.begin; }
  const struct tm *getEventStart_p() const { return &fAppointmentInfo.begin; }
  void setEventStart(struct tm& start) { fAppointmentInfo.begin = start; }

  struct tm getEventEnd() const { return fAppointmentInfo.end; }
  const struct tm *getEventEnd_p() const { return &fAppointmentInfo.end; }
  void setEventEnd(struct tm& end) { fAppointmentInfo.end = end; }

  int getAlarm() const { return fAppointmentInfo.alarm; }
  void setAlarm(int alarm) { fAppointmentInfo.alarm = alarm; }
  
  int getAdvance() const { return fAppointmentInfo.advance; }
  void setAdvance(int advance) { fAppointmentInfo.advance = advance; }

  int getAdvanceUnits() const { return fAppointmentInfo.advanceUnits; }
  void setAdvanceUnits(int units) { fAppointmentInfo.advanceUnits = units; }

  // The following need set routines written
  repeatTypes getRepeatType() const { return fAppointmentInfo.repeatType; }
  void setRepeatType(repeatTypes r) { fAppointmentInfo.repeatType = r; }

  int getRepeatForever() const { return fAppointmentInfo.repeatForever; }
  void setRepeatForever(int f = 1) { fAppointmentInfo.repeatForever = f; }

  struct tm getRepeatEnd() const { return fAppointmentInfo.repeatEnd; }
  void setRepeatEnd(struct tm tm) { fAppointmentInfo.repeatEnd = tm; }

  int getRepeatFrequency() const { return fAppointmentInfo.repeatFrequency; }
  void setRepeatFrequency(int f) { fAppointmentInfo.repeatFrequency = f; }

  DayOfMonthType getRepeatDay() const { return fAppointmentInfo.repeatDay; }
  void setRepeatDay(DayOfMonthType rd) { fAppointmentInfo.repeatDay = rd; };

  const int *getRepeatDays() const { return fAppointmentInfo.repeatDays; }
  void setRepeatDays(int *rd) {
    for (int i = 0; i < 7; i++)
      fAppointmentInfo.repeatDays[i] = rd[i];
  }
  void setRepeatDays(QBitArray rba) {
    for (int i = 0; i < 7; i++)
      fAppointmentInfo.repeatDays[i] = (rba[i] ? 1 : 0);
  }

  int getExceptionCount() const { return fAppointmentInfo.exceptions; }
  void setExceptionCount(int e) { fAppointmentInfo.exceptions = e; }

  const struct tm *getExceptions() const { return fAppointmentInfo.exception; }
  void setExceptions(struct tm *e);

  void  setDescription(const char* desc);
  const char* getDescription() const { return fAppointmentInfo.description; }

  void  setNote(const char* note);
  const char* getNote() const { return fAppointmentInfo.note; }

  bool isMultiDay() const {
    return ((fAppointmentInfo.repeatType == repeatDaily) &&
            (fAppointmentInfo.repeatFrequency == 1) &&
            (!fAppointmentInfo.repeatForever) &&
            fAppointmentInfo.event);
  }

protected:
  void *pack(void *, int *);
  void unpack(const void *, int = 0) { }

private:
  struct Appointment fAppointmentInfo;

	void _copyExceptions(const PilotDateEntry &e);
};



#else
#ifdef DEBUG
#warning "File doubly included"
#endif
#endif



// $Log$
// Revision 1.6  2002/07/23 00:49:18  kainhofe
// Fixed several bugs with recurrences.
//
// Revision 1.5  2002/05/14 22:57:40  adridg
// Merge from _BRANCH
//
// Revision 1.4  2002/04/16 23:42:27  kainhofe
// setExceptions now deletes the old exceptions array to prevent memory leaks
//
// Revision 1.3.2.1  2002/04/28 16:10:15  kainhofe
// Checks for null pointer before accessing it. Exceptions are deleted before the new are set.
//
// Revision 1.3  2002/01/21 23:14:03  adridg
// Old code removed; extra abstractions added; utility extended
//
// Revision 1.2  2001/12/28 12:55:24  adridg
// Fixed email addresses; added isBackup() to interface
//
// Revision 1.1  2001/12/27 23:08:30  adridg
// Restored some deleted wrapper files
//
// Revision 1.11  2001/06/18 19:51:40  cschumac
// Fixed todo and datebook conduits to cope with KOrganizers iCalendar format.
// They use libkcal now.
//
// Revision 1.10  2001/05/24 10:31:38  adridg
// Philipp Hullmann's extensive memory-leak hunting patches
//
// Revision 1.9  2001/04/16 13:48:35  adridg
// --enable-final cleanup and #warning reduction
//
// Revision 1.8  2001/04/01 17:32:06  adridg
// Fiddling around with date properties
//
// Revision 1.7  2001/03/09 09:46:15  adridg
// Large-scale #include cleanup
//
// Revision 1.6  2001/02/06 08:05:20  adridg
// Fixed copyright notices, added CVS log, added surrounding #ifdefs. No code changes.
//
