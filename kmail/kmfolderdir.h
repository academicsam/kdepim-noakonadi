#ifndef kmfolderdir_h
#define kmfolderdir_h

#include <QString>
#include <QByteArray>
#include "kmfoldernode.h"
#include "kmfoldertype.h"

class KMFolder;
class KMFolderMgr;


/** KMail list that manages the contents of one directory that may
 * contain folders and/or other directories.
 */
class KMFolderDir: public KMFolderNode, public KMFolderNodeList
{
  Q_OBJECT

public:
  explicit KMFolderDir( KMFolder * owner, KMFolderDir * parent = 0,
               const QString& path = QString(),
	       KMFolderDirType = KMStandardDir );
  virtual ~KMFolderDir();

  virtual bool isDir() const { return true; }

  /**
   * Adds the given subdirectory of this directory to the associated folder.
   */
  void addDirToParent( const QString &dirName, KMFolder *parentFolder );

  /** Read contents of directory. */
  virtual bool reload();

  /** Return full pathname of this directory. */
  virtual QString path() const;

  /** Returns the label of the folder for visualization. */
  QString label() const;

  /** URL of the node for visualization purposes. */
  virtual QString prettyUrl() const;

  /** Create a mail folder in this directory with given name. If sysFldr==true
   the folder is marked as a (KMail) system folder.
   Returns Folder on success. */
  virtual KMFolder* createFolder(const QString& folderName,
				 bool sysFldr=false,
                                 KMFolderType folderType=KMFolderTypeMbox);

  /** Returns folder with given name or zero if it does not exist */
  virtual KMFolderNode* hasNamedFolder(const QString& name);

  /** Returns the folder manager that manages this folder */
  virtual KMFolderMgr* manager() const;

  /** Returns the folder whose children we are holding */
  KMFolder* owner() const { return mOwner; }

  virtual KMFolderDirType type() const { return mDirType; }

protected:
  KMFolder * mOwner;
  KMFolderDirType mDirType;
};


//-----------------------------------------------------------------------------

class KMFolderRootDir: public KMFolderDir
{
  Q_OBJECT

public:
  explicit KMFolderRootDir(KMFolderMgr* manager,
		  const QString& path=QString(),
		  KMFolderDirType dirType = KMStandardDir);
  virtual ~KMFolderRootDir();
  virtual QString path() const;

  /** set the absolute path */
  virtual void setPath(const QString&);

  virtual QString prettyUrl() const;

  void setBaseURL( const QByteArray& baseURL );

  virtual KMFolderMgr* manager() const;

protected:
  QString mPath;
  KMFolderMgr *mManager;
  QByteArray mBaseURL;
};

#endif /*kmfolderdir_h*/

