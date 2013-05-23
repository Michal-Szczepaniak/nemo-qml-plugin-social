/*
 * Copyright (C) 2013 Jolla Ltd. <chris.adams@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "facebookalbuminterface_p.h"
#include "facebookontology_p.h"
#include "facebookinterface.h"
// <<< include
#include <QtCore/QDebug>
// >>> include

FacebookAlbumInterfacePrivate::FacebookAlbumInterfacePrivate(FacebookAlbumInterface *q)
    : IdentifiableContentItemInterfacePrivate(q)
    , action(FacebookInterfacePrivate::NoAction)
// <<< custom
    , from(0)
    , albumType(FacebookAlbumInterface::Normal)
    , liked(false)
// >>> custom
{
}

void FacebookAlbumInterfacePrivate::finishedHandler()
{
// <<< finishedHandler
    Q_Q(FacebookAlbumInterface);
    if (!reply()) {
        // if an error occurred, it might have been deleted by the error handler.
        qWarning() << Q_FUNC_INFO << "network request finished but no reply";
        return;
    }

    QByteArray replyData = reply()->readAll();
    deleteReply();
    bool ok = false;
    QVariantMap responseData = ContentItemInterfacePrivate::parseReplyData(replyData, &ok);
    if (!ok)
        responseData.insert("response", replyData);

    switch (action) {
        case FacebookInterfacePrivate::LikeAction:        // flow down.
        case FacebookInterfacePrivate::DeleteLikeAction:  // flow down.
        case FacebookInterfacePrivate::DeletePhotoAction: // flow down.
        case FacebookInterfacePrivate::DeleteCommentAction: {
            if (replyData == QString(QLatin1String("true"))) {
                status = SocialNetworkInterface::Idle;
                if (action == FacebookInterfacePrivate::LikeAction) {
                    liked = true;
                    emit q->likedChanged();
                } else if (action == FacebookInterfacePrivate::DeleteLikeAction) {
                    liked = false;
                    emit q->likedChanged();
                }
                emit q->statusChanged();
                emit q->responseReceived(responseData);
            } else {
                error = SocialNetworkInterface::RequestError;
                errorMessage = QLatin1String("Album: request failed");
                status = SocialNetworkInterface::Error;
                emit q->statusChanged();
                emit q->errorChanged();
                emit q->errorMessageChanged();
                emit q->responseReceived(responseData);
            }
        }
        break;

        case FacebookInterfacePrivate::UploadPhotoAction: // flow down.
        case FacebookInterfacePrivate::UploadCommentAction: {
            if (!ok || responseData.value("id").toString().isEmpty()) {
                // failed.
                error = SocialNetworkInterface::RequestError;
                errorMessage = action == FacebookInterfacePrivate::UploadCommentAction
                    ? QLatin1String("Album: add comment request failed")
                    : QLatin1String("Album: add photo request failed");
                status = SocialNetworkInterface::Error;
                emit q->statusChanged();
                emit q->errorChanged();
                emit q->errorMessageChanged();
                emit q->responseReceived(responseData);
            } else {
                // succeeded.
                status = SocialNetworkInterface::Idle;
                emit q->statusChanged();
                emit q->responseReceived(responseData);
            }
        }
        break;

        default: {
            error = SocialNetworkInterface::OtherError;
            errorMessage = QLatin1String("Request finished but no action currently in progress");
            status = SocialNetworkInterface::Error;
            emit q->statusChanged();
            emit q->errorChanged();
            emit q->errorMessageChanged();
            emit q->responseReceived(responseData);
        }
        break;
    }
// >>> finishedHandler
}
void FacebookAlbumInterfacePrivate::emitPropertyChangeSignals(const QVariantMap &oldData,
                                                              const QVariantMap &newData)
{
    Q_Q(FacebookAlbumInterface);
    QString oldName = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_NAME).toString();
    QString newName = newData.value(FACEBOOK_ONTOLOGY_ALBUM_NAME).toString();
    QString oldDescription = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_DESCRIPTION).toString();
    QString newDescription = newData.value(FACEBOOK_ONTOLOGY_ALBUM_DESCRIPTION).toString();
    QUrl oldLink = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_LINK).toUrl();
    QUrl newLink = newData.value(FACEBOOK_ONTOLOGY_ALBUM_LINK).toUrl();
    QUrl oldCoverPhoto = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_COVERPHOTO).toUrl();
    QUrl newCoverPhoto = newData.value(FACEBOOK_ONTOLOGY_ALBUM_COVERPHOTO).toUrl();
    QString oldPrivacy = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_PRIVACY).toString();
    QString newPrivacy = newData.value(FACEBOOK_ONTOLOGY_ALBUM_PRIVACY).toString();
    int oldCount = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_COUNT).toInt();
    int newCount = newData.value(FACEBOOK_ONTOLOGY_ALBUM_COUNT).toInt();
    QString oldCreatedTime = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_CREATEDTIME).toString();
    QString newCreatedTime = newData.value(FACEBOOK_ONTOLOGY_ALBUM_CREATEDTIME).toString();
    QString oldUpdatedTime = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_UPDATEDTIME).toString();
    QString newUpdatedTime = newData.value(FACEBOOK_ONTOLOGY_ALBUM_UPDATEDTIME).toString();
    bool oldCanUpload = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_CANUPLOAD).toString() == QLatin1String("true");
    bool newCanUpload = newData.value(FACEBOOK_ONTOLOGY_ALBUM_CANUPLOAD).toString() == QLatin1String("true");

    if (newName != oldName)
        emit q->nameChanged();
    if (newDescription != oldDescription)
        emit q->descriptionChanged();
    if (newLink != oldLink)
        emit q->linkChanged();
    if (newCoverPhoto != oldCoverPhoto)
        emit q->coverPhotoChanged();
    if (newPrivacy != oldPrivacy)
        emit q->privacyChanged();
    if (newCount != oldCount)
        emit q->countChanged();
    if (newCreatedTime != oldCreatedTime)
        emit q->createdTimeChanged();
    if (newUpdatedTime != oldUpdatedTime)
        emit q->updatedTimeChanged();
    if (newCanUpload != oldCanUpload)
        emit q->canUploadChanged();

// <<< emitPropertyChangeSignals
    // TODO: liked is not updated

    QVariantMap oldFromMap = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_FROM).toMap();
    QString oldFromId = oldFromMap.value(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTIDENTIFIER).toString();
    QString oldFromName = oldFromMap.value(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTNAME).toString();
    QVariantMap newFromMap = newData.value(FACEBOOK_ONTOLOGY_ALBUM_FROM).toMap();
    QString newFromId = newFromMap.value(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTIDENTIFIER).toString();
    QString newFromName = newFromMap.value(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTNAME).toString();

    // Update the from object if required
    if (newFromId != oldFromId || newFromName != oldFromName) {
        QVariantMap newFromData;
        newFromData.insert(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTTYPE, FacebookInterface::User);
        newFromData.insert(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTIDENTIFIER, newFromId);
        newFromData.insert(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTNAME, newFromName);
        qobject_cast<FacebookInterface*>(q->socialNetwork())->setFacebookContentItemData(from, newFromData);
        emit q->fromChanged();
    }

    QString oldType = oldData.value(FACEBOOK_ONTOLOGY_ALBUM_ALBUMTYPE).toString().toLower();
    QString newType = newData.value(FACEBOOK_ONTOLOGY_ALBUM_ALBUMTYPE).toString().toLower();

    if (newType != oldType) {
        if (newType == QLatin1String("normal"))
            albumType = FacebookAlbumInterface::Normal;
        else if (newType == QLatin1String("wall"))
            albumType = FacebookAlbumInterface::Wall;
        else if (newType == QLatin1String("profile"))
            albumType = FacebookAlbumInterface::Profile;
        else if (newType == QLatin1String("mobile"))
            albumType = FacebookAlbumInterface::Mobile;
        else
            albumType = FacebookAlbumInterface::Album;
        emit q->albumTypeChanged();
    }

    // Check if the user liked this album
    QString currentUserIdentifier
            = qobject_cast<FacebookInterface*>(q->socialNetwork())->currentUserIdentifier();
    bool newLiked = false;
    QVariant likes = newData.value(FACEBOOK_ONTOLOGY_CONNECTIONS_LIKES);
    if (!likes.isNull()) {
        QVariantMap likesMap = likes.toMap();
        QVariantList data = likesMap.value(FACEBOOK_ONTOLOGY_METADATA_DATA).toList();
        foreach (QVariant dataEntry, data) {
            QVariant idVariant
                    = dataEntry.toMap().value(FACEBOOK_ONTOLOGY_OBJECTREFERENCE_OBJECTIDENTIFIER);
            if (idVariant.toString() == currentUserIdentifier) {
                newLiked = true;
            }
        }
    }

    if (liked != newLiked) {
        liked = newLiked;
        emit q->likedChanged();
    }

// >>> emitPropertyChangeSignals

    // Call super class implementation
    QVariantMap oldDataWithId = oldData;
    oldDataWithId.insert(NEMOQMLPLUGINS_SOCIAL_CONTENTITEMID,
                         oldData.value(FACEBOOK_ONTOLOGY_METADATA_ID));
    QVariantMap newDataWithId = newData;
    newDataWithId.insert(NEMOQMLPLUGINS_SOCIAL_CONTENTITEMID,
                         newData.value(FACEBOOK_ONTOLOGY_METADATA_ID));
    IdentifiableContentItemInterfacePrivate::emitPropertyChangeSignals(oldDataWithId, newDataWithId);
}

//-------------------------------

/*!
    \qmltype FacebookAlbum
    \instantiates FacebookAlbumInterface
    \inqmlmodule org.nemomobile.social 1
    \brief A FacebookAlbum represents an Album object from the Facebook OpenGraph API
    
    Every album has a unique identifier, and thus an album may be
    set as the \c node (or central content item) in the Facebook
    adapter.  The content items related to an album include various
    photos or videos, likes and comments.
    
    An example of usage of a FacebookAlbum as the node in a Facebook
    model is as follows:
    
    \qml
    import QtQuick 1.1
    import org.nemomobile.social 1.0
    
    Item {
        id: root
        width: 400
        height: 800
    
        Flickable {
            anchors.top: parent.verticalCenter
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
    
            ListView {
                model: fb
                anchors.fill: parent
                delegate: Label { text: "id: " + contentItemIdentifier } // Photo ids
            }
        }
    
        Facebook {
            id: fb
            accessToken: "your access token"    // you must supply a valid access token
            nodeIdentifier: "10150146071791729" // some valid Facebook album id.
            filters: [ ContentItemTypeFilter { type: Facebook.Photo } ]
        }
    
        Component.onCompleted: {
            fb.populate()
        }
    }
    \endqml
    
    A FacebookAlbum may also be used "directly" by clients, in order to
    upload photos or comments, or like the album.  An example of direct
    usage of the FacebookAlbum type is as follows:
    
    \qml
    import QtQuick 1.1
    import org.nemomobile.social 1.0
    
    Item {
        id: root
        width: 400
        height: 800
    
        Facebook {
            id: fb
            accessToken: "your access token"    // you must supply a valid access token
        }
    
        FacebookAlbum {
            id: fba
            socialNetwork: fb
            identifier: "10150146071791729"     // some valid Facebook Album fbid
    
            onStatusChanged: {
                if (status == SocialNetwork.Idle) {
                    // could comment on the album
                    fba.uploadComment("I really like this album!")
                    // could like the album
                    fba.like()
                    // could unlike the album
                    fba.unlike()
                    // could upload another photo to the album
                    fba.uploadPhoto(fileName, "A photo I took recently")
                }
            }
        }
    }
    \endqml
*/
FacebookAlbumInterface::FacebookAlbumInterface(QObject *parent)
    : IdentifiableContentItemInterface(*(new FacebookAlbumInterfacePrivate(this)), parent)
{
// <<< constructor
    Q_D(FacebookAlbumInterface);
    d->from = new FacebookObjectReferenceInterface(this);
// >>> constructor
}

