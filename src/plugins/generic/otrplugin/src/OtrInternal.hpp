/*
 * OtrInternal.h - manages the otr-connection.
 *
 * Copyright (C) Timo Engel (timo-e@freenet.de), Berlin 2007.
 * This program was written as part of a diplom thesis advised by 
 * Prof. Dr. Ruediger Weis (PST Labor)
 * at the Technical University of Applied Sciences Berlin.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef OTRINTERNAL_HPP_
#define OTRINTERNAL_HPP_

#include "OtrMessaging.hpp"

#include <QList>
#include <QHash>

extern "C"
{
#include <libotr/proto.h>
#include <libotr/message.h>
#include <libotr/privkey.h>
#include "otrl_extensions.h"
}

class QString;
namespace psiotr { class PsiOtrPlugin; }

// ---------------------------------------------------------------------------

/** 
* Handles all libotr calls and callbacks.
*/
class OtrInternal
{
public:

    OtrInternal(psiotr::OtrCallback* callback, psiotr::OtrPolicy& policy);

    ~OtrInternal();

    QString encryptMessage(const QString& from, const QString& to,
                           const QString& message);

    bool decryptMessage(const QString& from, const QString& to,
                        const QString& message, QString& decrypted);

    QList<psiotr::Fingerprint> getFingerprints();

    void verifyFingerprint(const psiotr::Fingerprint& fingerprint, bool verified);

    void deleteFingerprint(const psiotr::Fingerprint& fingerprint);

    QHash<QString, QString> getPrivateKeys();

    void deleteKey(const QString& account);


    void startSession(const QString& account, const QString& jid);
    
    void endSession(const QString& account, const QString& jid);

    void expireSession(const QString& account, const QString& jid);


    void startSMP(const QString& account, const QString& jid,
                  const QString& question, const QString& secret);
    void startSMP(ConnContext *context,
                  const QString& question, const QString& secret);

    void continueSMP(const QString& account, const QString& jid, const QString& secret);
    void continueSMP(ConnContext *context, const QString& secret);
    
    void abortSMP(const QString& account, const QString& jid);
    void abortSMP(ConnContext *context);


    psiotr::OtrMessageState getMessageState(const QString& account,
                                            const QString& contact);

    QString getMessageStateString(const QString& account,
                                  const QString& contact);

    QString getSessionId(const QString& account, const QString& contact);

    psiotr::Fingerprint getActiveFingerprint(const QString& account,
                                             const QString& contact);

    bool isVerified(const QString& account, const QString& contact);

    bool smpSucceeded(const QString& account, const QString& contact);

    void generateKey(const QString& account);

    static QString humanFingerprint(const unsigned char *fingerprint);

    /*** otr callback functions ***/
    OtrlPolicy policy(ConnContext *context);
    void create_privkey(const char *accountname, const char *protocol);
    int is_logged_in(const char *accountname, const char *protocol,
                     const char *recipient);
    void inject_message(const char *accountname, const char *protocol,
                        const char *recipient, const char *message);
    void notify(OtrlNotifyLevel level, const char *accountname,
                const char *protocol, const char *username, const char *title,
                const char *primary, const char *secondary);
    int display_otr_message(const char *accountname, const char *protocol,
                            const char *username, const char *msg);
    void update_context_list();
    const char* protocol_name(const char *protocol);
    void protocol_name_free(const char *protocol_name);
    void new_fingerprint(OtrlUserState us, const char *accountname,
                         const char *protocol, const char *username,
                         unsigned char fingerprint[20]);
    void write_fingerprints();
    void gone_secure(ConnContext *context);
    void gone_insecure(ConnContext *context);
    void still_secure(ConnContext *context, int is_reply);
    void log_message(const char *message);

    const char* account_name(const char *account,
                             const char *protocol);
    void account_name_free(const char *account_name);


    /*** static otr callback wrapper-functions ***/
    static OtrlPolicy cb_policy(void *opdata, ConnContext *context);
    static void cb_create_privkey(void *opdata, const char *accountname, 
                                  const char *protocol);
    static int cb_is_logged_in(void *opdata, const char *accountname, 
                               const char *protocol, const char *recipient);
    static void cb_inject_message(void *opdata, const char *accountname,
                                  const char *protocol, const char *recipient,
                                  const char *message);
    static void cb_notify(void *opdata, OtrlNotifyLevel level,
                          const char *accountname, const char *protocol,
                          const char *username, const char *title,
                          const char *primary, const char *secondary);
    static int cb_display_otr_message(void *opdata, const char *accountname,
                                      const char *protocol, const char *username,
                                      const char *msg);
    static void cb_update_context_list(void *opdata);
    static const char* cb_protocol_name(void *opdata, const char *protocol);
    static void cb_protocol_name_free(void *opdata, const char *protocol_name);
    static void cb_new_fingerprint(void *opdata, OtrlUserState us,
                                   const char *accountname, const char *protocol,
                                   const char *username, unsigned char fingerprint[20]);
    static void cb_write_fingerprints(void *opdata);
    static void cb_gone_secure(void *opdata, ConnContext *context);
    static void cb_gone_insecure(void *opdata, ConnContext *context);
    static void cb_still_secure(void *opdata, ConnContext *context, int is_reply);
    static void cb_log_message(void *opdata, const char *message);
    
    static const char* cb_account_name(void *opdata, const char *account, const char *protocol);
    static void cb_account_name_free(void *opdata, const char *account_name);
private:

    /** 
    * The userstate contains keys and known fingerprints.
    */
    OtrlUserState m_userstate;
    
    /** 
    * Pointers to callback functions. 
    */
    OtrlMessageAppOps m_uiOps;

    /** 
    * Pointer to a class for callbacks from OTR to application. 
    */
    psiotr::OtrCallback* m_callback;

    /** 
    * Name of the file storing dsa-keys. 
    */
    QString m_keysFile;

    /** 
    * Name of the file storing known fingerprints.
    */
    QString m_fingerprintFile;

    /** 
    * Reference to the default OTR policy
    */
    psiotr::OtrPolicy& m_otrPolicy;
};

// ---------------------------------------------------------------------------

#endif