/*  -*- mode: C++; c-file-style: "gnu" -*-
    objecttreeparser.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003      Marc Mutz <mutz@kde.org>
    Copyright (C) 2002-2004 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Copyright (c) 2009 Andras Mantia <andras@kdab.net>

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

// my header file
#include "objecttreeparser.h"
#include "objecttreeparser_p.h"
#include "objecttreesourceif.h"

// other KMail headers
#include "viewer_p.h"
#include <kpimutils/email.h>
#include "partmetadata.h"
#include "attachmentstrategy.h"
#include "interfaces/htmlwriter.h"
#include "htmlstatusbar.h"
#include "csshelper.h"
#include "bodypartformatter.h"
#include "bodypartformatterfactory.h"
#include "partnodebodypart.h"
#include "interfaces/bodypartformatter.h"
#include "globalsettings.h"
#include "util.h"
#include "global.h"
#include "kleojobexecutor.h"
#include "stringutil.h"
#include "nodehelper.h"
#include "iconnamecache.h"
#include "htmlquotecolorer.h"

#include <kleo/specialjob.h>
#include <kleo/cryptobackendfactory.h>
#include <kleo/decryptverifyjob.h>
#include <kleo/verifydetachedjob.h>
#include <kleo/verifyopaquejob.h>
#include <kleo/keylistjob.h>
#include <kleo/importjob.h>
#include <kleo/dn.h>

#include <gpgme++/importresult.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme.h>

#include <libkpgp/kpgpblock.h>
#include <libkpgp/kpgp.h>
#include <kpimutils/linklocator.h>
using KPIMUtils::LinkLocator;

#include <ktnef/ktnefparser.h>
#include <ktnef/ktnefmessage.h>
#include <ktnef/ktnefattach.h>

// other KDE headers
#include <kdebug.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <ktemporaryfile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kcodecs.h>
#include <kconfiggroup.h>
#include <kstyle.h>

// other Qt headers
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QByteArray>
#include <QBuffer>
#include <QPixmap>
#include <QPainter>
#include <QPointer>

// other headers
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
// FIXME(Andras) port to akonadi #include "chiasmuskeyselector.h"

#include <kmime/kmime_message.h>

using namespace MessageViewer;

// A small class that eases temporary CryptPlugWrapper changes:
class ObjectTreeParser::CryptoProtocolSaver {
  ObjectTreeParser * otp;
  const Kleo::CryptoBackend::Protocol * protocol;
public:
  CryptoProtocolSaver( ObjectTreeParser * _otp, const Kleo::CryptoBackend::Protocol* _w )
    : otp( _otp ), protocol( _otp ? _otp->cryptoProtocol() : 0 )
  {
    if ( otp )
      otp->setCryptoProtocol( _w );
  }

  ~CryptoProtocolSaver() {
    if ( otp )
      otp->setCryptoProtocol( protocol );
  }
};


ObjectTreeParser::ObjectTreeParser( ObjectTreeSourceIf *source,
                                    NodeHelper* nodeHelper,
                                    const Kleo::CryptoBackend::Protocol * protocol,
                                    bool showOnlyOneMimePart, bool keepEncryptions,
                                    bool includeSignatures,
                                    const AttachmentStrategy * strategy,
                                    HtmlWriter * htmlWriter,
                                    CSSHelper * cssHelper )
  : mSource( source ),
    mCryptoProtocol( protocol ),
    mShowOnlyOneMimePart( showOnlyOneMimePart ),
    mKeepEncryptions( keepEncryptions ),
    mIncludeSignatures( includeSignatures ),
    mHasPendingAsyncJobs( false ),
    mAllowAsync( false ),
    mAttachmentStrategy( strategy ),
    mHtmlWriter( htmlWriter ),
    mCSSHelper( cssHelper )
{
  assert( source );
  if ( !attachmentStrategy() )
    mAttachmentStrategy = source->attachmentStrategy();
  if ( !this->htmlWriter() )
    mHtmlWriter = source->htmlWriter();
  if ( !this->cssHelper() )
    mCSSHelper = source->cssHelper();

  if ( !nodeHelper ) {
    mNodeHelper = new NodeHelper();
    mDeleteNodeHelper = true;
  } else {
    mNodeHelper = nodeHelper;
    mDeleteNodeHelper = false;
  }
}

ObjectTreeParser::ObjectTreeParser( const ObjectTreeParser & other )
  : mSource( other.mSource ),
    mNodeHelper( other.nodeHelper() ), //TODO(Andras) hm, review what happens if mDeleteNodeHelper was true in the source
    mCryptoProtocol( other.cryptoProtocol() ),
    mShowOnlyOneMimePart( other.showOnlyOneMimePart() ),
    mKeepEncryptions( other.keepEncryptions() ),
    mIncludeSignatures( other.includeSignatures() ),
    mHasPendingAsyncJobs( other.hasPendingAsyncJobs() ),
    mAllowAsync( other.allowAsync() ),
    mAttachmentStrategy( other.attachmentStrategy() ),
    mHtmlWriter( other.htmlWriter() ),
    mCSSHelper( other.cssHelper() ),
    mDeleteNodeHelper( false ) // TODO see above
{

}

ObjectTreeParser::~ObjectTreeParser()
{
  if ( mDeleteNodeHelper ) {
    delete mNodeHelper;
    mNodeHelper = 0;
  }
}

void ObjectTreeParser::insertAndParseNewChildNode( KMime::Content& startNode,
                                                    const char* content,
                                                    const char* cntDesc,
                                                    bool append, bool addToTextualContent )
{
  KMime::Content *newNode = new KMime::Content();
  newNode->setContent( content );
  newNode->parse();

/* TODO see below
  // Build the object tree of the new node before setting the parent, as otherwise
  // buildObjectTree() would erronously modify the parents as well
  newNode->buildObjectTree( false );
*/

/*TODO (Andras) port it
  if ( ( !myBody->Body().FirstBodyPart() ||
          myBody->body().length() == 0 ) &&
        startNode.dwPart() &&
        startNode.dwPart()->Body().Message() &&
        startNode.dwPart()->Body().Message()->Body().FirstBodyPart() )
  {
    // if encapsulated imap messages are loaded the content-string is not complete
    // so we need to keep the child dwparts
    myBody = new DwBodyPart( *(startNode.dwPart()->Body().Message()) );
  }
  */

  if ( !newNode->head().isEmpty() ) {
    newNode->contentDescription()->from7BitString( cntDesc );
  }

  startNode.addContent( newNode, !append );

/*TODO(Andras) check what should we do here:
  newNode->buildObjectTree( false ); // <-- that was moved up there ^ as part of a merge

  if ( startNode.mimePartTreeItem() ) {
    newNode->fillMimePartTree( startNode.mimePartTreeItem(), 0,
                                QString(), QString(), QString(), 0,
                                append );
  }*/
  ObjectTreeParser otp( mSource, mNodeHelper, cryptoProtocol() );
  otp.parseObjectTree( newNode );
  if ( addToTextualContent ) {
    mRawReplyString += otp.rawReplyString();
    mTextualContent += otp.textualContent();
    if ( !otp.textualContentCharset().isEmpty() )
      mTextualContentCharset = otp.textualContentCharset();
  }
}


//-----------------------------------------------------------------------------

void ObjectTreeParser::parseObjectTree( KMime::Content * node ) {

  if ( !node )
    return;

  // reset pending async jobs state (we'll rediscover pending jobs as we go)
  mHasPendingAsyncJobs = false;

  // reset "processed" flags for...
  if ( showOnlyOneMimePart() ) {
    // ... this node and all descendants
    mNodeHelper->setNodeUnprocessed( node, false );
    if ( NodeHelper::firstChild( node ) ) {
      mNodeHelper->setNodeUnprocessed( node, true );
    }
  } else if ( !node->parent() ) {
    // ...this node and all it's siblings and descendants
    mNodeHelper->setNodeUnprocessed( node, true );
  }

  // Make sure the whole content is relative, so that nothing is painted over the header
  // if a malicious message uses absolute positioning.
  bool isRoot = node->isTopLevel();
  if ( isRoot && mHtmlWriter )
    htmlWriter()->queue( "<div style=\"position: relative\">\n" );

  KMime::Content::List contents = node->contents();
  contents.prepend(node); //the toplevel node needs to parsed as well
  Q_FOREACH(KMime::Content *c, contents)
  {
    if ( mNodeHelper->nodeProcessed( c ) )
      continue;
    ProcessResult processResult( mNodeHelper );

    if ( mHtmlWriter )
      htmlWriter()->queue( QString::fromLatin1("<a name=\"att%1\"/>").arg( node->indexForContent(c).toString() ) );
    if ( const Interface::BodyPartFormatter * formatter
          = BodyPartFormatterFactory::instance()->createFor( c->contentType()->mediaType(), c->contentType()->subType() ) ) {
      PartNodeBodyPart part( c, mNodeHelper, codecFor( c ) );
      // Set the default display strategy for this body part relying on the
      // identity of Interface::BodyPart::Display and AttachmentStrategy::Display
      part.setDefaultDisplay( (Interface::BodyPart::Display) attachmentStrategy()->defaultDisplay( c ) );

     writeAttachmentMarkHeader( c );
     mNodeHelper->setNodeDisplayedEmbedded( c, true );
     const Interface::BodyPartFormatter::Result result = formatter->format( &part, htmlWriter() );
     writeAttachmentMarkFooter();

     switch ( result ) {
      case Interface::BodyPartFormatter::AsIcon:
        processResult.setNeverDisplayInline( true );
        // fall through:
      case Interface::BodyPartFormatter::Failed:
        defaultHandling( node, processResult );
        break;
      case Interface::BodyPartFormatter::Ok:
      case Interface::BodyPartFormatter::NeedContent:
        // FIXME: incomplete content handling
        ;
      }
    } else {
      const BodyPartFormatter * bpf
        = BodyPartFormatter::createFor( c->contentType()->mediaType(), c->contentType()->subType() );
      kFatal( !bpf, 5006 ) <<"THIS SHOULD NO LONGER HAPPEN ("
                            << c->contentType()->mediaType() << '/' << c->contentType()->subType() << ')';
      writeAttachmentMarkHeader( c );
      if ( bpf && !bpf->process( this, c, processResult ) )
        defaultHandling( c, processResult );
      writeAttachmentMarkFooter();
    }
    mNodeHelper->setNodeProcessed( c, false);

    // adjust signed/encrypted flags if inline PGP was found
    processResult.adjustCryptoStatesOfNode( node );

    if ( showOnlyOneMimePart() )
      break;
  }

  if ( isRoot && mHtmlWriter )
    htmlWriter()->queue( "</div>\n" );
}

void ObjectTreeParser::defaultHandling( KMime::Content * node, ProcessResult & result ) {
  // ### (mmutz) default handling should go into the respective
  // ### bodypartformatters.
  if ( !mHtmlWriter )
    return;

  // always show images in multipart/related when showing in html, not with an additional icon
  if ( result.isImage() && node->parent() &&
        node->parent()->contentType()->subType() == "related" && mSource->htmlMail() && !showOnlyOneMimePart() ) {
    QString fileName = mNodeHelper->writeNodeToTempFile( node );
    QString href = "file:" + KUrl::toPercentEncoding( fileName );
    QByteArray cid = node->contentID()->as7BitString(false);
    if ( cid.startsWith('<') )
      cid = cid.mid(1);
    if ( cid.endsWith('>') )
      cid = cid.left( cid.length() - 1 );
    htmlWriter()->embedPart( cid, href );
    return;
  }

  if ( attachmentStrategy() == AttachmentStrategy::hidden() &&
        !showOnlyOneMimePart() &&
        node->parent() /* message is not an attachment */ )
    return;

  bool asIcon = true;
  if ( !result.neverDisplayInline() )
    if ( const AttachmentStrategy * as = attachmentStrategy() )
      asIcon = as->defaultDisplay( node ) == AttachmentStrategy::AsIcon;

   // Show it inline if showOnlyOneMimePart(), which means the user clicked the image
   // in the message structure viewer manually, and therefore wants to see the full image
   if ( result.isImage() && showOnlyOneMimePart() && !result.neverDisplayInline() )
     asIcon = false;

  // neither image nor text -> show as icon
  if ( !result.isImage()
        && !node->contentType()->isText() )
    asIcon = true;

/*FIXME(Andras) port it
  // if the image is not complete do not try to show it inline
  if ( result.isImage() && !node->msgPart().isComplete() )
    asIcon = true;
    */

  if ( asIcon ) {
    if ( attachmentStrategy() != AttachmentStrategy::hidden()
          || showOnlyOneMimePart() )
      // Write the node as icon only
      writePartIcon( node );
  } else if ( result.isImage() ) {
    // Embed the image
    mNodeHelper->setNodeDisplayedEmbedded( node, true );
    writePartIcon( node, true );
  } else {
    mNodeHelper->setNodeDisplayedEmbedded( node, true );
    KMime::Message *topLevel = dynamic_cast<KMime::Message*>(node->topLevel());
    writeBodyString( node->decodedContent(),
                      topLevel ? topLevel->from()->asUnicodeString() : QString(),
                      codecFor( node ), result, false );
  }
  // end of ###
}

void ProcessResult::adjustCryptoStatesOfNode( KMime::Content * node ) const {
  if ( ( inlineSignatureState()  != KMMsgNotSigned ) ||
        ( inlineEncryptionState() != KMMsgNotEncrypted ) ) {
    mNodeHelper->setSignatureState( node, inlineSignatureState() );
    mNodeHelper->setEncryptionState( node, inlineEncryptionState() );
  }
}

//////////////////
//////////////////
//////////////////

static int signatureToStatus( const GpgME::Signature &sig )
{
  switch ( sig.status().code() ) {
    case GPG_ERR_NO_ERROR:
      return GPGME_SIG_STAT_GOOD;
    case GPG_ERR_BAD_SIGNATURE:
      return GPGME_SIG_STAT_BAD;
    case GPG_ERR_NO_PUBKEY:
      return GPGME_SIG_STAT_NOKEY;
    case GPG_ERR_NO_DATA:
      return GPGME_SIG_STAT_NOSIG;
    case GPG_ERR_SIG_EXPIRED:
      return GPGME_SIG_STAT_GOOD_EXP;
    case GPG_ERR_KEY_EXPIRED:
      return GPGME_SIG_STAT_GOOD_EXPKEY;
    default:
      return GPGME_SIG_STAT_ERROR;
  }
}

