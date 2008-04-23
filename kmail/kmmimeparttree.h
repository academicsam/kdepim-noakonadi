/* -*- c++ -*-
    kmmimeparttree.h A MIME part tree viwer.

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2002-2004 Klarälvdalens Datakonsult AB

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License, version 2, as
    published by the Free Software Foundation.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

// -*- c++ -*-
#ifndef KMMIMEPARTTREE_H
#define KMMIMEPARTTREE_H

#include <kio/global.h>

#include <QTreeWidget>
#include <QString>

class partNode;
class KMReaderWin;
class KMMimePartTreeItem;
class QAction;

/**
 * @short The widget that displays the message MIME tree
 *
 * This widget displays the message tree and allows viewing
 * the contents of single nodes in the KMReaderWin.
 */
class KMMimePartTree : public QTreeWidget
{
  Q_OBJECT
  friend class ::KMReaderWin;

public:
  KMMimePartTree( KMReaderWin *readerWin,
                  QWidget *parent );
  virtual ~KMMimePartTree();

public:
  /**
  * Corrects the size displayed by the specified item
  * by accounting for its current children.
  *
  * @param the item to correct the size for
  */
  void correctSize( QTreeWidgetItem *item );

protected slots:
  void slotItemClicked( QTreeWidgetItem * );
  void slotContextMenuRequested( const QPoint & );
  void slotHeaderContextMenuRequested( const QPoint & );
  void slotToggleColumn( QAction * );
  void slotSaveAs();
  void slotSaveAsEncoded();
  void slotSaveAll();
  void slotDelete();
  void slotEdit();
  void slotOpen();
  void slotOpenWith();
  void slotView();
  void slotProperties();
  void slotCopy();

protected:
#if 0
  /* FIXME: Remove this ifdefed code if nobody complains (2008.04.23) */
  /** reimplemented in order to update the frame width in case of a changed
      GUI style FIXME: Still needed with Qt4 ? */
  virtual void styleChange( QStyle& oldStyle );
#endif
  virtual void startDrag( Qt::DropActions actions );
  virtual void showEvent( QShowEvent* e );

#if 0
  /* FIXME: Remove this ifdefed code if nobody complains (2008.04.23) */
  /** Set the width of the frame to a reasonable value for the current GUI
      style */
  void setStyleDependantFrameWidth();
#endif
  void restoreLayoutIfPresent();
  void startHandleAttachmentCommand( int action );
  void saveSelectedBodyParts( bool encoded );

protected:
  KMReaderWin* mReaderWin;         // pointer to the main reader window
  bool mLayoutColumnsOnFirstShow;  // provide layout defaults on first show ?

};


/**
 * @short The message MIME tree items used in KMMimePartTree
 *
 * Each display item has a reference to the partNode (which is a node
 * of the "real" message tree).
 */
class KMMimePartTreeItem : public QTreeWidgetItem
{
public:
  KMMimePartTreeItem( KMMimePartTree *parent,
                      partNode *node,
                      const QString &labelDescr,
                      const QString &labelCntType  = QString(),
                      const QString &labelEncoding = QString(),
                      KIO::filesize_t size = 0 );

  KMMimePartTreeItem( KMMimePartTreeItem * parent,
                      partNode *node,
                      const QString &labelDescr,
                      const QString &labelCntType  = QString(),
                      const QString &labelEncoding = QString(),
                      KIO::filesize_t size = 0,
                      bool revertOrder = false );

public:
  /**
  * @returns a pointer to the partNode this item is displaying.
  */
  partNode* node() const { return mPartNode; }
  /**
  * @returns the initial size of the tree node data.
  */
  KIO::filesize_t origSize() const { return mOrigSize; }
  /**
  * Sets the initial size of the tree node data.
  */
  void setOrigSize( KIO::filesize_t size ) { mOrigSize = size; }

private:
  void setIconAndTextForType( const QString &mimetype );

private:
  partNode* mPartNode;
  KIO::filesize_t mOrigSize;
};

#endif // KMMIMEPARTTREE_H
