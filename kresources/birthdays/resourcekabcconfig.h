/*
    This file is part of libkcal.

    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KCAL_RESOURCEKABCCONFIG_H
#define KCAL_RESOURCEKABCCONFIG_H

#include <kresources/resource.h>
#include <kresources/configwidget.h>

class QCheckBox;
class QLabel;
class KLineEdit;

namespace KPIM {
class CategorySelectWidget;
}


namespace KCal {

/**
  Configuration widget for birtday/anniversaries resource.
  
  @see ResourceKABC
*/
class ResourceKABCConfig : public KRES::ConfigWidget
{
    Q_OBJECT
  public:
    ResourceKABCConfig( QWidget* parent );

  public Q_SLOTS:
    virtual void loadSettings( KRES::Resource *resource);
    virtual void saveSettings( KRES::Resource *resource );

  private Q_SLOTS:
    void alarmClicked();

  private:
    QCheckBox *mAlarm;
    KLineEdit *mAlarmTimeEdit;
    QLabel *mALabel;
    QCheckBox *mUseCategories;
    KPIM::CategorySelectWidget *mCategoryView;

    class Private;
    Private *d;
};

}

#endif
