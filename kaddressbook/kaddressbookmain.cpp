/*
    This file is part of KAddressbook.
    Copyright (c) 1999 Don Sanders <dsanders@kde.org>

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

#include "kaddressbookmain.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kedittoolbar.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kshortcutsdialog.h>
#include <kstandardaction.h>
#include <kstatusbar.h>
#include <libkdepim/statusbarprogresswidget.h>
#include <libkdepim/progressdialog.h>

#include "kabcore.h"
#include "kaddressbookadaptor.h"

KAddressBookMain::KAddressBookMain( const QString &file )
  : KXmlGuiWindow( 0 )
{
  // Set this to be the group leader for all subdialogs - this means
  // modal subdialogs will only affect this dialog, not the other windows
  setAttribute( Qt::WA_GroupLeader );

  setCaption( i18n( "Address Book Browser" ) );

  mCore = new KABCore( this, true, this, file );
  mCore->restoreSettings();

  initActions();

  setCentralWidget( mCore->widget() );

  statusBar()->show();
  statusBar()->insertItem( "", 1 );

  KPIM::ProgressDialog *progressDialog = new KPIM::ProgressDialog( statusBar(),
    this );
  progressDialog->hide();

  KPIM::StatusbarProgressWidget *progressWidget;
  progressWidget = new KPIM::StatusbarProgressWidget( progressDialog,
    statusBar() );
  progressWidget->show();

  statusBar()->addPermanentWidget( progressWidget, 0 );

  mCore->setStatusBar( statusBar() );

  setStandardToolBarMenuEnabled( true );

  createGUI( "kaddressbookui.rc" );

  resize( 400, 300 ); // initial size
  setAutoSaveSettings();

  new CoreAdaptor( this );
  QDBusConnection::sessionBus().registerObject("/KAddressBook", this, QDBusConnection::ExportAdaptors);
}

KAddressBookMain::~KAddressBookMain()
{
  mCore->saveSettings();
}

void KAddressBookMain::addEmail( QString addr )
{
  mCore->addEmail( addr );
}

void KAddressBookMain::importVCard( const KUrl& url )
{
  mCore->importVCard( url );
}

void KAddressBookMain::importVCardFromData( const QString& vCard )
{
  mCore->importVCardFromData( vCard );
}

void KAddressBookMain::showContactEditor( QString uid )
{
  mCore->editContact( uid );
}

void KAddressBookMain::newDistributionList()
{
  mCore->newDistributionList();
}

void KAddressBookMain::newContact()
{
  mCore->newContact();
}

QString KAddressBookMain::getNameByPhone( QString phone )
{
  return mCore->getNameByPhone( phone );
}

void KAddressBookMain::save()
{
  mCore->save();
}

void KAddressBookMain::exit()
{
  close();
}

bool KAddressBookMain::handleCommandLine()
{
  return mCore->handleCommandLine();
}

void KAddressBookMain::saveProperties( KConfigGroup& )
{
}

void KAddressBookMain::readProperties( const KConfigGroup& )
{
}

bool KAddressBookMain::queryClose()
{
  return mCore->queryClose();
}

void KAddressBookMain::initActions()
{
  KStandardAction::quit( this, SLOT( close() ), actionCollection() );

  KAction *action;
  action = KStandardAction::keyBindings( this, SLOT( configureKeyBindings() ), actionCollection() );
  action->setWhatsThis( i18n( "You will be presented with a dialog, where you can configure the application wide shortcuts." ) );

  KStandardAction::configureToolbars( this, SLOT( configureToolbars() ), actionCollection() );
}

void KAddressBookMain::configureKeyBindings()
{
  KShortcutsDialog::configure( actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this );
}

void KAddressBookMain::configureToolbars()
{
  saveMainWindowSettings( KGlobal::config()->group( "MainWindow" ) );

  KEditToolBar edit( factory() );
  connect( &edit, SIGNAL( newToolbarConfig() ),
           this, SLOT( newToolbarConfig() ) );

  edit.exec();
}

void KAddressBookMain::newToolbarConfig()
{
  createGUI( "kaddressbookui.rc" );
  applyMainWindowSettings( KGlobal::config()->group( "MainWindow" ) );
}

#include "kaddressbookmain.moc"