/*! \reimp */
int FacebookAlbumInterface::type() const
{
    return FacebookInterface::Album;
}

/*! \reimp */
bool FacebookAlbumInterface::remove()
{
// <<< remove
    return IdentifiableContentItemInterface::remove();
// >>> remove
}

/*! \reimp */
bool FacebookAlbumInterface::reload(const QStringList &whichFields)
{
// <<< reload
    return IdentifiableContentItemInterface::reload(whichFields);
// >>> reload
}

/*!
    \qmlmethod bool FacebookAlbumInterface::like()
    Initiates a "like" operation on the album.
    
    If the network request was started successfully, the function
    will return true and the status of the album will change to
    \c SocialNetwork::Busy.  Otherwise, the function will return
    false.*/

bool FacebookAlbumInterface::like()
{
// <<< like
    Q_D(FacebookAlbumInterface);
    bool requestMade = d->request(IdentifiableContentItemInterfacePrivate::Post,
                                  identifier(), QLatin1String("likes"));

    if (!requestMade)
        return false;

    d->action = FacebookInterfacePrivate::LikeAction;
    d->connectFinishedAndErrors();
    return true;
// >>> like
}
/*!
    \qmlmethod bool FacebookAlbumInterface::unlike()
    Initiates a "delete like" operation on the album.
    
    If the network request was started successfully, the function
    will return true and the status of the album will change to
    \c SocialNetwork::Busy.  Otherwise, the function will return
    false.*/

