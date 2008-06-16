/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Find Duplicates View.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#define ICONSIZE 64

// Qt includes.

#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <QProgressBar>
#include <QTreeWidget>

// KDE include.

#include <klocale.h>
#include <kdialog.h>

// Local includes.

#include "album.h"
#include "albummanager.h"
#include "albumdb.h"
#include "databaseaccess.h"
#include "ddebug.h"
#include "imagelister.h"
#include "haariface.h"
#include "searchxml.h"
#include "searchtextbar.h"
#include "findduplicatesview.h"
#include "findduplicatesview.moc"

namespace Digikam
{

FindDuplicatesAlbumItem::FindDuplicatesAlbumItem(QTreeWidget* parent, SAlbum* album)
                       : /*QObject(parent), */TreeFolderItem(parent, QString())
{
    m_album = album;
    if (m_album)
    {
        m_album->setExtraData(treeWidget(), this);

        m_info = ImageInfo(m_album->title().toLongLong());
        setText(0, m_info.name());

        /*connect(ThumbnailLoadThread::defaultThread(), SIGNAL(signalThumbnailLoaded(const LoadingDescription&, const QPixmap&)),
                this, SLOT(slotThumbnailLoaded(const LoadingDescription&, const QPixmap&)));

        ThumbnailLoadThread::defaultThread()->find(m_info.fileUrl().path());*/
    }
}

FindDuplicatesAlbumItem::~FindDuplicatesAlbumItem()
{
    if (m_album)
        m_album->removeExtraData(treeWidget());
}
/*
void FindDuplicatesAlbumItem::slotThumbnailLoaded(const LoadingDescription& desc, const QPixmap& pix)
{
    if (desc.filePath == m_info.fileUrl().path())
    {
        QPixmap thumb = pix;
        thumb         = thumb.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio);
        QPixmap pixmap(ICONSIZE+2, ICONSIZE+2);
        pixmap.fill(Qt::color0);
        QPainter p(&pixmap);
        p.drawPixmap((pixmap.width()/2) - (thumb.width()/2),
                     (pixmap.height()/2) - (thumb.height()/2), thumb);
        setIcon(0, QIcon(pixmap));
    }
}
*/
SAlbum* FindDuplicatesAlbumItem::album() const
{
    return m_album;
}

int FindDuplicatesAlbumItem::id() const
{
    return album() ? album()->id() : 0;
}

// ------------------------------------------------------------------------

class FindDuplicatesViewPriv
{

public:

    FindDuplicatesViewPriv()
    {
        listView           = 0;
        scanDuplicatesBtn  = 0;
        updateFingerPrtBtn = 0;
        progressBar        = 0;
    }

    QPushButton         *scanDuplicatesBtn;
    QPushButton         *updateFingerPrtBtn;

    QProgressBar        *progressBar;

    QTreeWidget         *listView;
};

FindDuplicatesView::FindDuplicatesView(QWidget *parent)
                  : QWidget(parent)
{
    d = new FindDuplicatesViewPriv;

    setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout *grid = new QGridLayout(this);
    d->listView       = new QTreeWidget(this);
    d->listView->setRootIsDecorated(false);
    d->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setIconSize(QSize(ICONSIZE, ICONSIZE));
    d->listView->setColumnCount(1);
    d->listView->setHeaderLabels(QStringList() << i18n("My Duplicates Searches"));

    d->listView->setWhatsThis(i18n("<p>This shows all duplicates items found in whole collections."));

    d->updateFingerPrtBtn = new QPushButton(i18n("Update finger-prints"), this);
    d->updateFingerPrtBtn->setIcon(KIcon("run-build"));
    d->updateFingerPrtBtn->setWhatsThis(i18n("<p>Use this button to scan whole collection to find all "
                                              "duplicates items."));

    d->scanDuplicatesBtn = new QPushButton(i18n("Find duplicates"), this);
    d->scanDuplicatesBtn->setIcon(KIcon("system-search"));
    d->scanDuplicatesBtn->setWhatsThis(i18n("<p>Use this button to scan whole collection to find all "
                                            "duplicates items."));

    d->progressBar = new QProgressBar(this);

    grid->addWidget(d->listView,           0, 0, 1, 3);
    grid->addWidget(d->updateFingerPrtBtn, 1, 0, 1, 3);
    grid->addWidget(d->scanDuplicatesBtn,  2, 0, 1, 1);
    grid->addWidget(d->progressBar,        2, 1, 1, 2);
    grid->setRowStretch(0, 10);
    grid->setColumnStretch(1, 10);
    grid->setMargin(KDialog::spacingHint());
    grid->setSpacing(KDialog::spacingHint());

    // ---------------------------------------------------------------

    connect(d->updateFingerPrtBtn, SIGNAL(clicked()),
            this, SIGNAL(signalUpdateFingerPrints()));

    connect(d->scanDuplicatesBtn, SIGNAL(clicked()),
            this, SLOT(slotFindDuplicates()));

    connect(d->listView, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotDuplicatesAlbumActived(QTreeWidgetItem*, int)));
}

