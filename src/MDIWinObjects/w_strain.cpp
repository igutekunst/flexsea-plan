/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] w_strain.h: Strain View Window
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-15 | jfduval | New code
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "w_strain.h"
#include "flexsea_generic.h"
#include "ui_w_strain.h"
#include "main.h"
#include <QString>
#include <QDebug>
#include <QTextStream>

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

W_Strain::W_Strain(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::W_Strain)
{
	ui->setupUi(this);

	setWindowTitle("6ch StrainAmp - Barebone");
	setWindowIcon(QIcon(":icons/d_logo_small.png"));

	init();
}

W_Strain::~W_Strain()
{
	emit windowClosed();
	delete ui;
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

//Call this function to refresh the display
void W_Strain::refreshDisplayStrain(void)
{
	struct strain_s *stPtr;
	FlexSEA_Generic::assignStrainPtr(&stPtr, SL_BASE_ALL, \
									   ui->comboBoxSlave->currentIndex());
	displayStrain(stPtr);
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void W_Strain::init(void)
{
	//Populates Slave list:
	FlexSEA_Generic::populateSlaveComboBox(ui->comboBoxSlave, \
											SL_BASE_STRAIN, SL_LEN_STRAIN);
}

void W_Strain::displayStrain(struct strain_s *st)
{
	//Unpack:
	//=======

	unpackCompressed6ch(st->compressedBytes, &st->ch[0].strain_filtered,
						&st->ch[1].strain_filtered, &st->ch[2].strain_filtered,
						&st->ch[3].strain_filtered, &st->ch[4].strain_filtered,
						&st->ch[5].strain_filtered);

	//Raw values:
	//===========

	ui->disp_strain_ch1->setText(QString::number(st->ch[0].strain_filtered));
	ui->disp_strain_ch2->setText(QString::number(st->ch[1].strain_filtered));
	ui->disp_strain_ch3->setText(QString::number(st->ch[2].strain_filtered));
	ui->disp_strain_ch4->setText(QString::number(st->ch[3].strain_filtered));
	ui->disp_strain_ch5->setText(QString::number(st->ch[4].strain_filtered));
	ui->disp_strain_ch6->setText(QString::number(st->ch[5].strain_filtered));

	//Decoded values:
	//===================

	ui->disp_strain_ch1_d->setText(QString::number(st->decoded.strain[0],'i',0));
	ui->disp_strain_ch2_d->setText(QString::number(st->decoded.strain[1],'i',0));
	ui->disp_strain_ch3_d->setText(QString::number(st->decoded.strain[2],'i',0));
	ui->disp_strain_ch4_d->setText(QString::number(st->decoded.strain[3],'i',0));
	ui->disp_strain_ch5_d->setText(QString::number(st->decoded.strain[4],'i',0));
	ui->disp_strain_ch6_d->setText(QString::number(st->decoded.strain[5],'i',0));

	//==========
}

//Unpack from buffer
void W_Strain::unpackCompressed6ch(uint8_t *buf, uint16_t *v0, uint16_t *v1, uint16_t *v2, \
							uint16_t *v3, uint16_t *v4, uint16_t *v5)
{
	*v0 = ((*(buf+0) << 8 | *(buf+1)) >> 4);
	*v1 = (((*(buf+1) << 8 | *(buf+2))) & 0xFFF);
	*v2 = ((*(buf+3) << 8 | *(buf+4)) >> 4);
	*v3 = (((*(buf+4) << 8 | *(buf+5))) & 0xFFF);
	*v4 = ((*(buf+6) << 8 | *(buf+7)) >> 4);
	*v5 = (((*(buf+7) << 8 | *(buf+8))) & 0xFFF);
}


//****************************************************************************
// Private slot(s):
//****************************************************************************
