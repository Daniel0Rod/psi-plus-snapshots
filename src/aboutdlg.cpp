/*
 * aboutdlg.cpp
 * Copyright (C) 2001-2003  Justin Karneges, Michail Pishchagin
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

#include <QTextStream>
#include <QFile>
#include <QtCrypto>

#include "applicationinfo.h"
#include "aboutdlg.h"
#include "iconset.h"

AboutDlg::AboutDlg(QWidget* parent)
	: QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui_.setupUi(this);
	setWindowIcon(IconsetFactory::icon("psi/psiplus_logo").icon());

	setModal(false);

	ui_.lb_name->setText ( QString("<h3><b>%1 v%2</b></h3>").arg(ApplicationInfo::name()).arg(ApplicationInfo::version()) );

	ui_.te_license->setText ( loadText(":/COPYING") );

	QString lang_name = qApp->translate( "@default", "language_name" );
	if ( lang_name == "language_name" ) // remove the translation tab, if no translation is used
		ui_.tw_tabs->removeTab ( 3 );

	// fill in Authors tab...
	QString authors;
	authors += details(QString::fromUtf8("Justin Karneges"),
			   "justin@affinix.com", "", "",
			   tr("Current Maintainer and Original Author"));
	authors += details(QString::fromUtf8("Kevin Smith"),
			   "kismith@psi-im.org", "", "",
			   tr("Past Lead Developer and Maintainer"));
	authors += details(QString::fromUtf8("Remko Tronçon"),
			   "", "", "http://el-tramo.be",
			   tr("Past Lead Developer"));
	authors += details(QString::fromUtf8("Michail Pishchagin"),
			   "mblsha@psi-im.org", "", "",
			   tr("Lead Widget Developer"));
	authors += details(QString::fromUtf8("Maciej Niedzielski"),
			   "machekku@psi-im.org", "", "",
			   tr("Developer"));
	authors += details(QString::fromUtf8("Martin Hostettler"),
			   "martin@psi-im.org", "", "",
			   tr("Developer"));
	ui_.te_authors->setText( authors );

	// fill in About Psi+ tab...
	QString psiplus;
	psiplus += details(QString::fromUtf8("Psi+ Project"),
			   "", "psi-dev@conference.jabber.ru", "http://psi-dev.googlecode.com/",
			   tr("Active project members are presents below:"));
	psiplus += details(QString::fromUtf8("zet"),
			   "vladimir.shelukhin@gmail.com", "", "",
			   tr("Founder, Patcher and MS Windows x86 Packager"));
	psiplus += details(QString::fromUtf8("rion"),
			   "rion4ik@gmail.com", "", "",
			   tr("Founder and Lead Patcher"));
	psiplus += details(QString::fromUtf8("majik"),
			   "", "maj@jabber.ru", "",
			   tr("Founder, Designer and Patcher"));
	psiplus += details(QString::fromUtf8("Dealer_WeARE"),
			   "", "", "",
			   tr("Patcher and Plugins Writer"));
	psiplus += details(QString::fromUtf8("tux-den"),
			   "", "", "",
			   tr("Designer and Patcher"));
	psiplus += details(QString::fromUtf8("taurus"),
			   "", "", "",
			   tr("Patcher, Plugins Writer and Fedora Packager"));
	psiplus += details(QString::fromUtf8("KukuRuzo"),
			   "", "", "",
			   tr("Patcher"));
	psiplus += details(QString::fromUtf8("ivan1986"),
			   "", "", "",
			   tr("Ubuntu Packager and Patcher"));
	psiplus += details(QString::fromUtf8("nexor (aka zerkalica)"),
			   "", "", "",
			   tr("Ubuntu Packager"));
	psiplus += details(QString::fromUtf8("Flint (aka Флинт)"),
			   "", "", "",
			   tr("Mandriva Linux Packager"));
	psiplus += details(QString::fromUtf8("ivan101"),
			   "ivan101@users.sourceforge.net", "", "",
			   tr("Patcher and Russian localization"));
	psiplus += details(QString::fromUtf8("Z_God"),
			   "", "", "",
			   tr("Psimedia Patcher and English localization"));
	psiplus += details(QString::fromUtf8("buharev.v.p"),
			   "", "", "",
			   tr("Mac OS X Packager"));
	psiplus += details(QString::fromUtf8("liuch"),
			   "", "", "",
			   tr("Patcher and Plugins Writer"));
	psiplus += details(QString::fromUtf8("sz"),
			   "", "", "",
			   tr("OpenSUSE Packager"));
	ui_.te_psiplus->setText( psiplus );

	// fill in Thanks To tab...
	QString thanks;
	thanks += details(QString::fromUtf8("Frederik Schwarzer"),
			  "schwarzerf@gmail.com", "", "",
			  tr("Language coordinator, miscellaneous assistance"));
	thanks += details(QString::fromUtf8("Akito Nozaki"),
			  "anpluto@usa.net", "", "",
			  tr("Former language coordinator, miscellaneous assistance"));
	thanks += details(QString::fromUtf8("Jan Niehusmann"),
			  "jan@gondor.com", "", "",
			  tr("Build setup, miscellaneous assistance"));
	thanks += details(QString::fromUtf8("Everaldo Coelho"),
			  "", "", "http://www.everaldo.com",
			  tr("Many icons are from his Crystal icon theme"));
	thanks += details(QString::fromUtf8("Jason Kim"),
			  "", "", "",
			  tr("Graphics"));
	thanks += details(QString::fromUtf8("Hideaki Omuro"),
			  "", "", "",
			  tr("Graphics"));
	thanks += details(QString::fromUtf8("Bill Myers"),
			  "", "", "",
			  tr("Original Mac Port"));
	thanks += details(QString::fromUtf8("Eric Smith (Tarkvara Design, Inc.)"),
			 "eric@tarkvara.org", "", "",
			 tr("Mac OS X Port"));
	thanks += details(QString::fromUtf8("Tony Collins"),
	 		 "", "", "",
	 		 tr("Original End User Documentation"));
	thanks += details(QString::fromUtf8("Hal Rottenberg"),
			  "", "", "",
			 tr("Webmaster, Marketing"));
	thanks += details(QString::fromUtf8("Mircea Bardac"),
			 "", "", "",
			 tr("Bug Tracker Management"));
	thanks += details(QString::fromUtf8("Jacek Tomasiak"),
			 "", "", "",
			 tr("Patches"));

	foreach(QCA::Provider *p, QCA::providers()) {
		QString credit = p->credit();
		if(!credit.isEmpty()) {
			thanks += details(tr("Security plugin: %1").arg(p->name()),
				"", "", "",
				credit);
		}
	}

	//thanks += tr("Thanks to many others.\n"
	//	     "The above list only reflects the contributors I managed to keep track of.\n"
	//	     "If you're not included but you think that you must be in the list, contact the developers.");
	ui_.te_thanks->setText( thanks );

	QString translation = tr(
		"I. M. Anonymous <note text=\"replace with your real name\"><br>\n"
		"&nbsp;&nbsp;<a href=\"http://me.com\">http://me.com</a><br>\n"
		"&nbsp;&nbsp;Jabber: <a href=\"xmpp:me@me.com\">me@me.com</a><br>\n"
		"&nbsp;&nbsp;<a href=\"mailto:me@me.com\">me@me.com</a><br>\n"
		"&nbsp;&nbsp;Translator<br>\n"
		"<br>\n"
		"Join the translation team today! Go to \n"
		"<a href=\"http://forum.psi-im.org/forum/14\">\n"
		"http://forum.psi-im.org/forum/14</a> for further details!"
	);
	ui_.te_translation->appendText(translation);
}

QString AboutDlg::loadText( const QString & fileName )
{
	QString text;

	QFile f(fileName);
	if(f.open(QIODevice::ReadOnly)) {
		QTextStream t(&f);
		while(!t.atEnd())
			text += t.readLine() + '\n';
		f.close();
	}

	return text;
}


QString AboutDlg::details( QString name, QString email, QString jabber, QString www, QString desc )
{
	QString ret;
	const QString nbsp = "&nbsp;&nbsp;";
	ret += name + "<br>\n";
	if ( !email.isEmpty() )
		ret += nbsp + "E-mail: " + "<a href=\"mailto:" + email + "\">" + email + "</a><br>\n";
	if ( !jabber.isEmpty() )
		ret += nbsp + "Jabber: " + "<a href=\"xmpp:" + jabber + "?join\">" + jabber + "</a><br>\n";
	if ( !www.isEmpty() )
		ret += nbsp + "WWW: " + "<a href=\"" + www + "\">" + www + "</a><br>\n";
	if ( !desc.isEmpty() )
		ret += nbsp + desc + "<br>\n";
	ret += "<br>\n";

	return ret;
}