FindDuplicatesView::~FindDuplicatesView()
{
    delete d;
}

void FindDuplicatesView::populateTreeView()
{
    const AlbumList& aList = AlbumManager::instance()->allSAlbums();

    for (AlbumList::const_iterator it = aList.begin(); it != aList.end(); ++it)
    {
        SAlbum* salbum = dynamic_cast<SAlbum*>(*it);
        if (salbum && salbum->isDuplicatesSearch())
        {
            new FindDuplicatesAlbumItem(d->listView, salbum);
        }
    }
}

void FindDuplicatesView::slotFindDuplicates()
{
    d->listView->clear();
    d->scanDuplicatesBtn->setEnabled(false);
    d->updateFingerPrtBtn->setEnabled(false);
    d->progressBar->setEnabled(true);

    AlbumList albums = AlbumManager::instance()->allPAlbums();
    QStringList idsStringList;
    foreach(Album *a, albums)
        idsStringList << QString::number(a->id());

    KIO::Job *job = ImageLister::startListJob(DatabaseUrl::searchUrl(-1));
    job->addMetaData("duplicates", "true");
    job->addMetaData("albumids", idsStringList.join(","));
    job->addMetaData("threshold", QString::number(0.5));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotDuplicatesSearchResult(KJob*)));

    connect(job, SIGNAL(totalAmount(KJob *, KJob::Unit, qulonglong)),
            this, SLOT(slotDuplicatesSearchTotalAmount(KJob *, KJob::Unit, qulonglong)));

    connect(job, SIGNAL(processedAmount(KJob *, KJob::Unit, qulonglong)),
            this, SLOT(slotDuplicatesSearchProcessedAmount(KJob *, KJob::Unit, qulonglong)));
}

void FindDuplicatesView::slotDuplicatesSearchTotalAmount(KJob* /*job*/, KJob::Unit /*unit*/, qulonglong amount)
{
    d->progressBar->setMinimum(0);
    d->progressBar->setMaximum(amount);
}

void FindDuplicatesView::slotDuplicatesSearchProcessedAmount(KJob* /*job*/, KJob::Unit /*unit*/, qulonglong amount)
{
    d->progressBar->setValue(amount);
}

void FindDuplicatesView::slotDuplicatesSearchResult(KJob*)
{
    d->scanDuplicatesBtn->setEnabled(true);
    d->updateFingerPrtBtn->setEnabled(true);
    d->progressBar->reset();
    d->progressBar->setEnabled(false);
    populateTreeView();
}

void FindDuplicatesView::slotDuplicatesAlbumActived(QTreeWidgetItem* item, int)
{
    FindDuplicatesAlbumItem* sitem = dynamic_cast<FindDuplicatesAlbumItem*>(item);
    if (sitem)
        AlbumManager::instance()->setCurrentAlbum(sitem->album());
}

}  // NameSpace Digikam
