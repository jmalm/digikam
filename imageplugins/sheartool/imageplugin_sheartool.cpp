/* ============================================================
 * File  : imageplugin_sheartool.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2004-11-23
 * Description : 
 * 
 * Copyright 2004 by Gilles Caulier
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

// KDE includes.
  
#include <klocale.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kaction.h>
#include <kcursor.h>
#include <kdebug.h>

// Local includes.

#include "imageeffect_sheartool.h"
#include "imageplugin_sheartool.h"

K_EXPORT_COMPONENT_FACTORY( digikamimageplugin_sheartool,
                            KGenericFactory<ImagePlugin_ShearTool>("digikamimageplugin_sheartool"));

ImagePlugin_ShearTool::ImagePlugin_ShearTool(QObject *parent, const char*, const QStringList &)
                     : Digikam::ImagePlugin(parent, "ImagePlugin_ShearTool")
{
    new KAction(i18n("Shear..."), 0, 
                this, SLOT(slotShearTool()),
                actionCollection(), "imageplugin_sheartool");
                
    kdDebug() << "ImagePlugin_ShearTool plugin loaded" << endl;
}

ImagePlugin_ShearTool::~ImagePlugin_ShearTool()
{
}

QStringList ImagePlugin_ShearTool::guiDefinition() const
{
    QStringList guiDef;
    guiDef.append("MenuBar/Menu/&Transform/Transform/Action/imageplugin_sheartool/ ");
    return guiDef;
}

void ImagePlugin_ShearTool::slotShearTool()
{
    DigikamShearToolImagesPlugin::ImageEffect_ShearTool dlg(parentWidget());
    dlg.exec();
}


#include "imageplugin_sheartool.moc"
