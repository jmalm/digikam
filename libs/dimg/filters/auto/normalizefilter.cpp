/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-24-01
 * Description : normalize image filter.
 *
 * Copyright (C) 2005-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "normalizefilter.h"

// C++ includes

#include <cstdio>
#include <cmath>

// KDE includes

#include <kdebug.h>

// Local includes

#include "dimg.h"

namespace Digikam
{

NormalizeFilter::NormalizeFilter(QObject* parent)
    : DImgThreadedFilter(parent)
{
    initFilter();
}

NormalizeFilter::NormalizeFilter(DImg* orgImage, const DImg* refImage, QObject* parent)
    : DImgThreadedFilter(orgImage, parent, "NormalizeFilter"),
      m_refImage(*refImage)
{
    initFilter();
}

NormalizeFilter::~NormalizeFilter()
{
    cancelFilter();
}

void NormalizeFilter::filterImage()
{
    if (m_refImage.isNull())
    {
        m_refImage = m_orgImage;
    }

    normalizeImage();
    m_destImage = m_orgImage;
}

/** This method scales brightness values across the active
    image so that the darkest point becomes black, and the
    brightest point becomes as bright as possible without
    altering its hue. This is often a magic fix for
    images that are dim or washed out.*/
void NormalizeFilter::normalizeImage()
{
    if (m_orgImage.sixteenBit() != m_refImage.sixteenBit())
    {
        kDebug() << "Ref. image and Org. has different bits depth";
        return;
    }

    NormalizeParam param;
    uint           i;
    unsigned short range;
    int            progress;


    bool sixteenBit = m_orgImage.sixteenBit();
    int segments    = sixteenBit ? NUM_SEGMENTS_16BIT : NUM_SEGMENTS_8BIT;

    // Memory allocation.

    param.lut = new unsigned short[segments];

    // Find min. and max. values.

    param.min    = segments - 1;
    param.max    = 0;
    uint refSize = m_refImage.width() * m_refImage.height();

    if (!sixteenBit)        // 8 bits image.
    {
        uchar  red, green, blue;
        uchar* ptr = m_refImage.bits();

        for (i = 0 ; runningFlag() && (i < refSize) ; ++i)
        {
            blue  = ptr[0];
            green = ptr[1];
            red   = ptr[2];

            if (red < param.min)
            {
                param.min = red;
            }

            if (red > param.max)
            {
                param.max = red;
            }

            if (green < param.min)
            {
                param.min = green;
            }

            if (green > param.max)
            {
                param.max = green;
            }

            if (blue < param.min)
            {
                param.min = blue;
            }

            if (blue > param.max)
            {
                param.max = blue;
            }

            ptr += 4;
        }
    }
    else                    // 16 bits image.
    {
        unsigned short  red, green, blue;
        unsigned short* ptr = (unsigned short*)m_refImage.bits();

        for (i = 0 ; runningFlag() && (i < refSize) ; ++i)
        {
            blue  = ptr[0];
            green = ptr[1];
            red   = ptr[2];

            if (red < param.min)
            {
                param.min = red;
            }

            if (red > param.max)
            {
                param.max = red;
            }

            if (green < param.min)
            {
                param.min = green;
            }

            if (green > param.max)
            {
                param.max = green;
            }

            if (blue < param.min)
            {
                param.min = blue;
            }

            if (blue > param.max)
            {
                param.max = blue;
            }

            ptr += 4;
        }
    }

    // Calculate LUT.

    if (runningFlag())
    {
        range = (unsigned short)(param.max - param.min);

        if (range != 0)
        {
            for (int x = (int)param.min ; x <= (int)param.max ; ++x)
            {
                param.lut[x] = (unsigned short)((segments - 1) * (x - param.min) / range);
            }
        }
        else
        {
            param.lut[(int)param.min] = (unsigned short)param.min;
        }
    }

    uchar* data = m_orgImage.bits();
    int w       = m_orgImage.width();
    int h       = m_orgImage.height();
    uint size   = w * h;

    // Apply LUT to image.

    if (!sixteenBit)        // 8 bits image.
    {
        uchar  red, green, blue;
        uchar* ptr = data;

        for (i = 0 ; runningFlag() && (i < size) ; ++i)
        {
            blue   = ptr[0];
            green  = ptr[1];
            red    = ptr[2];

            ptr[0] = param.lut[blue];
            ptr[1] = param.lut[green];
            ptr[2] = param.lut[red];

            ptr += 4;

            progress = (int)(((double)i * 100.0) / size);

            if (progress % 5 == 0)
            {
                postProgress(progress);
            }
        }
    }
    else                    // 16 bits image.
    {
        unsigned short  red, green, blue;
        unsigned short* ptr = (unsigned short*)data;

        for (i = 0 ; runningFlag() && (i < size) ; ++i)
        {
            blue   = ptr[0];
            green  = ptr[1];
            red    = ptr[2];

            ptr[0] = param.lut[blue];
            ptr[1] = param.lut[green];
            ptr[2] = param.lut[red];

            ptr += 4;

            progress = (int)(((double)i * 100.0) / size);

            if (progress % 5 == 0)
            {
                postProgress(progress);
            }
        }
    }

    delete [] param.lut;
}

FilterAction NormalizeFilter::filterAction()
{
    return DefaultFilterAction<NormalizeFilter>();
}

void NormalizeFilter::readParameters(const Digikam::FilterAction& /*action*/)
{
    return;
}


}  // namespace Digikam