bool ObjectTreeParser::writeOpaqueOrMultipartSignedData( KMime::Content* data,
                                                    KMime::Content& sign,
                                                    const QString& fromAddress,
                                                    bool doCheck,
                                                    QByteArray* cleartextData,
                                                    const std::vector<GpgME::Signature> & paramSignatures,
                                                    bool hideErrors )
{
  kDebug() << "DECRYPT" << data;
  bool bIsOpaqueSigned = false;
  enum { NO_PLUGIN, NOT_INITIALIZED, CANT_VERIFY_SIGNATURES }
    cryptPlugError = NO_PLUGIN;

  const Kleo::CryptoBackend::Protocol* cryptProto = cryptoProtocol();

  QString cryptPlugLibName;
  QString cryptPlugDisplayName;
  if ( cryptProto ) {
    cryptPlugLibName = cryptProto->name();
    cryptPlugDisplayName = cryptProto->displayName();
  }

#ifdef DEBUG_SIGNATURE
#ifndef NDEBUG
  if ( !doCheck )
    kDebug() << "showing OpenPGP (Encrypted+Signed) data";
  else
    if ( data )
      kDebug() << "processing Multipart Signed data";
    else
      kDebug() << "processing Opaque Signed data";
#endif

  if ( doCheck && cryptProto ) {
    kDebug() << "going to call CRYPTPLUG" << cryptPlugLibName;
  }
#endif

  QByteArray cleartext;
  QByteArray signaturetext;

  if ( doCheck && cryptProto ) {
    if ( data ) {
      cleartext = data->head() + "\n" + data->body();
#ifdef DEBUG_SIGNATURE
      kDebug() << "ClearText : " << cleartext;

      dumpToFile( "dat_01_reader_signedtext_before_canonicalization",
                  cleartext.data(), cleartext.length() );

      // replace simple LFs by CRLSs
      // according to RfC 2633, 3.1.1 Canonicalization
      kDebug() <<"Converting LF to CRLF (see RfC 2633, 3.1.1 Canonicalization)";
#endif
      cleartext = KMime::LFtoCRLF( cleartext );
#ifdef DEBUG_SIGNATURE
      kDebug() <<"                                                       done.";
#endif
}

    dumpToFile( "dat_02_reader_signedtext_after_canonicalization",
                cleartext.data(), cleartext.length() );

    signaturetext = sign.body();
    dumpToFile( "dat_03_reader.sig", signaturetext.data(),
                signaturetext.size() );
  }

  std::vector<GpgME::Signature> signatures;
  if ( !doCheck )
    signatures = paramSignatures;

  PartMetaData messagePart;
  messagePart.isSigned = true;
  messagePart.technicalProblem = ( cryptProto == 0 );
  messagePart.isGoodSignature = false;
  messagePart.isEncrypted = false;
  messagePart.isDecryptable = false;
  messagePart.keyTrust = Kpgp::KPGP_VALIDITY_UNKNOWN;
  messagePart.status = i18n("Wrong Crypto Plug-In.");
  messagePart.status_code = GPGME_SIG_STAT_NONE;

  GpgME::Key key;

  if ( doCheck && cryptProto ) {
    GpgME::VerificationResult result;
    if ( data ) { // detached
      const VerifyDetachedBodyPartMemento * m
        = dynamic_cast<VerifyDetachedBodyPartMemento*>( mNodeHelper->bodyPartMemento( &sign, "verifydetached" ) );
      if ( !m ) {
        Kleo::VerifyDetachedJob * job = cryptProto->verifyDetachedJob();
        if ( !job ) {
          cryptPlugError = CANT_VERIFY_SIGNATURES;
          // PENDING(marc) cryptProto = 0 here?
        } else {
          QByteArray plainData = cleartext;
          VerifyDetachedBodyPartMemento * newM
            = new VerifyDetachedBodyPartMemento( job, cryptProto->keyListJob(), signaturetext, plainData );
          if ( allowAsync() ) {
            QObject::connect(newM, SIGNAL(update(MessageViewer::Viewer::UpdateMode)), mSource->sourceObject(), SLOT(update(MessageViewer::Viewer::UpdateMode)));
            if ( newM->start() ) {
              messagePart.inProgress = true;
              mHasPendingAsyncJobs = true;
            } else {
              m = newM;
            }
          } else {
            newM->exec();
            m = newM;
          }
          mNodeHelper->setBodyPartMemento( &sign, "verifydetached", newM );
        }
      } else if ( m->isRunning() ) {
        messagePart.inProgress = true;
        mHasPendingAsyncJobs = true;
        m = 0;
      }

      if ( m ) {
        result = m->verifyResult();
        messagePart.auditLogError = m->auditLogError();
        messagePart.auditLog = m->auditLogAsHtml();
        key = m->signingKey();
      }
    } else { // opaque
      const VerifyOpaqueBodyPartMemento * m
        = dynamic_cast<VerifyOpaqueBodyPartMemento*>( mNodeHelper->bodyPartMemento( &sign, "verifyopaque" ) );
      if ( !m ) {
        Kleo::VerifyOpaqueJob * job = cryptProto->verifyOpaqueJob();
        if ( !job ) {
          cryptPlugError = CANT_VERIFY_SIGNATURES;
          // PENDING(marc) cryptProto = 0 here?
        } else {
          VerifyOpaqueBodyPartMemento * newM
            = new VerifyOpaqueBodyPartMemento( job, cryptProto->keyListJob(), signaturetext );
          if ( allowAsync() ) {
            QObject::connect(newM, SIGNAL(update(MessageViewer::Viewer::UpdateMode)), mSource->sourceObject(), SLOT(update(MessageViewer::Viewer::UpdateMode)));
            if ( newM->start() ) {
              messagePart.inProgress = true;
              mHasPendingAsyncJobs = true;
            } else {
              m = newM;
            }
          } else {
            newM->exec();
            m = newM;
          }
          mNodeHelper->setBodyPartMemento( &sign, "verifyopaque", newM );
        }
      } else if ( m->isRunning() ) {
        messagePart.inProgress = true;
        mHasPendingAsyncJobs = true;
        m = 0;
      }

      if ( m ) {
        result = m->verifyResult();
        cleartext = m->plainText();
        messagePart.auditLogError = m->auditLogError();
        messagePart.auditLog = m->auditLogAsHtml();
        key = m->signingKey();
      }
    }
    std::stringstream ss;
    ss << result;
#ifdef DEBUG_SIGNATURE
    kDebug() << ss.str().c_str();
#endif
    signatures = result.signatures();
  }
  else
    messagePart.auditLogError = GpgME::Error( GPG_ERR_NOT_IMPLEMENTED );

#ifdef DEBUG_SIGNATURE
  if ( doCheck )
    kDebug() << "returned from CRYPTPLUG";
#endif

  // ### only one signature supported
  if ( !signatures.empty() ) {
#ifdef DEBUG_SIGNATURE
    kDebug() << "\nFound signature";
#endif
    GpgME::Signature signature = signatures.front();

    messagePart.status_code = signatureToStatus( signature );
    messagePart.status = QString::fromLocal8Bit( signature.status().asString() );
    for ( uint i = 1; i < signatures.size(); ++i ) {
      if ( signatureToStatus( signatures[i] ) != messagePart.status_code ) {
        messagePart.status_code = GPGME_SIG_STAT_DIFF;
        messagePart.status = i18n("Different results for signatures");
      }
    }
    if ( messagePart.status_code & GPGME_SIG_STAT_GOOD )
      messagePart.isGoodSignature = true;

    // save extended signature status flags
    messagePart.sigSummary = signature.summary();

    if ( key.keyID() )
      messagePart.keyId = key.keyID();
    if ( messagePart.keyId.isEmpty() )
      messagePart.keyId = signature.fingerprint();
    // ### Ugh. We depend on two enums being in sync:
    messagePart.keyTrust = (Kpgp::Validity)signature.validity();
    if ( key.numUserIDs() > 0 && key.userID( 0 ).id() )
      messagePart.signer = Kleo::DN( key.userID( 0 ).id() ).prettyDN();
    for ( uint iMail = 0; iMail < key.numUserIDs(); ++iMail ) {
      // The following if /should/ always result in TRUE but we
      // won't trust implicitely the plugin that gave us these data.
      if ( key.userID( iMail ).email() ) {
        QString email = QString::fromUtf8( key.userID( iMail ).email() );
        // ### work around gpgme 0.3.x / cryptplug bug where the
        // ### email addresses are specified as angle-addr, not addr-spec:
        if ( email.startsWith( '<' ) && email.endsWith( '>' ) )
          email = email.mid( 1, email.length() - 2 );
        if ( !email.isEmpty() )
          messagePart.signerMailAddresses.append( email );
      }
    }

    if ( signature.creationTime() )
      messagePart.creationTime.setTime_t( signature.creationTime() );
    else
      messagePart.creationTime = QDateTime();
    if ( messagePart.signer.isEmpty() ) {
      if ( key.numUserIDs() > 0 && key.userID( 0 ).name() )
        messagePart.signer = Kleo::DN( key.userID( 0 ).name() ).prettyDN();
      if ( !messagePart.signerMailAddresses.empty() ) {
        if ( messagePart.signer.isEmpty() )
          messagePart.signer = messagePart.signerMailAddresses.front();
        else
          messagePart.signer += " <" + messagePart.signerMailAddresses.front() + '>';
      }
    }
#ifdef DEBUG_SIGNATURE
    kDebug() << "\n  key id:" << messagePart.keyId
              << "\n  key trust:" << messagePart.keyTrust
              << "\n  signer:" << messagePart.signer;
#endif
  } else {
    messagePart.creationTime = QDateTime();
  }

  if ( !doCheck || !data ){
    if ( cleartextData || !cleartext.isEmpty() ) {
      if ( mHtmlWriter )
        htmlWriter()->queue( writeSigstatHeader( messagePart,
                                                  cryptProto,
                                                  fromAddress ) );
      bIsOpaqueSigned = true;

      CryptoProtocolSaver cpws( this, cryptProto );
      insertAndParseNewChildNode( sign, doCheck ? cleartext.data() : cleartextData->data(),
                                  "opaqued signed data" );

      if ( mHtmlWriter )
        htmlWriter()->queue( writeSigstatFooter( messagePart ) );

    }
    else if ( !hideErrors ) {
      QString txt;
      txt = "<hr><b><h2>";
      txt.append( i18n( "The crypto engine returned no cleartext data." ) );
      txt.append( "</h2></b>" );
      txt.append( "<br/>&nbsp;<br/>" );
      txt.append( i18n( "Status: " ) );
      if ( !messagePart.status.isEmpty() ) {
        txt.append( "<i>" );
        txt.append( messagePart.status );
        txt.append( "</i>" );
      }
      else
        txt.append( i18nc("Status of message unknown.","(unknown)") );
      if ( mHtmlWriter )
        htmlWriter()->queue(txt);
    }
  }
  else {
    if ( mHtmlWriter ) {
      if ( !cryptProto ) {
        QString errorMsg;
        switch ( cryptPlugError ) {
        case NOT_INITIALIZED:
          errorMsg = i18n( "Crypto plug-in \"%1\" is not initialized.",
                        cryptPlugLibName );
          break;
        case CANT_VERIFY_SIGNATURES:
          errorMsg = i18n( "Crypto plug-in \"%1\" cannot verify signatures.",
                        cryptPlugLibName );
          break;
        case NO_PLUGIN:
          if ( cryptPlugDisplayName.isEmpty() )
            errorMsg = i18n( "No appropriate crypto plug-in was found." );
          else
            errorMsg = i18nc( "%1 is either 'OpenPGP' or 'S/MIME'",
                              "No %1 plug-in was found.",
                            cryptPlugDisplayName );
          break;
        }
        messagePart.errorText = i18n( "The message is signed, but the "
                                      "validity of the signature cannot be "
                                      "verified.<br />"
                                      "Reason: %1",
                                  errorMsg );
      }

      htmlWriter()->queue( writeSigstatHeader( messagePart,
                                               cryptProto,
                                               fromAddress ) );
    }

    ObjectTreeParser otp( mSource, mNodeHelper, cryptProto, true );
    otp.parseObjectTree( data );
    mRawReplyString += otp.rawReplyString();
    mTextualContent += otp.textualContent();
    if ( !otp.textualContentCharset().isEmpty() )
      mTextualContentCharset = otp.textualContentCharset();

    if ( mHtmlWriter )
      htmlWriter()->queue( writeSigstatFooter( messagePart ) );
  }
#ifdef DEBUG_SIGNATURE
  kDebug() << "done, returning" << ( bIsOpaqueSigned ? "TRUE" : "FALSE" );
#endif
  kDebug() << "DECRYPTED" << data;
  return bIsOpaqueSigned;
}

void ObjectTreeParser::writeDeferredDecryptionBlock()
{
  const QString iconName = KIconLoader::global()->iconPath( "document-decrypt",
                                                            KIconLoader::Small );
  const QString decryptedData = "<div style=\"font-size:large; text-align:center;"
        "padding-top:20pt;\">"
        + i18n("This message is encrypted.")
        + "</div>"
        "<div style=\"text-align:center; padding-bottom:20pt;\">"
        "<a href=\"kmail:decryptMessage\">"
        "<img src=\"" + iconName.toUtf8() + "\"/>"
        + i18n("Decrypt Message")
        + "</a></div>";
  PartMetaData messagePart;
  messagePart.isDecryptable = true;
  messagePart.isEncrypted = true;
  messagePart.isSigned = false;
  mRawReplyString += decryptedData.toUtf8();

  if ( mHtmlWriter ) { //TODO: check if this check should be here or at the beginning of the method
    htmlWriter()->queue( writeSigstatHeader( messagePart,
                                              cryptoProtocol(),
                                              QString() ) );
    htmlWriter()->queue( decryptedData );
    htmlWriter()->queue( writeSigstatFooter( messagePart ) );
  }
}


void ObjectTreeParser::writeDecryptionInProgressBlock()
{
  kDebug(5006) << k_funcinfo << endl;
  if ( !mHtmlWriter )
    return;
  // PENDING(marc) find an animated icon here:
  //const QString iconName = KGlobal::instance()->iconLoader()->iconPath( "decrypted", KIcon::Small );
  const QString decryptedData = i18n("Encrypted data not shown");
  PartMetaData messagePart;
  messagePart.isDecryptable = true;
  messagePart.isEncrypted = true;
  messagePart.isSigned = false;
  messagePart.inProgress = true;
  htmlWriter()->queue( writeSigstatHeader( messagePart,
                                            cryptoProtocol(),
                                            QString() ) );
  //htmlWriter()->queue( decryptedData );
  htmlWriter()->queue( writeSigstatFooter( messagePart ) );
}

