/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#include "messagelistview/pane.h"
#include "messagelistview/messageset.h"
#include "messagelistview/widget.h"

#include "messagelistview/core/view.h"
#include "messagelistview/core/manager.h"

#include <KLineEdit>
#include <KLocale>
#include <KIcon>
#include <KMenu>

#include <QAction>
#include <QIcon>
#include <QToolButton>

#include <libkdepim/messagestatus.h>

#include "globalsettings.h"
#include "kmfolder.h"
#include "kmmessage.h"
#include "kmmainwidget.h"
#include "mainfolderview.h"

namespace KMail
{

namespace MessageListView
{


Pane::Pane( KMMainWidget * mainWidget, QWidget *pParent )
  : KTabWidget( pParent ), mMainWidget( mainWidget ), mCurrentWidget( 0 ),
    mCurrentFolder( 0 )
{
  // setCornerWidget
  mNewTabButton = new QToolButton( this );
  mNewTabButton->setIcon( KIcon( "tab-new" ) );
  mNewTabButton->adjustSize();
  mNewTabButton->setToolTip( i18n("Open a new tab"));
  setCornerWidget( mNewTabButton, Qt::TopLeftCorner );

  mCloseTabButton = new QToolButton( this );
  mCloseTabButton->setIcon( KIcon( "tab-close" ) );
  mCloseTabButton->adjustSize();
  mCloseTabButton->setToolTip( i18n("Close the current tab"));
  setCornerWidget( mCloseTabButton, Qt::TopRightCorner );

  setTabReorderingEnabled( true ); // this works with middle button (not really intuitive, found it by reading sources)
  //setCloseButtonEnabled( true ); <-- not that nice (often one closes tabs by accident)
  //setHoverCloseButton( true ); <-- deprecated

  connect( mNewTabButton, SIGNAL( clicked() ),
           SLOT( slotNewTab() ) );

  connect( mCloseTabButton, SIGNAL( clicked() ),
           SLOT( slotCloseCurrentTab() ) );

  connect( this, SIGNAL( currentChanged( int ) ),
           SLOT( slotCurrentTabChanged( int ) ) );

  connect( this, SIGNAL( contextMenu( QWidget *, const QPoint & ) ),
           SLOT( slotTabContextMenuRequest( QWidget *, const QPoint & ) ) );

  connect( this, SIGNAL( testCanDecode(const QDragMoveEvent *, bool &) ),
           SLOT( slotTabDragMoveEvent( const QDragMoveEvent *, bool &) ) );


  addNewWidget();
}

Pane::~Pane()
{
}

Widget * Pane::messageListViewWidgetWithFolder( KMFolder * fld ) const
{
  int tabCount = count();

  Widget * w;

  for ( int i = 0; i < tabCount; i++ )
  {
    QWidget * qw = widget( i );
    if ( !qw )
      continue;
    if ( !qw->inherits( "KMail::MessageListView::Widget" ) )
      continue;

    w = static_cast< Widget * >( qw );

    if ( w->folder() == fld )
      return w;
  }

  return 0;
}

bool Pane::isFolderOpen( KMFolder * fld ) const
{
  return messageListViewWidgetWithFolder( fld ) != 0;
}

void Pane::setCurrentFolder( KMFolder *fld, bool preferEmptyTab, Core::PreSelectionMode preSelectionMode )
{
  if ( mCurrentFolder == fld )
    return;

  Widget * w = messageListViewWidgetWithFolder( fld );

  if ( w )
  {
    // Already open, just activate it
    if ( w != mCurrentWidget )
      setCurrentWidget( w ); // will call internalSetCurrentWidget -> internalSetCurrentFolder
    w->view()->applyMessagePreSelection( preSelectionMode );
    return;
  }

  // Not open yet.

  // Set a nice icon for the folder. We actually set this icon on Widget
  // and not directly with setTabIcon(). This is because Widget may manipulate
  // it to show progress of the internal jobs. Widget will call widgetIconChangeRequest()
  // when it wants its icon to be updated.

  QIcon icon;

  if ( fld )
  {
    FolderViewItem * fvi = mMainWidget->mainFolderView()->findItemByFolder( fld );
    if ( fvi )
      icon = SmallIcon( fvi->normalIcon() );
    else
      icon = QIcon(); // FIXME: find a nicer empty icon
  } else {
    icon = QIcon(); // FIXME: find a nicer empty icon
  }

  w = preferEmptyTab ? messageListViewWidgetWithFolder( 0 ) : currentMessageListViewWidget();

  if ( !w )
  {
    w = addNewWidget();
    w->setFolder( fld, icon, preSelectionMode );
    if ( fld )
      setTabText( indexOf( w ), fld->label() );
    setCurrentWidget( w ); // will call internalSetCurrentWidget -> internalSetCurrentFolder
  } else {
    w->setFolder( fld, icon, preSelectionMode );
    if ( fld )
      setTabText( indexOf( w ), fld->label() );
    else
      setTabText( indexOf( w ), i18n( "Empty" ) );
    // is already current
    internalSetCurrentFolder( fld );
  }

}

void Pane::widgetIconChangeRequest( Widget * w, const QIcon &icon )
{
  int idx = indexOf( w );
  if ( idx < 0 )
    return;

  setTabIcon( idx, icon );
}


void Pane::slotTabDragMoveEvent( const QDragMoveEvent *e, bool &accept )
{
  accept = false;

  const int tabCount = count();

  if ( !tabBar() )
    return; // no way

  int tabIndex = -1;

  // This could be substituted by KTabBar::selectTab(). Unfortunately
  // KTabWidget doesn't expose it...

  for ( int i = 0; i < tabCount; ++i )
  {
    if ( tabBar()->tabRect( i ).contains( e->pos() ) )
    {
      tabIndex = i;
      break;
    }
  }

  if ( tabIndex < 0 )
    return;

  QWidget * qw = widget( tabIndex );
  if ( !qw )
    return;
  if ( !qw->inherits( "KMail::MessageListView::Widget" ) )
    return;

  Widget * w = static_cast< Widget * >( qw );

  accept = w->canAcceptDrag( e );
}

void Pane::slotTabContextMenuRequest( QWidget * tab, const QPoint &pos )
{
  if ( !tab )
    return; // sanity

  KMenu menu( this );

  if ( !tab->inherits( "KMail::MessageListView::Widget" ) )
    return;

  Widget * w = dynamic_cast< Widget * >( tab );
  if ( !w )
    return; // sanity

  
  QVariant data;
  data.setValue< void * >( static_cast< void * >( w ) );

  QAction * act;

  act = menu.addAction( i18n( "Close Tab" ) );
  act->setData( data );
  act->setEnabled( count() > 1 );
  act->setIcon( SmallIcon( "tab-close" ) );
  QObject::connect(
      act, SIGNAL( triggered( bool ) ),
      SLOT( slotActionTabCloseRequest() )
    );

  act = menu.addAction( i18n( "Close All Other Tabs" ) );
  act->setData( data );
  act->setEnabled( count() > 1 );
  act->setIcon( SmallIcon( "tab-close-other" ) );
  QObject::connect(
      act, SIGNAL( triggered( bool ) ),
      SLOT( slotActionTabCloseAllButThis() )
    );

  if ( menu.isEmpty() )
    return; // nuthin to show

  menu.exec( pos );
}

void Pane::slotActionTabCloseRequest()
{
  QAction * act = dynamic_cast< QAction * >( sender() );
  if ( !act )
    return;

  QVariant v = act->data();
  if ( !v.isValid() )
    return;

  Widget * w = static_cast< Widget * >( v.value< void * >() );
  if ( !w )
    return;

  if ( count() < 2 )
    return;

  delete w; // should call slotCurrentTabChanged() if the tab is current

  updateTabControls();
}

void Pane::slotActionTabCloseAllButThis()
{
  QAction * act = dynamic_cast< QAction * >( sender() );
  if ( !act )
    return;

  QVariant v = act->data();
  if ( !v.isValid() )
    return;

  Widget * w = static_cast< Widget * >( v.value< void * >() );
  if ( !w )
    return;

  if ( count() < 2 )
    return;

  int c = count();

  QList< Widget * > widgets;

  for ( int i = 0; i < c; i++ )
  {
    QWidget * other = widget( i );
    if ( other->inherits( "KMail::MessageListView::Widget" ) )
    {
      if ( static_cast< QWidget * >( w ) != other )
        widgets.append( static_cast< Widget * >( other ) );
    }
  }

  for ( QList< Widget * >::Iterator it = widgets.begin(); it != widgets.end(); ++it )
    delete ( *it ); // should call slotCurrentTabChanged() if the current tab is closed

  updateTabControls();
}

void Pane::slotCloseCurrentTab()
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  if ( count() < 2 )
    return;

