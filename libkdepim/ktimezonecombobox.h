/*
  This file is part of KOrganizer.

  Copyright (C) 2007 Bruno Virlet <bruno.virlet@gmail.com>
  Copyright 2008-2009 Allen Winter <winter@kde.org>

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

#ifndef KDEPIM_KTIMEZONECOMBOBOX_H
#define KDEPIM_KTIMEZONECOMBOBOX_H

#include "kdepim_export.h"

#include <KComboBox>
#include <KDateTime>

namespace KCal {
  class Calendar;
}

namespace KPIM {

/**
 * A combobox that shows the system timezones available in KSystemTimeZones::zones()
 * and provides methods to easily select the item corresponding to a given
 * KDateTime::Spec or to retrieve the KDateTime::Spec associated with the
 * selected item.
 */
class KDEPIM_EXPORT KTimeZoneComboBox : public KComboBox
{
  public:
    explicit KTimeZoneComboBox( KCal::Calendar *calendar, QWidget *parent = 0 );
    ~KTimeZoneComboBox();

    /**
     * Sets the associated calendar.
     * @param calendar is a pointer to a Calendar instance.
     */
    void setCalendar( KCal::Calendar *calendar );

    /**
     * Selects the item in the combobox corresponding to the given @p spec.
     */
    void selectTimeSpec( const KDateTime::Spec &spec );

    /**
     * Convenience version of selectTimeSpec(const KDateTime::Spec &).
     * Selects the local timezone specified in the user settings.
     */
    void selectLocalTimeSpec();

    /**
     * If @p floating is true, selects floating timezone.
     * Else if @spec is valid, selects @p spec timezone, if not selects
     * local timezone.
     */
    void setFloating( bool floating, const KDateTime::Spec &spec = KDateTime::Spec() );

    /**
     * @returns The timespec associated with the currently selected item.
     */
    KDateTime::Spec selectedTimeSpec();

  private:
    class Private;
    Private *const d;
};

}

#endif