bool ObjectTreeParser::okDecryptMIME( KMime::Content& data,
                                    QByteArray& decryptedData,
                                    bool& signatureFound,
                                    std::vector<GpgME::Signature> &signatures,
                                    bool showWarning,
                                    bool& passphraseError,
                                    bool& actuallyEncrypted,
                                    bool& decryptionStarted,
                                    PartMetaData &partMetaData )
{
  passphraseError = false;
  decryptionStarted = false;
  partMetaData.errorText.clear();
  partMetaData.auditLogError = GpgME::Error();
  partMetaData.auditLog.clear();
  bool bDecryptionOk = false;
  enum { NO_PLUGIN, NOT_INITIALIZED, CANT_DECRYPT }
    cryptPlugError = NO_PLUGIN;

  const Kleo::CryptoBackend::Protocol* cryptProto = cryptoProtocol();

  QString cryptPlugLibName;
  if ( cryptProto )
    cryptPlugLibName = cryptProto->name();

  assert( mSource->decryptMessage() );

  const QString errorMsg = i18n( "Could not decrypt the data." );
  if ( cryptProto /*FIXME(Andras) port to akonadi
        && !kmkernel->contextMenuShown()*/ ) {
    QByteArray ciphertext = data.decodedContent();
  #ifdef MARCS_DEBUG
    QString cipherStr = QString::fromLatin1( ciphertext );
    bool cipherIsBinary = ( !cipherStr.contains("BEGIN ENCRYPTED MESSAGE", Qt::CaseInsensitive ) ) &&
                          ( !cipherStr.contains("BEGIN PGP ENCRYPTED MESSAGE", Qt::CaseInsensitive ) ) &&
                          ( !cipherStr.contains("BEGIN PGP MESSAGE", Qt::CaseInsensitive ) );

    dumpToFile( "dat_04_reader.encrypted", ciphertext.data(), ciphertext.size() );

    QString deb;
    deb =  "\n\nE N C R Y P T E D    D A T A = ";
    if ( cipherIsBinary )
      deb += "[binary data]";
    else {
      deb += "\"";
      deb += cipherStr;
      deb += "\"";
    }
    deb += "\n\n";
    kDebug() << deb;
  #endif


    kDebug() << "going to call CRYPTPLUG" << cryptPlugLibName;
    mSource->emitNoDrag(); // in case pineentry pops up, don't let kmheaders start a drag afterwards

    // Check whether the memento contains a result from last time:
    const DecryptVerifyBodyPartMemento * m
      = dynamic_cast<DecryptVerifyBodyPartMemento*>( mNodeHelper->bodyPartMemento( &data, "decryptverify" ) );
    if ( !m ) {
      Kleo::DecryptVerifyJob * job = cryptProto->decryptVerifyJob();
      if ( !job ) {
        cryptPlugError = CANT_DECRYPT;
        cryptProto = 0;
      } else {
        DecryptVerifyBodyPartMemento * newM
          = new DecryptVerifyBodyPartMemento( job, ciphertext );
        if ( allowAsync() ) {
          QObject::connect(newM, SIGNAL(update(MessageViewer::Viewer::UpdateMode)), mSource->sourceObject(), SLOT(update(MessageViewer::Viewer::UpdateMode)));
          if ( newM->start() ) {
            decryptionStarted = true;
            mHasPendingAsyncJobs = true;
          } else {
            m = newM;
          }
        } else {
          newM->exec();
          m = newM;
        }
        mNodeHelper->setBodyPartMemento( &data, "decryptverify", newM );
      }
    } else if ( m->isRunning() ) {
      decryptionStarted = true;
      mHasPendingAsyncJobs = true;
      m = 0;
    }

    if ( m ) {
      const QByteArray & plainText = m->plainText();
      const GpgME::DecryptionResult & decryptResult = m->decryptResult();
      const GpgME::VerificationResult & verifyResult = m->verifyResult();
      std::stringstream ss;
      ss << decryptResult << '\n' << verifyResult;
      kDebug() << ss.str().c_str();
      signatureFound = verifyResult.signatures().size() > 0;
      signatures = verifyResult.signatures();
      bDecryptionOk = !decryptResult.error();
      passphraseError =  decryptResult.error().isCanceled()
        || decryptResult.error().code() == GPG_ERR_NO_SECKEY;
      actuallyEncrypted = decryptResult.error().code() != GPG_ERR_NO_DATA;
      partMetaData.errorText = QString::fromLocal8Bit( decryptResult.error().asString() );
      partMetaData.auditLogError = m->auditLogError();
      partMetaData.auditLog = m->auditLogAsHtml();
      partMetaData.isEncrypted = actuallyEncrypted;
      if ( actuallyEncrypted && decryptResult.numRecipients() > 0 )
        partMetaData.keyId = decryptResult.recipient( 0 ).keyID();

      kDebug() << "ObjectTreeParser::decryptMIME: returned from CRYPTPLUG";
      if ( bDecryptionOk )
        decryptedData = plainText;
      else if ( mHtmlWriter && showWarning ) {
        decryptedData = "<div style=\"font-size:x-large; text-align:center;"
                        "padding:20pt;\">"
                      + errorMsg.toUtf8()
                      + "</div>";
        if ( !passphraseError )
          partMetaData.errorText = i18n("Crypto plug-in \"%1\" could not decrypt the data.", cryptPlugLibName )
                    + "<br />"
                    + i18n("Error: %1", partMetaData.errorText );
      }
    }
}

if ( !cryptProto ) {
  decryptedData = "<div style=\"text-align:center; padding:20pt;\">"
                + errorMsg.toUtf8()
                + "</div>";
  switch ( cryptPlugError ) {
  case NOT_INITIALIZED:
    partMetaData.errorText = i18n( "Crypto plug-in \"%1\" is not initialized.",
                      cryptPlugLibName );
    break;
  case CANT_DECRYPT:
    partMetaData.errorText = i18n( "Crypto plug-in \"%1\" cannot decrypt messages.",
                      cryptPlugLibName );
    break;
  case NO_PLUGIN:
    partMetaData.errorText = i18n( "No appropriate crypto plug-in was found." );
    break;
  }
} else if (/*FIXME(Andras) port to akonadi
            kmkernel->contextMenuShown()*/ false ) {
  // ### Workaround for bug 56693 (kmail freeze with the complete desktop
  // ### while pinentry-qt appears)
  QByteArray ciphertext( data.decodedContent() );
  QString cipherStr = QString::fromLatin1( ciphertext );
  bool cipherIsBinary = ( !cipherStr.contains("BEGIN ENCRYPTED MESSAGE", Qt::CaseInsensitive ) ) &&
                        ( !cipherStr.contains("BEGIN PGP ENCRYPTED MESSAGE", Qt::CaseInsensitive ) ) &&
                        ( !cipherStr.contains("BEGIN PGP MESSAGE", Qt::CaseInsensitive ) );
  if ( !cipherIsBinary ) {
    decryptedData = ciphertext;
  }
  else {
    decryptedData = "<div style=\"font-size:x-large; text-align:center;"
                    "padding:20pt;\">"
                  + errorMsg.toUtf8()
                  + "</div>";
  }
}

dumpToFile( "dat_05_reader.decrypted", decryptedData.data(), decryptedData.size() );

return bDecryptionOk;
}

//static
bool ObjectTreeParser::containsExternalReferences( const QString & str )
{
  int httpPos = str.indexOf( "\"http:", Qt::CaseInsensitive );
  int httpsPos = str.indexOf( "\"https:", Qt::CaseInsensitive );

  while ( httpPos >= 0 || httpsPos >= 0 ) {
    // pos = index of next occurrence of "http: or "https: whichever comes first
    int pos = ( httpPos < httpsPos )
              ? ( ( httpPos >= 0 ) ? httpPos : httpsPos )
              : ( ( httpsPos >= 0 ) ? httpsPos : httpPos );
    // look backwards for "href"
    if ( pos > 5 ) {
      int hrefPos = str.lastIndexOf( "href", pos - 5, Qt::CaseInsensitive );
      // if no 'href' is found or the distance between 'href' and '"http[s]:'
      // is larger than 7 (7 is the distance in 'href = "http[s]:') then
      // we assume that we have found an external reference
      if ( ( hrefPos == -1 ) || ( pos - hrefPos > 7 ) ) {

        // HTML messages created by KMail itself for now contain the following:
        // <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        // Make sure not to show an external references warning for this string
        int dtdPos = str.indexOf( "http://www.w3.org/TR/REC-html40/strict.dtd", pos + 1 );
        if ( dtdPos != ( pos + 1 ) )
          return true;
      }
    }
    // find next occurrence of "http: or "https:
    if ( pos == httpPos ) {
      httpPos = str.indexOf( "\"http:", httpPos + 6, Qt::CaseInsensitive );
    }
    else {
      httpsPos = str.indexOf( "\"https:", httpsPos + 7, Qt::CaseInsensitive );
    }
  }
  return false;
}

bool ObjectTreeParser::processTextHtmlSubtype( KMime::Content * curNode, ProcessResult & ) {
  const QByteArray partBody( curNode->decodedContent() );

  mRawReplyString = partBody;
  if ( curNode->topLevel()->textContent() == curNode ) {
    mTextualContent += curNode->decodedText();
    mTextualContentCharset = curNode->defaultCharset();
  }

  if ( !mHtmlWriter )
    return true;

  QString bodyText;
  if ( mSource->htmlMail() )
    bodyText = codecFor( curNode )->toUnicode( partBody );
  else
    bodyText = StringUtil::html2source( partBody );

  if ( curNode->topLevel()->textContent() == curNode  || attachmentStrategy()->defaultDisplay( curNode ) == AttachmentStrategy::Inline ||
        showOnlyOneMimePart() )
  {
    if ( mSource->htmlMail() ) {

      HTMLQuoteColorer colorer;
      for ( int i = 0; i < 2; i++ )
        colorer.setQuoteColor( i, cssHelper()->quoteColor( i ) );
      bodyText = colorer.process( bodyText );
      mNodeHelper->setNodeDisplayedEmbedded( curNode, true );

        // Strip <html>, <head>, and <body>, so we don't end up having those tags
      // twice, which confuses KHTML (especially with a signed
      // multipart/alternative message, the signature bars get rendered at the
      // wrong place)
      bodyText = bodyText.remove( "<html>", Qt::CaseInsensitive );
      bodyText = bodyText.remove( "<head>", Qt::CaseInsensitive );
      bodyText = bodyText.remove( "</head>", Qt::CaseInsensitive );
      QRegExp bodyRegExp( "<body.*>" ); //the body tag might have additional attributes,
      bodyRegExp.setMinimal( true );    //so make sure to match them as well
      bodyRegExp.setCaseSensitivity( Qt::CaseInsensitive );
      bodyText = bodyText.remove( bodyRegExp );

      // Strip </BODY> and </HTML> from end.
      // We must do this, or else the message will not be displayed correctly
      // because we have two </body> or </html> tags in the generated HTML.
      // It is IMHO enough to search only for </BODY> and put \0 there.
      int i = bodyText.lastIndexOf( "</body>", -1, Qt::CaseInsensitive );
      if ( 0 <= i )
        bodyText.truncate(i);
      else { // just in case - search for </html>
        i = bodyText.lastIndexOf( "</html>", -1, Qt::CaseInsensitive );
        if ( 0 <= i )
          bodyText.truncate(i);
      }

      // Show the "external references" warning (with possibility to load
      // external references only if loading external references is disabled
      // and the HTML code contains obvious external references). For
      // messages where the external references are obfuscated the user won't
      // have an easy way to load them but that shouldn't be a problem
      // because only spam contains obfuscated external references.
      if ( !mSource->htmlLoadExternal() &&
            containsExternalReferences( bodyText ) ) {
        htmlWriter()->queue( "<div class=\"htmlWarn\">\n" );
        htmlWriter()->queue( i18n("<b>Note:</b> This HTML message may contain external "
                                  "references to images etc. For security/privacy reasons "
                                  "external references are not loaded. If you trust the "
                                  "sender of this message then you can load the external "
                                  "references for this message "
                                  "<a href=\"kmail:loadExternal\">by clicking here</a>.") );
        htmlWriter()->queue( "</div><br><br>" );
      }
    } else {
      htmlWriter()->queue( "<div class=\"htmlWarn\">\n" );
      htmlWriter()->queue( i18n("<b>Note:</b> This is an HTML message. For "
                                "security reasons, only the raw HTML code "
                                "is shown. If you trust the sender of this "
                                "message then you can activate formatted "
                                "HTML display for this message "
                                "<a href=\"kmail:showHTML\">by clicking here</a>.") );
      htmlWriter()->queue( "</div><br><br>" );
    }
    // Make sure the body is relative, so that nothing is painted over above "Note: ..."
    // if a malicious message uses absolute positioning. #137643
    htmlWriter()->queue( "<div style=\"position: relative\">\n" );
    htmlWriter()->queue( bodyText );
    htmlWriter()->queue( "</div>\n" );
    mSource->setHtmlMode( true );
    return true;
  }
  return false;
}

bool ObjectTreeParser::isMailmanMessage( KMime::Content * curNode )
{
  if ( !curNode || curNode->head().isEmpty() )
    return false;
  if ( curNode->hasHeader("X-Mailman-Version") )
    return true;
  if ( curNode->hasHeader("X-Mailer") ) {
      KMime::Headers::Base *header = curNode->headerByType("X-Mailer");
      if ( header->asUnicodeString().contains("MAILMAN", Qt::CaseInsensitive ) )
        return true;
  }
  return false;
}

bool ObjectTreeParser::processMailmanMessage( KMime::Content* curNode ) {
  const QString str = QString::fromLatin1( curNode->decodedContent() );

  //###
  const QLatin1String delim1( "--__--__--\n\nMessage:" );
  const QLatin1String delim2( "--__--__--\r\n\r\nMessage:" );
  const QLatin1String delimZ2( "--__--__--\n\n_____________" );
  const QLatin1String delimZ1( "--__--__--\r\n\r\n_____________" );
  QString partStr, digestHeaderStr;
  int thisDelim = str.indexOf( delim1, Qt::CaseInsensitive );
  if ( thisDelim == -1 ) {
    thisDelim = str.indexOf( delim2, Qt::CaseInsensitive );
  }
  if ( thisDelim == -1 ) {
    return false;
  }

  int nextDelim = str.indexOf( delim1, thisDelim+1, Qt::CaseInsensitive );
  if ( -1 == nextDelim ) {
    nextDelim = str.indexOf( delim2, thisDelim+1, Qt::CaseInsensitive );
  }
  if ( -1 == nextDelim ) {
    nextDelim = str.indexOf( delimZ1, thisDelim+1, Qt::CaseInsensitive );
  }
  if ( -1 == nextDelim ) {
    nextDelim = str.indexOf( delimZ2, thisDelim+1, Qt::CaseInsensitive );
  }
  if ( nextDelim < 0) {
    return false;
  }

  //if ( curNode->mRoot )
  //  curNode = curNode->mRoot;

  // at least one message found: build a mime tree
  digestHeaderStr = "Content-Type: text/plain\nContent-Description: digest header\n\n";
  digestHeaderStr += str.mid( 0, thisDelim );
  insertAndParseNewChildNode( *curNode,
                              digestHeaderStr.toLatin1(),
                              "Digest Header", true );
  //mReader->queueHtml("<br><hr><br>");
  // temporarily change curent node's Content-Type
  // to get our embedded RfC822 messages properly inserted
  curNode->contentType()->setMimeType( "multipart/digest" );
  while( -1 < nextDelim ){
    int thisEoL = str.indexOf("\nMessage:", thisDelim, Qt::CaseInsensitive );
    if ( -1 < thisEoL )
      thisDelim = thisEoL+1;
    else{
      thisEoL = str.indexOf("\n_____________", thisDelim, Qt::CaseInsensitive );
      if ( -1 < thisEoL )
        thisDelim = thisEoL+1;
    }
    thisEoL = str.indexOf( '\n', thisDelim );
    if ( -1 < thisEoL )
      thisDelim = thisEoL+1;
    else
      thisDelim = thisDelim+1;
    //while( thisDelim < cstr.size() && '\n' == cstr[thisDelim] )
    //  ++thisDelim;

    partStr = "Content-Type: message/rfc822\nContent-Description: embedded message\n\n";
    partStr += str.mid( thisDelim, nextDelim-thisDelim );
    QString subject = QString::fromLatin1("embedded message");
    QString subSearch = QString::fromLatin1("\nSubject:");
    int subPos = partStr.indexOf(subSearch, 0, Qt::CaseInsensitive );
    if ( -1 < subPos ){
      subject = partStr.mid(subPos+subSearch.length());
      thisEoL = subject.indexOf('\n');
      if ( -1 < thisEoL )
        subject.truncate( thisEoL );
    }
    kDebug() << "        embedded message found: \"" << subject;
    insertAndParseNewChildNode( *curNode,
                                partStr.toLatin1(),
                                subject.toLatin1(), true );
    //mReader->queueHtml("<br><hr><br>");
    thisDelim = nextDelim+1;
    nextDelim = str.indexOf(delim1, thisDelim, Qt::CaseInsensitive );
    if ( -1 == nextDelim )
      nextDelim = str.indexOf(delim2, thisDelim, Qt::CaseInsensitive);
    if ( -1 == nextDelim )
      nextDelim = str.indexOf(delimZ1, thisDelim, Qt::CaseInsensitive);
    if ( -1 == nextDelim )
      nextDelim = str.indexOf(delimZ2, thisDelim, Qt::CaseInsensitive);
  }
  // reset curent node's Content-Type
  curNode->contentType()->setMimeType( "text/plain" );
  int thisEoL = str.indexOf( "_____________", thisDelim );
  if ( -1 < thisEoL ){
    thisDelim = thisEoL;
    thisEoL = str.indexOf( '\n', thisDelim );
    if ( -1 < thisEoL )
      thisDelim = thisEoL+1;
  }
  else
    thisDelim = thisDelim+1;
  partStr = "Content-Type: text/plain\nContent-Description: digest footer\n\n";
  partStr += str.mid( thisDelim );
  insertAndParseNewChildNode( *curNode,
                              partStr.toLatin1(),
                              "Digest Footer", true );
  return true;
}

