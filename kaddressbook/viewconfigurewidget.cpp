/*
    This file is part of KAddressBook.
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "viewconfigurewidget.h"

#include <QtGui/QPixmap>
#include <QtGui/QVBoxLayout>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpagewidget.h>
#include <ktoolinvocation.h>
#include <kvbox.h>

#include "viewconfigurefieldspage.h"
#include "viewconfigurefilterpage.h"
#include "viewmanager.h"

ViewConfigureWidget::ViewConfigureWidget( KABC::AddressBook *ab, QWidget *parent )
  : KAB::ConfigureWidget( ab, parent )
{
  QVBoxLayout *topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 0 );
  topLayout->setSpacing( KDialog::spacingHint() );

  mMainWidget = new KPageWidget( this );
  mMainWidget->setFaceType( KPageView::List );
  topLayout->addWidget( mMainWidget );

  // Add the first page, the attributes
  KVBox *page = addPage( i18n( "Fields" ), QString(),
                         KIconLoader::global()->loadIcon( "view-list-details",
                         KIconLoader::Panel ) );

  // Add the select fields page
  mFieldsPage = new ViewConfigureFieldsPage( addressBook(), page );

  // Add the second page, the filter selection
  page = addPage( i18n( "Default Filter" ), QString(),
                  KIconLoader::global()->loadIcon( "view-filter",
                  KIconLoader::Panel ) );

  mFilterPage = new ViewConfigureFilterPage( page );
}

ViewConfigureWidget::~ViewConfigureWidget()
{
}

void ViewConfigureWidget::restoreSettings( const KConfigGroup &config )
{
  mFieldsPage->restoreSettings( config );
  mFilterPage->restoreSettings( config );
}

void ViewConfigureWidget::saveSettings( KConfigGroup &config )
{
  mFieldsPage->saveSettings( config );
  mFilterPage->saveSettings( config );
}

KVBox *ViewConfigureWidget::addPage( const QString &item, const QString &header,
                                   const QPixmap &pixmap )
{
  KVBox *page = new KVBox( mMainWidget );
  KPageWidgetItem*itemPage=mMainWidget->addPage( page, item );
  itemPage->setHeader(header);
  itemPage->setIcon(KIcon(pixmap));
  return page;
}

ViewConfigureDialog::ViewConfigureDialog( ViewConfigureWidget *wdg, const QString &viewName,
                                          QWidget *parent )
  : KDialog( parent ), mConfigWidget( wdg )
{
  setCaption( i18n( "Modify View: " ) + viewName );
  setButtons( Help | Ok | Cancel );
  setDefaultButton( Ok );
  showButtonSeparator( true );

  setMainWidget( mConfigWidget );
  connect(this,SIGNAL(helpClicked()),this,SLOT(slotHelp()));
  resize( 600, 300 );
}

ViewConfigureDialog::~ViewConfigureDialog()
{
}

void ViewConfigureDialog::restoreSettings( const KConfigGroup &config )
{
  mConfigWidget->restoreSettings( config );
}

void ViewConfigureDialog::saveSettings( KConfigGroup &config )
{
  mConfigWidget->saveSettings( config );
}

void ViewConfigureDialog::slotHelp()
{
  KToolInvocation::invokeHelp( "using-views" );
}

#include "viewconfigurewidget.moc"
