/*
    Empath - Mailer for KDE
    
    Copyright (C) 1998, 1999 Rik Hemsley rik@kde.org
    
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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef __GNUG__
# pragma implementation "EmpathFolderWidget.h"
#endif

// Qt includes
#include <qdragobject.h>
#include <qstringlist.h>

// KDE includes
#include <klocale.h>
#include <klineeditdlg.h>
#include <kconfig.h>
#include <kglobal.h>

// Local includes
#include "EmpathUIUtils.h"
#include "EmpathConfig.h"
#include "EmpathConfigMaildirDialog.h"
#include "EmpathConfigPOP3Dialog.h"
#include "EmpathConfigIMAP4Dialog.h"
#include "EmpathFolderWidget.h"
#include "EmpathMailboxList.h"
#include "EmpathFolderList.h"

#include "EmpathMailboxMaildir.h"
#include "EmpathMailboxPOP3.h"
#include "EmpathMailboxIMAP4.h"

EmpathFolderWidget::EmpathFolderWidget(
    QWidget * parent, const char * name, bool wait)
    :    QListView(parent, name),
        waitForShown_(wait)
{
    empathDebug("ctor");

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    
    addColumn(i18n("Folder name"));
    addColumn(i18n("Unread"));
    addColumn(i18n("Total"));
    
    setAllColumnsShowFocus(true);
    setRootIsDecorated(true);
    setSorting(0);

    QObject::connect(this, SIGNAL(currentChanged(QListViewItem *)),
            this, SLOT(s_currentChanged(QListViewItem *)));
    
    QObject::connect(
        this,
        SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)),
        this,
        SLOT(s_rightButtonPressed(QListViewItem *, const QPoint &, int)));
    
    QObject::connect(
        empath, SIGNAL(updateFolderLists()), this, SLOT(s_update()));
    
    ////////
    
    otherPopup_.insertItem(i18n("Set up accounts"),
        this, SLOT(s_setUpAccounts()));
    
    ////////
    
    folderPopup_.insertItem(i18n("New subfolder"),
        this, SLOT(s_newFolder()));
    
    folderPopup_.insertItem(i18n("Remove folder"),
        this, SLOT(s_removeFolder()));
    
    folderPopup_.insertSeparator();
    
    folderPopup_.insertItem(i18n("Properties"),
        this, SLOT(s_folderProperties()));
    
    /////////
    
    mailboxPopup_.insertItem(i18n("New folder"),
        this, SLOT(s_newFolder()));
    
    mailboxPopup_.insertItem(i18n("Check mail"),
        this, SLOT(s_mailboxCheck()));
    
    mailboxPopup_.insertSeparator();
    
    mailboxPopup_.insertItem(i18n("Properties"),
        this, SLOT(s_mailboxProperties()));

    /////////
    
    update();
}

    void
EmpathFolderWidget::update()
{
    setUpdatesEnabled(false);
    
    EmpathMailboxListIterator mit(empath->mailboxList());

    for (; mit.current(); ++mit) {
        
        empathDebug("Adding mailbox " + mit.current()->name());
        _addMailbox(*mit.current());
    }
    
    QListIterator<EmpathFolderListItem> it(itemList_);
    
    for (; it.current(); ++it)
        if (!it.current()->isTagged()) {
            itemList_.remove(it.current());
            QListView::removeItem((QListViewItem *)it.current());
        } else {
            it.current()->s_update();
        }

    setUpdatesEnabled(true);
    triggerUpdate();
}

    void
EmpathFolderWidget::_addMailbox(const EmpathMailbox & mailbox)
{
    empathDebug("============================================================");
    empathDebug("Add mailbox called for mailbox \"" +
        mailbox.name() + "\"");
    
    EmpathFolderListItem * newItem;
    EmpathFolderListItem * found = find(mailbox.url());

    if (found != 0) {
        
        found->tag(true);
        newItem = found;
        
    } else {
    
        newItem =
            new EmpathFolderListItem(this, mailbox.url());
        CHECK_PTR(newItem);
        newItem->tag(true);
        
        QObject::connect(
            newItem, SIGNAL(opened()),
            this, SLOT(s_openChanged()));
            
        itemList_.append(newItem);
    }
    
    empathDebug("============================================================");
    empathDebug("Adding folders of mailbox \"" + mailbox.name() + "\"");
    EmpathFolderListIterator fit(mailbox.folderList());

    for (; fit.current(); ++fit) {

        if (fit.current()->parent() == 0)
            _addChildren(fit.current(), newItem);
    }
    empathDebug("============================================================");
}

    void
EmpathFolderWidget::_addChildren(
    EmpathFolder * item,
    EmpathFolderListItem * parent)
{
    empathDebug("_addChildren(" + item->url().folderPath() + ") called");
    
    // Add this item first.

    EmpathFolderListItem * newItem;
    
    EmpathFolderListItem * found = find(item->url());

    if (found != 0) {
        
        found->tag(true);
        newItem = found;

    } else {
    
        newItem =
            new EmpathFolderListItem(parent, item->url());
        CHECK_PTR(newItem);
        newItem->tag(true);
    
        QObject::connect(
            newItem, SIGNAL(opened()),
            this, SLOT(s_openChanged()));

        itemList_.append(newItem);
    }

    EmpathMailbox * m = empath->mailbox(item->url());
    if (m == 0) return;
       
    EmpathFolderListIterator it(m->folderList());

    for (; it.current(); ++it) {
        if (it.current()->parent() == item)
            _addChildren(it.current(), newItem);
    }
}

    void
EmpathFolderWidget::s_currentChanged(QListViewItem * item)
{
    EmpathFolderListItem * i = (EmpathFolderListItem *)item;

    if (!i->url().hasFolder()) { // Item is mailbox.
    
        empathDebug("mailbox " + i->url().mailboxName() + " selected");
        
    } else { // Item is folder.
    
        empathDebug("folder " + i->url().folderPath() + " selected");

        if (waitForShown_)
        {
            setEnabled(false);
            setCursor(waitCursor);
            emit(showFolder(i->url()));
        }
    }
}

    void
EmpathFolderWidget::s_showing()
{
    setCursor(arrowCursor);
    setEnabled(true);
}

    EmpathURL
EmpathFolderWidget::selected() const
{
    EmpathURL url;
    if (!currentItem()) return url;
    
    return (((EmpathFolderListItem *)currentItem())->url());
}

    void
EmpathFolderWidget::s_rightButtonPressed(
    QListViewItem * item, const QPoint &, int)
{
    if (item == 0) {
    
        otherPopup_.exec(QCursor::pos());
        return;
    }
    setSelected(item, true);
    
    EmpathFolderListItem * i = (EmpathFolderListItem *)item;
    
    empathDebug("Right button pressed over object with url \"" +
        i->url().asString() + "\"");
    
    popupMenuOverType = (i->depth() == 0 ? Mailbox : Folder);
    popupMenuOver = i;
    
    if (popupMenuOverType == Folder)
        folderPopup_.exec(QCursor::pos());
    else
        mailboxPopup_.exec(QCursor::pos());
}

    void
EmpathFolderWidget::s_folderProperties()
{
    empathDebug("s_folderProperties() called");
    
    if (popupMenuOverType != Folder) {
        empathDebug("The popup menu wasn't over a folder !");
        return;
    }
    
    empathDebug("Nothing to do for folder properties as yet");
    return;
}

    void
EmpathFolderWidget::s_mailboxCheck()
{
    empathDebug("s_mailboxCheck() called");

    if (popupMenuOverType != Mailbox) {
        empathDebug("The popup menu wasn't over a mailbox !");
        return;
    }
    
    EmpathMailbox * m = empath->mailbox(popupMenuOver->url());
    
    if (m == 0) return;

    m->s_checkNewMail();
}

    void
EmpathFolderWidget::s_mailboxProperties()
{
    empathDebug("s_mailboxProperties() called");

    if (popupMenuOverType != Mailbox) {
        empathDebug("The popup menu wasn't over a mailbox !");
        return;
    }
    
    EmpathMailbox * m = empath->mailbox(popupMenuOver->url());

    if (m == 0) return;
    
    empathDebug("Mailbox name = " + m->name());

    switch (m->type()) {

        case EmpathMailbox::Maildir:
            {
                EmpathConfigMaildirDialog configDialog(
                        (EmpathMailboxMaildir *)m, this, "configDialog");
                if (configDialog.exec() == QDialog::Accepted)
                    update();
            }
            break;

        case EmpathMailbox::POP3:
            {
                EmpathConfigPOP3Dialog configDialog(
                        (EmpathMailboxPOP3 *)m, true, this, "configDialog");
                if (configDialog.exec() == QDialog::Accepted)
                    update();
            }
            break;

        case EmpathMailbox::IMAP4:
            {
                EmpathConfigIMAP4Dialog configDialog(
                        (EmpathMailboxIMAP4 *)m, this, "configDialog");
                if (configDialog.exec() == QDialog::Accepted)
                    update();
            }
            break;

        default:
            break;
    }

}

    void
EmpathFolderWidget::s_update()
{
    update();
}

    void
EmpathFolderWidget::s_newFolder()
{
    empathDebug("s_newFolder() called");
    KLineEditDlg led(i18n("Folder name"), QString::null, this, false);
    led.exec();

    QString name = led.text();
    
    if (name.isEmpty()) return;
        
    EmpathMailbox * m = empath->mailbox(popupMenuOver->url());
    if (m == 0) return;
    
    EmpathURL newFolderURL(popupMenuOver->url().asString() + "/" + name + "/");

    if (!m->addFolder(newFolderURL)) return;
    
    empathDebug("popupMenuOver == " + popupMenuOver->url().asString());
    empathDebug("newFolderURL  == " + newFolderURL.asString());

    EmpathFolderListItem * item =
        new EmpathFolderListItem(popupMenuOver, newFolderURL);
        
    QObject::connect(
        item, SIGNAL(opened()),
        this, SLOT(s_openChanged()));
    
    itemList_.append(item);
}

    void
EmpathFolderWidget::s_removeFolder()
{
    empathDebug("s_removeFolder \"" +
        popupMenuOver->url().asString() + "\" called");

    EmpathMailbox * m = empath->mailbox(popupMenuOver->url());
    if (m == 0) return;
    if (!m->removeFolder(popupMenuOver->url())) {
        empathDebug("Couldn't remove folder \"" +
            popupMenuOver->url().asString() + "\"");
        return;
    }
    
    delete popupMenuOver;
}

    void
EmpathFolderWidget::s_setUpAccounts()
{
}

    void
EmpathFolderWidget::dragMoveEvent(QDragMoveEvent *)
{
}
    
    void
EmpathFolderWidget::dragLeaveEvent(QDragMoveEvent *)
{
}

    void
EmpathFolderWidget::dragEnterEvent(QDragMoveEvent * e)
{
    if (itemAt(e->pos()) != 0) {
        itemAt(e->pos())->setSelected(true);
        if (QTextDrag::canDecode(e))
            e->accept();
    }
}

    void
EmpathFolderWidget::dropEvent(QDropEvent * e)
{
    if (itemAt(e->pos()) == 0) {
        empathDebug("Item was dropped on nothing");
        return;
    }
    
    QString str;
    
    if ( QTextDrag::decode( e, str ) ) {
        itemAt(e->pos())->setText(0, str);
        return;
    }
}

    EmpathFolderListItem *
EmpathFolderWidget::find(const EmpathURL & url)
{
    QListIterator<EmpathFolderListItem> it(itemList_);
    
    for (; it.current(); ++it)
        if (it.current()->url() == url) {
            empathDebug("Found a folder list item with url \""
                + url.asString() + "\" already in list");
            return it.current();
        }
    
    return 0;
}

    void
EmpathFolderWidget::s_openChanged()
{
    QStringList l;
    
    QListIterator<EmpathFolderListItem> it(itemList_);
    
    for (; it.current(); ++it) {
        l.append(it.current()->url().asString());
    }
    
    KConfig * c(KGlobal::config());
    c->setGroup(EmpathConfig::GROUP_DISPLAY);
    
    c->writeEntry(EmpathConfig::KEY_FOLDER_ITEMS_OPEN, l, ',');
}
#if 0
    void
EmpathFolderWidget::contentsDragEnterEvent(QDragEnterEvent * e)
{
    if (!QUriDrag::canDecode(e)) {
        e->ignore();
        return;
    }
}

    void
EmpathFolderWidget::contentsDragMoveEvent(QDragMoveEvent * e)
{
    if (!QUriDrag::canDecode(e)) {
        e->ignore();
        return;
    }
    
    QPoint vp = contentsToViewport(e->pos());
    
    QRect inside_margin(
        autoscroll_margin, autoscroll_margin,
        visibleWidth()    - autoscroll_margin * 2,
        visibleHeight()    - autoscroll_margin * 2);
    
    QListViewItem * i = itemAt(vp);
    
    if (!i) {
        e->ignore();
        autoOpenTimer.stop();
        dropItem = 0;
        return;
    }
    
    setSelected(i, true);
    
    if (!insideMargin.contains(vp)) {
        startAutoScroll();
        e->accept(QRect(0,0,0,0));
        autoOpenTimer.stop();
    } else {
        e->accept();
        
        if (i != dropItem) {
            autoOpenTimer.stop();
            dropItem = i;
            autoOpenTimer.start(autoOpenTime);
        }
        
        switch (e->action()) {
            
            case QDropEvent::Copy:
                break;
                
            case QDropEvent::Move:
                e->acceptAction();
                break;
                
            case QDropEvent::Link:
                e->acceptAction();
                break;
                
            default:
                break;
        }
    }
}

    void
EmpathFolderWidget::contentsDragLeaveEvent(QDragLeaveEvent *)
{
    autoOpenTimer.stop();
    stopAutoScroll();
    dropItem = 0;
    
    setCurrentItem(oldCurrent);
    setSelected(oldCurrent, true);
}

    void
EmpathFolderWidget::contentsDropEvent(QDropEvent * e)
{
    
    autoOpenTimer.stop();
    stopAutoScroll();
    
    if (!QUriDrag::canDecode(e)) {
        e->ignore();
        return;
    }
    
    QListViewItem * item = itemAt(contentsToViewport(e->pos()));
    
    if (!item) {
        e->ignore();
        return;
    }
    
    QStrList l;
    
    l.setAutoDelete(false);
    
    QUriDrag::decode(e, l);
    
    QString s;
    
    switch (e->action()) {
        
        case QDropEvent::Copy:
            s = "Copy";
            break;
    
        case QDropEvent::Move:
            s = "Move";
            break;
    
        case QDropEvent::Link:
            s = "Link";
            break;
    
        default:
            str = "Unknown";
            break;
    }
    
    str += "\n\n";
    
    e->accept();

}


    void
EmpathFolderWidget::contentsMousePressEvent(QMouseEvent * e)
{
    QListView::contentsMousePressEvent(e);
    pressPos = e->pos();
}

    void
EmpathFolderWidget::contentsMouseMoveEvent(QMouseEvent * e)
{
    if ((e->pos() - pressPos).manhattanLength() > 4) {
        QListViewItem * item = itemAt(contentsToViewport(pressPos));
        if (item) {
        }
    }
}

#endif
// vim:ts=4:sw=4:tw=78