bool ObjectTreeParser::processTextPlainSubtype( KMime::Content *curNode, ProcessResult & result )
{
  bool isFirstTextPart = (curNode->topLevel()->textContent() == curNode);

  if ( !mHtmlWriter ) {
    mRawReplyString = curNode->decodedContent();
    if ( isFirstTextPart ) {
      mTextualContent += curNode->decodedText();
      mTextualContentCharset = curNode->defaultCharset();
    }
    return true;
  }

  if ( !isFirstTextPart && attachmentStrategy()->defaultDisplay( curNode ) != AttachmentStrategy::Inline &&
        !showOnlyOneMimePart() )
    return false;

  mRawReplyString = curNode->decodedContent();
  if ( isFirstTextPart ) {
    mTextualContent += curNode->decodedText();
    mTextualContentCharset = curNode->defaultCharset();
  }

  QString label = NodeHelper::fileName( curNode );

  const bool bDrawFrame = !isFirstTextPart
                        && !showOnlyOneMimePart()
                        && !label.isEmpty();
  if ( bDrawFrame ) {
    label = StringUtil::quoteHtmlChars( label, true );

    const QString comment =
      StringUtil::quoteHtmlChars( curNode->contentDescription()->asUnicodeString(), true );

    const QString fileName;
    mNodeHelper->writeNodeToTempFile( curNode );
    const QString dir = QApplication::isRightToLeft() ? "rtl" : "ltr" ;

    QString htmlStr = "<table cellspacing=\"1\" class=\"textAtm\">"
                "<tr class=\"textAtmH\"><td dir=\"" + dir + "\">";
    if ( !fileName.isEmpty() )
      htmlStr += "<a href=\"" +NodeHelper::asHREF( curNode, "body" ) + "\">"
                  + label + "</a>";
    else
      htmlStr += label;
    if ( !comment.isEmpty() )
      htmlStr += "<br>" + comment;
    htmlStr += "</td></tr><tr class=\"textAtmB\"><td>";

    htmlWriter()->queue( htmlStr );
  }
  // process old style not-multipart Mailman messages to
  // enable verification of the embedded messages' signatures
  if ( !isMailmanMessage( curNode ) ||
        !processMailmanMessage( curNode ) ) {
      writeBodyString( mRawReplyString, static_cast<KMime::Message*>(curNode->topLevel())->from()->asUnicodeString(),
                      codecFor( curNode ), result, !bDrawFrame );
      mNodeHelper->setNodeDisplayedEmbedded( curNode, true );
  }
  if ( bDrawFrame )
    htmlWriter()->queue( "</td></tr></table>" );

  return true;
}

void ObjectTreeParser::stdChildHandling( KMime::Content * child ) {
  if ( !child )
    return;

  ObjectTreeParser otp( *this );
  otp.setShowOnlyOneMimePart( false );
  otp.parseObjectTree( child );
  mRawReplyString += otp.rawReplyString();
  mTextualContent += otp.textualContent();
  if ( !otp.textualContentCharset().isEmpty() )
    mTextualContentCharset = otp.textualContentCharset();
}

bool ObjectTreeParser::processMultiPartMixedSubtype( KMime::Content * node, ProcessResult & )
{
  KMime::Content * child = NodeHelper::firstChild( node );
  if ( !child )
    return false;

  // normal treatment of the parts in the mp/mixed container
  stdChildHandling( child );
  return true;
}

bool ObjectTreeParser::processMultiPartAlternativeSubtype( KMime::Content * node, ProcessResult & )
{
  KMime::Content * child = NodeHelper::firstChild( node );
  if ( !child )
    return false;

  KMime::Content* dataHtml = findType( child, "text/html", false, true );
  if ( !dataHtml ) {
    // If we didn't find the HTML part as the first child of the multipart/alternative, it might
    // be that this is a HTML message with images, and text/plain and multipart/related are the
    // immediate children of this multipart/alternative node.
    // In this case, the HTML node is a child of multipart/related.
    dataHtml = findType( child, "multipart/related", false, true );

    // Still not found? Stupid apple mail actually puts the attachments inside of the
    // multipart/alternative, which is wrong. Therefore we also have to look for multipart/mixed
    // here.
    // Do this only when prefering HTML mail, though, since otherwise the attachments are hidden
    // when displaying plain text.
    if ( !dataHtml && mSource->htmlMail() ) {
      dataHtml = findType( child, "multipart/mixed", false, true );
    }

  }
  KMime::Content* dataPlain = findType( child, "text/plain", false, true );

  if ( ( mSource->htmlMail() && dataHtml) ||
        (dataHtml && dataPlain && dataPlain->body().isEmpty()) ) {
    if ( dataPlain )
      mNodeHelper->setNodeProcessed( dataPlain, false);
    stdChildHandling( dataHtml );
    return true;
  }

  if ( !mHtmlWriter || (!mSource->htmlMail() && dataPlain) ) {
    mNodeHelper->setNodeProcessed( dataHtml, false );
    stdChildHandling( dataPlain );
    return true;
  }

  stdChildHandling( child );
  return true;
}

bool ObjectTreeParser::processMultiPartDigestSubtype( KMime::Content * node, ProcessResult & result ) {
  return processMultiPartMixedSubtype( node, result );
}

bool ObjectTreeParser::processMultiPartParallelSubtype( KMime::Content * node, ProcessResult & result ) {
  return processMultiPartMixedSubtype( node, result );
}

bool ObjectTreeParser::processMultiPartSignedSubtype( KMime::Content * node, ProcessResult & )
{
  KMime::Content * child = NodeHelper::firstChild( node );
  if ( node->contents().size() != 2 ) {
    kDebug() << "mulitpart/signed must have exactly two child parts!" << endl
              << "processing as multipart/mixed";
    if ( child )
      stdChildHandling( child );
    return child;
  }

  KMime::Content * signedData = child;
  assert( signedData );

  KMime::Content * signature = node->contents().at(1);
  assert( signature );

  mNodeHelper->setNodeProcessed( signature, true);

  if ( !includeSignatures() ) {
    stdChildHandling( signedData );
    return true;
  }

  QString protocolContentType = node->contentType()->parameter( "protocol" ).toLower();
  const QString signatureContentType = signature->contentType()->mimeType().toLower();
  if ( protocolContentType.isEmpty() ) {
    kWarning() << "Message doesn't set the protocol for the multipart/signed content-type, "
                  "using content-type of the signature:" << signatureContentType;
    protocolContentType = signatureContentType;
  }

  const Kleo::CryptoBackend::Protocol *protocol = 0;
  if ( protocolContentType == "application/pkcs7-signature" ||
        protocolContentType == "application/x-pkcs7-signature" )
    protocol = Kleo::CryptoBackendFactory::instance()->smime();
  else if ( protocolContentType == "application/pgp-signature" ||
            protocolContentType == "application/x-pgp-signature" )
    protocol = Kleo::CryptoBackendFactory::instance()->openpgp();

  if ( !protocol ) {
    mNodeHelper->setNodeProcessed( signature, true );
    stdChildHandling( signedData );
    return true;
  }

  CryptoProtocolSaver saver( this, protocol );
  mNodeHelper->setSignatureState( node, KMMsgFullySigned);

  writeOpaqueOrMultipartSignedData( signedData, *signature,
                                    static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString() );
  return true;
}

bool ObjectTreeParser::processMultiPartEncryptedSubtype( KMime::Content * node, ProcessResult & result )
{
  KMime::Content * child = NodeHelper::firstChild( node );
  if ( !child )
    return false;

  if ( keepEncryptions() ) {
    mNodeHelper->setEncryptionState( node, KMMsgFullyEncrypted );
    const QByteArray cstr = node->decodedContent();
    if ( mHtmlWriter )
      writeBodyString( cstr, static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString(),
                        codecFor( node ), result, false );
    mRawReplyString += cstr;
    return true;
  }

  const Kleo::CryptoBackend::Protocol * useThisCryptProto = 0;

  /*
    ATTENTION: This code is to be replaced by the new 'auto-detect' feature. --------------------------------------
  */
  KMime::Content* data = findType( child, "application/octet-stream", false, true );
  if ( data ) {
    useThisCryptProto = Kleo::CryptoBackendFactory::instance()->openpgp();
  }
  if ( !data ) {
    data = findType( child, "application/pkcs7-mime", false, true );
    if ( data ) {
      useThisCryptProto = Kleo::CryptoBackendFactory::instance()->smime();
    }
  }
  /*
    ---------------------------------------------------------------------------------------------------------------
  */

  if ( !data ) {
    stdChildHandling( child );
    return true;
  }

  CryptoProtocolSaver cpws( this, useThisCryptProto );

  KMime::Content * dataChild = NodeHelper::firstChild( data );
  if ( dataChild ) {
    stdChildHandling( dataChild );
    return true;
  }

  mNodeHelper->setEncryptionState( node, KMMsgFullyEncrypted );

  if ( !mSource->decryptMessage() ) {
    writeDeferredDecryptionBlock();
    mNodeHelper->setNodeProcessed( data, false );// Set the data node to done to prevent it from being processed
    return true;
  }

  PartMetaData messagePart;
  QByteArray decryptedData;
  bool signatureFound;
  std::vector<GpgME::Signature> signatures;
  bool passphraseError;
  bool actuallyEncrypted = true;
  bool decryptionStarted;

  bool bOkDecrypt = okDecryptMIME( *data,
                                    decryptedData,
                                    signatureFound,
                                    signatures,
                                    true,
                                    passphraseError,
                                    actuallyEncrypted,
                                    decryptionStarted,
                                    messagePart );

  if ( decryptionStarted ) {
    writeDecryptionInProgressBlock();
    return true;
  }

  // paint the frame
  if ( mHtmlWriter ) {
    messagePart.isDecryptable = bOkDecrypt;
    messagePart.isEncrypted = true;
    messagePart.isSigned = false;
    htmlWriter()->queue( writeSigstatHeader( messagePart,
                                              cryptoProtocol(),
                                              static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString() ) );
  }

  if ( bOkDecrypt ) {
    // Note: Multipart/Encrypted might also be signed
    //       without encapsulating a nicely formatted
    //       ~~~~~~~                 Multipart/Signed part.
    //                               (see RFC 3156 --> 6.2)
    // In this case we paint a _2nd_ frame inside the
    // encryption frame, but we do _not_ show a respective
    // encapsulated MIME part in the Mime Tree Viewer
    // since we do want to show the _true_ structure of the
    // message there - not the structure that the sender's
    // MUA 'should' have sent.  :-D       (khz, 12.09.2002)
    //
    if ( signatureFound ) {
      writeOpaqueOrMultipartSignedData( 0,
                                        *node,
                                        static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString(),
                                        false,
                                        &decryptedData,
                                        signatures,
                                        false );
      mNodeHelper->setSignatureState( node, KMMsgFullySigned);
    } else {
      decryptedData = KMime::CRLFtoLF( decryptedData ); //KMime works with LF only inside insertAndParseNewChildNode
      insertAndParseNewChildNode( *node,
                                  decryptedData.constData(),
                                  "encrypted data" );
    }
  } else {
    mRawReplyString += decryptedData;
    if ( mHtmlWriter ) {
      // print the error message that was returned in decryptedData
      // (utf8-encoded)
      htmlWriter()->queue( QString::fromUtf8( decryptedData.data() ) );
    }
  }

  if ( mHtmlWriter )
    htmlWriter()->queue( writeSigstatFooter( messagePart ) );
  mNodeHelper->setNodeProcessed( data, false ); // Set the data node to done to prevent it from being processed
  mNodeHelper->setPartMetaData( node, messagePart );
  return true;
}


bool ObjectTreeParser::processMessageRfc822Subtype( KMime::Content * node, ProcessResult & )
{
  if ( mHtmlWriter
        && !attachmentStrategy()->inlineNestedMessages()
        && !showOnlyOneMimePart() )
    return false;

  KMime::Content * child = NodeHelper::firstChild( node );
  if ( child ) {
    ObjectTreeParser otp( mSource, mNodeHelper, cryptoProtocol() );
    otp.parseObjectTree( child );
    mRawReplyString += otp.rawReplyString();
    mTextualContent += otp.textualContent();
    if ( !otp.textualContentCharset().isEmpty() )
      mTextualContentCharset = otp.textualContentCharset();
    return true;
  }
  // paint the frame
  PartMetaData messagePart;
  if ( mHtmlWriter ) {
    messagePart.isEncrypted = false;
    messagePart.isSigned = false;
    messagePart.isEncapsulatedRfc822Message = true;
    QString filename = mNodeHelper->writeNodeToTempFile( node );
    htmlWriter()->queue( writeSigstatHeader( messagePart,
                                              cryptoProtocol(),
                                              static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString(),
                                              node ) );
  }
  QByteArray rfc822messageStr( node->decodedContent() );
  // display the headers of the encapsulated message
  KMime::Message* rfc822message = new KMime::Message();
  rfc822message->setContent( rfc822messageStr );
  rfc822message->parse();
  static_cast<KMime::Message*>(node->topLevel())->from()->from7BitString( rfc822message->from()->as7BitString() );
  if ( mHtmlWriter )
    htmlWriter()->queue( mSource->createMessageHeader( rfc822message ) );
  delete rfc822message;
    //mReader->parseMsgHeader( &rfc822message );
  // display the body of the encapsulated message
  insertAndParseNewChildNode( *node,
                              rfc822messageStr.constData(),
                              "encapsulated message", false /*append*/,
                              false /*add to textual content*/  );
  mNodeHelper->setNodeDisplayedEmbedded( node, true );

  if ( mHtmlWriter )
    htmlWriter()->queue( writeSigstatFooter( messagePart ) );
  mNodeHelper->setPartMetaData( node, messagePart );
  return true;
}


bool ObjectTreeParser::processApplicationOctetStreamSubtype( KMime::Content * node, ProcessResult & result )
{
  KMime::Content * child = NodeHelper::firstChild( node );
  if ( child ) {
    ObjectTreeParser otp( mSource, mNodeHelper, cryptoProtocol() );
    otp.parseObjectTree( child );
    mRawReplyString += otp.rawReplyString();
    mTextualContent += otp.textualContent();
    if ( !otp.textualContentCharset().isEmpty() )
      mTextualContentCharset = otp.textualContentCharset();
    return true;
  }

  const Kleo::CryptoBackend::Protocol* oldUseThisCryptPlug = cryptoProtocol();
  if (    node->parent()
          && node->parent()->contentType()->mimeType() == "multipart/encrypted" ) {
    mNodeHelper->setEncryptionState( node, KMMsgFullyEncrypted );
    if ( keepEncryptions() ) {
      const QByteArray cstr = node->decodedContent();
      if ( mHtmlWriter )
        writeBodyString( cstr, static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString(),
                          codecFor( node ), result, false );
      mRawReplyString += cstr;
    } else if ( !mSource->decryptMessage() ) {
      writeDeferredDecryptionBlock();
    } else {
      /*
        ATTENTION: This code is to be replaced by the planned 'auto-detect' feature.
      */
      PartMetaData messagePart;
      setCryptoProtocol( Kleo::CryptoBackendFactory::instance()->openpgp() );
      QByteArray decryptedData;
      bool signatureFound;
      std::vector<GpgME::Signature> signatures;
      bool passphraseError;
      bool actuallyEncrypted = true;
      bool decryptionStarted;

      bool bOkDecrypt = okDecryptMIME( *node,
                                        decryptedData,
                                        signatureFound,
                                        signatures,
                                        true,
                                        passphraseError,
                                        actuallyEncrypted,
                                        decryptionStarted,
                                        messagePart );

      if ( decryptionStarted ) {
        writeDecryptionInProgressBlock();
        return true;
      }

      // paint the frame
      if ( mHtmlWriter ) {
        messagePart.isDecryptable = bOkDecrypt;
        messagePart.isEncrypted = true;
        messagePart.isSigned = false;
        htmlWriter()->queue( writeSigstatHeader( messagePart,
                                                  cryptoProtocol(),
                                                  static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString() ) );
      }

      if ( bOkDecrypt ) {
        // fixing the missing attachments bug #1090-b
        insertAndParseNewChildNode( *node,
                                    decryptedData.constData(),
                                    "encrypted data" );
      } else {
        mRawReplyString += decryptedData;
        if ( mHtmlWriter ) {
          // print the error message that was returned in decryptedData
          // (utf8-encoded)
          htmlWriter()->queue( QString::fromUtf8( decryptedData.data() ) );
        }
      }

      if ( mHtmlWriter )
        htmlWriter()->queue( writeSigstatFooter( messagePart ) );
      mNodeHelper->setPartMetaData( node, messagePart );
    }
    return true;
  }
  setCryptoProtocol( oldUseThisCryptPlug );
  return false;
}

