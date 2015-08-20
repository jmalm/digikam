/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-01-05
 * Description : Metadata handling
 *
 * Copyright (C) 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef METADATAHUB_H
#define METADATAHUB_H

// Qt includes

#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QMap>

// Local includes

#include "metadatasettings.h"
#include "captionvalues.h"
#include "dmetadata.h"
#include "dimg.h"

namespace Digikam
{

class ApplicationSettings;
class ImageInfo;
class Template;

class MetadataHub
{
public:

    /**
        The status enum describes the result of joining several metadata sets.
        If only one set has been added, the status is always MetadataAvailable.
        If no set has been added, the status is always MetadataInvalid
    */
    enum Status
    {
        MetadataInvalid,   /// not yet filled with any value
        MetadataAvailable /// only one data set has been added, or a common value is available
//        MetadataDisjoint   /// No common value is available. For rating and dates, the interval is available.
    };

public:

    /**
        Describes the complete status of a Tag: The metadata status, and the fact if it has the tag or not.
    */
    class TagStatus
    {
    public:

        explicit TagStatus(Status status, bool hasTag = false)
            : status(status),
              hasTag(hasTag)
        {
        };

        TagStatus()
            : status(MetadataInvalid),
              hasTag(false)
        {
        };

        Status status;
        bool   hasTag;

        bool operator==(const TagStatus& otherstatus) const
        {
            return otherstatus.status == status &&
                   otherstatus.hasTag == hasTag;
        }

        bool operator==(const Status& otherstatus) const
        {
            return otherstatus == status;
        }
    };

public:

    enum WriteMode
    {
        /**
            Write all available information
        */
        FullWrite,
        /**
            Do a full write if and only if
                - metadata fields changed
                - the changed fields shall be written according to write settings
            "Changed" in this context means changed by one of the set... methods,
            the load() methods are ignored for this attribute.
            This mode allows to avoid write operations when e.g. the user does not want
            keywords to be written and only changes keywords.
        */
        FullWriteIfChanged,
        /**
            Write only the changed parts.
            Metadata fields which cannot be changed from MetadataHub (photographer ID etc.)
            will never be written
        */
        PartialWrite
    };

public:

    /**
        Constructs a MetadataHub.
        @param dbmode Determines if the database may be accessed or not. See the enum description above.
    */
    MetadataHub();
    virtual ~MetadataHub();

    /// Copies by value - no sharing involved.
    MetadataHub& operator=(const MetadataHub&);
    MetadataHub(const MetadataHub&);

    /**
     * Creates a copy (as always, by value) of this hub.
     * Shall be reimplemented by subclasses to return an object of the correct type.
     */
    virtual MetadataHub* clone() const;

    void reset();

    // --------------------------------------------------

    /**
        Add metadata information contained in the ImageInfo object.
        This method (or in combination with the other load methods)
        can be called multiple times on the same MetadataHub object.
        In this case, the metadata will be combined.
    */
    void load(const ImageInfo& info);

//    /**
//        Add metadata information from the DMetadata object
//    */
//    void load(const DMetadata& metadata);

//    /**
//        Load metadata information from the given file.
//        (Uses DMetadata, QFileInfo)
//        @returns True if the metadata could be loaded
//    */
//    bool load(const QString& filePath, const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());

    // --------------------------------------------------

    /**
        Applies the set of metadata contained in this MetadataHub
        to the given ImageInfo object.
        @return Returns true if the info object has been changed
    */
    bool write(ImageInfo info, WriteMode writeMode = FullWrite);

    /**
     * @brief writeToMetadata - write to metadata using image info to retrieve tags and filepath
     *                          use this method when multiple image infos are loaded in hub
     * @param info - image info to retrieve current tags
     * @param writeMode
     * @param settings
     * @return true           - if everything is succesfull
     */
    bool writeToMetadata(ImageInfo info, WriteMode writeMode = FullWrite,
               const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());
    /**
        Applies the set of metadata contained in this MetadataHub
        to the given DMetadata object.
        The MetadataSettingsContainer determine whether data is actually
        set or not.
        The following metadata fields may be set (depending on settings):
        - Comment
        - Date
        - Rating
        - Tags
        - Photographer ID (data from settings)
        - Credits (data from settings)

        The data fields taken from this MetadataHub object are only set if
        their status is MetadataAvailable.
        If the status is MetadataInvalid or MetadataDisjoint, the respective
        metadata field is not touched.
        @return Returns true if the metadata object has been touched
    */
    bool write(DMetadata& metadata, WriteMode writeMode = FullWrite,
               const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());

