/*
    This file is part of KitchenSync.

    Copyright (c) 2003,2004 Cornelius Schumacher <schumacher@kde.org>

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <configwidget.h>
#include <klocale.h>
#include <konnector.h>
#include <konnectorinfo.h>
#include <konnectormanager.h>

#include <qlayout.h>
#include <qlistview.h>

#include "konnectorview.h"

using namespace KSync;

class KonnectorCheckItem : public QCheckListItem
{
  public:
    KonnectorCheckItem( Konnector *konnector, QListView *view )
      : QCheckListItem( view, konnector->resourceName(), CheckBox ),
        mKonnector( konnector )
    {
    }

    Konnector *konnector() const { return mKonnector; }

  private:
    Konnector *mKonnector;
};


KonnectorView::KonnectorView( QWidget *parent, const char *name )
  : QWidget( parent, name )
{  
  QBoxLayout *topLayout = new QVBoxLayout( this );

  mKonnectorList = new KListView( this );
  mKonnectorList->addColumn( i18n( "Konnector" ) );
  mKonnectorList->setAllColumnsShowFocus( true );
  mKonnectorList->setFullWidth( true );

  topLayout->addWidget( mKonnectorList, 1 );

  updateKonnectorList();
}

void KonnectorView::updateKonnectorList()
{
  mKonnectorList->clear();

  KRES::Manager<Konnector> *manager = KonnectorManager::self();
  
  KRES::Manager<Konnector>::ActiveIterator it;
  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
    KonnectorCheckItem *item = new KonnectorCheckItem( *it, mKonnectorList );
    item->setOn( true );
  }
}

Konnector::List KonnectorView::selectedKonnectors()
{
  Konnector::List result;

  QListViewItemIterator it( mKonnectorList );
  while ( it.current() ) {
    KonnectorCheckItem *item = static_cast<KonnectorCheckItem *>( it.current() );
    if ( item->isOn() ) {
      result.append( item->konnector() );
    }
    ++it;
  }

  return result;
}

#include "konnectorview.moc"
