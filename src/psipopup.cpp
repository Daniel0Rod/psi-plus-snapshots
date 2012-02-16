/*
 * psipopup.cpp - the Psi passive popup class
 * Copyright (C) 2003  Michail Pishchagin
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

#include "psipopup.h"
#include "common.h"
#include "fancypopup.h"
#include "fancylabel.h"
#include "userlist.h"
#include "alerticon.h"
#include "psievent.h"
#include "psicon.h"
#include "textutil.h"
#include "psiaccount.h"
#include "psiiconset.h"
#include "iconlabel.h"
#include "psioptions.h"
#include "coloropt.h"
#include "avatars.h"

#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QList>
#include <QTextDocument>

/**
 * Limits number of popups that could be displayed 
 * simultaneously on screen. Old popups momentally
 * disappear when new ones appear.
 */
static int MaxPopups = 5;

/**
 * Holds a list of Psi Popups.
 */
static QList<PsiPopup *> *psiPopupList = 0;

//----------------------------------------------------------------------------
// PsiPopup::Private
//----------------------------------------------------------------------------

class PsiPopup::Private : public QObject
{
	Q_OBJECT
public:
	Private(PsiPopup *p);
	~Private();

	void init(const PsiIcon *titleIcon, const QString& titleText, PsiAccount *_acc, PopupType type);
	static QString clipText(QString);
	QBoxLayout *createContactInfo(const QPixmap *avatar, const PsiIcon *icon, const QString& text);

private slots:
	void popupDestroyed();
	void popupClicked(int);
	void eventDestroyed();

public:
	PsiCon *psi;
	PsiAccount *account;
	FancyPopup *popup;
	PsiPopup *psiPopup;
	QString id;
	PopupType popupType;
	Jid jid;
	Status status;
	PsiEvent *event;
	PsiIcon *titleIcon;
	bool display;
};

PsiPopup::Private::Private(PsiPopup *p)
{
	psiPopup = p;
	popup = 0;
	popupType = AlertNone;
	event = 0;
	titleIcon = 0;
}

PsiPopup::Private::~Private()
{
	if ( psiPopupList )
		psiPopupList->removeAll(psiPopup);

	if ( popup )
		delete popup;
	if ( titleIcon )
		delete titleIcon;
	popup = 0;
}

void PsiPopup::Private::init(const PsiIcon *_titleIcon, const QString& titleText, PsiAccount *acc, PopupType type)
{
	psi = 0;
	if(acc) psi = acc->psi();
	account = acc;
	display = true;

	if ( !psiPopupList )
		psiPopupList = new QList<PsiPopup *>();

	if ( psiPopupList->count() >= MaxPopups && MaxPopups > 0 )
		delete psiPopupList->first();

	FancyPopup *lastPopup = 0;
	if ( psiPopupList->count() && psiPopupList->last() )
		lastPopup = psiPopupList->last()->popup();

	if ( type != AlertNone )
		titleIcon = new AlertIcon(_titleIcon);
	else
		titleIcon = new PsiIcon(*_titleIcon);

	FancyPopup::setHideTimeout(psiPopup->timeout(type));
	FancyPopup::setBorderColor( ColorOpt::instance()->color("options.ui.look.colors.passive-popup.border") );

	popup = new FancyPopup(titleText, titleIcon, lastPopup, false);
	connect(popup, SIGNAL(clicked(int)), SLOT(popupClicked(int)));
	connect(popup, SIGNAL(destroyed()), SLOT(popupDestroyed()));

	// create id
	if ( _titleIcon )
		id += _titleIcon->name();
	id += titleText;
}

void PsiPopup::Private::popupDestroyed()
{
	popup = 0;
	psiPopup->deleteLater();
}

void PsiPopup::Private::popupClicked(int button)
{
	if ( button == (int)Qt::LeftButton ) {
		if ( event )
			psi->processEvent(event, UserAction);
		else if ( account ) {
			// FIXME: it should work in most cases, but
			// maybe it's better to fix UserList::find()?
			Jid j( jid.bare() );
			account->actionDefault( j );
		}
	}
	else if ( event && event->account() && button == (int)Qt::RightButton ) {
		event->account()->psi()->removeEvent(event);
	}

	popup->deleteLater();
}

void PsiPopup::Private::eventDestroyed()
{
	event = 0;
}

QString PsiPopup::Private::clipText(QString text)
{
	int len = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.maximum-text-length").toInt();
	if (len > 0) {
		// richtext will give us trouble here
		if (((int)text.length()) > len) {
			text = text.left(len);

			// delete last unclosed tag
			/*if ( text.find("</") > text.find(">") ) {

				text = text.left( text.find("</") );
			}*/

			text += "...";
		}
	}

	return text;
}

