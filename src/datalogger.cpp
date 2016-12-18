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
	[This file]
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "datalogger.h"
#include "ricnuDevice.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>

#include "batteryDevice.h"
#include "executeDevice.h"
#include "gossipDevice.h"
#include "manageDevice.h"
#include "ricnuDevice.h"
#include "strainDevice.h"

//****************************************************************************
// Constructor & Destructor:
//****************************************************************************

DataLogger::DataLogger(QWidget *parent) : QWidget(parent)
{
	initLogDirectory();
	init();
}

//****************************************************************************
// Public function(s):
//****************************************************************************

//****************************************************************************
// Public slot(s):
//****************************************************************************

void DataLogger::openRecordingFile(FlexseaDevice *devicePtr, uint8_t item)
{
	QString shortFileName = devicePtr->shortFileName;

	if(logRecordingFile[item].isOpen())
	{
		qDebug() << "File already open. Close it before opening a new one";
	}

	else
	{
		//Add .csv extension if not present
		if(shortFileName.mid(shortFileName.length()-4) != ".csv")
		{
			shortFileName.append(".csv");
		}

		// Add date and time to the short file name
		shortFileName.prepend(QDate::currentDate().toString("yyyy-MM-dd_") +
							  QTime::currentTime().toString("HH'h'mm'm'ss's'_"));

		openfile(item, shortFileName);
	}
}

void DataLogger::openfile(uint8_t item, QString shortFileName)
{
	// Replace whitespace by underscore
	shortFileName.replace(" ", "_");

	// Remove invalid character for a filename(According to Windows)
	shortFileName.remove(QRegExp("[<>:\"/|?*]"));
	shortFileName.remove("\\");


	// Set the folder to current directory
	QDir::setCurrent(planGUIRootPath + "\\" + logFolder + "\\" + sessionFolder);

	// Set the filename from the current directory
	QString fileName = QDir::currentPath() + "/" + shortFileName;

	logRecordingFile[item].setFileName(fileName);
	// Try to open the file.
	if(logRecordingFile[item].open(QIODevice::ReadWrite))
	{
		// TODO Datalogger should not know that there's a logFile and bar
		// status. Abstraction principle is not respected here. Is there a way
		// to use some sort of return value instead of signal slot?

		//Associate stream to file:
		logFileStream.setDevice(&logRecordingFile[item]);

		emit setStatusBarMessage(tr("Opened '") + fileName + "'.");
		qDebug() << tr("Opened '") + fileName + "'.";
	}

	//If no file selected
	else
	{
		qDebug() << tr("No log file selected.");
		emit setStatusBarMessage(
					tr("No log file selected, or the file couldn't be opened."));
	}
}

void DataLogger::openReadingFile(bool * isOpen)
{
	QString msg = "";
	*isOpen = false;

	//File Dialog (returns the selected file name):
	QDir::setCurrent(planGUIRootPath + "\\" + logFolder);
	QString filename = QFileDialog::getOpenFileName( \
				this,
				tr("Open Log File"),
				QDir::currentPath() + "\\.csv" ,
				tr("Log files (*.txt *.csv);;All files (*.*)"));

	//Extract filename to simplify UI:
	QString path = QDir::currentPath();
	int pathLen = path.length();
	QString shortFileName = filename.mid(pathLen+1);

	//Now we open it:
	logReadingFile.setFileName(filename);

	//Check if the file was successfully opened
	if(logReadingFile.open(QIODevice::ReadOnly) == false)
	{
		msg = tr("Error : No log file selected or the file couldn't be opened.");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		return;
	}

	//Check if the file is empty
	if(logReadingFile.size() == 0)
	{
		msg = tr("Error : Loaded file was empty.");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		return;
	}

	// Read and save the logfile informations.
	QString line;
	QStringList splitLine;

	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);

	//Check if the file header contain the expected number of data
	if(splitLine.length() < 12)
	{
		msg = tr("Error : Loaded file format was not compatible");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		return;
	}

	myLogFile.dataloggingItem	= splitLine[1].toInt();
	myLogFile.SlaveIndex		= splitLine[3].toInt();
	myLogFile.SlaveName			= splitLine[5];
	myLogFile.experimentIndex	= splitLine[7].toInt();
	myLogFile.experimentName	= splitLine[9];
	myLogFile.frequency			= splitLine[11].toInt();
	myLogFile.shortFileName		= shortFileName;
	myLogFile.fileName			= filename;

	//Clear the column's header.
	line = logReadingFile.readLine();
	splitLine = line.split(',', QString::KeepEmptyParts);
	int test = splitLine.length();
	//Check if data header contain the number of expected field
	if(splitLine.length() < 20)
	{
		msg = tr("File format is not compatible");
		emit setStatusBarMessage(msg);
		qDebug() << msg;
		myLogFile.clear();
		return;
	}

	while (!logReadingFile.atEnd())
	{
		line = logReadingFile.readLine();
		splitLine = line.split(',', QString::KeepEmptyParts);

		//Check if data line contain the number of data expected
		if(splitLine.length() >= 20)
		{
			myLogFile.newDataLine();
			myLogFile.data.last().timeStampDate		= splitLine[0];
			myLogFile.data.last().timeStamp_ms		= splitLine[1].toInt();
			myLogFile.data.last().execute.accel.x	= splitLine[2].toInt();
			myLogFile.data.last().execute.accel.y	= splitLine[3].toInt();
			myLogFile.data.last().execute.accel.z	= splitLine[4].toInt();
			myLogFile.data.last().execute.gyro.x	= splitLine[5].toInt();
			myLogFile.data.last().execute.gyro.y	= splitLine[6].toInt();
			myLogFile.data.last().execute.gyro.z	= splitLine[7].toInt();
			myLogFile.data.last().execute.strain	= splitLine[8].toInt();
			myLogFile.data.last().execute.analog[0]	= splitLine[9].toInt();
			myLogFile.data.last().execute.analog[1]	= splitLine[10].toInt();
			myLogFile.data.last().execute.current	= splitLine[11].toInt();
			myLogFile.data.last().execute.enc_display= splitLine[12].toInt();
			myLogFile.data.last().execute.enc_control= splitLine[13].toInt();
			myLogFile.data.last().execute.enc_commut= splitLine[14].toInt();
			myLogFile.data.last().execute.volt_batt	= splitLine[15].toInt();
			myLogFile.data.last().execute.volt_int	= splitLine[16].toInt();
			myLogFile.data.last().execute.temp		= splitLine[17].toInt();
			myLogFile.data.last().execute.status1	= splitLine[18].toInt();
			myLogFile.data.last().execute.status2	= splitLine[19].toInt();
		}
	}

	myLogFile.decodeAllLine();

	msg = tr("Opened '") + filename + "'.";
	emit setStatusBarMessage(msg);
	qDebug() << msg;

	*isOpen = true;
}

