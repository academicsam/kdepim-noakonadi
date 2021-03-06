/*
    This file is part of KAddressbook.
    Copyright (c) 2003  Alexander Kellett <lypanov@kde.org>
                        Tobias Koenig <tokoe@kde.org>

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

#include "bookmark_xxport.h"

#include <kbookmark.h>
#include <kbookmarkdombuilder.h>
#include <kbookmarkmanager.h>
#include <kbookmarkmenu.h>
#include <klocale.h>
#include <konqbookmarkmenu.h>
#include <kstandarddirs.h>

K_EXPORT_KADDRESSBOOK_XXFILTER( kaddrbk_bookmark_xxport, BookmarkXXPort )

BookmarkXXPort::BookmarkXXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : KAB::XXPort( ab, parent, name )
{
  createExportAction( i18n( "Export Bookmarks Menu..." ) );
}

bool BookmarkXXPort::exportContacts( const KABC::AddresseeList &list, const QString& )
{
  const QString fileName = KStandardDirs::locateLocal( "data", "kabc/bookmarks.xml" );

  KBookmarkManager *mgr = KBookmarkManager::managerForFile( fileName, "KAddressbook" );
  KBookmarkDomBuilder *builder = new KBookmarkDomBuilder( mgr->root(), mgr );
  builder->connectImporter( this );

  KABC::AddresseeList::ConstIterator it;
  emit newFolder( i18n( "AddressBook" ), false, "" );
  for ( it = list.constBegin(); it != list.constEnd(); ++it ) {
    if ( !(*it).url().isEmpty() ) {
      QString name = (*it).givenName() + ' ' + (*it).familyName();
      emit newBookmark( name, (*it).url().url().toLatin1(), QString( "" ) );
    }
  }
  emit endFolder();
  delete builder;
  mgr->save();

#ifdef __GNUC__
#warning FIXME KonqBookmarkMenu is now protected.
#endif
  // I think we have to subclass KonqBookmarkMenu to get access to 
  // the struct and the method.
  /*KonqBookmarkMenu::DynMenuInfo menu;
  menu.name = i18n( "Addressbook Bookmarks" );
  menu.location = fileName;
  menu.type = "xbel";
  menu.show = true;
  KonqBookmarkMenu::setDynamicBookmarks( "kabc", menu );*/

  return true;
}

#include "bookmark_xxport.moc"
