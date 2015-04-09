#!/usr/bin/env python

import __main__


from PyQt5.QtCore import QRect, QSize, QRectF, Qt, pyqtSignal
from PyQt5.QtGui import QPalette, QPainter, QPen, QColor, QTransform, QBrush
from PyQt5.QtWidgets import (QAction, QApplication, QMessageBox, QWidget, QSizePolicy, 
	QMainWindow, QToolBox, QHBoxLayout, QPushButton, QListWidget, QSplitter, QToolBar, QToolButton,
	QCheckBox, QRadioButton,QLabel, QListWidgetItem, QSlider, QGridLayout, QDial, QPushButton
	)

import time, random, math, colorsys
from datetime import datetime

class Monitor(QLabel):
	def __init__(self, parent):
		QLabel.__init__(self, '[ Mon ]', parent)
		self.setAutoFillBackground(True)

	def setColor(self, r, g, b):
		self.palette = QPalette()
		self.palette.setColor(QPalette.Background, QColor(round(r*255), round(g*255), round(b*255)))
		self.setPalette(self.palette)

class LightChannel(QWidget):
	changed = pyqtSignal(object)
	def __init__(self, parent, update_callback):
		QWidget.__init__(self, parent)
		self.layout = QGridLayout()
		self.setLayout(self.layout)
		self.update_callback = update_callback


		self.light_value = QSlider(self)
		self.hue_value = QDial(self)
		self.saturation_value = QDial(self)
		self.monitor = Monitor(self)

		self.layout.addWidget(QLabel('Hue'), 0, 0, Qt.AlignHCenter)
		self.layout.addWidget(self.hue_value, 1, 0, Qt.AlignHCenter)
		self.layout.addWidget(QLabel('Saturation'), 2, 0, Qt.AlignHCenter)
		self.layout.addWidget(self.saturation_value, 3, 0, Qt.AlignHCenter)
		self.layout.addWidget(self.monitor, 4, 0, Qt.AlignHCenter)
		self.layout.addWidget(QLabel('Intensity'), 5, 0, Qt.AlignHCenter)
		self.layout.addWidget(self.light_value, 6, 0, Qt.AlignHCenter)

		self.update()

		self.hue_value.valueChanged.connect(self.update)
		self.saturation_value.valueChanged.connect(self.update)
		self.light_value.valueChanged.connect(self.update)


	def get_rgb(self):
		saturation = self.saturation_value.value()/99.0
		light = self.light_value.value()/99.0
		hue = self.hue_value.value()/99.0

		return colorsys.hsv_to_rgb(hue, saturation, light)

	def update(self):
		r, g, b = self.get_rgb()
		self.monitor.setColor(r, g, b)
		self.update_callback.setColor(r, g, b)
		self.changed.emit(self)

# class DMX_Monitor(QWidget):
# 	update_dmx = pyqtSignal(int, int)
# 	def __init__(self, parent):
# 		QWidget.__init__(self, parent)
# 		self.layout = QGridLayout()
# 		self.setLayout(self.layout)

# 		self.layout.addWidget(QLabel('DMX Monitor'), 0, 0, 1, 8)


# 		self.channel = list()
# 		for row in range(32):
# 			for col in range(8):
# 				l = QLabel('0x%02x' % 0)
# 				self.channel.append(l)
# 				self.layout.addWidget(l, row+1, col)


# 	def update_value(self, channel, value):
# 		self.channel[channel].setText("0x%02x" % value)


import test as testsystem

class par64lampa:
	def __init__(self, adr, r_gamma=math.e, g_gamma=math.e, b_gamma=math.e, r_comp=1.0, g_comp=1.0, b_comp=1.0, channels=None):
		self.r_comp = r_comp
		self.g_comp = g_comp
		self.b_comp = b_comp
		self.r_gamma = r_gamma
		self.g_gamma = g_gamma
		self.b_gamma = b_gamma
		self.adr = adr
		self.channels = channels or (0, 1, 2)

	def setColor(self, r, g, b):

		r = (r * self.r_comp) ** self.r_gamma
		g = (g * self.g_comp) ** self.g_gamma
		b = (b * self.b_comp) ** self.b_gamma

		testsystem.set_pwm(self.adr, self.channels[0], round(r*4095))
		testsystem.set_pwm(self.adr, self.channels[1], round(g*4095))
		testsystem.set_pwm(self.adr, self.channels[2], round(b*4095))

lampa = par64lampa(1, r_comp=0.82, b_comp=0.93)
#lampa2 = par64lampa(2, channels=(3, 4, 5))

class ControlPanel(QWidget):
	def __init__(self, parent, dmx_mon=None):
		QWidget.__init__(self, parent)
		self.layout = QGridLayout()
		self.dmx_mon = dmx_mon
		self.setLayout(self.layout)



		#self.channel = [LightChannel(self, i*6) for i in range(16)]
		self.channel = list()
		self.channel = [
			LightChannel(self, lampa),
			#LightChannel(self, lampa2),
		]


		for i, chan in enumerate(self.channel):
			self.layout.addWidget(chan, 0, i)
			chan.changed.connect(self.channel_change)

	def channel_change(self, chan):
		r, g, b =chan.get_rgb()
		if self.dmx_mon:
			self.dmx_mon.update_value(chan.address, round(r*255))
			self.dmx_mon.update_value(chan.address+1, round(g*255))
			self.dmx_mon.update_value(chan.address+2, round(b*255))

class MainWindow(QMainWindow):


	def __init__(self):
		super(MainWindow, self).__init__()

		self.container = QSplitter(self)
		self.setCentralWidget(self.container)

		#mon = DMX_Monitor(self)
		mon = None

		self.container.addWidget(ControlPanel(self, mon))
		if mon: 
			self.container.addWidget(mon)

		self.createActions()
		self.createMenus()
		self.createStatusBar()


	def about(self):
		QMessageBox.about(self, "About Application", __main__.__doc__)


	def createActions(self):
		self.exitAct = QAction("E&xit", self, shortcut="Ctrl+Q",
				statusTip="Exit the application", triggered=self.close)

		self.aboutAct = QAction("&About", self,
				statusTip="Show the application's About box",
				triggered=self.about)

		self.aboutQtAct = QAction("About &Qt", self,
				statusTip="Show the Qt library's About box",
				triggered=QApplication.instance().aboutQt)


	def createMenus(self):
		self.fileMenu = self.menuBar().addMenu("&File")
		self.fileMenu.addAction(self.exitAct)
		self.helpMenu = self.menuBar().addMenu("&About")
		self.helpMenu.addAction(self.aboutAct)
		self.helpMenu.addAction(self.aboutQtAct)


	def createStatusBar(self):
		self.statusBar().showMessage("Ready")



if __name__ == '__main__':

	import sys, threading

	app = QApplication(sys.argv+['--style','gtk'])
	mainWin = MainWindow()
	mainWin.show()


	status = app.exec_()
	print("Exiting")
	sys.exit(status)


