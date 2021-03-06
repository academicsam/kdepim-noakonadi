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
#include "phoneeditwidget.h"

#include <QtCore/QSignalMapper>
#include <QtCore/QString>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QVBoxLayout>

#include <kabc/phonenumber.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>

PhoneTypeCombo::PhoneTypeCombo( QWidget *parent )
  : KComboBox( parent ),
    mType( KABC::PhoneNumber::Home ),
    mLastSelected( 0 )
{
  setObjectName( "TypeCombo" );

  for ( int i = 0; i < KABC::PhoneNumber::typeList().count(); ++i )
    mTypeList.append( KABC::PhoneNumber::typeList()[ i ] );
  mTypeList.append( -1 ); // Others...

  update();

  connect( this, SIGNAL( activated( int ) ),
           this, SLOT( selected( int ) ) );
  connect( this, SIGNAL( activated( int ) ),
           this, SIGNAL( modified() ) );
}

PhoneTypeCombo::~PhoneTypeCombo()
{
}

void PhoneTypeCombo::setType( KABC::PhoneNumber::Type type )
{
  if ( !mTypeList.contains( type ) )
    mTypeList.insert( mTypeList.at( mTypeList.count() - 1 ), type );

  mType = type;
  update();
}

KABC::PhoneNumber::Type PhoneTypeCombo::type() const
{
  return mType;
}

void PhoneTypeCombo::update()
{
  bool blocked = signalsBlocked();
  blockSignals( true );

  clear();
  QList<int>::ConstIterator it;
  for ( it = mTypeList.constBegin(); it != mTypeList.constEnd(); ++it ) {
    if ( *it == -1 ) { // "Other..." entry
      addItem( i18nc( "@item:inlistbox Category of contact info field", "Other..." ) );
    } else {
      KABC::PhoneNumber number;
      number.setType( KABC::PhoneNumber::Type( *it ) );
      addItem( number.typeLabel() );
    }
  }

  setCurrentIndex( mLastSelected = mTypeList.indexOf( mType ) );

  blockSignals( blocked );
}

void PhoneTypeCombo::selected( int pos )
{
  if ( mTypeList[ pos ] == -1 )
    otherSelected();
  else {
    mType = KABC::PhoneNumber::Type( mTypeList[ pos ] );
    mLastSelected = pos;
  }
}

void PhoneTypeCombo::otherSelected()
{
  PhoneTypeDialog dlg( mType, this );
  if ( dlg.exec() ) {
    mType = dlg.type();
    if ( !mTypeList.contains( mType ) )
      mTypeList.insert( mTypeList.at( mTypeList.count() - 1 ), mType );
  } else {
    setType( KABC::PhoneNumber::Type( mTypeList[ mLastSelected ] ) );
  }

  update();
}

PhoneNumberWidget::PhoneNumberWidget( QWidget *parent )
  : QWidget( parent )
{
  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setSpacing( 11 );
  layout->setMargin( 6 );

  mTypeCombo = new PhoneTypeCombo( this );
  mNumberEdit = new KLineEdit( this );

  layout->addWidget( mTypeCombo );
  layout->addWidget( mNumberEdit );

  connect( mTypeCombo, SIGNAL( modified() ), SIGNAL( modified() ) );
  connect( mNumberEdit, SIGNAL( textChanged( const QString& ) ), SIGNAL( modified() ) );
}

void PhoneNumberWidget::setNumber( const KABC::PhoneNumber &number )
{
  mNumber = number;

  mTypeCombo->setType( number.type() );
  mNumberEdit->setText( number.number() );
}

KABC::PhoneNumber PhoneNumberWidget::number() const
{
  KABC::PhoneNumber number( mNumber );

  number.setType( mTypeCombo->type() );
  number.setNumber( mNumberEdit->text() );

  return number;
}

void PhoneNumberWidget::setReadOnly( bool readOnly )
{
  mTypeCombo->setEnabled( !readOnly );
  mNumberEdit->setReadOnly( readOnly );
}

