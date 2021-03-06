/*
    This file is part of KAddressBook.
    Copyright (C) 1999 Don Sanders <sanders@kde.org>
                  2005 Tobias Koenig <tokoe@kde.org>

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

#include "undocmds.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QMimeData>

#include <kabc/resource.h>
#include <kapplication.h>
#include <klocale.h>
#include <krandom.h>

#include "addresseeconfig.h"
#include "addresseeutil.h"
#include "core.h"
#include "kablock.h"

bool Command::resourceExist( KABC::Resource *resource )
{
 foreach( KABC::Resource *res, addressBook()->resources() ) {
    if ( res == resource )
      return true;
  }
  return false;
}

DeleteCommand::DeleteCommand( KABC::AddressBook *addressBook,
                              const QStringList &uidList)
  : Command( addressBook ), mUIDList( uidList )
{
}

QString DeleteCommand::text() const
{
  return i18np( "Delete Contact", "Delete %1 Contacts", mUIDList.count() );
}

void DeleteCommand::undo()
{
  // Put it back in the document
  KABC::Addressee::List::ConstIterator it;
  const KABC::Addressee::List::ConstIterator endIt( mAddresseeList.constEnd() );

  // lock resources
  for ( it = mAddresseeList.constBegin(); it != endIt; ++it )
    lock()->lock( (*it).resource() );

  for ( it = mAddresseeList.constBegin(); it != endIt; ++it ) {
    if ( resourceExist( ( *it ).resource() ) )
      addressBook()->insertAddressee( *it );
    lock()->unlock( (*it).resource() );
  }

  mAddresseeList.clear();
}

void DeleteCommand::redo()
{
  KABC::Addressee addr;

  QStringList::ConstIterator it;
  const QStringList::ConstIterator endIt( mUIDList.constEnd() );
  for ( it = mUIDList.constBegin(); it != endIt; ++it ) {
    addr = addressBook()->findByUid( *it );
    lock()->lock( addr.resource() );
    mAddresseeList.append( addr );
    AddresseeConfig cfg( addr );
    cfg.remove();
  }

  KABC::Addressee::List::ConstIterator addrIt;
  const KABC::Addressee::List::ConstIterator addrEndIt( mAddresseeList.constEnd() );
  for ( addrIt = mAddresseeList.constBegin(); addrIt != addrEndIt; ++addrIt ) {
    if ( resourceExist( ( *addrIt ).resource() ) )
      addressBook()->removeAddressee( *addrIt );
    lock()->unlock( (*addrIt).resource() );
  }
}

DeleteDistListsCommand::DeleteDistListsCommand( KABC::AddressBook *addressBook,
                                                const QStringList &uidList)
  : Command( addressBook ), mUIDList( uidList )
{
}

DeleteDistListsCommand::~DeleteDistListsCommand()
{
  qDeleteAll( mDistributionLists );
}

QString DeleteDistListsCommand::text() const
{
  return i18np( "Delete Distribution List",
                "Delete %1 Distribution Lists", mUIDList.count() );
}

void DeleteDistListsCommand::undo()
{
  // Put it back in the document
  QList<KABC::DistributionList*>::ConstIterator it;
  const QList<KABC::DistributionList*>::ConstIterator
    endIt( mDistributionLists.constEnd() );

  // lock resources
  for ( it = mDistributionLists.constBegin(); it != endIt; ++it )
    lock()->lock( (*it)->resource() );

  for ( it = mDistributionLists.constBegin(); it != endIt; ++it ) {
    (*it)->resource()->insertDistributionList( *it );
    lock()->unlock( (*it)->resource() );
  }

  mDistributionLists.clear();
}

void DeleteDistListsCommand::redo()
{
  KABC::DistributionList *list;

  QStringList::ConstIterator it;
  const QStringList::ConstIterator endIt( mUIDList.constEnd() );
  for ( it = mUIDList.constBegin(); it != endIt; ++it ) {
    list = addressBook()->findDistributionListByIdentifier( *it );
    if ( !list )
      continue;
    lock()->lock( list->resource() );
    mDistributionLists.append( list );
  }

  QList<KABC::DistributionList*>::ConstIterator distListIt;
  const QList<KABC::DistributionList*>::ConstIterator
    distListEndIt( mDistributionLists.constEnd() );
  for ( distListIt = mDistributionLists.constBegin(); distListIt != distListEndIt;
        ++distListIt ) {
    addressBook()->removeDistributionList( *distListIt );
    lock()->unlock( (*distListIt)->resource() );
  }
}

PasteCommand::PasteCommand( KAB::Core *core, const KABC::Addressee::List &addressees )
  : Command( core->addressBook() ), mAddresseeList( addressees ), mCore( core )
{
}

QString PasteCommand::text() const
{
  return i18np( "Paste Contact", "Paste %1 Contacts", mAddresseeList.count() );
}

void PasteCommand::undo()
{
  KABC::Addressee::List::ConstIterator it;
  const KABC::Addressee::List::ConstIterator endIt( mAddresseeList.constEnd() );

  // lock resources
  for ( it = mAddresseeList.constBegin(); it != endIt; ++it )
    lock()->lock( (*it).resource() );

  for ( it = mAddresseeList.constBegin(); it != endIt; ++it ) {
    if ( resourceExist( ( *it ).resource() ) )
      addressBook()->removeAddressee( *it );
    lock()->unlock( (*it).resource() );
  }
}

void PasteCommand::redo()
{
  QStringList uids;

  KABC::Addressee::List::ConstIterator constIt;
  const KABC::Addressee::List::ConstIterator constEndIt( mAddresseeList.constEnd() );

  // lock resources
  for ( constIt = mAddresseeList.constBegin(); constIt != constEndIt; ++constIt )
    lock()->lock( (*constIt).resource() );

  KABC::Addressee::List::Iterator it;
  const KABC::Addressee::List::Iterator endIt( mAddresseeList.end() );
  for ( it = mAddresseeList.begin(); it != endIt; ++it ) {
    if ( resourceExist( ( *it ).resource() ) ) {

      /**
         We have to set a new uid for the contact, otherwise insertAddressee()
         ignore it.
      */
      (*it).setUid( KRandom::randomString( 10 ) );
      uids.append( (*it).uid() );
      addressBook()->insertAddressee( *it );
    }
    lock()->unlock( (*it).resource() );
  }
}


