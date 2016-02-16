/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IMPORTWIZARD_DLG_H
#define IMPORTWIZARD_DLG_H

// Qt includes

#include <QString>
#include <QWidget>
#include <QWizard>

// Local includes

#include "expoblendingactions.h"

namespace Digikam
{

class ExpoBlendingManager;

class ImportWizardDlg : public QWizard
{
    Q_OBJECT

public:

    explicit ImportWizardDlg(ExpoBlendingManager* const mngr, QWidget* const parent = 0);
    ~ImportWizardDlg();

    QList<QUrl> itemUrls() const;

    ExpoBlendingManager* manager() const;

    virtual bool validateCurrentPage();

private Q_SLOTS:

    void slotCurrentIdChanged(int);
    void slotIntroPageIsValid(bool);
    void slotItemsPageIsValid(bool);
    void slotPreProcessed(const ItemUrlsMap&);

private:

    class ImportWizardDlgPriv;
    ImportWizardDlgPriv* const d;
};

}   // namespace Digikam

#endif /* IMPORTWIZARD_DLG_H */
