/*
    This file is part of KAddressbook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#include "xxportmanager.h"

#include <kabc/addressbook.h>
#include <kabc/resource.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kservicetypetrader.h>

#include "core.h"
#include "kablock.h"
#include "undocmds.h"
#include "xxportselectdialog.h"

KUrl XXPortManager::importURL = KUrl();
QString XXPortManager::importData = QString();

XXPortManager::XXPortManager( KAB::Core *core, QObject *parent, const char *name )
  : QObject( parent ), mCore( core )
{
  setObjectName( name );
  loadPlugins();
}

XXPortManager::~XXPortManager()
{
}

void XXPortManager::restoreSettings()
{
}

void XXPortManager::saveSettings()
{
}

void XXPortManager::importVCard( const KUrl &url )
{
  importURL = url;
  slotImport( "vcard", "<empty>" );
  importURL = KUrl();
}

void XXPortManager::importVCardFromData( const QString &vCard )
{
  importData = vCard;
  slotImport( "vcard", "<empty>" );
  importData = "";
}

void XXPortManager::slotImport( const QString &identifier, const QString &data )
{
  KAB::XXPort *obj = mXXPortObjects[ identifier ];
  if ( !obj ) {
    KMessageBox::error( mCore->widget(), i18n( "<qt>No import plugin available for <b>%1</b>.</qt>", identifier ) );
    return;
  }

  KABC::Resource *resource = mCore->requestResource( mCore->widget() );
  if ( !resource )
    return;

  KABC::Addressee::List list = obj->importContacts( data );
  KABC::Addressee::List::Iterator it;
  for ( it = list.begin(); it != list.end(); ++it )
    (*it).setResource( resource );

  if ( !list.isEmpty() ) {
    NewCommand *command = new NewCommand( mCore->addressBook(), list );
    mCore->commandHistory()->push( command );
    emit modified();
  }
}

void XXPortManager::slotExport( const QString &identifier, const QString &data )
{
  KAB::XXPort *obj = mXXPortObjects[ identifier ];
  if ( !obj ) {
    KMessageBox::error( mCore->widget(), i18n( "<qt>No export plugin available for <b>%1</b>.</qt>", identifier ) );
    return;
  }

  KABC::AddresseeList addrList;
  XXPortSelectDialog dlg( mCore, obj->requiresSorting(), mCore->widget() );
  if ( dlg.exec() )
    addrList = dlg.contacts();
  else
    return;

  if ( !obj->exportContacts( addrList, data ) )
    KMessageBox::error( mCore->widget(), i18n( "Unable to export contacts." ) );
}

void XXPortManager::loadPlugins()
{
  mXXPortObjects.clear();

  const KService::List plugins = KServiceTypeTrader::self()->query( "KAddressBook/XXPort",
    QString( "[X-KDE-KAddressBook-XXPortPluginVersion] == %1" ).arg( KAB_XXPORT_PLUGIN_VERSION ) );
  foreach ( KService::Ptr pluginService, plugins ) {
    KPluginFactory *factory = KPluginLoader( *pluginService ).factory();
    if ( !factory ) {
      kDebug(5720) <<"XXPortManager::loadExtensions(): Factory creation failed";
      continue;
    }

    KAB::XXPortFactory *xxportFactory = qobject_cast<KAB::XXPortFactory*>( factory );

    if ( !xxportFactory ) {
      kDebug(5720) <<"XXPortManager::loadExtensions(): Cast failed";
      continue;
    }

    KAB::XXPort *obj = xxportFactory->xxportObject( mCore->addressBook(), mCore->widget() );
    if ( obj ) {
      if ( mCore->guiClient() )
        mCore->guiClient()->insertChildClient( obj );

      mXXPortObjects.insert( obj->identifier(), obj );
      connect( obj, SIGNAL( exportActivated( const QString&, const QString& ) ),
               this, SLOT( slotExport( const QString&, const QString& ) ) );
      connect( obj, SIGNAL( importActivated( const QString&, const QString& ) ),
               this, SLOT( slotImport( const QString&, const QString& ) ) );

      obj->setKApplication( kapp );
    }
  }
}

#include "xxportmanager.moc"