bool FacebookAlbumInterface::unlike()
{
// <<< unlike
    Q_D(FacebookAlbumInterface);
    bool requestMade = d->request(IdentifiableContentItemInterfacePrivate::Delete,
                                  identifier(), QLatin1String("likes"));

    if (!requestMade)
        return false;

    d->action = FacebookInterfacePrivate::DeleteLikeAction;
    d->connectFinishedAndErrors();
    return true;
// >>> unlike
}
/*!
    \qmlmethod bool FacebookAlbumInterface::uploadComment(const QString &message)
    Initiates a "post comment" operation on the album.  The comment
    will contain the specified \a message.
    
    If the network request was started successfully, the function
    will return true and the status of the album will change to
    \c SocialNetwork::Busy.  Otherwise, the function will return
    false.
    
    Once the network request completes, the \c responseReceived()
    signal will be emitted.  The \c data parameter of the signal
    will contain the \c id of the newly uploaded comment.*/

bool FacebookAlbumInterface::uploadComment(const QString &message)
{
// <<< uploadComment
    Q_D(FacebookAlbumInterface);
    QVariantMap postData;
    postData.insert("message", message);

    bool requestMade = d->request(IdentifiableContentItemInterfacePrivate::Post,
                                  identifier(), QLatin1String("comments"),
                                  QStringList(), postData);

    if (!requestMade)
        return false;

    d->action = FacebookInterfacePrivate::UploadCommentAction;
    d->connectFinishedAndErrors();
    return true;
// >>> uploadComment
}
/*!
    \qmlmethod bool FacebookAlbumInterface::removeComment(const QString &commentIdentifier)
    Initiates a "delete comment" operation on the comment specified by
    the given \a identifier.
    
    If the network request was started successfully, the function
    will return true and the status of the album will change to
    \c SocialNetwork::Busy.  Otherwise, the function will return
    false.*/