bool ObjectTreeParser::processApplicationPkcs7MimeSubtype( KMime::Content * node, ProcessResult & result )
{
  KMime::Content * child = NodeHelper::firstChild( node );
  if ( child ) {
    ObjectTreeParser otp( mSource, mNodeHelper, cryptoProtocol() );
    otp.parseObjectTree( child );
    mRawReplyString += otp.rawReplyString();
    mTextualContent += otp.textualContent();
    if ( !otp.textualContentCharset().isEmpty() )
      mTextualContentCharset = otp.textualContentCharset();
    return true;
  }

  if ( node->head().isEmpty() )
    return false;

  const Kleo::CryptoBackend::Protocol * smimeCrypto = Kleo::CryptoBackendFactory::instance()->smime();

  const QString smimeType = node->contentType()->parameter("smime-type").toLower();

  if ( smimeType == "certs-only" ) {
    result.setNeverDisplayInline( true );
    if ( !smimeCrypto || !mHtmlWriter )
      return false;

    const KConfigGroup reader( Global::instance()->config(), "Reader" );
    if ( !reader.readEntry( "AutoImportKeys", false ) )
      return false;

    const QByteArray certData = node->decodedContent();

    Kleo::ImportJob *import = smimeCrypto->importJob();
    KleoJobExecutor executor;
    const GpgME::ImportResult res = executor.exec( import, certData );
    if ( res.error() ) {
      htmlWriter()->queue( i18n( "Sorry, certificate could not be imported.<br />"
                                  "Reason: %1", QString::fromLocal8Bit( res.error().asString() ) ) );
      return true;
    }

    const int nImp = res.numImported();
    const int nUnc = res.numUnchanged();
    const int nSKImp = res.numSecretKeysImported();
    const int nSKUnc = res.numSecretKeysUnchanged();
    if ( !nImp && !nSKImp && !nUnc && !nSKUnc ) {
      htmlWriter()->queue( i18n( "Sorry, no certificates were found in this message." ) );
      return true;
    }
    QString comment = "<b>" + i18n( "Certificate import status:" ) + "</b><br>&nbsp;<br>";
    if ( nImp )
      comment += i18np( "1 new certificate was imported.",
                        "%1 new certificates were imported.", nImp ) + "<br>";
    if ( nUnc )
      comment += i18np( "1 certificate was unchanged.",
                        "%1 certificates were unchanged.", nUnc ) + "<br>";
    if ( nSKImp )
      comment += i18np( "1 new secret key was imported.",
                        "%1 new secret keys were imported.", nSKImp ) + "<br>";
    if ( nSKUnc )
      comment += i18np( "1 secret key was unchanged.",
                        "%1 secret keys were unchanged.", nSKUnc ) + "<br>";
    comment += "&nbsp;<br>";
    htmlWriter()->queue( comment );
    if ( !nImp && !nSKImp ) {
      htmlWriter()->queue( "<hr>" );
      return true;
    }
    const std::vector<GpgME::Import> imports = res.imports();
    if ( imports.empty() ) {
      htmlWriter()->queue( i18n( "Sorry, no details on certificate import available." ) + "<hr>" );
      return true;
    }
    htmlWriter()->queue( "<b>" + i18n( "Certificate import details:" ) + "</b><br>" );
    for ( std::vector<GpgME::Import>::const_iterator it = imports.begin() ; it != imports.end() ; ++it ) {
      if ( (*it).error() ) {
        htmlWriter()->queue( i18nc( "Certificate import failed.", "Failed: %1 (%2)", (*it).fingerprint(),
                                QString::fromLocal8Bit( (*it).error().asString() ) ) );
      } else if ( (*it).status() & ~GpgME::Import::ContainedSecretKey ) {
        if ( (*it).status() & GpgME::Import::ContainedSecretKey ) {
          htmlWriter()->queue( i18n( "New or changed: %1 (secret key available)", (*it).fingerprint() ) );
        } else {
          htmlWriter()->queue( i18n( "New or changed: %1", (*it).fingerprint() ) );
        }
      }
      htmlWriter()->queue( "<br>" );
    }

    htmlWriter()->queue( "<hr>" );
    return true;
  }

  if ( !smimeCrypto )
    return false;
  CryptoProtocolSaver cpws( this, smimeCrypto );

  bool isSigned      = smimeType == "signed-data";
  bool isEncrypted   = smimeType == "enveloped-data";

  // Analyze "signTestNode" node to find/verify a signature.
  // If zero this verification was successfully done after
  // decrypting via recursion by insertAndParseNewChildNode().
  KMime::Content* signTestNode = isEncrypted ? 0 : node;


  // We try decrypting the content
  // if we either *know* that it is an encrypted message part
  // or there is neither signed nor encrypted parameter.
  if ( !isSigned ) {
    if ( isEncrypted )
      kDebug() << "pkcs7 mime     ==      S/MIME TYPE: enveloped (encrypted) data";
    else
      kDebug() << "pkcs7 mime  -  type unknown  -  enveloped (encrypted) data ?";
    QByteArray decryptedData;
    PartMetaData messagePart;
    messagePart.isEncrypted = true;
    messagePart.isSigned = false;
    bool signatureFound;
    std::vector<GpgME::Signature> signatures;
    bool passphraseError;
    bool actuallyEncrypted = true;
    bool decryptionStarted;

    if ( !mSource->decryptMessage() ) {
      writeDeferredDecryptionBlock();
      isEncrypted = true;
    } else if ( okDecryptMIME( *node,
                        decryptedData,
                        signatureFound,
                        signatures,
                        false,
                        passphraseError,
                        actuallyEncrypted,
                        decryptionStarted,
                        messagePart ) ) {
      kDebug() << "pkcs7 mime  -  encryption found  -  enveloped (encrypted) data !";
      isEncrypted = true;
      mNodeHelper->setEncryptionState( node, KMMsgFullyEncrypted );
      signTestNode = 0;
      if ( decryptionStarted ) {
        writeDecryptionInProgressBlock();
      } else {
        // paint the frame
        messagePart.isDecryptable = true;
        if ( mHtmlWriter )
          htmlWriter()->queue( writeSigstatHeader( messagePart,
                                                    cryptoProtocol(),
                                                    static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString() ) );
        insertAndParseNewChildNode( *node,
                                    decryptedData.constData(),
                                    "encrypted data" );
        if ( mHtmlWriter )
          htmlWriter()->queue( writeSigstatFooter( messagePart ) );
      }
    } else {
        // decryption failed, which could be because the part was encrypted but
        // decryption failed, or because we didn't know if it was encrypted, tried,
        // and failed. If the message was not actually encrypted, we continue
        // assuming it's signed
      if ( passphraseError || ( smimeType.isEmpty() && actuallyEncrypted ) ) {
        isEncrypted = true;
        signTestNode = 0;
      }

      if ( isEncrypted ) {
        kDebug() << "pkcs7 mime  -  ERROR: COULD NOT DECRYPT enveloped data !";
        // paint the frame
        messagePart.isDecryptable = false;
        if ( mHtmlWriter ) {
          htmlWriter()->queue( writeSigstatHeader( messagePart,
                                                    cryptoProtocol(),
                                                    static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString() ) );
          assert( mSource->decryptMessage() ); // handled above
          writePartIcon( node );
          htmlWriter()->queue( writeSigstatFooter( messagePart ) );
        }
      } else {
        kDebug() << "pkcs7 mime  -  NO encryption found";
      }
    }
    if ( isEncrypted )
      mNodeHelper->setEncryptionState( node, KMMsgFullyEncrypted );
    mNodeHelper->setPartMetaData( node, messagePart );
  }

  // We now try signature verification if necessarry.
  if ( signTestNode ) {
    if ( isSigned )
      kDebug() << "pkcs7 mime     ==      S/MIME TYPE: opaque signed data";
    else
      kDebug() << "pkcs7 mime  -  type unknown  -  opaque signed data ?";

    bool sigFound = writeOpaqueOrMultipartSignedData( 0,
                                                      *signTestNode,
                                                      static_cast<KMime::Message*>(node->topLevel())->from()->asUnicodeString(),
                                                      true,
                                                      0,
                                                      std::vector<GpgME::Signature>(),
                                                      isEncrypted );
    if ( sigFound ) {
      if ( !isSigned ) {
        kDebug() << "pkcs7 mime  -  signature found  -  opaque signed data !";
        isSigned = true;
      }

      mNodeHelper->setSignatureState( signTestNode, KMMsgFullySigned );
      if ( signTestNode != node )
        mNodeHelper->setSignatureState( node, KMMsgFullySigned );
    } else {
      kDebug() << "pkcs7 mime  -  NO signature found   :-(";
    }
  }

  return isSigned || isEncrypted;
}

bool ObjectTreeParser::decryptChiasmus( const QByteArray& data, QByteArray& bodyDecoded, QString& errorText )
{
  const Kleo::CryptoBackend::Protocol * chiasmus =
    Kleo::CryptoBackendFactory::instance()->protocol( "Chiasmus" );
  Q_ASSERT( chiasmus );
  if ( !chiasmus )
    return false;

  const std::auto_ptr<Kleo::SpecialJob> listjob( chiasmus->specialJob( "x-obtain-keys", QMap<QString,QVariant>() ) );
  if ( !listjob.get() ) {
    errorText = i18n( "Chiasmus backend does not offer the "
                      "\"x-obtain-keys\" function. Please report this bug." );
    return false;
  }

  if ( listjob->exec() ) {
    errorText = i18n( "Chiasmus Backend Error" );
    return false;
  }

  const QVariant result = listjob->property( "result" );
  if ( result.type() != QVariant::StringList ) {
    errorText = i18n( "Unexpected return value from Chiasmus backend: "
                      "The \"x-obtain-keys\" function did not return a "
                      "string list. Please report this bug." );
    return false;
  }

  const QStringList keys = result.toStringList();
  if ( keys.empty() ) {
    errorText = i18n( "No keys have been found. Please check that a "
                      "valid key path has been set in the Chiasmus "
                      "configuration." );
    return false;
  }

  mSource->emitNoDrag();
  /*FIXME(Andras) port to akonadi
  ChiasmusKeySelector selectorDlg( mReader, i18n( "Chiasmus Decryption Key Selection" ),
                                    keys, GlobalSettings::chiasmusDecryptionKey(),
                                    GlobalSettings::chiasmusDecryptionOptions() );
  if ( selectorDlg.exec() != KDialog::Accepted )
    return false;
  GlobalSettings::setChiasmusDecryptionOptions( selectorDlg.options() );
  GlobalSettings::setChiasmusDecryptionKey( selectorDlg.key() );
  assert( !GlobalSettings::chiasmusDecryptionKey().isEmpty() );
  */

  Kleo::SpecialJob * job = chiasmus->specialJob( "x-decrypt", QMap<QString,QVariant>() );
  if ( !job ) {
    errorText = i18n( "Chiasmus backend does not offer the "
                      "\"x-decrypt\" function. Please report this bug." );
    return false;
  }

  if ( !job->setProperty( "key", GlobalSettings::chiasmusDecryptionKey() ) ||
        !job->setProperty( "options", GlobalSettings::chiasmusDecryptionOptions() ) ||
        !job->setProperty( "input", data ) ) {
    errorText = i18n( "The \"x-decrypt\" function does not accept "
                      "the expected parameters. Please report this bug." );
    return false;
  }

  if ( job->exec() ) {
    errorText = i18n( "Chiasmus Decryption Error" );
    return false;
  }

  const QVariant resultData = job->property( "result" );
  if ( resultData.type() != QVariant::ByteArray ) {
    errorText = i18n( "Unexpected return value from Chiasmus backend: "
                      "The \"x-decrypt\" function did not return a "
                      "byte array. Please report this bug." );
    return false;
  }
  bodyDecoded = resultData.toByteArray();
  return true;
  }

  bool ObjectTreeParser::processApplicationChiasmusTextSubtype( KMime::Content * curNode, ProcessResult & result )
  {
  if ( !mHtmlWriter ) {
    mRawReplyString = curNode->decodedContent();
    mTextualContent += curNode->decodedText();
    mTextualContentCharset = curNode->defaultCharset();
    return true;
  }

  QByteArray decryptedBody;
  QString errorText;
  const QByteArray data = curNode->decodedContent();
  bool bOkDecrypt = decryptChiasmus( data, decryptedBody, errorText );
  PartMetaData messagePart;
  messagePart.isDecryptable = bOkDecrypt;
  messagePart.isEncrypted = true;
  messagePart.isSigned = false;
  messagePart.errorText = errorText;
  if ( mHtmlWriter )
    htmlWriter()->queue( writeSigstatHeader( messagePart,
                                              0, //cryptPlugWrapper(),
                                              static_cast<KMime::Message*>(curNode->topLevel())->from()->asUnicodeString() ) );
  const QByteArray body = bOkDecrypt ? decryptedBody : data;
  const QString chiasmusCharset = curNode->contentType()->parameter("chiasmus-charset");
  const QTextCodec* aCodec = chiasmusCharset.isEmpty() ? codecFor( curNode )
                              : NodeHelper::codecForName( chiasmusCharset.toAscii() );
  htmlWriter()->queue( quotedHTML( aCodec->toUnicode( body ), false /*decorate*/ ) );
  result.setInlineEncryptionState( KMMsgFullyEncrypted );
  if ( mHtmlWriter )
    htmlWriter()->queue( writeSigstatFooter( messagePart ) );
  mNodeHelper->setPartMetaData( curNode, messagePart );
  return true;
}

