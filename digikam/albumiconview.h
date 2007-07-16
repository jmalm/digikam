/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : album icon view 
 * 
 * Copyright (C) 2002-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2002-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef ALBUMICONVIEW_H
#define ALBUMICONVIEW_H

// KDE includes.

#include <QRect>
#include <QFont>
#include <QList>

// Local includes.

#include "iconview.h"
#include "imageinfo.h"
#include "imageinfolist.h"
#include "albumitemhandler.h"

class QResizeEvent;
class QDragMoveEvent;
class QDropEvent;
class QPoint;
class QString;
class QPixmap;

namespace KIO
{
class Job;
}

namespace Digikam
{

class AlbumIconItem;
class AlbumSettings;
class ThumbnailSize;
class Album;
class PixmapManager;
class AlbumIconViewPrivate;

class AlbumIconView : public IconView,
                      public AlbumItemHandler
{
    Q_OBJECT

public:

    AlbumIconView(QWidget* parent);
    ~AlbumIconView();

    void setAlbum(Album* album);
    void setThumbnailSize(const ThumbnailSize& thumbSize);
    ThumbnailSize thumbnailSize() const;

    void applySettings(const AlbumSettings* settings);
    const AlbumSettings* settings() const;

    void refreshIcon(AlbumIconItem* item);

    AlbumIconItem* firstSelectedItem() const;

    KUrl::List allItems();
    KUrl::List selectedItems();

    ImageInfoList allImageInfos() const;
    ImageInfoList selectedImageInfos() const;

    void refresh();
    void refreshItems(const KUrl::List& itemList);

    QRect    itemRect() const;
    QRect    itemRatingRect() const;
    QRect    itemDateRect() const;
    QRect    itemModDateRect() const;
    QRect    itemPixmapRect() const;
    QRect    itemNameRect() const;
    QRect    itemCommentsRect() const;
    QRect    itemResolutionRect() const;
    QRect    itemSizeRect() const;
    QRect    itemTagRect() const;
    QRect    bannerRect() const;

    QPixmap* itemBaseRegPixmap() const;
    QPixmap* itemBaseSelPixmap() const;
    QPixmap  bannerPixmap() const;
    QPixmap  ratingPixmap() const;

    QFont    itemFontReg() const;
    QFont    itemFontCom() const;
    QFont    itemFontXtra() const;

    void     clear(bool update=true);

    AlbumIconItem* findItem(const QPoint& pos);
    AlbumIconItem* findItem(const QString& url) const;
    AlbumIconItem* nextItemToThumbnail() const;
    PixmapManager* pixmapManager() const;

    void insertSelectionToLightTable();
    void insertToLightTable(const ImageInfoList& list, const ImageInfo &current);

signals:

    void signalPreviewItem(AlbumIconItem*);
    void signalItemsAdded();
    void signalItemDeleted(AlbumIconItem*);
    void signalCleared();
    void signalProgressBarMode(int, const QString&);
    void signalProgressValue(int);
    void signalItemsUpdated(const KUrl::List&);

public slots:

    void slotSetExifOrientation(int orientation);
    void slotRename(AlbumIconItem* item);
    void slotDeleteSelectedItems(bool deletePermanently = false);
    void slotDeleteSelectedItemsDirectly(bool useTrash);
    void slotDisplayItem(AlbumIconItem *item=0);
    void slotAlbumModified();
    void slotSetAlbumThumbnail(AlbumIconItem *iconItem);
    void slotCopy();
    void slotPaste();

    void slotAssignRating(int rating);
    void slotAssignRatingNoStar();
    void slotAssignRatingOneStar();
    void slotAssignRatingTwoStar();
    void slotAssignRatingThreeStar();
    void slotAssignRatingFourStar();
    void slotAssignRatingFiveStar();

protected:

    void resizeEvent(QResizeEvent* e);

    // DnD
    void startDrag();
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDropEvent(QDropEvent *e);

    bool acceptToolTip(IconItem *item, const QPoint &mousePos);

private slots:

    void slotImageListerNewItems(const ImageInfoList& itemList);
    void slotImageListerDeleteItem(const ImageInfo &item);
    void slotImageListerClear();

    void slotDoubleClicked(IconItem *item);
    void slotRightButtonClicked(const QPoint& pos);
    void slotRightButtonClicked(IconItem *item, const QPoint& pos);

    void slotGotThumbnail(const KUrl& url);
    void slotSelectionChanged();

    void slotFilesModified();
    void slotFilesModified(const KUrl& url);
    void slotImageWindowURLChanged(const KUrl &url);

    void slotShowToolTip(IconItem* item);

    void slotThemeChanged();

    void slotAssignTag(int tagID);
    void slotRemoveTag(int tagID);

    void slotDIOResult(KIO::Job* job);
    void slotRenamed(KIO::Job*, const KUrl &, const KUrl&);

    void slotImageAttributesChanged(qlonglong imageId);
    void slotAlbumImagesChanged(int albumId);

private:

    void updateBannerRectPixmap();
    void updateItemRectsPixmap();
    void changeTagOnImageInfos(const ImageInfoList &list, const QList<int> &tagIDs, bool addOrRemove, bool progress);

private:

    AlbumIconViewPrivate *d;
};

}  // namespace Digikam

#endif // ALBUMICONVIEW_H
