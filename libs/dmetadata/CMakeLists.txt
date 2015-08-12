#
# Copyright (c) 2010-2015 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015 by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libdmetadata_SRCS
    metadatasettings.cpp
    metadatasettingscontainer.cpp
    metadatainfo.cpp
    infocontainer.cpp
    dmetadata.cpp
    dmetadatasettings.cpp
    dmetadatasettingscontainer.cpp
    geodetictools.cpp
    template.cpp
    captionvalues.cpp
)

add_library(dmetadata_src OBJECT ${libdmetadata_SRCS})