bool ObjectTreeParser::processApplicationMsTnefSubtype( KMime::Content *node, ProcessResult &result )
{
  Q_UNUSED( result );
  if ( !mHtmlWriter )
    return false;

  const QString fileName = mNodeHelper->writeNodeToTempFile( node );
  KTnef::KTNEFParser parser;
  if ( !parser.openFile( fileName ) || !parser.message()) {
    kDebug() << "Could not parse" << fileName;
    return false;
  }

  QList<KTnef::KTNEFAttach*> tnefatts = parser.message()->attachmentList();
  if ( tnefatts.isEmpty() ) {
    kDebug() << "No attachments found in" << fileName;
    return false;
  }

  if ( !showOnlyOneMimePart() ) {
    QString label = NodeHelper::fileName( node );
    label = StringUtil::quoteHtmlChars( label, true );
    const QString comment = StringUtil::quoteHtmlChars( node->contentDescription()->asUnicodeString(), true );
    const QString dir = QApplication::isRightToLeft() ? "rtl" : "ltr" ;

    QString htmlStr = "<table cellspacing=\"1\" class=\"textAtm\">"
                "<tr class=\"textAtmH\"><td dir=\"" + dir + "\">";
    if ( !fileName.isEmpty() )
    htmlStr += "<a href=\"" +NodeHelper::asHREF( node, "body" ) + "\">"
                + label + "</a>";
    else
      htmlStr += label;
    if ( !comment.isEmpty() )
      htmlStr += "<br>" + comment;
    htmlStr += "</td></tr><tr class=\"textAtmB\"><td>";
    htmlWriter()->queue( htmlStr );
  }

  for ( int i = 0; i < tnefatts.count(); ++i ) {
    KTnef::KTNEFAttach *att = tnefatts.at( i );
    QString label = att->displayName();
    if( label.isEmpty() )
      label = att->name();
    label = StringUtil::quoteHtmlChars( label, true );

    QString dir = mNodeHelper->createTempDir( "ktnef-" + QString::number( i ) );
    parser.extractFileTo( att->name(), dir );
    mNodeHelper->addTempFile( dir + QDir::separator() + att->name() );
    QString href = "file:" + KUrl::toPercentEncoding( dir + QDir::separator() + att->name() );

    KMimeType::Ptr mimeType = KMimeType::mimeType( att->mimeTag(), KMimeType::ResolveAliases );
    QString iconName = KIconLoader::global()->iconPath( mimeType->iconName(), KIconLoader::Desktop );

    htmlWriter()->queue( "<div><a href=\"" + href + "\"><img src=\"" +
                          iconName + "\" border=\"0\" style=\"max-width: 100%\">" + label +
                          "</a></div><br>" );
  }

  if ( !showOnlyOneMimePart() )
    htmlWriter()->queue( "</td></tr></table>" );

  return true;
}

void ObjectTreeParser::writeBodyString( const QByteArray & bodyString,
                                        const QString & fromAddress,
                                        const QTextCodec * codec,
                                        ProcessResult & result,
                                        bool decorate )
{
  if ( !mHtmlWriter )
    return;
  assert( codec );
  KMMsgSignatureState inlineSignatureState = result.inlineSignatureState();
  KMMsgEncryptionState inlineEncryptionState = result.inlineEncryptionState();
  writeBodyStr( bodyString, codec, fromAddress,
                inlineSignatureState, inlineEncryptionState, decorate );
  result.setInlineSignatureState( inlineSignatureState );
  result.setInlineEncryptionState( inlineEncryptionState );
}

void ObjectTreeParser::writePartIcon( KMime::Content * msgPart, bool inlineImage )
{
  if ( !mHtmlWriter || !msgPart )
    return;

  QString label = NodeHelper::fileName( msgPart );
  if ( label.isEmpty() )
    label = i18nc( "display name for an unnamed attachment", "Unnamed" );
  label = StringUtil::quoteHtmlChars( label, true );

  QString comment = msgPart->contentDescription()->asUnicodeString();
  comment = StringUtil::quoteHtmlChars( comment, true );
  if ( label == comment )
    comment.clear();

  QString fileName = mNodeHelper->writeNodeToTempFile( msgPart );
  QString href = QString( "attachment:%1?place=body" ).arg( msgPart->index().toString() );

  QString iconName;
  QByteArray contentId = msgPart->index().toString().toUtf8();
  if ( inlineImage ) {
    iconName = href;
  }
  else {
    iconName = mNodeHelper->iconName( msgPart );
    if( iconName.right( 14 ) == "mime_empty.png" ) {
      mNodeHelper->magicSetType( msgPart );
      iconName = mNodeHelper->iconName( msgPart );
    }
  }

  if ( inlineImage ) {
    // show the filename of the image below the embedded image
    htmlWriter()->queue( "<div><a href=\"" + href + "\">"
                          "<img src=\"" + fileName + "\" border=\"0\" style=\"max-width: 100%\"></a>"
                          "</div>"
                          "<div><a href=\"" + href + "\">" + label + "</a>"
                          "</div>"
                          "<div>" + comment + "</div><br>" );
  }
  else {
    // show the filename next to the image
    htmlWriter()->queue( "<div><a href=\"" + href + "\"><img src=\"" +
                          iconName + "\" border=\"0\" style=\"max-width: 100%\">" + label +
                          "</a></div>"
                          "<div>" + comment + "</div><br>" );
  }
}

#define SIG_FRAME_COL_UNDEF  99
#define SIG_FRAME_COL_RED    -1
#define SIG_FRAME_COL_YELLOW  0
#define SIG_FRAME_COL_GREEN   1
QString ObjectTreeParser::sigStatusToString( const Kleo::CryptoBackend::Protocol* cryptProto,
                                      int status_code,
                                      GpgME::Signature::Summary summary,
                                      int& frameColor,
                                      bool& showKeyInfos )
{
  // note: At the moment frameColor and showKeyInfos are
  //       used for CMS only but not for PGP signatures
  // pending(khz): Implement usage of these for PGP sigs as well.
  showKeyInfos = true;
  QString result;
  if( cryptProto ) {
      if( cryptProto == Kleo::CryptoBackendFactory::instance()->openpgp() ) {
          // process enum according to it's definition to be read in
          // GNU Privacy Guard CVS repository /gpgme/gpgme/gpgme.h
          switch( status_code ) {
          case 0: // GPGME_SIG_STAT_NONE
              result = i18n("Error: Signature not verified");
              break;
          case 1: // GPGME_SIG_STAT_GOOD
              result = i18n("Good signature");
              break;
          case 2: // GPGME_SIG_STAT_BAD
              result = i18n("<b>Bad</b> signature");
              break;
          case 3: // GPGME_SIG_STAT_NOKEY
              result = i18n("No public key to verify the signature");
              break;
          case 4: // GPGME_SIG_STAT_NOSIG
              result = i18n("No signature found");
              break;
          case 5: // GPGME_SIG_STAT_ERROR
              result = i18n("Error verifying the signature");
              break;
          case 6: // GPGME_SIG_STAT_DIFF
              result = i18n("Different results for signatures");
              break;
          /* PENDING(khz) Verify exact meaning of the following values:
          case 7: // GPGME_SIG_STAT_GOOD_EXP
              return i18n("Signature certificate is expired");
          break;
          case 8: // GPGME_SIG_STAT_GOOD_EXPKEY
              return i18n("One of the certificate's keys is expired");
          break;
          */
          default:
              result = "";   // do *not* return a default text here !
              break;
          }
      }
      else if ( cryptProto == Kleo::CryptoBackendFactory::instance()->smime() ) {
          // process status bits according to SigStatus_...
          // definitions in kdenetwork/libkdenetwork/cryptplug.h

          if( summary == GpgME::Signature::None ) {
              result = i18n("No status information available.");
              frameColor = SIG_FRAME_COL_YELLOW;
              showKeyInfos = false;
              return result;
          }

          if( summary & GpgME::Signature::Valid ) {
              result = i18n("Good signature.");
              // Note:
              // Here we are work differently than KMail did before!
              //
              // The GOOD case ( == sig matching and the complete
              // certificate chain was verified and is valid today )
              // by definition does *not* show any key
              // information but just states that things are OK.
              //           (khz, according to LinuxTag 2002 meeting)
              frameColor = SIG_FRAME_COL_GREEN;
              showKeyInfos = false;
              return result;
          }

          // we are still there?  OK, let's test the different cases:

          // we assume green, test for yellow or red (in this order!)
          frameColor = SIG_FRAME_COL_GREEN;
          QString result2;
          if( summary & GpgME::Signature::KeyExpired ){
              // still is green!
              result2 += i18n("One key has expired.");
          }
          if( summary & GpgME::Signature::SigExpired ){
              // and still is green!
              result2 += i18n("The signature has expired.");
          }

          // test for yellow:
          if( summary & GpgME::Signature::KeyMissing ) {
              result2 += i18n("Unable to verify: key missing.");
              // if the signature certificate is missing
              // we cannot show information on it
              showKeyInfos = false;
              frameColor = SIG_FRAME_COL_YELLOW;
          }
          if( summary & GpgME::Signature::CrlMissing ){
              result2 += i18n("CRL not available.");
              frameColor = SIG_FRAME_COL_YELLOW;
          }
          if( summary & GpgME::Signature::CrlTooOld ){
              result2 += i18n("Available CRL is too old.");
              frameColor = SIG_FRAME_COL_YELLOW;
          }
          if( summary & GpgME::Signature::BadPolicy ){
              result2 += i18n("A policy was not met.");
              frameColor = SIG_FRAME_COL_YELLOW;
          }
          if( summary & GpgME::Signature::SysError ){
              result2 += i18n("A system error occurred.");
              // if a system error occurred
              // we cannot trust any information
              // that was given back by the plug-in
              showKeyInfos = false;
              frameColor = SIG_FRAME_COL_YELLOW;
          }

          // test for red:
          if( summary & GpgME::Signature::KeyRevoked ){
              // this is red!
              result2 += i18n("One key has been revoked.");
              frameColor = SIG_FRAME_COL_RED;
          }
          if( summary & GpgME::Signature::Red ) {
              if( result2.isEmpty() )
                  // Note:
                  // Here we are work differently than KMail did before!
                  //
                  // The BAD case ( == sig *not* matching )
                  // by definition does *not* show any key
                  // information but just states that things are BAD.
                  //
                  // The reason for this: In this case ALL information
                  // might be falsificated, we can NOT trust the data
                  // in the body NOT the signature - so we don't show
                  // any key/signature information at all!
                  //         (khz, according to LinuxTag 2002 meeting)
                  showKeyInfos = false;
              frameColor = SIG_FRAME_COL_RED;
          }
          else
              result = "";

          if( SIG_FRAME_COL_GREEN == frameColor ) {
              result = i18n("Good signature.");
          } else if( SIG_FRAME_COL_RED == frameColor ) {
              result = i18n("<b>Bad</b> signature.");
          } else
              result = "";

          if( !result2.isEmpty() ) {
              if( !result.isEmpty() )
                  result.append("<br />");
              result.append( result2 );
          }
      }
      /*
      // add i18n support for 3rd party plug-ins here:
      else if ( cryptPlug->libName().contains( "yetanotherpluginname", Qt::CaseInsensitive )) {

      }
      */
  }
  return result;
}


static QString writeSimpleSigstatHeader( const PartMetaData &block )
{
QString html;
html += "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\"><tr><td>";

if ( block.signClass == "signErr" ) {
  html += i18n( "Invalid signature." );
} else if ( block.signClass == "signOkKeyBad" || block.signClass == "signWarn" ) {
  html += i18n( "Not enough information to check signature validity." );
} else if ( block.signClass == "signOkKeyOk" ) {
  QString addr;
  if ( !block.signerMailAddresses.isEmpty() )
    addr = block.signerMailAddresses.first();
  QString name = addr;
  if ( name.isEmpty() )
    name = block.signer;
  if ( addr.isEmpty() ) {
    html += i18n( "Signature is valid." );
  } else {
    html += i18n( "Signed by <a href=\"mailto:%1\">%2</a>.", addr, name );
  }
} else {
  // should not happen
  html += i18n( "Unknown signature state" );
}
html += "</td><td align=\"right\">";
html += "<a href=\"kmail:showSignatureDetails\">";
html += i18n( "Show Details" );
html += "</a></td></tr></table>";
return html;
}

static QString beginVerboseSigstatHeader()
{
return "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\"><tr><td rowspan=\"2\">";
}

static QString makeShowAuditLogLink( const GpgME::Error & err, const QString & auditLog ) {
if ( const unsigned int code = err.code() ) {
  if ( code == GPG_ERR_NOT_IMPLEMENTED ) {
    kDebug() << "makeShowAuditLogLink: not showing link (not implemented)";
    return QString();
  } else if ( code == GPG_ERR_NO_DATA ) {
    kDebug() << "makeShowAuditLogLink: not showing link (not available)";
    return i18n("No Audit Log available");
  } else {
    return i18n("Error Retrieving Audit Log: %1", QString::fromLocal8Bit( err.asString() ) );
  }
}

if ( !auditLog.isEmpty() ) {
  KUrl url;
  url.setProtocol( "kmail" );
  url.setPath( "showAuditLog" );
  url.addQueryItem( "log", auditLog );

  return "<a href=\"" + url.url() + "\">" + i18nc("The Audit Log is a detailed error log from the gnupg backend", "Show Audit Log") + "</a>";
}

return QString();
}

static QString endVerboseSigstatHeader( const PartMetaData & pmd )
{
QString html;
html += "</td><td align=\"right\" valign=\"top\" nowrap=\"nowrap\">";
html += "<a href=\"kmail:hideSignatureDetails\">";
html += i18n( "Hide Details" );
html += "</a></td></tr>";
html += "<tr><td align=\"right\" valign=\"bottom\" nowrap=\"nowrap\">";
html += makeShowAuditLogLink( pmd.auditLogError, pmd.auditLog );
html += "</td></tr></table>";
return html;
}