    /**
        Constructs a DMetadata object for given filePath,
        calls the above method, writes the changes out to the file,
        and notifies the ImageAttributesWatch.
        WARNING: Do not use this method when multiple image infos are loaded
                 It will result in disjoint tags not being written
                 Use writeToMetadata(Image info ...) instead
        @return Returns if the file has been touched
    */
    bool write(const QString& filePath, WriteMode writeMode = FullWrite,
               const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());

    /**
        Constructs a DMetadata object from the metadata stored in the given DImg object,
        calls the above method, and changes the stored metadata in the DImg object.
        @return Returns if the DImg object has been touched
    */
    bool write(DImg& image, WriteMode writeMode = FullWrite,
               const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());

    /**
        Will write only Tags to image. Used by TagsManager to write tags to image
        Other metadata are not updated.
        @return if tags were successfully written.
    */
    bool writeTags(const QString& filePath, WriteMode writeMode = FullWrite,
                   const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());

    /**
     * @brief writeTags - used to deduplicate code from writeTags and usual write, all write to tags
     *                    operations must be done here
     * @param metadata  - DMetadata object that apply changes
     * @param saveTags  - save switch
     * @return          - if tags were successfully set
     */
    bool writeTags(DMetadata& metadata, bool saveTags);

    /**
     * @brief cleanupTags - remove duplicates and obsolete tags before setting metadata
     * @param toClean     - tag list to be cleared and de-duplicated
     * @return            - clean tag list
     */
    QStringList cleanupTags(const QStringList& toClean);
    /**
        With the currently applied changes, the given writeMode and settings,
        returns if write(DMetadata), write(QString) or write(DImg) will actually
        apply any changes.
    */
    bool willWriteMetadata(WriteMode writeMode,
                           const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings()) const;

    /**
     * @brief writeToBaloo - write tags, comments and rating to KDE Nepomuk replacement: Baloo
     * @param filePath     - path to file to add comments, tags and rating
     * @param settings     - metadata settings to be set
     */
    void writeToBaloo(const QString& filePath, const MetadataSettingsContainer& settings = MetadataSettings::instance()->settings());

    // --------------------------------------------------

//    Status dateTimeStatus()   const;
//    Status titlesStatus()     const;
//    Status commentsStatus()   const;
//    Status pickLabelStatus()  const;
//    Status colorLabelStatus() const;
//    Status ratingStatus()     const;
//    Status templateStatus()   const;

//    TagStatus tagStatus(int albumId) const;
//    TagStatus tagStatus(const QString& tagPath) const;

    // --------------------------------------------------

    /**
//        Set dateTime to the given value, and the dateTime status to MetadataAvailable
//    */
    void setDateTime(const QDateTime& dateTime, Status status = MetadataAvailable);
    void setTitles(const CaptionsMap& titles, Status status = MetadataAvailable);
    void setComments(const CaptionsMap& comments, Status status = MetadataAvailable);
    void setPickLabel(int pickId, Status status = MetadataAvailable);
    void setColorLabel(int colorId, Status status = MetadataAvailable);
    void setRating(int rating, Status status = MetadataAvailable);
    void setMetadataTemplate(const Template& t, Status status = MetadataAvailable);
    void setTag(int albumID, bool hasTag, Status status = MetadataAvailable);

    /**
        Resets the information that metadata fields have been changed with one of the
        set... methods (see commentChanged, dateTimeChanged etc.)
    */
    void resetChanged();

    /**
        Dedicated method to set face rectangles from database
        When called from outside the metadatahub and  ImageInfo is cached,
        method dimension() can return wrong values, QSize must be specified
        manually
     */
    void loadFaceTags(const ImageInfo& info, const QSize& size);

protected:

    void load(const QDateTime& dateTime,
              const CaptionsMap& titles, const CaptionsMap& comment,
              int colorLabel, int pickLabel,
              int rating, const Template& t);
    void loadTags(const QList<int>& loadedTagIds);
    void loadTags(const QStringList& loadedTagPaths);
    void notifyTagDeleted(int id);

    virtual void applyChangeNotifications();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // METADATAHUB_H
