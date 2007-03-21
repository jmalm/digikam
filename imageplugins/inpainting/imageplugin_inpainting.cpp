/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2005-03-30
 * Description : 
 * 
 * Copyright 2005-2007 by Gilles Caulier 
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

// Local includes.

#include "ddebug.h"
#include "imageeffect_inpainting.h"
#include "imageplugin_inpainting.h"
#include "imageplugin_inpainting.moc"

K_EXPORT_COMPONENT_FACTORY(digikamimageplugin_inpainting,
                           KGenericFactory<ImagePlugin_InPainting>("digikamimageplugin_inpainting"));

ImagePlugin_InPainting::ImagePlugin_InPainting(QObject *parent, const char*, const QStringList &)
                      : Digikam::ImagePlugin(parent, "ImagePlugin_InPainting")
{
    m_inPaintingAction = new KAction(i18n("Inpainting..."), "inpainting",
                             CTRL+Key_E, 
                             this, SLOT(slotInPainting()),
                             actionCollection(), "imageplugin_inpainting");
    
    m_inPaintingAction->setWhatsThis( i18n( "This filter can be used to inpaint a part in a photo. "
                                            "Select a region to inpaint to use this option.") );                
                
    setXMLFile( "digikamimageplugin_inpainting_ui.rc" );                                
    
    DDebug() << "ImagePlugin_InPainting plugin loaded" << endl;
}

ImagePlugin_InPainting::~ImagePlugin_InPainting()
{
}

void ImagePlugin_InPainting::setEnabledActions(bool enable)
{
    m_inPaintingAction->setEnabled(enable);
}

void ImagePlugin_InPainting::slotInPainting()
{
    DigikamInPaintingImagesPlugin::ImageEffect_InPainting::inPainting(parentWidget());
}