QString ObjectTreeParser::writeSigstatHeader( PartMetaData & block,
                                            const Kleo::CryptoBackend::Protocol * cryptProto,
                                            const QString & fromAddress,
                                            KMime::Content *node )
{
  const bool isSMIME = cryptProto && ( cryptProto == Kleo::CryptoBackendFactory::instance()->smime() );
  QString signer = block.signer;

  QString htmlStr, simpleHtmlStr;
  QString dir = ( QApplication::isRightToLeft() ? "rtl" : "ltr" );
  QString cellPadding("cellpadding=\"1\"");

  if( block.isEncapsulatedRfc822Message )
  {
      htmlStr += "<table cellspacing=\"1\" "+cellPadding+" class=\"rfc822\">"
          "<tr class=\"rfc822H\"><td dir=\"" + dir + "\">";
      if( node ) {
          htmlStr += "<a href=\"" + NodeHelper::asHREF( node, "body" ) + "\">"
                    + i18n("Encapsulated message") + "</a>";
      } else {
          htmlStr += i18n("Encapsulated message");
      }
      htmlStr += "</td></tr><tr class=\"rfc822B\"><td>";
  }

  if( block.isEncrypted ) {
      htmlStr += "<table cellspacing=\"1\" "+cellPadding+" class=\"encr\">"
          "<tr class=\"encrH\"><td dir=\"" + dir + "\">";
      if ( block.inProgress ) {
          htmlStr += i18n("Please wait while the message is being decrypted...");
      } else if( block.isDecryptable ) {
          htmlStr += i18n("Encrypted message");
      } else {
          htmlStr += i18n("Encrypted message (decryption not possible)");
          if( !block.errorText.isEmpty() ) {
              htmlStr += "<br />" + i18n("Reason: %1", block.errorText );
          }
      }
      htmlStr += "</td></tr><tr class=\"encrB\"><td>";
  }

  if ( block.isSigned && block.inProgress ) {
      block.signClass = "signInProgress";
      htmlStr += "<table cellspacing=\"1\" "+cellPadding+" class=\"signInProgress\">"
      "<tr class=\"signInProgressH\"><td dir=\"" + dir + "\">";
      htmlStr += i18n("Please wait while the signature is being verified...");
      htmlStr += "</td></tr><tr class=\"signInProgressB\"><td>";
  }

  simpleHtmlStr = htmlStr;

  if( block.isSigned && !block.inProgress ) {
      QStringList& blockAddrs( block.signerMailAddresses );
      // note: At the moment frameColor and showKeyInfos are
      //       used for CMS only but not for PGP signatures
      // pending(khz): Implement usage of these for PGP sigs as well.
      int frameColor = SIG_FRAME_COL_UNDEF;
      bool showKeyInfos;
      bool onlyShowKeyURL = false;
      bool cannotCheckSignature = true;
      QString statusStr = sigStatusToString( cryptProto,
                                              block.status_code,
                                              block.sigSummary,
                                              frameColor,
                                              showKeyInfos );
      // if needed fallback to english status text
      // that was reported by the plugin
      if( statusStr.isEmpty() )
          statusStr = block.status;
      if( block.technicalProblem )
          frameColor = SIG_FRAME_COL_YELLOW;

      switch( frameColor ){
          case SIG_FRAME_COL_RED:
              cannotCheckSignature = false;
              break;
          case SIG_FRAME_COL_YELLOW:
              cannotCheckSignature = true;
              break;
          case SIG_FRAME_COL_GREEN:
              cannotCheckSignature = false;
              break;
      }

      // compose the string for displaying the key ID
      // either as URL or not linked (for PGP)
      // note: Once we can start PGP key manager programs
      //       from within KMail we could change this and
      //       always show the URL.    (khz, 2002/06/27)
      QString startKeyHREF;
      if( isSMIME )
          startKeyHREF =
              QString("<a href=\"kmail:showCertificate#%1 ### %2 ### %3\">")
              .arg( cryptProto->displayName(),
                    cryptProto->name(),
                    QString::fromLatin1( block.keyId ) );
      QString keyWithWithoutURL
          // FIXME: Kleopatra misses a -query option, so disable this for now.
          = /*isSMIME
          ? QString("%1%2</a>")
              .arg( startKeyHREF,
                    cannotCheckSignature ? i18n("[Details]") : ("0x" + block.keyId) )
          : */"0x" + QString::fromUtf8( block.keyId );


      // temporary hack: always show key information!
      showKeyInfos = true;

      // Sorry for using 'black' as null color but .isValid()
      // checking with QColor default c'tor did not work for
      // some reason.
      if( isSMIME && (SIG_FRAME_COL_UNDEF != frameColor) ) {

          // new frame settings for CMS:
          // beautify the status string
          if( !statusStr.isEmpty() ) {
              statusStr.prepend("<i>");
              statusStr.append( "</i>");
          }

          // special color handling: S/MIME uses only green/yellow/red.
          switch( frameColor ) {
              case SIG_FRAME_COL_RED:
                  block.signClass = "signErr";//"signCMSRed";
                  onlyShowKeyURL = true;
                  break;
              case SIG_FRAME_COL_YELLOW:
                  if( block.technicalProblem )
                      block.signClass = "signWarn";
                  else
                      block.signClass = "signOkKeyBad";//"signCMSYellow";
                  break;
              case SIG_FRAME_COL_GREEN:
                  block.signClass = "signOkKeyOk";//"signCMSGreen";
                  // extra hint for green case
                  // that email addresses in DN do not match fromAddress
                  QString greenCaseWarning;
                  QString msgFrom( KPIMUtils::extractEmailAddress(fromAddress) );
                  QString certificate;
                  if( block.keyId.isEmpty() )
                      certificate = i18n("certificate");
                  else
                      certificate = startKeyHREF + i18n("certificate") + "</a>";
                  if( !blockAddrs.empty() ){
                      if( !blockAddrs.contains( msgFrom, Qt::CaseInsensitive ) ) {
                          greenCaseWarning =
                              "<u>" +
                              i18nc("Start of warning message."
                                ,"Warning:") +
                              "</u> " +
                              i18n("Sender's mail address is not stored "
                                    "in the %1 used for signing.", certificate) +
                              "<br />" +
                              i18n("sender: ") +
                              msgFrom +
                              "<br />" +
                              i18n("stored: ");
                          // We cannot use Qt's join() function here but
                          // have to join the addresses manually to
                          // extract the mail addresses (without '<''>')
                          // before including it into our string:
                          bool bStart = true;
                          for(QStringList::ConstIterator it = blockAddrs.constBegin();
                              it != blockAddrs.constEnd(); ++it ){
                              if( !bStart )
                                  greenCaseWarning.append(", <br />&nbsp; &nbsp;");
                              bStart = false;
                              greenCaseWarning.append( KPIMUtils::extractEmailAddress(*it) );
                          }
                      }
                  } else {
                      greenCaseWarning =
                          "<u>" +
                          i18nc("Start of warning message.","Warning:") +
                          "</u> " +
                          i18n("No mail address is stored in the %1 used for signing, "
                                "so we cannot compare it to the sender's address %2.",
                            certificate,
                            msgFrom);
                  }
                  if( !greenCaseWarning.isEmpty() ) {
                      if( !statusStr.isEmpty() )
                          statusStr.append("<br />&nbsp;<br />");
                      statusStr.append( greenCaseWarning );
                  }
                  break;
          }

          QString frame = "<table cellspacing=\"1\" "+cellPadding+" "
              "class=\"" + block.signClass + "\">"
              "<tr class=\"" + block.signClass + "H\"><td dir=\"" + dir + "\">";
          htmlStr += frame + beginVerboseSigstatHeader();
          simpleHtmlStr += frame;
          simpleHtmlStr += writeSimpleSigstatHeader( block );
          if( block.technicalProblem ) {
              htmlStr += block.errorText;
          }
          else if( showKeyInfos ) {
              if( cannotCheckSignature ) {
                  htmlStr += i18n( "Not enough information to check "
                                    "signature. %1",
                                keyWithWithoutURL );
              }
              else {

                  if (block.signer.isEmpty())
                      signer = "";
                  else {
                      if( !blockAddrs.empty() ){
                          QString address = StringUtil::encodeMailtoUrl( blockAddrs.first() );
                          signer = "<a href=\"mailto:" + address + "\">" + signer + "</a>";
                      }
                  }

                  if( block.keyId.isEmpty() ) {
                      if( signer.isEmpty() || onlyShowKeyURL )
                          htmlStr += i18n( "Message was signed with unknown key." );
                      else
                          htmlStr += i18n( "Message was signed by %1.",
                                    signer );
                  } else {
                      QDateTime created = block.creationTime;
                      if( created.isValid() ) {
                          if( signer.isEmpty() ) {
                              if( onlyShowKeyURL )
                                  htmlStr += i18n( "Message was signed with key %1.",
                                                keyWithWithoutURL );
                              else
                                  htmlStr += i18n( "Message was signed on %1 with key %2.",
                                                KGlobal::locale()->formatDateTime( created ),
                                                keyWithWithoutURL );
                          }
                          else {
                              if( onlyShowKeyURL )
                                  htmlStr += i18n( "Message was signed with key %1.",
                                            keyWithWithoutURL );
                              else
                                  htmlStr += i18n( "Message was signed by %3 on %1 with key %2",
                                            KGlobal::locale()->formatDateTime( created ),
                                            keyWithWithoutURL,
                                            signer );
                          }
                      }
                      else {
                          if( signer.isEmpty() || onlyShowKeyURL )
                              htmlStr += i18n( "Message was signed with key %1.",
                                        keyWithWithoutURL );
                          else
                              htmlStr += i18n( "Message was signed by %2 with key %1.",
                                        keyWithWithoutURL,
                                        signer );
                      }
                  }
              }
              htmlStr += "<br />";
              if( !statusStr.isEmpty() ) {
                  htmlStr += "&nbsp;<br />";
                  htmlStr += i18n( "Status: " );
                  htmlStr += statusStr;
              }
          } else {
              htmlStr += statusStr;
          }
          frame = "</td></tr><tr class=\"" + block.signClass + "B\"><td>";
          htmlStr += endVerboseSigstatHeader( block ) + frame;
          simpleHtmlStr += frame;

      } else {

          // old frame settings for PGP:

          if( block.signer.isEmpty() || block.technicalProblem ) {
              block.signClass = "signWarn";
              QString frame = "<table cellspacing=\"1\" "+cellPadding+" "
                  "class=\"" + block.signClass + "\">"
                  "<tr class=\"" + block.signClass + "H\"><td dir=\"" + dir + "\">";
              htmlStr += frame + beginVerboseSigstatHeader();
              simpleHtmlStr += frame;
              simpleHtmlStr += writeSimpleSigstatHeader( block );
              if( block.technicalProblem ) {
                  htmlStr += block.errorText;
              }
              else {
                if( !block.keyId.isEmpty() ) {
                  QDateTime created = block.creationTime;
                  if ( created.isValid() )
                      htmlStr += i18n( "Message was signed on %1 with unknown key %2.",
                                KGlobal::locale()->formatDateTime( created ),
                                keyWithWithoutURL );
                  else
                      htmlStr += i18n( "Message was signed with unknown key %1.",
                                keyWithWithoutURL );
                }
                else
                  htmlStr += i18n( "Message was signed with unknown key." );
                htmlStr += "<br />";
                htmlStr += i18n( "The validity of the signature cannot be "
                                  "verified." );
                if( !statusStr.isEmpty() ) {
                  htmlStr += "<br />";
                  htmlStr += i18n( "Status: " );
                  htmlStr += "<i>";
                  htmlStr += statusStr;
                  htmlStr += "</i>";
                }
              }
              frame = "</td></tr><tr class=\"" + block.signClass + "B\"><td>";
              htmlStr += endVerboseSigstatHeader( block ) + frame;
              simpleHtmlStr += frame;
          }
          else
          {
              // HTMLize the signer's user id and create mailto: link
              signer = StringUtil::quoteHtmlChars( signer, true );
              signer = "<a href=\"mailto:" + signer + "\">" + signer + "</a>";

              if (block.isGoodSignature) {
                  if( block.keyTrust < Kpgp::KPGP_VALIDITY_MARGINAL )
                      block.signClass = "signOkKeyBad";
                  else
                      block.signClass = "signOkKeyOk";
                  QString frame = "<table cellspacing=\"1\" "+cellPadding+" "
                      "class=\"" + block.signClass + "\">"
                      "<tr class=\"" + block.signClass + "H\"><td dir=\"" + dir + "\">";
                  htmlStr += frame + beginVerboseSigstatHeader();
                  simpleHtmlStr += frame;
                  simpleHtmlStr += writeSimpleSigstatHeader( block );
                  if( !block.keyId.isEmpty() )
                      htmlStr += i18n( "Message was signed by %2 (Key ID: %1).",
                                    keyWithWithoutURL,
                                    signer );
                  else
                      htmlStr += i18n( "Message was signed by %1.", signer );
                  htmlStr += "<br />";

                  switch( block.keyTrust )
                  {
                      case Kpgp::KPGP_VALIDITY_UNKNOWN:
                      htmlStr += i18n( "The signature is valid, but the key's "
                              "validity is unknown." );
                      break;
                      case Kpgp::KPGP_VALIDITY_MARGINAL:
                      htmlStr += i18n( "The signature is valid and the key is "
                              "marginally trusted." );
                      break;
                      case Kpgp::KPGP_VALIDITY_FULL:
                      htmlStr += i18n( "The signature is valid and the key is "
                              "fully trusted." );
                      break;
                      case Kpgp::KPGP_VALIDITY_ULTIMATE:
                      htmlStr += i18n( "The signature is valid and the key is "
                              "ultimately trusted." );
                      break;
                      default:
                      htmlStr += i18n( "The signature is valid, but the key is "
                              "untrusted." );
                  }
                  frame = "</td></tr>"
                      "<tr class=\"" + block.signClass + "B\"><td>";
                  htmlStr += endVerboseSigstatHeader( block ) + frame;
                  simpleHtmlStr += frame;
              }
              else
              {
                  block.signClass = "signErr";
                  QString frame = "<table cellspacing=\"1\" "+cellPadding+" "
                      "class=\"" + block.signClass + "\">"
                      "<tr class=\"" + block.signClass + "H\"><td dir=\"" + dir + "\">";
                  htmlStr += frame + beginVerboseSigstatHeader();
                  simpleHtmlStr += frame;
                  simpleHtmlStr += writeSimpleSigstatHeader( block );
                  if( !block.keyId.isEmpty() )
                      htmlStr += i18n( "Message was signed by %2 (Key ID: %1).",
                        keyWithWithoutURL,
                        signer );
                  else
                      htmlStr += i18n( "Message was signed by %1.", signer );
                  htmlStr += "<br />";
                  htmlStr += i18n("Warning: The signature is bad.");
                  frame = "</td></tr>"
                      "<tr class=\"" + block.signClass + "B\"><td>";
                  htmlStr += endVerboseSigstatHeader( block ) + frame;
                  simpleHtmlStr += frame;
              }
          }
      }
  }

  if ( mSource->showSignatureDetails() )
    return htmlStr;
  return simpleHtmlStr;
}

QString ObjectTreeParser::writeSigstatFooter( PartMetaData& block )
{
  QString dir = ( QApplication::isRightToLeft() ? "rtl" : "ltr" );

  QString htmlStr;

  if (block.isSigned) {
      htmlStr += "</td></tr><tr class=\"" + block.signClass + "H\">";
      htmlStr += "<td dir=\"" + dir + "\">" +
          i18n( "End of signed message" ) +
          "</td></tr></table>";
  }

  if (block.isEncrypted) {
      htmlStr += "</td></tr><tr class=\"encrH\"><td dir=\"" + dir + "\">" +
              i18n( "End of encrypted message" ) +
          "</td></tr></table>";
  }

  if( block.isEncapsulatedRfc822Message )
  {
      htmlStr += "</td></tr><tr class=\"rfc822H\"><td dir=\"" + dir + "\">" +
          i18n( "End of encapsulated message" ) +
          "</td></tr></table>";
  }

  return htmlStr;
}


//-----------------------------------------------------------------------------

void ObjectTreeParser::writeAttachmentMarkHeader( KMime::Content *node )
{
  if ( !mHtmlWriter )
    return;

  htmlWriter()->queue( QString( "<div id=\"attachmentDiv%1\">\n" ).arg( node->index().toString() ) );
}

//-----------------------------------------------------------------------------

void ObjectTreeParser::writeAttachmentMarkFooter()
{
  if ( !mHtmlWriter )
    return;

  htmlWriter()->queue( QString( "</div>" ) );
}



//-----------------------------------------------------------------------------
void ObjectTreeParser::writeBodyStr( const QByteArray& aStr, const QTextCodec *aCodec,
                              const QString& fromAddress )
{
  KMMsgSignatureState dummy1;
  KMMsgEncryptionState dummy2;
  writeBodyStr( aStr, aCodec, fromAddress, dummy1, dummy2, false );
}

