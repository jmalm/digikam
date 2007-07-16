/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-02-02
 * Description : a pop-up menu to show stars rating selection.
 * 
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QString>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>

// KDE includes.

#include <klocale.h>
#include <kstandarddirs.h>

// Local includes.

#include "constants.h"
#include "themeengine.h"
#include "ratingpopupmenu.h"
#include "ratingpopupmenu.moc"

namespace Digikam
{

RatingPopupMenu::RatingPopupMenu(QWidget* parent)
               : Q3PopupMenu(parent)
{
    QString ratingPixPath = KStandardDirs::locate("data", "digikam/data/rating.png");

    QAction *action = addAction(i18n("None"));
    action->setData(0);

    QBitmap starbm(ratingPixPath);
    QBitmap clearbm(starbm.width(), starbm.height());
    clearbm.clear();

    for (int i = 1 ; i <= RatingMax ; i++)
    {
        QPixmap pix(starbm.width() * 5, starbm.height());
        pix.fill(ThemeEngine::componentData()->textSpecialRegColor());
        QBitmap mask(starbm.width() * 5, starbm.height());
        QPainter painter(&mask);
        painter.drawTiledPixmap(0, 0, 
                                i*starbm.width(), pix.height(), 
                                starbm);
        painter.drawTiledPixmap(i*starbm.width(), 0, 
                                5*starbm.width()-i*starbm.width(), pix.height(), 
                                clearbm);
        painter.end();
        pix.setMask(mask);
        QAction *action = addAction(pix, QString(), this, SLOT(slotRatingTriggered()));
        action->setData(i);
    }
}

RatingPopupMenu::~RatingPopupMenu()
{
}

void RatingPopupMenu::slotRatingTriggered()
{
    int r = qobject_cast<QAction*>(sender())->data().toInt();
    emit rating(r);
}

}  // namespace Digikam
