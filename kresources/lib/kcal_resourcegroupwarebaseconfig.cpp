/*
    This file is part of kdepim.

    Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2004 Till Adam <adam@kde.org>
    Copyright (c) 2005 Reinhold Kainhofer <reinhold@kainhofer.com>

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
*/

#include "kcal_resourcegroupwarebaseconfig.h"

#include "kcal_cachesettingsdlg.h"
#include "kcal_resourcegroupwarebase.h"
#include "kresources_groupwareprefs.h"
#include "folderconfig.h"

#include <kcal/resourcecachedconfig.h>

#include <klocale.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kdialog.h>
#include <kpushbutton.h>

#include <QLabel>
#include <QLayout>
#include <khbox.h>
//Added by qt3to4:
#include <QGridLayout>


using namespace KCal;

ResourceGroupwareBaseConfig::ResourceGroupwareBaseConfig( QWidget* parent,
                                                          const char* name )
    : KRES::ConfigWidget( parent)
{
  setObjectName(name);
  resize( 245, 115 );

  QGridLayout *mainLayout = new QGridLayout( this );
  mainLayout->setSpacing( KDialog::spacingHint() );

  QLabel *label = new QLabel( i18n("URL:"), this );
  mainLayout->addWidget( label, 1, 0 );
  mUrl = new KLineEdit( this );
  mainLayout->addWidget( mUrl, 1, 1 );

  label = new QLabel( i18n("User:"), this );
  mainLayout->addWidget( label, 2, 0 );
  mUserEdit = new KLineEdit( this );
  mainLayout->addWidget( mUserEdit, 2, 1 );

  label = new QLabel( i18n("Password:"), this );
  mainLayout->addWidget( label, 3, 0 );
  mPasswordEdit = new KLineEdit( this );
  mainLayout->addWidget( mPasswordEdit, 3, 1 );
  mPasswordEdit->setEchoMode( KLineEdit::Password );

  mFolderConfig = new KPIM::FolderConfig( this );
  connect( mFolderConfig, SIGNAL( updateFoldersClicked() ),
    SLOT( updateFolders() ) );
  mainLayout->addWidget( mFolderConfig, 4, 0, 1, 2 );

  KHBox *hBox = new KHBox( this );
  mCacheSettingsButton = new KPushButton( i18n("Configure Cache Settings..."), hBox );
  mainLayout->addWidget( hBox, 5, 0, 1, 2 );
  connect( mCacheSettingsButton, SIGNAL( clicked() ),
           SLOT( showCacheSettings() ) );

  mCacheDialog = new CacheSettingsDialog( this );
}

void ResourceGroupwareBaseConfig::loadSettings( KRES::Resource *resource )
{
  kDebug(7000) <<"KCal::ResourceGroupwareBaseConfig::loadSettings()";

  ResourceGroupwareBase *res = static_cast<ResourceGroupwareBase *>( resource );
  if ( res ) {
    if ( !res->prefs() ) {
      kError() <<"No PREF";
      return;
    }

    mUrl->setText( res->prefs()->url() );
    mUserEdit->setText( res->prefs()->user() );
    mPasswordEdit->setText( res->prefs()->password() );
    if ( mCacheDialog ) mCacheDialog->loadSettings( res );

    mFolderConfig->setFolderLister( res->folderLister() );
    mFolderConfig->updateFolderList();
  } else {
    kError(5700) <<"KCalResourceGroupwareBaseConfig::loadSettings():"
                     "no KCalOpenGroupware, cast failed";
  }
}

void ResourceGroupwareBaseConfig::saveSettings( KRES::Resource *resource )
{
  ResourceGroupwareBase *res = static_cast<ResourceGroupwareBase*>( resource );
  if ( res ) {
    res->prefs()->setUrl( mUrl->text() );
    res->prefs()->setUser( mUserEdit->text() );
    res->prefs()->setPassword( mPasswordEdit->text() );
    if ( mCacheDialog ) mCacheDialog->saveSettings( res );
    mFolderConfig->saveSettings();
  } else {
    kError(5700) <<"KCalResourceGroupwareBaseConfig::saveSettings():"
                     "no KCalOpenGroupware, cast failed";
  }
}

void ResourceGroupwareBaseConfig::updateFolders()
{
  KUrl url = mUrl->text();
  url.setUser( mUserEdit->text() );
  url.setPass( mPasswordEdit->text() );

  mFolderConfig->retrieveFolderList( url );
}

void ResourceGroupwareBaseConfig::showCacheSettings()
{
  if ( mCacheDialog ) mCacheDialog->exec();
}

#include "kcal_resourcegroupwarebaseconfig.moc"