void DataLogger::writeToFile(FlexseaDevice *devicePtr, uint8_t item)
{
	// Verify that the log file is properly opened.
	if(logRecordingFile[item].isOpen())
	{
		//Writting for the first time?
		if(logRecordingFile[item].pos() == 0)
		{
			//Header:
			writeIdentifier(devicePtr, item);
			logFileStream << devicePtr->getHeaderStr() << endl;
		}

		//And we add to the text file:
		logFileStream << devicePtr->getLastLineStr() << endl;
	}
	else
	{
		emit setStatusBarMessage("Datalogger: no file selected.");
	}
}

void DataLogger::closeRecordingFile(uint8_t item)
{
	if(logRecordingFile[item].isOpen())
	{
		logFileStream << endl;
		logRecordingFile[item].close();
	}
}

void DataLogger::closeReadingFile(void)
{
	if(logReadingFile.isOpen())
	{
		logReadingFile.close();
	}

	myLogFile.clear();
}

//****************************************************************************
// Private function(s):
//****************************************************************************

void DataLogger::init(void)
{
	myTime = new QDateTime;
}

void DataLogger::initLogDirectory()
{
	// Save the root path of the execution of the program
	planGUIRootPath = QDir::currentPath();

	// Set the default folder
	logFolder = "Plan-GUI-Logs";
	sessionFolder = QDate::currentDate().toString("yyyy-MM-dd_") + \
					QTime::currentTime().toString("HH'h'mm'm'ss's'");

	sessionFolder.replace(" ", "_");
	sessionFolder.replace(":", "-");

	//Do we already have a "Plan-GUI-Logs" directory?
	if(!QDir().exists(logFolder))
	{
		//No, create it:
		QDir().mkdir(logFolder);
		qDebug() << QString("Created ") + logFolder;
		emit setStatusBarMessage("Created the " + logFolder + " directory.");
		//ui->statusBar->showMessage("Created the Plan-GUI-Logs directory.");
	}
	else
	{
		qDebug() << "Using existing """ + logFolder + """ directory";
	}

	QDir::setCurrent(logFolder);

	// Create this session folder
	QDir().mkdir(sessionFolder);
}

void DataLogger::writeIdentifier(FlexseaDevice *devicePtr, uint8_t item)
{
	QString msg;
	//Top of the file description:
	msg =	QString("Datalogging Item:")				+ QString(',') +
			QString::number(item)						+ QString(',') +

			QString("Slave Index:")						+ QString(',') +
			QString::number(devicePtr->slaveIndex)		+ QString(',') +

			QString("Slave Name:")						+ QString(',') +
			devicePtr->SlaveName						+ QString(',') +

			QString("Experiment Index:")				+ QString(',') +
			QString::number(devicePtr->experimentIndex)	+ QString(',') +

			QString("Experiment Name:")					+ QString(',') +
			devicePtr->experimentName					+ QString(',') +

			QString("Aquisition Frequency:")			+ QString(',') +
			QString::number(devicePtr->frequency)		+ QString("\n");

	if(logRecordingFile[item].isOpen())
	{
		logFileStream << msg;
	}
}

//****************************************************************************
// Private slot(s):
//****************************************************************************

