/* -*- mode: C++; c-file-style: "gnu" -*-
  This file is part of KMail, the KDE mail client.
  Copyright (c) 1997 Markus Wuebben <markus.wuebben@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef __KMComposeWin
#define __KMComposeWin

// KMail includes
#include "composer.h"
#include "messagesender.h"

// Qt includes
#include <QFont>
#include <QList>
#include <QPalette>
#include <QPointer>

// KDE includes
#include <kglobalsettings.h>

// LIBKDEPIM includes
#include <libkdepim/kmeditor.h>

// Other includes
#include "kleo/enum.h"

class QByteArray;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLabel;
class QPushButton;
class QSplitter;
class QTreeWidgetItem;

class KLineEdit;
class KMComposeWin;
class KMComposerEditor;
class KMFolderComboBox;
class KMFolder;
class KMMessage;
class KMMessagePart;
class KSelectAction;
class KSelectAction;
class KAction;
class KJob;
class KToggleAction;
class KTemporaryFile;
class KTempDir;
class KToggleAction;
class KUrl;
class KRecentFilesAction;
class MessageComposer;
class RecipientsEditor;
class KMLineEdit;
class KMAtmListViewItem;
class SnippetWidget;
class partNode;

namespace KPIM {
}

namespace Sonnet {
  class DictionaryComboBox;
}

namespace KPIMIdentities {
  class IdentityCombo;
  class Identity;
}

namespace MailTransport {
  class TransportComboBox;
}

namespace KMail {
  class AttachmentListView;
  class EditorWatcher;
}

namespace KIO {
  class Job;
}

namespace GpgME {
  class Error;
}

//-----------------------------------------------------------------------------
class KMComposeWin : public KMail::Composer
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.kmail.mailcomposer")

  friend class ::KMComposerEditor;

  friend class ::MessageComposer;

  private: // mailserviceimpl, kmkernel, kmcommands, callback, kmmainwidget
    explicit KMComposeWin( KMMessage *msg = 0, TemplateContext context = NoTemplate,
                           uint identity = 0, const QString & textSelection = QString(),
                           const QString & customTemplate = QString() );
    ~KMComposeWin();

  public:
    static Composer *create( KMMessage *msg = 0, TemplateContext context = NoTemplate,
                             uint identity = 0, const QString & textSelection = QString(),
                             const QString & customTemplate = QString() );

  QString dbusObjectPath() const;
  QString smartQuote( const QString & msg );

  /**
   * Start of D-Bus callable stuff. The D-Bus methods need to be public slots,
   * otherwise they can't be accessed.
   */
  public slots:

    Q_SCRIPTABLE void send( int how );

    Q_SCRIPTABLE void addAttachmentsAndSend( const KUrl::List & urls,
                                             const QString & comment,
                                             int how );

    Q_SCRIPTABLE void addAttachment( const KUrl & url,
                                     const QString & comment );

    Q_SCRIPTABLE void addAttachment( const QString & name,
                                     const QByteArray & cte,
                                     const QByteArray & data,
                                     const QByteArray & type,
                                     const QByteArray & subType,
                                     const QByteArray & paramAttr,
                                     const QString & paramValue,
                                     const QByteArray & contDisp );

  /**
   * End of D-Bus callable stuff
   */

  private:

    /**
     * Write settings to app's config file.
     */
    void writeConfig( void );

  public: // kmkernel, kmcommands, callback
    /**
     * Set the message the composer shall work with. This discards
     * previous messages without calling applyChanges() on them before.
     */
     void setMsg( KMMessage *newMsg, bool mayAutoSign=true,
                 bool allowDecryption=false, bool isModified=false );

     /**
      * Disables word wrap completely. No wrapping at all will occur, not even
      * at the right end of the editor.
      * This is useful when sending invitations.
      */
     void disableWordWrap();

     /**
      * Disables HTML completely. It disables HTML at the point of calling this and disables it
      * again when sending the message, to be sure. Useful when sending invitations.
      * This will <b>not</b> remove the actions for activating HTML mode again, it is only
      * meant for automatic invitation sending.
      * Also calls @sa disableHtml() internally.
      */
     void forceDisableHtml();

     /** Don't check if there are too many recipients for a mail, eg. when sending out invitations. */
     void disableRecipientNumberCheck();

    /** Don't check for forgotten attachments for a mail, eg. when sending out invitations. */
    void disableForgottenAttachmentsCheck();

    /**
    * Ignore the "sticky" setting of the transport combo box and prefer the X-KMail-Transport
    * header field of the message instead.
    * Do the same for the identity combo box, don't obey the "sticky" setting but use the
    * X-KMail-Identity header field instead.
    *
    * This is useful when sending out invitations, since you don't see the GUI and want the
    * identity and transport to be set to the values stored in the messages.
    */
    void ignoreStickyFields();

     /**
      * Returns @c true while the message composing is in progress.
      */
     bool isComposing() const { return mComposer != 0; }

     /**
      * Set the text selection the message is a response to.
      */
     void setTextSelection( const QString& selection );

     /**
      * Set custom template to be used for the message.
      */
     void setCustomTemplate( const QString& customTemplate );

  private: // kmedit
    /**
     * Returns message of the composer. To apply the user changes to the
     * message, call applyChanges() first.
     */
     KMMessage *msg() const { return mMsg; }

  public: // kmkernel
    /**
     * Set the filename which is used for autosaving.
     */
    void setAutoSaveFilename( const QString & filename );

  private:
    /**
     * Returns true if the message was modified by the user.
     */
    bool isModified() const;

    /**
     * Set whether the message should be treated as modified or not.
     */
    void setModified( bool modified );

  public: // kmkernel, callback
    /**
     * If this flag is set the message of the composer is deleted when
     * the composer is closed and the message was not sent. Default: false
     */
     inline void setAutoDelete( bool f ) { mAutoDeleteMsg = f; }

  public: // kmcommand
    /**
     * If this folder is set, the original message is inserted back after
     * canceling
     */
     void setFolder( KMFolder *aFolder ) { mFolder = aFolder; }

  public: // kmkernel, kmcommand, mailserviceimpl

    /**
     * Reimplemented
     */
     virtual void setCharset( const QByteArray &aCharset, bool forceDefault = false );

  public: // kmcommand
    /**
     * Sets the focus to the edit-widget.
     */
     void setReplyFocus( bool hasMessage = true );

    /**
     * Sets the focus to the subject line edit. For use when creating a
     * message to a known recipient.
     */
     void setFocusToSubject();

  private:
    /**
     * determines whether inline signing/encryption is selected
     */
     bool inlineSigningEncryptionSelected();

     /**
      * Enables HTML mode, by showing the HTML toolbar and checking the
      * "Formatting" action
      */
     void enableHtml();

     /**
      * Disables the HTML mode, by hiding the HTML toolbar and unchecking the
      * "Formatting" action. Also, removes all rich-text formatting.
      */
     enum Confirmation { LetUserConfirm, NoConfirmationNeeded };
     void disableHtml( Confirmation confirmation );

     /**
      * Tries to find the given mimetype @p type in the KDE Mimetype registry.
      * If found, returns its localized description, otherwise the @p type
      * in lowercase.
      */
     static QString prettyMimeType( const QString &type );

  public: // callback
    /** Disabled signing and encryption completely for this composer window. */
    void setSigningAndEncryptionDisabled( bool v )
    {
      mSigningAndEncryptionExplicitlyDisabled = v;
    }

  private slots:

    /**
     * Actions:
     */
    void slotPrint();
    void slotAttachFile();
    void slotInsertRecentFile( const KUrl & );
    void slotAttachedFile( const KUrl & );
    void slotRecentListFileClear();

  public slots: // kmkernel, callback
    void slotSendNow();

  private slots:
    void slotSendNowVia( QAction * );
    void slotSendLater();
    void slotSendLaterVia( QAction * );
    void getTransportMenu();

    /**
     * Returns true when saving was successful.
     */
    void slotSaveDraft();
    void slotSaveTemplate();
    void slotNewComposer();
    void slotNewMailReader();
    void slotClose();
    void slotHelp();
    void slotUndo();
    void slotRedo();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotPasteAsAttachment();
    void slotFormatReset();
    void slotMarkAll();

    void slotFolderRemoved( KMFolder * );
    void slotEditDone( KMail::EditorWatcher* watcher );
    void slotLanguageChanged( const QString &language );

    void slotEditorTextChanged();
  public slots: // kmkernel
    /**
       Tell the composer to always send the message, even if the user
       hasn't changed the next. This is useful if a message is
       autogenerated (e.g., via a D-Bus call), and the user should
       simply be able to confirm the message and send it.
    */
    void slotSetAlwaysSend( bool bAlwaysSend );

  private slots:
    /**
     * toggle fixed width font.
     */
    void slotUpdateFont();

    /**
     * Open addressbook editor dialog.
     */
    void slotAddrBook();

    /**
     * Insert a file to the end of the text in the editor.
     */
    void slotInsertFile();

    void slotSetCharset();
    /**
     * Check spelling of text.
     */
    void slotSpellcheckConfig();

    /**
     * Change crypto plugin to be used for signing/encrypting messages,
     * or switch to built-in OpenPGP code.
     */
    void slotSelectCryptoModule( bool init = false );

    /**
     * XML-GUI stuff
     */
    void slotEditToolbars();
    void slotUpdateToolbars();
    void slotEditKeys();

    /**
     * Read settings from app's config file.
     */
    void readConfig( bool reload = false );

    /**
     * Change window title to given string.
     */
    void slotUpdWinTitle( const QString & );

    /**
     * Switch the icon to lock or unlock respectivly.
     * Change states of all encrypt check boxes in the attachments listview
     */
    void slotEncryptToggled( bool );

    /**
     * Change states of all sign check boxes in the attachments listview
     */
    void slotSignToggled( bool );

  public slots: // kmkernel, callback
    /**
     * Switch wordWrap on/off
     */
    void slotWordWrapToggled( bool );

  private slots:
    /**
     * Append signature to the end of the text in the editor.
     */
    void slotAppendSignature();

    /**
    * Prepend signature at the beginning of the text in the editor.
    */
    void slotPrependSignature();

    /**
    * Insert signature at the cursor position of the text in the editor.
    */
    void slotInsertSignatureAtCursor();

    /**
     * Attach sender's public key.
     */
    void slotInsertMyPublicKey();

    /**
     * Insert arbitrary public key from public keyring in the editor.
     */
    void slotInsertPublicKey();

    /**
     * Enable/disable some actions in the Attach menu
     */
    void slotUpdateAttachActions();
    void slotAttachEdit();
    void slotAttachEditWith();

    /**
     * Open a popup-menu in the attachments-listbox.
     */
    void slotAttachPopupMenu( QTreeWidgetItem* );

    /**
     * Attachment operations.
     */
    void slotAttachOpen();
    void slotAttachView();
    void slotAttachRemove();
    void slotAttachSave();
    void slotAttachProperties();
    void slotAttachmentDragStarted();

    void slotCleanSpace();
    void slotToggleMarkup();
    void slotTextModeChanged( KRichTextEdit::Mode );
    void htmlToolBarVisibilityChanged( bool visible );
    void slotSpellcheckDoneClearStatus();

  public slots: // kmkernel
    void autoSaveMessage();

  private slots:

    void slotView();

    /**
     * Update composer field to reflect new identity
     * @param initalChange if true, don't apply the template. This is useful when calling
     *                     this function from setMsg(), because there, the message already has the
     *                     template, and we want to avoid calling the template parser unnecessarily.
     */
    void slotIdentityChanged( uint uoid, bool initalChange = false );

    /**
     * KIO slots for attachment insertion
     */
    void slotAttachFileData( KIO::Job *, const QByteArray & );
    void slotAttachFileResult( KJob * );

    void slotCursorPositionChanged();

    void slotSpellCheckingStatus( const QString & status );

  public: // kmkernel, attachmentlistview
    bool addAttach( const KUrl &url );

  public: // kmcommand
    /**
     * Add an attachment to the list.
     */
    void addAttach( KMMessagePart *msgPart );

  private:
    const KPIMIdentities::Identity & identity() const;
    uint identityUid() const;
    Kleo::CryptoMessageFormat cryptoMessageFormat() const;
    bool encryptToSelf() const;

  signals:
    void applyChangesDone( bool );
    void attachmentAdded( const KUrl &, bool success );

  private:
    /**
     * Applies the user changes to the message object of the composer
     * and signs/encrypts the message if activated. Returns false in
     * case of an error (e.g. if PGP encryption fails).
     * Disables the controls of the composer window unless @p dontDisable
     * is true.
     */
    void applyChanges( bool dontSignNorEncrypt, bool dontDisable=false );

    /**
     * Install grid management and header fields. If fields exist that
     * should not be there they are removed. Those that are needed are
     * created if necessary.
     */
    void rethinkFields( bool fromslot=false );

    /**
      Connect signals for moving focus by arrow keys. Returns next edit.
    */
    QWidget *connectFocusMoving( QWidget *prev, QWidget *next );

    /**
     * Show or hide header lines
     */

    void rethinkHeaderLine( int aValue, int aMask, int &aRow,
                            QLabel *aLbl, KLineEdit *aEdt,
                            QPushButton *aBtn = 0 );

    void rethinkHeaderLine( int value, int mask, int &row,
                            QLabel *lbl, QComboBox *cbx, QCheckBox *chk ); // krazy:exclude=qclasses

    /**
     * Apply template to new or unmodified message.
     */
    void applyTemplate( uint uoid );

    /**
     * Set the quote prefix according to identity.
     */
    void setQuotePrefix( uint uoid );

    /**
     * Checks how many recipients are and warns if there are too many.
     * @return true, if the user accepted the warning and the message should be sent
    */
    bool checkRecipientNumber() const;

    bool checkTransport() const;
    /**
     * Initialization methods
     */
    void setupActions();
    void setupStatusBar();
    void setupEditor();

    /**
     * Header fields.
     */
    QString subject() const;
    QString to() const;
    QString cc() const;
    QString bcc() const;
    QString from() const;
    QString replyTo() const;

    /**
     * Use the given folder as sent-mail folder if the given folder exists.
     * Else show an error message and use the default sent-mail folder as
     * sent-mail folder.
     */
    void setFcc( const QString &idString );

    /**
     * Ask for confirmation if the message was changed before close.
     */
    virtual bool queryClose();

    /**
     * prevent kmail from exiting when last window is deleted (kernel rules)
     */
    virtual bool queryExit();

    /**
     * Open the attachment with the given index.
     */
    void openAttach( int index );

    /**
     * View the attachment with the given index.
     */
    void viewAttach( int index );

    /**
     * Remove an attachment from the list.
     */
    void removeAttach( const QString &url );
    void removeAttach( int idx );

    /**
     * Updates an item in the QListView to represnet a given message part
     */
    void msgPartToItem( const KMMessagePart *msgPart, KMAtmListViewItem *lvi,
                        bool loadDefaults = true );


    /**
     * Searches the mime tree, where root is the root node, for embedded images,
     * extracts them froom the body and adds them to the editor.
     */
    void collectImages( partNode *root );

  private:
    /**
     * Turn encryption on/off. If setByUser is true then a message box is shown
     * in case encryption isn't possible.
     */
    void setEncryption( bool encrypt, bool setByUser = false );

    /**
      Edit the attachment with the given index.
    */
    void editAttach( int index, bool openWith );

    /**
     * Turn signing on/off. If setByUser is true then a message box is shown
     * in case signing isn't possible.
     */
    void setSigning( bool sign, bool setByUser = false );

    /**
      Returns true if the user forgot to attach something.
    */
    bool userForgotAttachment();

    /**
     * Retrieve encrypt flag of an attachment
     * ( == state of it's check box in the attachments list view )
     */
    bool encryptFlagOfAttachment( int idx );

    /**
     * Retrieve sign flag of an attachment
     * ( == state of it's check box in the attachments list view )
     */
    bool signFlagOfAttachment( int idx );

    /**
     * Decrypt an OpenPGP block or strip off the OpenPGP envelope of a text
     * block with a clear text signature. This is only done if the given
     * string contains exactly one OpenPGP block.
     * This function is for example used to restore the unencrypted/unsigned
     * message text for editting.
     */
    static void decryptOrStripOffCleartextSignature( QByteArray & );

    /**
     * Save the message into the Drafts or Templates folder.
     */
    bool saveDraftOrTemplate( const QString &folderName, KMMessage *msg );

    enum SaveIn {
      None,
      Drafts,
      Templates
    };

    /**
     * Send the message. Returns true if the message was sent successfully.
     */
    void doSend( KMail::MessageSender::SendMethod method=KMail::MessageSender::SendDefault,
                 KMComposeWin::SaveIn saveIn = KMComposeWin::None );

    /**
     * Returns the autosave interval in milliseconds (as needed for QTimer).
     */
    int autoSaveInterval() const;

    /**
     * Initialize autosaving (timer and filename).
     */
    void initAutoSave();

    /**
     * Enables/disables autosaving depending on the value of the autosave
     * interval.
     */
    void updateAutoSave();

    /**
     * Stop autosaving and delete the autosaved message.
     */
    void cleanupAutoSave();

    /**
     * Helper to insert the signature of the current identity arbitrarily
     * in the editor, connecting slot functions to KMeditor::insertSignature().
     * @param placement the position of the signature
     */
    void insertSignatureHelper( KPIMIdentities::Signature::Placement = KPIMIdentities::Signature::End );


  private slots:
    /**
     * Compress an attachemnt with the given index
     */
    void compressAttach( KMAtmListViewItem *attachmentItem );
    void uncompressAttach( KMAtmListViewItem *attachmentItem );
    void recipientEditorSizeHintChanged();
    void setMaximumHeaderSize();

  private:
    QWidget   *mMainWidget;
    MailTransport::TransportComboBox *mTransport;
    Sonnet::DictionaryComboBox *mDictionaryCombo;
    KPIMIdentities::IdentityCombo    *mIdentity;
    KMFolderComboBox *mFcc;
    KMLineEdit *mEdtFrom, *mEdtReplyTo;
    KMLineEdit *mEdtSubject;
    QLabel    *mLblIdentity, *mLblTransport, *mLblFcc;
    QLabel    *mLblFrom, *mLblReplyTo;
    QLabel    *mLblSubject;
    QLabel    *mDictionaryLabel;
    QCheckBox *mBtnIdentity, *mBtnTransport, *mBtnFcc;
    bool mDone;
    bool mAtmModified;

    KMComposerEditor *mEditor;
    QGridLayout *mGrid;
    KMMessage *mMsg;
    QString mTextSelection;
    QString mCustomTemplate;
    QVector<KMMessage*> mComposedMessages;
    KMail::AttachmentListView *mAtmListView;
    QList<KMAtmListViewItem*> mAtmItemList;
    QList<KMMessagePart*> mAtmList;
    QMenu *mAttachMenu;
    QAction *mOpenId, *mViewId, *mRemoveId, *mSaveAsId, *mPropertiesId,
            *mEditAction, *mEditWithAction;
    bool mAutoDeleteMsg;
    bool mSigningAndEncryptionExplicitlyDisabled;
    bool mLastSignActionState, mLastEncryptActionState;
    bool mLastIdentityHasSigningKey, mLastIdentityHasEncryptionKey;
    KMFolder *mFolder;
    long mShowHeaders;
    bool mConfirmSend;
    bool mDisableBreaking;
    bool mForceDisableHtml;     // Completely disable any HTML. Useful when sending invitations in the
                                // mail body.
    int mNumHeaders;
    QFont mBodyFont, mFixedFont;
    QList<KTemporaryFile*> mAtmTempList;
    QPalette mPalette;
    uint mId;
    TemplateContext mContext;

    KAction *mAttachPK, *mAttachMPK, *mAttachRemoveAction, *mAttachSaveAction,
            *mAttachPropertiesAction, *mCleanSpace;
    KRecentFilesAction *mRecentAction;

    KToggleAction *mSignAction, *mEncryptAction, *mRequestMDNAction;
    KToggleAction *mUrgentAction, *mAllFieldsAction, *mFromAction;
    KToggleAction *mReplyToAction;
    KToggleAction *mSubjectAction;
    KToggleAction *mIdentityAction, *mTransportAction, *mFccAction;
    KToggleAction *mWordWrapAction, *mFixedFontAction, *mAutoSpellCheckingAction;
    KToggleAction *mDictionaryAction, *mSnippetAction;

    KToggleAction *markupAction;
    KAction *actionFormatReset;

    KSelectAction *mEncodingAction;
    KSelectAction *mCryptoModuleAction;

    KAction * mFindText, *mFindNextText, *mReplaceText, *mSelectAll;

    QByteArray mCharset;
    QByteArray mDefCharset;
    QStringList mCharsets;
    bool mAutoCharset;

    bool mAlwaysSend;

    QStringList mFolderNames;
    QList<QPointer<KMFolder> > mFolderList;
    QMap<KJob*, KUrl> mAttachJobs;
    KUrl::List mAttachFilesPending;
    int mAttachFilesSend;

  private:
  // helper method for slotInsert(My)PublicKey()
    void startPublicKeyExport();
    bool canSignEncryptAttachments() const {
      return cryptoMessageFormat() != Kleo::InlineOpenPGPFormat;
    }

    // helper method for rethinkFields
    int calcColumnWidth( int which, long allShowing, int width ) const;

    // Sends a key event with the control modifier and the specified key to
    // the given widget. This function is protected against recursion.
    void sendControlKeyEvent( QWidget *receiver, Qt::Key key );

  private slots:
    void slotCompletionModeChanged( KGlobalSettings::Completion );
    void slotConfigChanged();

    void slotComposerDone( bool );

    void slotContinueDoSend( bool );
    void slotContinuePrint( bool );
    void slotContinueAutoSave();

    void slotEncryptChiasmusToggled( bool );

    /**
     * Helper method (you could call is a bottom-half :) for
     * startPublicKeyExport()
     */
    void slotPublicKeyExportResult( const GpgME::Error &err,
                                    const QByteArray &keydata );

    /**
     *  toggle automatic spellchecking
     */
    void slotAutoSpellCheckingToggled( bool );

    /**
     * Updates the visibility and text of the signature and encryption state indicators.
     */
    void slotUpdateSignatureAndEncrypionStateIndicators();

  private:
    QFont mSaveFont;
    QSplitter *mHeadersToEditorSplitter;
    QWidget* mHeadersArea;
    QSplitter *mSplitter;
    QSplitter *mSnippetSplitter;
    struct atmLoadData
    {
      KUrl url;
      QByteArray data;
      bool insert;
      QByteArray encoding;
    };
    QMap<KIO::Job *, atmLoadData> mMapAtmLoadData;

    // These are for passing on methods over the applyChanges calls
    KMail::MessageSender::SendMethod mSendMethod;
    KMComposeWin::SaveIn mSaveIn;

    KToggleAction *mEncryptChiasmusAction;
    bool mEncryptWithChiasmus;

    // The temporary object that constructs the message out of the window
    MessageComposer *mComposer;

    // Temp var for slotPrint:
    bool mMessageWasModified;

    // Temp var for slotInsert(My)PublicKey():
    QString mFingerprint;

    RecipientsEditor *mRecipientsEditor;
    int mLabelWidth;

    QTimer *mAutoSaveTimer;
    QString mAutoSaveFilename;
    int mLastAutoSaveErrno; // holds the errno of the last try to autosave

    QMenu *mActNowMenu;
    QMenu *mActLaterMenu;

    QString mdbusObjectPath;
    static int s_composerNumber;

    QMap<KMail::EditorWatcher*, KMMessagePart*> mEditorMap;
    QMap<KMail::EditorWatcher*, KTemporaryFile*> mEditorTempFiles;

    SnippetWidget *mSnippetWidget;
    QList<KTempDir*> mTempDirs;

    QLabel *mSignatureStateIndicator;
    QLabel *mEncryptionStateIndicator;

    bool mPreventFccOverwrite;
    bool mCheckForRecipients;
    bool mCheckForForgottenAttachments;
    bool mIgnoreStickyFields;
};

#endif
