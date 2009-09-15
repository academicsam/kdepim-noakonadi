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

#ifndef __MESSAGELIST_CORE_MANAGER_H__
#define __MESSAGELIST_CORE_MANAGER_H__

#include <messagelist/core/sortorder.h>
#include <QList>
#include <QHash>
#include <QObject>

#include <messagelist/messagelist_export.h>

class QPixmap;

namespace KMime
{
  class DateFormatter;
}

namespace MessageList
{

namespace Core
{

class Aggregation;
class Theme;
class StorageModel;
class Widget;

/**
 * @brief: The manager for all the existing MessageList::Widget objects.
 *
 * This class is the "central" object of the whole MessageList framework.
 * It's a singleton that can be accessed only by the means of static methods,
 * is created automatically when the first MessageList::Widget object is created
 * and destroyed automatically when the last MessageList::Widget object is destroyed.
 *
 * This class takes care of loading/storing/mantaining the settings for the
 * whole MessageList framework. It also keeps track of all the existing
 * MessageList::Widget objects and takes care of uptdating them when settings change.
 *
 * This object relies on KMKernel existence for its whole lifetime (and in particular
 * for loading and saving settings) thus you must make sure that KMKernel is created
 * before the first MessageList::Widget and destroyed after the last MessageList::Widget.
 * But in KMail this is always the case :)
 */
class Manager : public QObject
{
  Q_OBJECT
protected:
  Manager();
  ~Manager();

private:
  static Manager * mInstance;
  QList< Widget * > mWidgetList;
  QHash< QString, Aggregation * > mAggregations;
  QHash< QString, Theme * > mThemes;
  KMime::DateFormatter * mDateFormatter;
  bool mDisplayMessageToolTips;
  QString mCachedLocalizedUnknownText;

  // pixmaps, never null

  QPixmap * mPixmapMessageNew;
  QPixmap * mPixmapMessageUnread;
  QPixmap * mPixmapMessageRead;
  QPixmap * mPixmapMessageDeleted;
  QPixmap * mPixmapMessageReplied;
  QPixmap * mPixmapMessageRepliedAndForwarded;
  QPixmap * mPixmapMessageQueued;
  QPixmap * mPixmapMessageActionItem;
  QPixmap * mPixmapMessageSent;
  QPixmap * mPixmapMessageForwarded;
  QPixmap * mPixmapMessageImportant; // "flag"
  QPixmap * mPixmapMessageWatched;
  QPixmap * mPixmapMessageIgnored;
  QPixmap * mPixmapMessageSpam;
  QPixmap * mPixmapMessageHam;
  QPixmap * mPixmapMessageFullySigned;
  QPixmap * mPixmapMessagePartiallySigned;
  QPixmap * mPixmapMessageUndefinedSigned;
  QPixmap * mPixmapMessageNotSigned;
  QPixmap * mPixmapMessageFullyEncrypted;
  QPixmap * mPixmapMessagePartiallyEncrypted;
  QPixmap * mPixmapMessageUndefinedEncrypted;
  QPixmap * mPixmapMessageNotEncrypted;
  QPixmap * mPixmapMessageAttachment;
  QPixmap * mPixmapShowMore;
  QPixmap * mPixmapShowLess;
  QPixmap * mPixmapVerticalLine;
  QPixmap * mPixmapHorizontalSpacer;

public:
  // instance management
  static Manager * instance()
    { return mInstance; };

  // widget registration
  static void registerWidget( Widget *pWidget );
  static void unregisterWidget( Widget *pWidget );

  const KMime::DateFormatter * dateFormatter() const
    { return mDateFormatter; };