PhoneNumberListWidget::PhoneNumberListWidget( QWidget *parent )
  : QWidget( parent ), mReadOnly( false )
{
  mWidgetLayout = new QVBoxLayout( this );

  mMapper = new QSignalMapper( this );
  connect( mMapper, SIGNAL( mapped( int ) ), SLOT( changed( int ) ) );

  setPhoneNumbers( KABC::PhoneNumber::List() );
}

PhoneNumberListWidget::~PhoneNumberListWidget()
{
}

void PhoneNumberListWidget::setReadOnly( bool readOnly )
{
  mReadOnly = readOnly;

  foreach ( PhoneNumberWidget *const w, mWidgets )
    w->setReadOnly( readOnly );
}

int PhoneNumberListWidget::phoneNumberCount() const
{
  return mPhoneNumberList.count();
}

void PhoneNumberListWidget::setPhoneNumbers( const KABC::PhoneNumber::List &list )
{
  mPhoneNumberList = list;

  KABC::PhoneNumber::TypeList types;
  types << KABC::PhoneNumber::Home;
  types << KABC::PhoneNumber::Work;
  types << KABC::PhoneNumber::Cell;
  // add an empty entry per default
  if ( mPhoneNumberList.count() < 3 )
    for ( int i = mPhoneNumberList.count(); i < 3; ++i )
      mPhoneNumberList.append( KABC::PhoneNumber( "", types[ i ] ) );

  recreateNumberWidgets();
}

KABC::PhoneNumber::List PhoneNumberListWidget::phoneNumbers() const
{
  KABC::PhoneNumber::List list;

  KABC::PhoneNumber::List::ConstIterator it;
  for ( it = mPhoneNumberList.constBegin(); it != mPhoneNumberList.constEnd(); ++it )
    if ( !(*it).number().isEmpty() )
      list.append( *it );

  return list;
}

void PhoneNumberListWidget::changed()
{
  if ( !mReadOnly )
    emit modified();
}

void PhoneNumberListWidget::add()
{
  mPhoneNumberList.append( KABC::PhoneNumber() );

  recreateNumberWidgets();
}

void PhoneNumberListWidget::remove()
{
  mPhoneNumberList.removeLast();
  changed();

  recreateNumberWidgets();
}

void PhoneNumberListWidget::recreateNumberWidgets()
{
  foreach ( QWidget *const w, mWidgets ) {
    mWidgetLayout->removeWidget( w );
    w->deleteLater();
  }
  mWidgets.clear();

  KABC::PhoneNumber::List::ConstIterator it;
  int counter = 0;
  for ( it = mPhoneNumberList.constBegin(); it != mPhoneNumberList.constEnd(); ++it ) {
    PhoneNumberWidget *wdg = new PhoneNumberWidget( this );
    wdg->setNumber( *it );

    mMapper->setMapping( wdg, counter );
    connect( wdg, SIGNAL( modified() ), mMapper, SLOT( map() ) );

    mWidgetLayout->addWidget( wdg );
    mWidgets.append( wdg );
    wdg->show();

    ++counter;
  }
  setReadOnly(mReadOnly);
}

void PhoneNumberListWidget::changed( int pos )
{
  mPhoneNumberList[ pos ] = mWidgets.at( pos )->number();
  changed();
}

PhoneEditWidget::PhoneEditWidget( QWidget *parent, const char *name )
  : QWidget( parent ), mReadOnly( false )
{
  setObjectName( name );
  QGridLayout *layout = new QGridLayout( this );
  layout->setSpacing( KDialog::spacingHint() );

  mListScrollArea = new QScrollArea( this );
  mPhoneNumberListWidget = new PhoneNumberListWidget;
  mListScrollArea->setWidget( mPhoneNumberListWidget );
  mListScrollArea->setWidgetResizable( true );

  // ugly but size policies seem to be messed up dialog (parent) wide
  const int scrollAreaMinHeight = mPhoneNumberListWidget->sizeHint().height() +
      mListScrollArea->horizontalScrollBar()->sizeHint().height();
  mListScrollArea->setMinimumHeight( scrollAreaMinHeight );
  layout->addWidget( mListScrollArea, 0, 0, 1, 2 );

  mAddButton = new QPushButton( i18n( "Add" ), this );
  mAddButton->setMaximumSize( mAddButton->sizeHint() );
  layout->addWidget( mAddButton, 1, 0, Qt::AlignRight );

  mRemoveButton = new QPushButton( i18n( "Remove" ), this );
  mRemoveButton->setMaximumSize( mRemoveButton->sizeHint() );
  layout->addWidget( mRemoveButton, 1, 1 );

  connect( mPhoneNumberListWidget, SIGNAL( modified() ), SIGNAL( modified() ) );

  connect( mAddButton, SIGNAL( clicked() ), mPhoneNumberListWidget, SLOT( add() ) );
  connect( mRemoveButton, SIGNAL( clicked() ), mPhoneNumberListWidget, SLOT( remove() ) );
  connect( mAddButton, SIGNAL( clicked() ), SLOT( changed() ) );
  connect( mRemoveButton, SIGNAL( clicked() ), SLOT( changed() ) );
}

