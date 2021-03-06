/*
   This file is part of libkdepim.

   Copyright (C) 2005 Con Hennessy <cp.hennessy@iname.com>
   Copyright (C)      Tobias Koenig <tokoe@kde.org>

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

#include "sendsmsdialog.h"

#include <KLocale>

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QTextEdit>

SendSMSDialog::SendSMSDialog( const QString &recipientName, QWidget *parent )
  : KDialog( parent )
{
  setCaption( i18n( "Send SMS" ) );
  setButtons( Ok|Cancel );
  setDefaultButton( Ok );
  setModal( true );
  showButtonSeparator( true );
  QWidget *page = new QWidget( this );
  setMainWidget( page );

  QGridLayout *layout = new QGridLayout( page );
  layout->setMargin( marginHint() );
  layout->setSpacing( spacingHint() );

  layout->addWidget( new QLabel( i18n( "Message" ), page ), 0, 0 );

  mMessageLength = new QLabel( "0/160", page );
  mMessageLength->setAlignment( Qt::AlignRight );
  layout->addWidget( mMessageLength, 0, 2 );

  mText = new QTextEdit( page );
  layout->addWidget( mText, 1, 0, 1, 3 );

  layout->addWidget( new QLabel( i18n( "Recipient:" ), page ), 2, 0 );
  layout->addWidget( new QLabel( recipientName, page ), 2, 2 );

  setButtonText( Ok, i18n( "Send" ) );

  connect( mText, SIGNAL( textChanged() ),
           this, SLOT( updateMessageLength() ) );
  connect( mText, SIGNAL( textChanged() ),
           this, SLOT( updateButtons() ) );

  updateButtons();

  mText->setFocus();
}

QString SendSMSDialog::text() const
{
  return mText->toPlainText();
}

void SendSMSDialog::updateMessageLength()
{
  int length = mText->toPlainText().length();

  if( length > 480 )
    mMessageLength->setText( QString( "%1/%2 (%3)" ).arg( length ).arg( 500 ).arg( 4 ) );
  else if( length > 320 )
    mMessageLength->setText( QString( "%1/%2 (%3)" ).arg( length ).arg( 480 ).arg( 3 ) );
  else if( length > 160 )
    mMessageLength->setText( QString( "%1/%2 (%3)" ).arg( length ).arg( 320 ).arg( 2 ) );
  else
    mMessageLength->setText( QString( "%1/%2" ).arg( length ).arg( 160 ) );
}

void SendSMSDialog::updateButtons()
{
  enableButton( Ok, mText->toPlainText().length() > 0 );
}

#include "sendsmsdialog.moc"
