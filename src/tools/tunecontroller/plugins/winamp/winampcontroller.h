/*
 * winampcontroller.h
 * Copyright (C) 2006  Remko Troncon
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

#ifndef WINAMPCONTROLLER_H
#define WINAMPCONTROLLER_H

#include "tunecontrollerinterface.h"
#include "pollingtunecontroller.h"

#include <windows.h>
#include <QPair>

class WinAmpController : public TuneController
{
	Q_OBJECT
public:
	WinAmpController();
	virtual Tune currentTune() const;

protected:
	QPair<bool, QString> getTrackTitle(HWND waWnd);
protected slots:
	void check();

private:
	Tune prev_tune_;
	QTimer timer_;
	int norminterval_, antiscrollinterval_;
	int antiscrollcounter_;
};

#endif