PhoneEditWidget::~PhoneEditWidget()
{
}

void PhoneEditWidget::setReadOnly( bool readOnly )
{
  mReadOnly = readOnly;
  mAddButton->setEnabled( !readOnly );
  mRemoveButton->setEnabled( !readOnly && mPhoneNumberListWidget->phoneNumberCount() > 3 );
  mPhoneNumberListWidget->setReadOnly( readOnly );
}

void PhoneEditWidget::changed()
{
  mRemoveButton->setEnabled( !mReadOnly && mPhoneNumberListWidget->phoneNumberCount() > 3 );
}

void PhoneEditWidget::setPhoneNumbers( const KABC::PhoneNumber::List &list )
{
  mPhoneNumberListWidget->setPhoneNumbers( list );
  mRemoveButton->setEnabled( !mReadOnly && list.count() > 3 );
}

KABC::PhoneNumber::List PhoneEditWidget::phoneNumbers() const
{
  return mPhoneNumberListWidget->phoneNumbers();
}

///////////////////////////////////////////
// PhoneTypeDialog
PhoneTypeDialog::PhoneTypeDialog( KABC::PhoneNumber::Type type, QWidget *parent )
  : KDialog( parent),
    mType( type )
{
  setCaption( i18n( "Edit Phone Number" ) );
  setButtons(  Ok | Cancel );
  setDefaultButton( Ok );
  showButtonSeparator( true );
  QWidget *page = new QWidget(this);
  setMainWidget( page );

  QVBoxLayout *layout = new QVBoxLayout( page );
  layout->setSpacing( spacingHint() );
  layout->setMargin( 0 );

  mPreferredBox = new QCheckBox( i18n( "This is the preferred phone number" ), page );
  layout->addWidget( mPreferredBox );

  QGroupBox *box  = new QGroupBox( i18n( "Types" ), page );
  layout->addWidget( box );

  QHBoxLayout *buttonLayout = new QHBoxLayout( box );

  // fill widgets
  mTypeList = KABC::PhoneNumber::typeList();
  mTypeList.removeAll( KABC::PhoneNumber::Pref );

  KABC::PhoneNumber::TypeList::ConstIterator it;
  mGroup = new QButtonGroup( box );
  int i = 0;
  for ( it = mTypeList.constBegin(); it != mTypeList.constEnd(); ++it, ++i ) {
    QCheckBox *cb = new QCheckBox( KABC::PhoneNumber::typeLabel( *it ), box );
    cb->setChecked( type & mTypeList[ i ] );
    buttonLayout->addWidget( cb );
    mGroup->addButton( cb );
  }
  mPreferredBox->setChecked( mType & KABC::PhoneNumber::Pref );
}

KABC::PhoneNumber::Type PhoneTypeDialog::type() const
{
  KABC::PhoneNumber::Type type = 0;

  for ( int i = 0; i < mGroup->buttons().count(); ++i ) {
    QCheckBox *box = dynamic_cast<QCheckBox*>( mGroup->buttons().at( i ) ) ;
    if ( box && box->isChecked() )
      type |= mTypeList[ i ];
  }

  if ( mPreferredBox->isChecked() )
    type = type | KABC::PhoneNumber::Pref;
  else
    type = type & ~KABC::PhoneNumber::Pref;

  return type;
}


#include "phoneeditwidget.moc"
