/*
    This file is part of kdepim.

    Copyright (c) 2004 Tobias Koenig <tokoe@kde.org>

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

#include "groupwarewizard.h"
#include "overviewpage.h"

#include <klocale.h>
#include <kicon.h>

#include <QApplication>
#include <QVBoxLayout>

GroupwareWizard::GroupwareWizard( QWidget *parent)
  : QWidget( parent )
{
  setWindowTitle( i18n( "KDE Groupware Wizard" ) );
  setWindowIcon( KIcon("kontact") );

  QVBoxLayout *layout = new QVBoxLayout;
  setLayout(layout);

  mOverViewPage = new OverViewPage( this );

  layout->addWidget( mOverViewPage );

  connect( mOverViewPage, SIGNAL( cancel() ),
           qApp, SLOT( quit() ) );

  resize( 400, 200 );
}

GroupwareWizard::~GroupwareWizard()
{
}

#include "groupwarewizard.moc"