  delete w; // should call slotCurrentTabChanged()

  updateTabControls();
}

void Pane::updateTabControls()
{
  bool moreThanOneTab = count() > 1;

  mCloseTabButton->setEnabled( moreThanOneTab );

  if ( GlobalSettings::self()->hideTabBarWithSingleTab() )
    setTabBarHidden( !moreThanOneTab );
  else
    setTabBarHidden( false );
}

void Pane::slotNewTab()
{
  setCurrentWidget( addNewWidget() );
}

Widget * Pane::addNewWidget()
{
  Widget * w = new Widget( mMainWidget, this );
  addTab( w, i18n( "Empty" ) );

  connect( w, SIGNAL( messageSelected( KMMessage * ) ),
           this, SLOT( slotMessageSelected( KMMessage * ) ) );
  connect( w, SIGNAL( messageActivated( KMMessage * ) ),
           this, SLOT( slotMessageActivated( KMMessage * ) ) );
  connect( w, SIGNAL( selectionChanged() ),
           this, SLOT( slotSelectionChanged() ) );
  connect( w, SIGNAL( messageStatusChangeRequest( KMMsgBase *, const KPIM::MessageStatus &, const KPIM::MessageStatus & ) ),
           this, SIGNAL( messageStatusChangeRequest( KMMsgBase *, const KPIM::MessageStatus &, const KPIM::MessageStatus & ) ) );
  connect( w, SIGNAL( fullSearchRequest() ),
           this, SIGNAL( fullSearchRequest() ) );

  updateTabControls();
  return w;
}

void Pane::slotSelectionChanged()
{
  if ( sender() != currentMessageListViewWidget() )
    return; // Don't forward, it should be hidden. (But may happen if a message is removed from that view)
  emit selectionChanged();
}

void Pane::slotMessageSelected( KMMessage * msg )
{
  if ( sender() != currentMessageListViewWidget() )
    return; // Don't forward, it should be hidden. (But may happen if a message is removed from that view)
  emit messageSelected( msg );
}

void Pane::slotMessageActivated( KMMessage * msg )
{
  if ( sender() != currentMessageListViewWidget() )
    return; // Don't forward, it should be hidden. (But may happen if a message is removed from that view)
  emit messageActivated( msg );
}


void Pane::slotCurrentTabChanged( int )
{
  QWidget * qw = currentWidget();
  if ( !qw )
  {
    internalSetCurrentWidget( 0 );
    return;
  }

  if ( !qw->inherits( "KMail::MessageListView::Widget" ) )
  {
    internalSetCurrentWidget( 0 );
    return;
  }

  internalSetCurrentWidget( static_cast< Widget * >( qw ) );
}


void Pane::internalSetCurrentFolder( KMFolder * folder )
{
  if ( mCurrentFolder == folder )
    return;

  mCurrentFolder = folder;

  emit currentFolderChanged( mCurrentFolder );
}

void Pane::internalSetCurrentWidget( Widget * newCurrentWidget )
{
  if ( mCurrentWidget == newCurrentWidget )
    return; // nothing changed

  mCurrentWidget = newCurrentWidget;

  internalSetCurrentFolder( mCurrentWidget ? mCurrentWidget->folder() : 0 );

  if ( mCurrentWidget )
  {
    emit messageSelected( mCurrentWidget->currentMessage() );
    emit selectionChanged();
  }
}

KMMessage * Pane::currentMessage() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return 0;
  return w->currentMessage();
}