//-----------------------------------------------------------------------------
void ObjectTreeParser::writeBodyStr( const QByteArray& aStr, const QTextCodec *aCodec,
                              const QString& fromAddress,
                              KMMsgSignatureState&  inlineSignatureState,
                              KMMsgEncryptionState& inlineEncryptionState,
                              bool decorate )
{
  bool goodSignature = false;
  Kpgp::Module* pgp = Kpgp::Module::getKpgp();
  assert(pgp != 0);
  bool isPgpMessage = false; // true if the message contains at least one
                              // PGP MESSAGE or one PGP SIGNED MESSAGE block
  QString dir = ( QApplication::isRightToLeft() ? "rtl" : "ltr" );
  QString headerStr = QString("<div dir=\"%1\">").arg(dir);

  inlineSignatureState  = KMMsgNotSigned;
  inlineEncryptionState = KMMsgNotEncrypted;
  QList<Kpgp::Block> pgpBlocks;
  QList<QByteArray> nonPgpBlocks;
  if( Kpgp::Module::prepareMessageForDecryption( aStr, pgpBlocks, nonPgpBlocks ) )
  {
      bool isEncrypted = false, isSigned = false;
      bool fullySignedOrEncrypted = true;
      bool firstNonPgpBlock = true;
      bool couldDecrypt = false;
      QString signer;
      QByteArray keyId;
      QString decryptionError;
      Kpgp::Validity keyTrust = Kpgp::KPGP_VALIDITY_FULL;

      QList<Kpgp::Block>::iterator pbit =  pgpBlocks.begin();
      QListIterator<QByteArray> npbit( nonPgpBlocks );
      QString htmlStr;
      for( ; pbit != pgpBlocks.end(); ++pbit )
      {
          // insert the next Non-OpenPGP block
          QByteArray str( npbit.next() );
          if( !str.isEmpty() ) {
            htmlStr += quotedHTML( aCodec->toUnicode( str ), decorate );
            kDebug() << "Non-empty Non-OpenPGP block found: '" << str  << "'";
            // treat messages with empty lines before the first clearsigned
            // block as fully signed/encrypted
            if( firstNonPgpBlock ) {
              // check whether str only consists of \n
              for( QByteArray::ConstIterator c = str.begin(); *c; ++c ) {
                if( *c != '\n' ) {
                  fullySignedOrEncrypted = false;
                  break;
                }
              }
            }
            else {
              fullySignedOrEncrypted = false;
            }
          }
          firstNonPgpBlock = false;

          //htmlStr += "<br>";

          Kpgp::Block &block = *pbit;
          if( ( block.type() == Kpgp::PgpMessageBlock /*FIXME(Andras) port to akonadi
                &&
                // ### Workaround for bug 56693
                !kmkernel->contextMenuShown() */) ||
              ( block.type() == Kpgp::ClearsignedBlock ) )
          {
              isPgpMessage = true;
              if( block.type() == Kpgp::PgpMessageBlock )
              {
                mSource->emitNoDrag();
                // try to decrypt this OpenPGP block
                couldDecrypt = block.decrypt();
                isEncrypted = block.isEncrypted();
                if (!couldDecrypt) {
                  decryptionError = pgp->lastErrorMsg();
                }
              }
              else
              {
                  // try to verify this OpenPGP block
                  block.verify();
              }

              isSigned = block.isSigned();
              if( isSigned )
              {
                  keyId = block.signatureKeyId();
                  signer = block.signatureUserId();
                  if( !signer.isEmpty() )
                  {
                      goodSignature = block.goodSignature();

                      if( !keyId.isEmpty() ) {
                        keyTrust = pgp->keyTrust( keyId );
                        Kpgp::Key* key = pgp->publicKey( keyId );
                        if ( key ) {
                          // Use the user ID from the key because this one
                          // is charset safe.
                          signer = key->primaryUserID();
                        }
                      }
                      else
                        // This is needed for the PGP 6 support because PGP 6 doesn't
                        // print the key id of the signing key if the key is known.
                        keyTrust = pgp->keyTrust( signer );
                  }
              }

              if( isSigned )
                inlineSignatureState = KMMsgPartiallySigned;
              if( isEncrypted )
                inlineEncryptionState = KMMsgPartiallyEncrypted;

              PartMetaData messagePart;

              messagePart.isSigned = isSigned;
              messagePart.technicalProblem = false;
              messagePart.isGoodSignature = goodSignature;
              messagePart.isEncrypted = isEncrypted;
              messagePart.isDecryptable = couldDecrypt;
              messagePart.decryptionError = decryptionError;
              messagePart.signer = signer;
              messagePart.keyId = keyId;
              messagePart.keyTrust = keyTrust;
              messagePart.auditLogError = GpgME::Error( GPG_ERR_NOT_IMPLEMENTED );

              htmlStr += writeSigstatHeader( messagePart, 0, fromAddress );

              if ( couldDecrypt || !isEncrypted ) {
                htmlStr += quotedHTML( aCodec->toUnicode( block.text() ), decorate );
              }
              else {
                htmlStr += QString( "<div align=\"center\">%1</div>" )
                            .arg( i18n( "The message could not be decrypted.") );
              }
              htmlStr += writeSigstatFooter( messagePart );
          }
          else // block is neither message block nor clearsigned block
            htmlStr += quotedHTML( aCodec->toUnicode( block.text() ),
                                    decorate );
      }

      // add the last Non-OpenPGP block
      QByteArray str( nonPgpBlocks.last() );
      if( !str.isEmpty() ) {
        htmlStr += quotedHTML( aCodec->toUnicode( str ), decorate );
        // Even if the trailing Non-OpenPGP block isn't empty we still
        // consider the message part fully signed/encrypted because else
        // all inline signed mailing list messages would only be partially
        // signed because of the footer which is often added by the mailing
        // list software. IK, 2003-02-15
      }
      if( fullySignedOrEncrypted ) {
        if( inlineSignatureState == KMMsgPartiallySigned )
          inlineSignatureState = KMMsgFullySigned;
        if( inlineEncryptionState == KMMsgPartiallyEncrypted )
          inlineEncryptionState = KMMsgFullyEncrypted;
      }
      htmlWriter()->queue( htmlStr );
  }
  else
    htmlWriter()->queue( quotedHTML( aCodec->toUnicode( aStr ), decorate ) );
}


QString ObjectTreeParser::quotedHTML( const QString& s, bool decorate )
{
  assert( cssHelper() );

  int convertFlags = LinkLocator::PreserveSpaces | LinkLocator::HighlightText;
  if ( decorate && GlobalSettings::self()->showEmoticons() ) {
    convertFlags |= LinkLocator::ReplaceSmileys;
  }
  QString htmlStr;
  const QString normalStartTag = cssHelper()->nonQuotedFontTag();
  QString quoteFontTag[3];
  QString deepQuoteFontTag[3];
  for ( int i = 0 ; i < 3 ; ++i ) {
    quoteFontTag[i] = cssHelper()->quoteFontTag( i );
    deepQuoteFontTag[i] = cssHelper()->quoteFontTag( i+3 );
  }
  const QString normalEndTag = "</div>";
  const QString quoteEnd = "</div>";

  const unsigned int length = s.length();
  bool paraIsRTL = false;
  bool startNewPara = true;
  unsigned int pos, beg;

  // skip leading empty lines
  for ( pos = 0; pos < length && s[pos] <= ' '; pos++ )
    ;
  while (pos > 0 && (s[pos-1] == ' ' || s[pos-1] == '\t')) pos--;
  beg = pos;

  int currQuoteLevel = -2; // -2 == no previous lines
  bool curHidden = false; // no hide any block

  if ( GlobalSettings::self()->showExpandQuotesMark() )
  {
    // Cache Icons
    if ( mCollapseIcon.isEmpty() ) {
      mCollapseIcon= LinkLocator::pngToDataUrl(
          IconNameCache::instance()->iconPath( "quotecollapse", 0 ));
    }
    if ( mExpandIcon.isEmpty() )
      mExpandIcon= LinkLocator::pngToDataUrl(
          IconNameCache::instance()->iconPath( "quoteexpand", 0 ));
  }

  while (beg<length)
  {
    QString line;

    /* search next occurrence of '\n' */
    pos = s.indexOf('\n', beg, Qt::CaseInsensitive);
    if (pos == (unsigned int)(-1))
        pos = length;

    line = s.mid(beg,pos-beg);
    beg = pos+1;

    /* calculate line's current quoting depth */
    int actQuoteLevel = -1;

    for (int p=0; p<line.length(); p++) {
      switch (line[p].toLatin1()) {
        case '>':
        case '|':
          actQuoteLevel++;
          break;
        case ' ':  // spaces and tabs are allowed between the quote markers
        case '\t':
        case '\r':
          break;
        default:  // stop quoting depth calculation
          p = line.length();
          break;
      }
    } /* for() */

    bool actHidden = false;
    QString textExpand;

    // This quoted line needs be hidden
    if (GlobalSettings::self()->showExpandQuotesMark() && mSource->levelQuote() >= 0
        && mSource->levelQuote() <= ( actQuoteLevel ) )
      actHidden = true;

    if ( actQuoteLevel != currQuoteLevel ) {
      /* finish last quotelevel */
      if (currQuoteLevel == -1)
        htmlStr.append( normalEndTag );
      else if ( currQuoteLevel >= 0 && !curHidden )
        htmlStr.append( quoteEnd );

      /* start new quotelevel */
      if (actQuoteLevel == -1) {
        htmlStr += normalStartTag;
      } else {
        if ( GlobalSettings::self()->showExpandQuotesMark() ) {
          if ( true ) {
            if ( actHidden ) {
              //only show the QuoteMark when is the first line of the level hidden
              if ( !curHidden ) {
                //Expand all quotes
                htmlStr += "<div class=\"quotelevelmark\" >" ;
                htmlStr += QString( "<a href=\"kmail:levelquote?%1 \">"
                                    "<img src=\"%2\" alt=\"\" title=\"\"/></a>" )
                  .arg(-1)
                  .arg( mExpandIcon );
                htmlStr += "</div><br/>";
                htmlStr += quoteEnd;
              }
            } else {
              htmlStr += "<div class=\"quotelevelmark\" >" ;
              htmlStr += QString( "<a href=\"kmail:levelquote?%1 \">"
                                  "<img src=\"%2\" alt=\"\" title=\"\"/></a>" )
                .arg(actQuoteLevel)
                .arg( mCollapseIcon);
              htmlStr += "</div>";
              if ( actQuoteLevel < 3 )
                htmlStr += quoteFontTag[actQuoteLevel];
              else
                htmlStr += deepQuoteFontTag[actQuoteLevel%3];
            }
          } else {
            if ( actQuoteLevel < 3 ) {
              htmlStr += quoteFontTag[actQuoteLevel];
            } else {
              htmlStr += deepQuoteFontTag[actQuoteLevel%3];
            }
          }
        }
      }
      currQuoteLevel = actQuoteLevel;
    }
    curHidden = actHidden;


    if ( !actHidden )
    {
      // don't write empty <div ...></div> blocks (they have zero height)
      // ignore ^M DOS linebreaks
      if( !line.remove( '\015' ).isEmpty() )
      {
          if ( startNewPara )
            paraIsRTL = line.isRightToLeft();
          htmlStr += QString( "<div dir=\"%1\">" ).arg( paraIsRTL ? "rtl" : "ltr" );
          htmlStr += LinkLocator::convertToHtml( line, convertFlags );
          htmlStr += QString( "</div>" );
          startNewPara = looksLikeParaBreak( s, pos );
      }
      else
      {
        htmlStr += "<br>";
        // after an empty line, always start a new paragraph
        startNewPara = true;
      }
    }
  } /* while() */

  /* really finish the last quotelevel */
  if (currQuoteLevel == -1)
      htmlStr.append( normalEndTag );
  else
      htmlStr.append( quoteEnd );

  //kDebug() << "========================================\n"
  //         << htmlStr
  //         << "\n======================================\n";
  return htmlStr;
}



const QTextCodec * ObjectTreeParser::codecFor( KMime::Content * node ) const
{
  assert( node );
  if ( mSource->overrideCodec() )
    return mSource->overrideCodec();
  return mNodeHelper->codec( node );
}

// Guesstimate if the newline at newLinePos actually separates paragraphs in the text s
// We use several heuristics:
// 1. If newLinePos points after or before (=at the very beginning of) text, it is not between paragraphs
// 2. If the previous line was longer than the wrap size, we want to consider it a paragraph on its own
//    (some clients, notably Outlook, send each para as a line in the plain-text version).
// 3. Otherwise, we check if the newline could have been inserted for wrapping around; if this
//    was the case, then the previous line will be shorter than the wrap size (which we already
//    know because of item 2 above), but adding the first word from the next line will make it
//    longer than the wrap size.
bool ObjectTreeParser::looksLikeParaBreak( const QString& s, unsigned int newLinePos ) const
{
  const unsigned int WRAP_COL = 78;

  unsigned int length = s.length();
  // 1. Is newLinePos at an end of the text?
  if ( newLinePos >= length-1 || newLinePos == 0 ) {
    return false;
  }

  // 2. Is the previous line really a paragraph -- longer than the wrap size?

  // First char of prev line -- works also for first line
  unsigned prevStart = s.lastIndexOf( '\n', newLinePos - 1 ) + 1;
  unsigned prevLineLength = newLinePos - prevStart;
  if ( prevLineLength > WRAP_COL ) {
    return true;
  }

  // find next line to delimit search for first word
  unsigned int nextStart = newLinePos + 1;
  int nextEnd = s.indexOf( '\n', nextStart );
  if ( nextEnd == -1 ) {
    nextEnd = length;
  }
  QString nextLine = s.mid( nextStart, nextEnd - nextStart );
  length = nextLine.length();
  // search for first word in next line
  unsigned int wordStart;
  bool found = false;
  for ( wordStart = 0; !found && wordStart < length; wordStart++ ) {
    switch ( nextLine[wordStart].toLatin1() ) {
      case '>':
      case '|':
      case ' ':  // spaces, tabs and quote markers don't count
      case '\t':
      case '\r':
        break;
      default:
        found = true;
        break;
    }
  } /* for() */

  if ( !found ) {
    // next line is essentially empty, it seems -- empty lines are
    // para separators
    return true;
  }
  //Find end of first word.
  //Note: flowText (in kmmessage.cpp) separates words for wrap by
  //spaces only. This should be consistent, which calls for some
  //refactoring.
  int wordEnd = nextLine.indexOf( ' ', wordStart );
  if ( wordEnd == (-1) ) {
    wordEnd = length;
  }
  int wordLength = wordEnd - wordStart;

  // 3. If adding a space and the first word to the prev line don't
  //    make it reach the wrap column, then the break was probably
  //    meaningful
  return prevLineLength + wordLength + 1 < WRAP_COL;
}

#ifdef MARCS_DEBUG
void ObjectTreeParser::dumpToFile( const char * filename, const char * start,
                                    size_t len ) {
  assert( filename );

  QFile f( filename );
  if ( f.open( QIODevice::WriteOnly ) ) {
    if ( start ) {
      QDataStream ds( &f );
      ds.writeRawData( start, len );
    }
    f.close();  // If data is 0 we just create a zero length file.
  }
}
#endif // !NDEBUG


KMime::Content* ObjectTreeParser::findType( KMime::Content* content, const QByteArray& mimeType, bool deep, bool wide )
{
    if( ( !content->contentType()->isEmpty() )
        && ( mimeType.isEmpty()  || ( mimeType == content->contentType()->mimeType() ) ) )
        return content;
    KMime::Content *child = NodeHelper::firstChild( content );
    if ( child && deep ) //first child
        return findType( child, mimeType, deep, wide );

    KMime::Content *next = NodeHelper::nextSibling( content );
    if (next &&  wide ) //next on the same level
      return findType( next, mimeType, deep, wide );

    return 0;
}

KMime::Content* ObjectTreeParser::findType( KMime::Content* content, const QByteArray& mediaType, const QByteArray& subType, bool deep, bool wide )
{
    if ( !content->contentType()->isEmpty() ) {
      if ( ( mediaType.isEmpty()  ||  mediaType == content->contentType()->mediaType() )
        && ( subType.isEmpty()  ||  subType == content->contentType()->subType() ) )
        return content;
    }
    KMime::Content *child = NodeHelper::firstChild( content );
    if ( child && deep ) //first child
        return findType( child, mediaType, subType, deep, wide );

    KMime::Content *next = NodeHelper::nextSibling( content );
    if (next &&  wide ) //next on the same level
      return findType( next, mediaType, subType, deep, wide );

    return 0;
}

KMime::Content* ObjectTreeParser::findTypeNot( KMime::Content * content, const QByteArray& mediaType, const QByteArray& subType, bool deep, bool wide )
{
    if( ( !content->contentType()->isEmpty() )
          && (  mediaType.isEmpty() || content->contentType()->mediaType() != mediaType )
          && ( subType.isEmpty() || content->contentType()->subType() != subType )
          )
        return content;
    KMime::Content *child = NodeHelper::firstChild( content );
    if ( child && deep )
        return findTypeNot( child, mediaType, subType, deep, wide );

    KMime::Content *next = NodeHelper::nextSibling( content );
    if ( next && wide )
        return findTypeNot( next,  mediaType, subType, deep, wide );
    return 0;
}