  // global pixmaps
  const QPixmap * pixmapMessageNew() const
    { return mPixmapMessageNew; };
  const QPixmap * pixmapMessageUnread() const
    { return mPixmapMessageUnread; };
  const QPixmap * pixmapMessageRead() const
    { return mPixmapMessageRead; };
  const QPixmap * pixmapMessageDeleted() const
    { return mPixmapMessageDeleted; };
  const QPixmap * pixmapMessageReplied() const
    { return mPixmapMessageReplied; };
  const QPixmap * pixmapMessageRepliedAndForwarded() const
    { return mPixmapMessageRepliedAndForwarded; };
  const QPixmap * pixmapMessageQueued() const
    { return mPixmapMessageQueued; };
  const QPixmap * pixmapMessageActionItem() const
    { return mPixmapMessageActionItem; };
  const QPixmap * pixmapMessageSent() const
    { return mPixmapMessageSent; };
  const QPixmap * pixmapMessageForwarded() const
    { return mPixmapMessageForwarded; };
  const QPixmap * pixmapMessageImportant() const
    { return mPixmapMessageImportant; };
  const QPixmap * pixmapMessageWatched() const
    { return mPixmapMessageWatched; };
  const QPixmap * pixmapMessageIgnored() const
    { return mPixmapMessageIgnored; };
  const QPixmap * pixmapMessageSpam() const
    { return mPixmapMessageSpam; };
  const QPixmap * pixmapMessageHam() const
    { return mPixmapMessageHam; };
  const QPixmap * pixmapMessageFullySigned() const
    { return mPixmapMessageFullySigned; };
  const QPixmap * pixmapMessagePartiallySigned() const
    { return mPixmapMessagePartiallySigned; };
  const QPixmap * pixmapMessageUndefinedSigned() const
    { return mPixmapMessageUndefinedSigned; };
  const QPixmap * pixmapMessageNotSigned() const
    { return mPixmapMessageNotSigned; };
  const QPixmap * pixmapMessageFullyEncrypted() const
    { return mPixmapMessageFullyEncrypted; };
  const QPixmap * pixmapMessagePartiallyEncrypted() const
    { return mPixmapMessagePartiallyEncrypted; };
  const QPixmap * pixmapMessageUndefinedEncrypted() const
    { return mPixmapMessageUndefinedEncrypted; };
  const QPixmap * pixmapMessageNotEncrypted() const
    { return mPixmapMessageNotEncrypted; };
  const QPixmap * pixmapMessageAttachment() const
    { return mPixmapMessageAttachment; };
  const QPixmap * pixmapShowMore() const
    { return mPixmapShowMore; };
  const QPixmap * pixmapShowLess() const
    { return mPixmapShowLess; };
  const QPixmap * pixmapVerticalLine() const
    { return mPixmapVerticalLine; };
  const QPixmap * pixmapHorizontalSpacer() const
    { return mPixmapHorizontalSpacer; };

  const QString & cachedLocalizedUnknownText() const
    { return mCachedLocalizedUnknownText; };

  /**
   * Returns true if the display of tooltips in the View instances is enabled
   * in the configuration and false otherwise.
   */
  bool displayMessageToolTips() const
    { return mDisplayMessageToolTips; };

  /**
   * Sets if the display of tooltips in the View instances is enabled
   * and false otherwise.
   */
  void setDisplayMessageToolTips( bool displayMessageToolTips )
    { mDisplayMessageToolTips = displayMessageToolTips; };

  /**
   * Returns the unique id of the last selected message for the specified StorageModel.
   * Returns 0 if this value isn't stored in the configuration.
   */
  unsigned long preSelectedMessageForStorageModel( const StorageModel *storageModel );

  /**
   * Stores in the unique id of the last selected message for the specified StorageModel.
   * The uniqueIdOfMessage may be 0 (which means that no selection shall be stored in the configuration).
   */
  void savePreSelectedMessageForStorageModel( const StorageModel * storageModel, unsigned long uniqueIdOfMessage );

  // aggregation sets management
  const Aggregation * aggregationForStorageModel( const StorageModel *storageModel, bool *storageUsesPrivateAggregation );
  void saveAggregationForStorageModel( const StorageModel *storageModel, const QString &id, bool storageUsesPrivateAggregation );
  const Aggregation * defaultAggregation();
  const Aggregation * aggregation( const QString &id );

  void addAggregation( Aggregation *set );
  void removeAllAggregations();

  const QHash< QString, Aggregation * > & aggregations() const
    { return mAggregations; };

  /**
   * This is called by the aggregation configuration dialog
   * once the sets have been changed.
   */
  void aggregationsConfigurationCompleted();

  // sort order management
  const SortOrder sortOrderForStorageModel( const StorageModel *storageModel, bool *storageUsesPrivateSortOrder );
  void saveSortOrderForStorageModel( const StorageModel *storageModel,
                                     const SortOrder& order, bool storageUsesPrivateSortOrder );

  // theme sets management
  const Theme * themeForStorageModel( const StorageModel *storageModel, bool *storageUsesPrivateTheme );
  void saveThemeForStorageModel( const StorageModel *storageModel, const QString &id, bool storageUsesPrivateTheme );
  const Theme * defaultTheme();
  const Theme * theme( const QString &id );

  void addTheme( Theme *set );
  void removeAllThemes();

  const QHash< QString, Theme * > & themes() const
    { return mThemes; };

  /**
   * This is called by the theme configuration dialog
   * once the sets have been changed.
   */
  void themesConfigurationCompleted();

protected slots:
  /**
   * Reloads the global configuration from the config files (so we assume it has changed)
   * The settings private to MessageList (like Themes or Aggregations) aren't reloaded.
   * If the global configuration has changed then all the views are reloaded.
   */
  void reloadGlobalConfiguration();

  /**
   * Explicitly reloads the contents of all the widgets.
   */
  void reloadAllWidgets();

signals:
  void aggregationsChanged();
  void themesChanged();

private:
  // internal configuration stuff
  void loadConfiguration();
  void saveConfiguration();
  void loadGlobalConfiguration();
  void saveGlobalConfiguration();

  // internal option set management
  void createDefaultAggregations();
  void createDefaultThemes();
};

} // namespace Core

} // namespace MessageList

#endif //!__MESSAGELIST_CORE_MANAGER_H__