bool FacebookAlbumInterface::removeComment(const QString &commentIdentifier)
{
// <<< removeComment
    Q_D(FacebookAlbumInterface);
    bool requestMade = d->request(IdentifiableContentItemInterfacePrivate::Delete,
                                  commentIdentifier);

    if (!requestMade)
        return false;

    d->action = FacebookInterfacePrivate::DeleteCommentAction;
    d->connectFinishedAndErrors();
    return true;
// >>> removeComment
}
/*!
    \qmlmethod bool FacebookAlbumInterface::uploadPhoto(const QUrl &source, const QString &message)
    Initiates a "post photo" operation on the album.  The photo will
    be loaded from the local filesystem and uploaded to Facebook with
    its caption set to the given \a message.
    
    If the network request was started successfully, the function
    will return true and the status of the album will change to
    \c SocialNetwork::Busy.  Otherwise, the function will return
    false.
    
    Once the network request completes, the \c responseReceived()
    signal will be emitted.  The \c data parameter of the signal
    will contain the \c id of the newly uploaded photo.
    */

bool FacebookAlbumInterface::uploadPhoto(const QUrl &source, const QString &message)
{
// <<< uploadPhoto
    Q_D(FacebookAlbumInterface);
    // XXX TODO: privacy parameter?

    QVariantMap extraData; // image upload is handled specially by the facebook adapter.
    extraData.insert("isImageUpload", true);

    QVariantMap postData;
    postData.insert("source", source);
    if (!message.isEmpty())
        postData.insert("message", message);

    bool requestMade = d->request(IdentifiableContentItemInterfacePrivate::Post,
                                  identifier(), QLatin1String("photos"),
                                  QStringList(), postData, extraData);

    if (!requestMade)
        return false;

    d->action = FacebookInterfacePrivate::UploadPhotoAction;
    d->connectFinishedAndErrors();
    return true;
// >>> uploadPhoto
}
/*!
    \qmlmethod bool FacebookAlbumInterface::removePhoto(const QString &photoIdentifier)
    Initiates a "delete photo" operation on the photo specified by
    the given \a identifier.
    
    If the network request was started successfully, the function
    will return true and the status of the album will change to
    \c SocialNetwork::Busy.  Otherwise, the function will return
    false.*/