NewCommand::NewCommand( KABC::AddressBook *addressBook, const KABC::Addressee::List &addressees )
  : Command( addressBook ), mAddresseeList( addressees )
{
}

QString NewCommand::text() const
{
  return i18np( "New Contact", "New %1 Contacts", mAddresseeList.count() );
}

void NewCommand::undo()
{
  KABC::Addressee::List::ConstIterator it;
  const KABC::Addressee::List::ConstIterator endIt( mAddresseeList.constEnd() );

  // lock resources
  for ( it = mAddresseeList.constBegin(); it != endIt; ++it )
    lock()->lock( (*it).resource() );

  for ( it = mAddresseeList.constBegin(); it != endIt; ++it ) {
    if ( resourceExist( ( *it ).resource() ) )
      addressBook()->removeAddressee( *it );
    lock()->unlock( (*it).resource() );
  }
}

void NewCommand::redo()
{
  KABC::Addressee::List::Iterator it;
  const KABC::Addressee::List::Iterator endIt( mAddresseeList.end() );

  // lock resources
  for ( it = mAddresseeList.begin(); it != endIt; ++it )
    lock()->lock( (*it).resource() );

  for ( it = mAddresseeList.begin(); it != endIt; ++it ) {
    if ( resourceExist( ( *it ).resource() ) )
      addressBook()->insertAddressee( *it );
    lock()->unlock( (*it).resource() );
  }
}


EditCommand::EditCommand( KABC::AddressBook *addressBook,
                          const KABC::Addressee &oldAddressee,
                          const KABC::Addressee &newAddressee )
  : Command( addressBook ),
    mOldAddressee( oldAddressee ), mNewAddressee( newAddressee )
{
}

QString EditCommand::text() const
{
  return i18n( "Edit Contact" );
}

void EditCommand::undo()
{
  if ( resourceExist( mOldAddressee.resource() ) )
  {
    lock()->lock( mOldAddressee.resource() );
    addressBook()->insertAddressee( mOldAddressee );
    lock()->unlock( mOldAddressee.resource() );
  }
}

void EditCommand::redo()
{
  if ( resourceExist( mNewAddressee.resource() ) )
  {
    lock()->lock( mNewAddressee.resource() );
    addressBook()->insertAddressee( mNewAddressee );
    lock()->unlock( mNewAddressee.resource() );
  }
}


CutCommand::CutCommand( KABC::AddressBook *addressBook, const QStringList &uidList )
  : Command( addressBook ), mUIDList( uidList )
{
}

QString CutCommand::text() const
{
  return i18np( "Cut Contact", "Cut %1 Contacts", mUIDList.count() );
}

void CutCommand::undo()
{
  KABC::Addressee::List::ConstIterator it;
  const KABC::Addressee::List::ConstIterator endIt( mAddresseeList.constEnd() );

  // lock resources
  for ( it = mAddresseeList.constBegin(); it != endIt; ++it )
    lock()->lock( (*it).resource() );

  for ( it = mAddresseeList.constBegin(); it != endIt; ++it ) {
    if ( resourceExist( ( *it ).resource() ) )
      addressBook()->insertAddressee( *it );
    lock()->unlock( (*it).resource() );
  }

  mAddresseeList.clear();

  QClipboard *cb = QApplication::clipboard();
  kapp->processEvents();
  cb->setText( mOldText );
}

void CutCommand::redo()
{
  KABC::Addressee addr;

  QStringList::ConstIterator it;
  const QStringList::ConstIterator endIt( mUIDList.constEnd() );
  for ( it = mUIDList.constBegin(); it != endIt; ++it ) {
    addr = addressBook()->findByUid( *it );
    mAddresseeList.append( addr );
    lock()->lock( addr.resource() );
  }

  KABC::Addressee::List::ConstIterator addrIt;
  const KABC::Addressee::List::ConstIterator addrEndIt( mAddresseeList.constEnd() );
  for ( addrIt = mAddresseeList.constBegin(); addrIt != addrEndIt; ++addrIt ) {
    if ( resourceExist( ( *addrIt ).resource() ) )
      addressBook()->removeAddressee( *addrIt );
    lock()->unlock( addr.resource() );
  }

  QClipboard *cb = QApplication::clipboard();
  mOldText = cb->text();
  kapp->processEvents();
  // Convert to clipboard
  mClipText = AddresseeUtil::addresseesToClipboard( mAddresseeList );
  QMimeData *data = new QMimeData();
  data->setData( "text/directory", mClipText );
  cb->setMimeData( data );
}