KMMsgBase * Pane::currentMsgBase() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return 0;
  return w->currentMsgBase();
}

QList< KMMessage * > Pane::selectionAsMessageList( bool includeCollapsedChildren ) const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return QList< KMMessage * >();

  return w->selectionAsMessageList( includeCollapsedChildren );
}

QList< KMMsgBase * > Pane::selectionAsMsgBaseList( bool includeCollapsedChildren ) const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return QList< KMMsgBase * >();

  return w->selectionAsMsgBaseList( includeCollapsedChildren );
}

MessageTreeCollection * Pane::selectionAsMessageTreeCollection( bool includeCollapsedChildren ) const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return 0;

  return w->selectionAsMessageTreeCollection( includeCollapsedChildren );
}

QList< KMMsgBase * > Pane::currentThreadAsMsgBaseList() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return QList< KMMsgBase * >();

  return w->currentThreadAsMsgBaseList();
}

QList< KMMessage * > Pane::currentThreadAsMessageList() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return QList< KMMessage * >();

  return w->currentThreadAsMessageList();
}

MessageTreeCollection * Pane::currentThreadAsMessageTreeCollection() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return 0;

  return w->currentThreadAsMessageTreeCollection();
}

void Pane::activateMessage( KMMsgBase *msg )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  w->activateMessageItemByMsgBase( msg );
}


bool Pane::getSelectionStats(
      QList< Q_UINT32 > &selectedSernums,
      QList< Q_UINT32 > &selectedVisibleSernums,
      bool * allSelectedBelongToSameThread,
      bool includeCollapsedChildren
    ) const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return false;

  return w->getSelectionStats(
      selectedSernums, selectedVisibleSernums,
      allSelectedBelongToSameThread, includeCollapsedChildren
    );
}

bool Pane::selectionEmpty() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return true;

  return w->selectionEmpty();
}


bool Pane::isThreaded() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return false;

  return w->view()->isThreaded();
}