bool FacebookAlbumInterface::removePhoto(const QString &photoIdentifier)
{
// <<< removePhoto
    Q_D(FacebookAlbumInterface);
    bool requestMade = d->request(IdentifiableContentItemInterfacePrivate::Delete, photoIdentifier);

    if (!requestMade)
        return false;

    d->action = FacebookInterfacePrivate::DeletePhotoAction;
    d->connectFinishedAndErrors();
    return true;
// >>> removePhoto
}

/*!
    \qmlproperty FacebookObjectReferenceInterface * FacebookAlbumInterface::from
    Holds a reference to the user or profile which created the album.
*/
FacebookObjectReferenceInterface * FacebookAlbumInterface::from() const
{
    Q_D(const FacebookAlbumInterface);
    return d->from;
}

/*!
    \qmlproperty QString FacebookAlbumInterface::name
    Holds the name of the album.
*/
QString FacebookAlbumInterface::name() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_NAME).toString();
}

/*!
    \qmlproperty QString FacebookAlbumInterface::description
    Holds the description of the album.
*/
QString FacebookAlbumInterface::description() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_DESCRIPTION).toString();
}

/*!
    \qmlproperty QUrl FacebookAlbumInterface::link
    Holds a link to the album.
*/
QUrl FacebookAlbumInterface::link() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_LINK).toUrl();
}

/*!
    \qmlproperty QUrl FacebookAlbumInterface::coverPhoto
    Holds a link to the cover photo of an album.
*/
QUrl FacebookAlbumInterface::coverPhoto() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_COVERPHOTO).toUrl();
}

/*!
    \qmlproperty QString FacebookAlbumInterface::privacy
    Holds the privacy setting of the album.
*/
QString FacebookAlbumInterface::privacy() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_PRIVACY).toString();
}

/*!
    \qmlproperty int FacebookAlbumInterface::count
    Holds the count of the number of photos in the album.
*/
int FacebookAlbumInterface::count() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_COUNT).toInt();
}

/*!
    \qmlproperty FacebookAlbumInterface::AlbumType FacebookAlbumInterface::albumType
    Holds the type of the album.  Valid values are:
    \list
    \li FacebookAlbum.Album
    \li FacebookAlbum.Normal
    \li FacebookAlbum.Wall
    \li FacebookAlbum.Profile
    \li FacebookAlbum.Mobile
    \endlist.
*/
FacebookAlbumInterface::AlbumType FacebookAlbumInterface::albumType() const
{
    Q_D(const FacebookAlbumInterface);
    return d->albumType;
}

/*!
    \qmlproperty QString FacebookAlbumInterface::createdTime
    Holds the creation time of the album in an ISO8601-formatted string.
*/
QString FacebookAlbumInterface::createdTime() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_CREATEDTIME).toString();
}

/*!
    \qmlproperty QString FacebookAlbumInterface::updatedTime
    Holds the last-update time of the album in an ISO8601-formatted string.
*/
QString FacebookAlbumInterface::updatedTime() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_UPDATEDTIME).toString();
}

/*!
    \qmlproperty bool FacebookAlbumInterface::canUpload
    Whether the current user can upload photos to the album.
*/
bool FacebookAlbumInterface::canUpload() const
{
    Q_D(const FacebookAlbumInterface);
    return d->data().value(FACEBOOK_ONTOLOGY_ALBUM_CANUPLOAD).toString() == QLatin1String("true");
}

/*!
    \qmlproperty bool FacebookAlbumInterface::liked
    Whether the album has been liked by the current user.
*/
bool FacebookAlbumInterface::liked() const
{
    Q_D(const FacebookAlbumInterface);
    return d->liked;
}

