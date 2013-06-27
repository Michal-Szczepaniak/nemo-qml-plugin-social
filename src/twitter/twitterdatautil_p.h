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

#ifndef TWITTERDATAUTIL_P_H
#define TWITTERDATAUTIL_P_H

#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QPair>
#include <QtCore/QList>

class TwitterDataUtil {
public:
    static QByteArray authorizationHeader(const QByteArray &oauthConsumerKey,
                                          const QByteArray &oauthConsumerSecret,
                                          const QByteArray &requestMethod,
                                          const QByteArray &requestUrl,
                                          const QList<QPair<QByteArray, QByteArray> > &parameters,
                                          const QByteArray &oauthToken = QByteArray(),
                                          const QByteArray &oauthTokenSecret = QByteArray(),
                                          const QByteArray &oauthNonce = QByteArray(),
                                          const QByteArray &oauthTimestamp = QByteArray());
//    static QVariant parseReplyData(const QByteArray &replyData, bool *ok);
};

#endif // TWITTERDATAUTIL_P_H