void Pane::setCurrentThreadExpanded( bool expand )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  return w->view()->setCurrentThreadExpanded( expand );
}

void Pane::setAllThreadsExpanded( bool expand )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  return w->view()->setAllThreadsExpanded( expand );
}

bool Pane::selectNextMessageItem( bool unread, bool expandSelection, bool centerItem, bool loop )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return false;

  return w->view()->selectNextMessageItem( unread, expandSelection, centerItem, loop );
}

void Pane::selectFirstUnreadMessage( bool centerItem )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  return w->view()->selectFirstUnreadMessageItem( centerItem );
}

bool Pane::selectPreviousMessageItem( bool unread, bool expandSelection, bool centerItem, bool loop )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return false;

  return w->view()->selectPreviousMessageItem( unread, expandSelection, centerItem, loop );
}

bool Pane::focusNextMessageItem( bool unread, bool centerItem, bool loop )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return false;

  return w->view()->focusNextMessageItem( unread, centerItem, loop );
}

bool Pane::focusPreviousMessageItem( bool unread, bool centerItem, bool loop )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return false;

  return w->view()->focusPreviousMessageItem( unread, centerItem, loop );
}

void Pane::selectFocusedMessageItem( bool centerItem )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  w->view()->selectFocusedMessageItem( centerItem );
}

void Pane::selectAll()
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return;

  w->view()->selectAll();
}

KPIM::MessageStatus Pane::currentFilterStatus() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return KPIM::MessageStatus();

  return w->currentFilterStatus();
}

QString Pane::currentFilterSearchString() const
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return QString();

  return w->currentFilterSearchString();
}


MessageSet * Pane::createMessageSetFromSelection( bool includeCollapsedChildren )
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return 0;

  Core::MessageItemSetReference ref = w->createPersistentSetFromMessageItemList(
      w->selectionAsMessageItemList( includeCollapsedChildren )
    );

  if ( ref == static_cast< Core::MessageItemSetReference >( 0 ) )
    return 0;

  Q_ASSERT( w->folder() ); // must be valid since the ref is valid

  return new MessageSet( this, w, w->folder(), ref );
}

MessageSet * Pane::createMessageSetFromCurrentThread()
{
  Widget * w = currentMessageListViewWidget();
  if ( !w )
    return 0;

  Core::MessageItemSetReference ref = w->createPersistentSetFromMessageItemList(
      w->currentThreadAsMessageItemList()
    );

  if ( ref == static_cast< Core::MessageItemSetReference >( 0 ) )
    return 0;

  Q_ASSERT( w->folder() ); // must be valid since the ref is valid

  return new MessageSet( this, w, w->folder(), ref );
}


QList< KMMsgBase * > Pane::messageSetContentsAsMsgBaseList( MessageSet * set )
{
  Q_ASSERT( set );

  if ( set->pane() != this )
    return QList< KMMsgBase * >();

  int idx = indexOf( set->widget() );
  if ( idx < 0 )
    return QList< KMMsgBase * >(); // widget dead

  return set->widget()->persistentSetContentsAsMsgBaseList( set->messageItemSetReference() );

  return QList< KMMsgBase * >();
}

void Pane::messageSetSelect( MessageSet * set, bool clearSelectionFirst )
{
  Q_ASSERT( set );

  if ( set->pane() != this )
    return;

  int idx = indexOf( set->widget() );
  if ( idx < 0 )
    return; // widget dead

  set->widget()->selectPersistentSet( set->messageItemSetReference(), clearSelectionFirst );

}

void Pane::messageSetMarkAsAboutToBeRemoved( MessageSet *set, bool bMark )
{
  Q_ASSERT( set );

  if ( set->pane() != this )
    return;

  int idx = indexOf( set->widget() );
  if ( idx < 0 )
    return; // widget dead

  set->widget()->markPersistentSetAsAboutToBeRemoved( set->messageItemSetReference(), bMark );
}

void Pane::messageSetDestroyed( MessageSet *set )
{
  Q_ASSERT( set );
  Q_ASSERT( set->pane() == this );

  int idx = indexOf( set->widget() );
  if ( idx < 0 )
    return; // widget dead

  set->widget()->deletePersistentSet( set->messageItemSetReference() );
}

void Pane::reloadGlobalConfiguration()
{
  if ( !Core::Manager::instance() )
    return; // manager doesn't exist (yet or anymore)

  Core::Manager::instance()->reloadGlobalConfiguration();

  updateTabControls();
}

void Pane::focusQuickSearch()
{
  Widget *widget = currentMessageListViewWidget();
  if ( widget ) {
    KLineEdit *quickSearch = widget->quickSearch();
    if ( quickSearch )
      quickSearch->setFocus();
  }
}

} // namespace MessageListView

} // namespace KMail
