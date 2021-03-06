/*
    This file is part of KAddressBook.
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

#include "kcmkabldapconfig.h"

#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>

#include <kaboutdata.h>
#include <kcomponentdata.h>
#include <kdemacros.h>
#include <kgenericfactory.h>
#include <klocale.h>

#include "ldapoptionswidget.h"

K_PLUGIN_FACTORY(KCMKabLdapConfigFactory, registerPlugin<KCMKabLdapConfig>();)
K_EXPORT_PLUGIN(KCMKabLdapConfigFactory( "kcmkabldapconfig" ))


KCMKabLdapConfig::KCMKabLdapConfig( QWidget *parent, const QVariantList & )
  : KCModule( KCMKabLdapConfigFactory::componentData(), parent )
{
  KGlobal::locale()->insertCatalog( "libkldap" );
  QVBoxLayout *layout = new QVBoxLayout( this );
  mConfigWidget = new LDAPOptionsWidget( this );
  layout->addWidget( mConfigWidget );

  connect( mConfigWidget, SIGNAL( changed( bool ) ), SIGNAL( changed( bool ) ) );

  load();

  KAboutData *about = new KAboutData( I18N_NOOP( "kcmkabldapconfig" ), 0,
                                      ki18n( "KAB LDAP Configure Dialog" ),
                                      0, KLocalizedString(), KAboutData::License_GPL,
                                      ki18n( "(c), 2003 - 2004 Tobias Koenig" ) );

  about->addAuthor( ki18n("Tobias Koenig"), KLocalizedString(), "tokoe@kde.org" );
  setAboutData( about );
}

void KCMKabLdapConfig::load()
{
  mConfigWidget->restoreSettings();
}

void KCMKabLdapConfig::save()
{
  mConfigWidget->saveSettings();
}

void KCMKabLdapConfig::defaults()
{
  mConfigWidget->defaults();
}

#include "kcmkabldapconfig.moc"
