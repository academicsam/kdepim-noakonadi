/*
    KNode, the KDE newsreader
    Copyright (c) 1999-2006 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#ifndef KNARTICLEFACTORY_H
#define KNARTICLEFACTORY_H

#include <QByteArray>
#include <QList>
#include <QListWidgetItem>

#include <kdialog.h>

#include "knjobdata.h"
#include "knarticle.h"

class QLabel;

class KNGroup;
class KNCollection;
class KNComposer;
class KNSendErrorDialog;
class KNNntpAccount;


/** Article factory, responsible for creating and sending news articles. */
class KNArticleFactory : public QObject , public KNJobConsumer {

  Q_OBJECT

  public:
    enum replyType { RTgroup, RTmail, RTboth };

    KNArticleFactory( QObject *parent = 0 );
    ~KNArticleFactory();

    //factory methods
    void createPosting(KNNntpAccount *a);
    void createPosting(KNGroup *g);
    void createReply( KNRemoteArticle *a, const QString &selectedText = QString(), bool post = true, bool mail = false );
    void createForward(KNArticle *a);
    void createCancel(KNArticle *a);
    void createSupersede(KNArticle *a);
    void createMail(KMime::Types::Mailbox *address);

    /// Send a mail via an external program.
    void sendMailExternal( const QString &address = QString(), const QString &subject = QString(), const QString &body = QString() );

    //article handling
    void edit(KNLocalArticle *a);
    void sendArticles( KNLocalArticle::List &l, bool now = true );
    void sendOutbox();

    //composer handling
    bool closeComposeWindows();    // try to close all composers, return false if user objects
    void deleteComposerForArticle(KNLocalArticle *a);
    KNComposer* findComposer(KNLocalArticle *a);
    void configChanged();

  protected:
    //job handling
    void processJob(KNJobData *j); //reimplemented from KNJobConsumer

    //article generation
    // col: group or account
    KNLocalArticle* newArticle(KNCollection *col, QString &sig, const QByteArray &defChset, bool withXHeaders=true, KNArticle *origPost=0);

    //cancel & supersede
    bool cancelAllowed(KNArticle *a);

    //send-errors
    void showSendErrorDialog();

    QList<KNComposer*> mCompList;
    KNSendErrorDialog *s_endErrDlg;

  protected slots:
    void slotComposerDone(KNComposer *com);
    void slotSendErrorDialogDone();

};


/** Message send error dialog. */
class KNSendErrorDialog : public KDialog  {

  Q_OBJECT

  public:
    KNSendErrorDialog();
    ~KNSendErrorDialog();

    void append(const QString &subject, const QString &error);

  protected:
    /** Error list widget item. */
    class ErrorListItem : public QListWidgetItem {
      public:
        /** Creates a new error list item.
         * @param text The item text.
         * @param error The error message.
         */
        ErrorListItem( const QString &text, const QString &error )
          : QListWidgetItem( text ), mError( error )  {}
        /** Returns the error message of this item. */
        QString error() const { return mError; }
      private:
        QString mError;
    };

    /// Error list widget.
    QListWidget *mErrorList;
    /// Error message label.
    QLabel *mError;

  protected slots:
    void slotHighlighted(int idx);
};

#endif //KNARTICLEFACTORY_H