QBoxLayout *PsiPopup::Private::createContactInfo(const QPixmap *avatar, const PsiIcon *icon, const QString& text)
{
	QHBoxLayout *dataBox = new QHBoxLayout();


	if (avatar && !avatar->isNull()) {
		int size = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.avatar-size").toInt();
		QLabel *avatarLabel = new QLabel(popup);
		avatarLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
		avatarLabel->setPixmap(avatar->scaled(QSize(size, size), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		dataBox->addWidget(avatarLabel);
		dataBox->addSpacing(5);
	}

	if (icon) {
		IconLabel *iconLabel = new IconLabel(popup);
		iconLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
		iconLabel->setPsiIcon(icon);
		dataBox->addWidget(iconLabel);

		dataBox->addSpacing(5);
	}

	QLabel *textLabel = new QLabel(popup);
	QFont font;
	font.fromString( PsiOptions::instance()->getOption("options.ui.look.font.passive-popup").toString() );
	textLabel->setFont(font);

	textLabel->setWordWrap(false);
	textLabel->setText(QString("<qt>%1</qt>").arg(clipText(text)));
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	dataBox->addWidget(textLabel);

	return dataBox;
}

//----------------------------------------------------------------------------
// PsiPopup
//----------------------------------------------------------------------------

PsiPopup::PsiPopup(const PsiIcon *titleIcon, const QString& titleText, PsiAccount *acc)
{
	d = new Private(this);
	d->init(titleIcon, titleText, acc, AlertNone);
}

PsiPopup::~PsiPopup()
{
	delete d;
}

PsiPopup::PsiPopup(PopupType type, PsiAccount *acc)
{
	d = new Private(this);

	d->popupType = type;
	PsiIcon *icon = 0;
	QString text = "Psi: ";
	bool doAlertIcon = false;

	text += title(type, &doAlertIcon, &icon);

	d->init(icon, text, acc, doAlertIcon ? type : AlertNone);
}

QString PsiPopup::title(PopupType type, bool *doAlertIcon, PsiIcon **icon)
{
	QString text;

	switch(type) {
	case AlertOnline:
		text = PsiPopup::tr("Contact online");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("status/online");
		*doAlertIcon = false;
		break;
	case AlertOffline:
		text = PsiPopup::tr("Contact offline");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("status/offline");
		*doAlertIcon = false;
		break;
	case AlertStatusChange:
		text = PsiPopup::tr("Status change");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("status/online");
		*doAlertIcon = false;
		break;
	case AlertMessage:
		text = PsiPopup::tr("Incoming message");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/message");
		*doAlertIcon = true;
		break;
	case AlertComposing:
		text = PsiPopup::tr("Typing notify");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/typing");
		*doAlertIcon = false;
		break;
	case AlertChat:
		text = PsiPopup::tr("Incoming chat message");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/chat");
		*doAlertIcon = true;
		break;
	case AlertHeadline:
		text = PsiPopup::tr("Headline");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/headline");
		*doAlertIcon = true;
		break;
	case AlertFile:
		text = PsiPopup::tr("Incoming file");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/file");
		*doAlertIcon = true;
		break;
	case AlertAvCall:
		text = PsiPopup::tr("Incoming call");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/call");
		*doAlertIcon = true;
		break;
	case AlertGcHighlight:
		text = PsiPopup::tr("Groupchat highlight");
		*icon = (PsiIcon *)IconsetFactory::iconPtr("psi/headline");
		*doAlertIcon = true;
		break;
	default:
		break;
	}

	return text;
}

QString PsiPopup::clipText(const QString &text)
{
	return PsiPopup::Private::clipText(text);
}

int PsiPopup::timeout(PopupType type)
{
	int ret = 5000;
	switch(type)
	{
		case AlertMessage:
			ret = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.delays.message").toInt();
			break;
		case AlertChat:
		case AlertHeadline:
		case AlertGcHighlight:
			ret = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.delays.chat").toInt();
			break;
		case AlertFile:
			ret = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.delays.file").toInt();
			break;
		default:
			ret = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.delays.status").toInt();
			break;
	}

	return ret;
}

void PsiPopup::setJid(const Jid &j)
{
	d->jid = j;
}

void PsiPopup::setData(const QPixmap *avatar, const PsiIcon *icon, const QString& text)
{
	if ( !d->popup ) {
		deleteLater();
		return;
	}

	d->popup->addLayout( d->createContactInfo(avatar, icon, text) );

	// update id
	if ( icon )
		d->id += icon->name();
	d->id += text;

	show();
}

void PsiPopup::setData(const Jid &j, const Resource &r, const UserListItem *u, const PsiEvent *event)
{
	if ( !d->popup ) {
		deleteLater();
		return;
	}

	d->jid    = j;
	d->status = r.status();
	if(d->popupType != AlertComposing)
		d->event  = (PsiEvent *)event;

	if ( d->event )
		connect(event, SIGNAL(destroyed()), d, SLOT(eventDestroyed()));

	PsiIcon *icon = PsiIconset::instance()->statusPtr(j, r.status());

	QString jid = j.full();
	int jidLen = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.maximum-jid-length").toInt();
	if (jidLen > 0 && ((int)jid.length()) > jidLen)
		jid = jid.left(jidLen) + "...";

	QString status;
	int len = PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.maximum-status-length").toInt();
	if (len != 0)
		status = r.status().status();
	if (len > 0)
		if ( ((int)status.length()) > len )
			status = status.left (len) + "...";

	QString name;
	if ( u && !u->name().isEmpty() ) {
		name = u->name();
	}
	else if (event && event->type() == PsiEvent::Auth) {
		name = ((AuthEvent*) event)->nick();
	}
	else if (event && event->type() == PsiEvent::Message) {
		name = ((MessageEvent*) event)->nick();
	}
		
	if (!name.isEmpty()) {
		if (!jidLen)
			name = "<nobr>" + Qt::escape(name) + "</nobr>";
		else
			name = "<nobr>" + Qt::escape(name) + " &lt;" + Qt::escape(jid) + "&gt;" + "</nobr>";
	}
	else 
		name = "<nobr>&lt;" + Qt::escape(jid) + "&gt;</nobr>";

	QString statusString = TextUtil::plain2rich(status);
	if ( PsiOptions::instance()->getOption("options.ui.emoticons.use-emoticons").toBool() )
		statusString = TextUtil::emoticonify(statusString);
	if( PsiOptions::instance()->getOption("options.ui.chat.legacy-formatting").toBool() )
		statusString = TextUtil::legacyFormat(statusString);

	if ( !statusString.isEmpty() )
		statusString = "<br>" + statusString;

	QString contactText = "<font size=\"+1\">" + name + "</font>" + statusString;

	// hack for duplicate "Contact Online"/"Status Change" popups
	foreach (PsiPopup *pp, *psiPopupList) {
		if ( d->jid.full() == pp->d->jid.full() && d->status.show() == pp->d->status.show() && d->status.status() == d->status.status() ) {
			if ( d->popupType == AlertStatusChange && pp->d->popupType == AlertOnline ) {
				d->display = false;
				deleteLater();
				break;
			}
		}
	}
	QPixmap avatar = d->account->avatarFactory()->getAvatar(jid);
	// show popup
	if ( d->popupType != AlertComposing && d->popupType != AlertHeadline && (d->popupType != AlertFile || !PsiOptions::instance()->getOption("options.ui.file-transfer.auto-popup").toBool()) )
	{
		if ((event && event->type() == PsiEvent::Message) && (PsiOptions::instance()->getOption("options.ui.notifications.passive-popups.showMessage").toBool())) {
			const Message *jmessage = &((MessageEvent *)event)->message();
			QString message;
			 
			if ( !jmessage->subject().isEmpty() )
				message += "<font color=\"red\"><b>" + tr("Subject:") + " " + jmessage->subject() + "</b></font><br>";
			message += TextUtil::plain2rich( jmessage->body() );
			
			if (!message.isEmpty()) {
				contactText += "<br/><font size=\"+1\">" + message + "</font>";
			}
		}
                setData(&avatar, icon, contactText);
	}
	else if ( d->popupType == AlertComposing ) {
		QString txt = "<font size=\"+1\">" + name + tr(" is typing...") + "</font>" ;
		setData(&avatar, icon, txt);
	}
	else if ( d->popupType == AlertHeadline ) {
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addLayout( d->createContactInfo(&avatar, icon, contactText) );

		vbox->addSpacing(5);

		const Message *jmessage = &((MessageEvent *)event)->message();
		QString message;
		if ( !jmessage->subject().isEmpty() )
			message += "<font color=\"red\"><b>" + tr("Subject:") + ' ' + jmessage->subject() + "</b></font><br>";
		message += TextUtil::plain2rich( jmessage->body() );

		QLabel *messageLabel = new QLabel(d->popup);
		QFont font = messageLabel->font();
		font.setPointSize(common_smallFontSize);
		messageLabel->setFont(font);

		messageLabel->setWordWrap(true);
		messageLabel->setTextFormat(Qt::RichText);
		messageLabel->setText( d->clipText(TextUtil::linkify( message )) );
		messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		vbox->addWidget(messageLabel);

		// update id
		if ( icon )
			d->id += icon->name();
		d->id += contactText;
		d->id += message;

		d->popup->addLayout( vbox );
		show();
	}
	else {
		deleteLater();
	}
}

void PsiPopup::show()
{
	if ( !d->popup ) {
		deleteLater();
		return;
	}

	if ( !d->id.isEmpty() /*&& LEGOPTS.ppNoDupes*/ ) {
		foreach (PsiPopup *pp, *psiPopupList) {
			if ( d->id == pp->id() && pp->popup() ) {
				pp->popup()->restartHideTimer();

				d->display = false;
				break;
			}
		}
	}

	if ( d->display ) {
		psiPopupList->append( this );
		d->popup->show();
	}
	else {
		deleteLater();
	}
}

QString PsiPopup::id() const
{
	return d->id;
}

FancyPopup *PsiPopup::popup()
{
	return d->popup;
}

void PsiPopup::deleteAll()
{
	if ( !psiPopupList )
		return;

	psiPopupList->clear();
	delete psiPopupList;
	psiPopupList = 0;
}

#include "psipopup.moc"