import sys
from PyQt5.QtWidgets import QApplication, QWidget, QGridLayout, QMainWindow, QMessageBox
from PyQt5.QtGui import QTextCursor, QIcon
from main_ui import *
from wifi_udp import *
import threading  # 引入并行
import numpy as np
import pyqtgraph as pg
import re
import time
from sharedcomponets import GUIToolKit
import socket
import webbrowser

import copy


class MyWindow(QMainWindow, Ui_MainWindow):
    signalColors = [GUIToolKit.RED_COLOR, GUIToolKit.BLUE_COLOR, GUIToolKit.PURPLE_COLOR, GUIToolKit.YELLOW_COLOR,
                    GUIToolKit.MAROON_COLOR, GUIToolKit.ORANGE_COLOR, GUIToolKit.GREEN_COLOR, GUIToolKit.GREEN_COLOR,
                    GUIToolKit.GREEN_COLOR, GUIToolKit.GREEN_COLOR, GUIToolKit.GREEN_COLOR]
    signalIcons = ['reddot', 'bluedot', 'purpledot', 'yellowdot', 'maroondot', 'orangedot', 'greendot', 'greendot',
                   'greendot', 'greendot', 'greendot']
    textColors = ['FD42AC', '398AD9', 'FF33FF', 'FFFF00', 'AA0000', 'FF5C5C', '5BEC8D', '5BEC8D', '5BEC8D', '5BEC8D',
                  '5BEC8D', '5BEC8D']

    def __init__(self, parent=None):
        super(MyWindow, self).__init__(parent)
        self.title = "ESP32 MPPT Monitor"
        self.version = "Ver: 0.1.2"

        self.wid = QWidget(self)
        self.setCentralWidget(self.wid)

        self.setupUi(self)
        self.setWindowTitle(self.title)

        self.pLabel1 = QtWidgets.QLabel()
        self.pLabel1.setMinimumWidth(65)
        self.statusbar.addWidget(self.pLabel1)
        self.pLabel1.setText(self.version)

        self.pLabel2 = QtWidgets.QLabel()
        self.pLabel2.setMinimumWidth(125)
        self.statusbar.addWidget(self.pLabel2)

        self.timerLocaltime = QtCore.QTimer()
        self.timerLocaltime.timeout.connect(self.updateUiTime)
        self.timerLocaltime.start(500)

        # 变量初始化
        self.wifi_open_flag = 0
        self.test_flag = 0
        self.wifi_IP_lineEdit.setText(self.get_host_ip())
        # self.variable_init()
        # 设置实例
        # self.CreateItems()
        # 设置信号与槽
        self.CreateSignalSlot()

    def updateUiTime(self):
        self.pLabel2.setText(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))

    def get_host_ip(self):
        """
        查询本机ip地址
        :return:
        """
        st = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            st.connect(('10.255.255.255', 1))
            IP = st.getsockname()[0]
        except Exception:
            IP = '127.0.0.1'
        finally:
            st.close()
        return IP

    # 设置信号与槽
    def CreateSignalSlot(self):

        self.wifi_config_pushButton.clicked.connect(self.wifi_config_pushButton_clicked)
        self.wifi_command_pushButton_1.clicked.connect(self.wifi_command_pushButton_1_clicked)

        self.SERIAL_pushButton.clicked.connect(self.SERIAL_pushButton_clicked)
        self.FWUPDATE_pushButton.clicked.connect(self.FWUPDATE_pushButton_clicked)
        self.ESPRESTART_pushButton.clicked.connect(self.ESPRESTART_pushButton_clicked)

        self.raw_pushButton.clicked.connect(self.raw_pushButton_clicked)
        self.wave_pushButton.clicked.connect(self.wave_pushButton_clicked)

    def raw_pushButton_clicked(self):
        self.change_state = 1
        self.stackedWidget.setCurrentIndex(1)

    def wave_pushButton_clicked(self):
        self.change_state = 0
        self.stackedWidget.setCurrentIndex(0)

    # 设置实例
    def CreateItems(self):
        # 定时器-绘图刷新
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(20)
        # wifi udp
        self.udp = udp(self.wifi_IP_lineEdit_3.text())
        # self.wifi_IP_lineEdit.setText(self.udp.user_ip)#设置为当前本机ID

    def variable_init(self):
        # wifi变量
        self.wifi_recv_flag = 0
        self.close_flag = 1
        self.change_state = 0
        self.re_item = []
        self.re_item2 = []
        self.raw_roll = 0

    def plot_init(self):
        # 绘图对象
        pg.setConfigOptions(antialias=True)
        self.plotWidget = pg.PlotWidget()
        self.plotWidget.showGrid(x=True, y=True, alpha=0.5)
        self.plotWidget.addLegend()
        self.controlPlotWidget = ControlPlotPanel(controllerPlotWidget=self)
        # 图表可视化数组
        self.numberOfSamples = int(self.SAMPLE_doubleSpinBox.text())
        self.signalDataArrays = []
        self.signalPlots = []
        self.signalPlotFlags = []
        self.timeArray = np.arange(-self.numberOfSamples, 0, 1)
        for (sig, sigColor, checkBox, tooltip) in zip(self.re_item, self.signalColors,
                                                      self.controlPlotWidget.signalCheckBox, self.re_item):
            # define signal plot data array
            self.signalDataArrays.append(np.zeros(self.numberOfSamples))
            # configure signal plot parameters
            signalPen = pg.mkPen(color=sigColor, width=1.5)
            self.signalPlots.append(pg.PlotDataItem(self.timeArray, self.signalDataArrays[-1],
                                                    pen=signalPen, name=tooltip))
            self.plotWidget.addItem(self.signalPlots[-1])
            # is plotted flag
            self.signalPlotFlags.append(True)
            # add callback
            checkBox.stateChanged.connect(self.signalPlotFlagUpdate)
        self.stackedWidget = QtWidgets.QStackedWidget()
        self.gridLayout.addWidget(self.stackedWidget)
        self.raw_line = QtWidgets.QTextEdit()
        self.raw_line.setStyleSheet('background: rgb(0, 0, 0)')
        self.stackedWidget.addWidget(self.plotWidget)
        self.stackedWidget.addWidget(self.raw_line)
        self.tool_layout.addWidget(self.controlPlotWidget)

    # checkbox
    def signalPlotFlagUpdate(self):
        for i, (checkBox, plotFlag) in enumerate(zip(self.controlPlotWidget.signalCheckBox, self.signalPlotFlags)):
            if checkBox.isChecked() and (not plotFlag):
                self.signalPlotFlags[i] = True
                self.plotWidget.addItem(self.signalPlots[i])
            elif (not checkBox.isChecked()) and plotFlag:
                self.signalPlotFlags[i] = False
                self.plotWidget.removeItem(self.signalPlots[i])

    # 滑条绑定
    def horizontalSlider_valueChanged(self, slider):
        i = self.slider_lab.index(slider.objectName())
        layout = getattr(self, 'horizontalLayout_{}'.format(i + 4))
        value = layout.itemAt(1).widget().value()
        value = float(layout.itemAt(0).widget().text()) + (value + 100) * (
                float(layout.itemAt(2).widget().text()) - float(layout.itemAt(0).widget().text())) / 200
        layout.itemAt(6).widget().setText(str(value))
        value = layout.itemAt(4).widget().text() + str(value)
        self.udp.send_message(str(value))

    # command命令发送事件
    def wifi_command_pushButton_1_clicked(self):
        self.udp.send_message(self.wifi_command_lineEdit_1.text())

    def SERIAL_pushButton_clicked(self):
        self.udp.send_message("SERIAL")

    def GETESPINFO_pushButton_clicked(self):
        self.udp.send_message("ESPINFO")

    def FWUPDATE_pushButton_clicked(self):
        self.udp.send_message("FWUPDATE")
        url = "http://"+self.wifi_IP_lineEdit_3.text()
        webbrowser.open_new_tab(url)

    def ESPRESTART_pushButton_clicked(self):
        self.udp.send_message("ESPRESTART")

    def INVD_pushButton_clicked(self):
        self.udp.send_message("INVD" + self.INVD_doubleSpinBox.text())

    # WIFI设置点击后判断有无接收到esp32发来的数据
    def wifi_config_pushButton_clicked(self):
        if self.wifi_open_flag == 0:
            try:
                self.variable_init()
                self.CreateItems()
                print(self.wifi_IP_lineEdit.text())
                self.udp.udpClientSocket.bind((self.wifi_IP_lineEdit.text(), 2333))
                self.udp.udpClientSocket.settimeout(1)
                # 第一次接受数据，用于判断项目数，
                self.udp.send_message("START1")
                recv_data = self.udp.udpClientSocket.recv(1024)
                recv_data = recv_data.decode('utf-8')
                recv_data = recv_data[:-1]
                recv_data = recv_data.split(',')
                """处理接受的信息"""
                for i, data in enumerate(recv_data):
                    if i< 5:
                        self.re_item2.append(''.join(re.split(r'[^A-Za-z]', data)))
                        if(i==0):
                            self.RNFA_doubleSpinBox.setProperty("value", data.replace(self.re_item2[i], ''))
                        elif(i==1):
                            self.RNFB_doubleSpinBox.setProperty("value", data.replace(self.re_item2[i], ''))
                        elif(i==2):
                            self.pwm_label.setText(data.replace(self.re_item2[i], ''))
                        elif(i==3):
                            self.ppwm_label.setText(data.replace(self.re_item2[i], ''))
                        elif(i==4):
                            self.pwmc_label.setText(data.replace(self.re_item2[i], ''))
                    else:
                        self.re_item.append(''.join(re.split(r'[^A-Za-z]', data)))
                        if(i==6):
                            print(data.replace(self.re_item[i-5], ''))
                            self.INVD_doubleSpinBox.setProperty("value", data.replace(self.re_item[i-5], ''))
                        elif(i==9):
                            print(data.replace(self.re_item[i-5], ''))
                            self.OUTVD_doubleSpinBox.setProperty("value", data.replace(self.re_item[i-5], ''))

                print(self.re_item)
                print(self.re_item2)
                # 图表初始化
                self.plot_init()
                t1 = threading.Thread(target=self.udp_recv)
                # t1.setDaemon(True)
                t1.start()
                self.wifi_open_flag = 1
                self.wifi_config_pushButton.setText("断开连接")
                self.wifi_config_pushButton.setStyleSheet("QPushButton{color:rgb(255,0,0,255);}")
            except Exception as e:
                print(e)
                QMessageBox.critical(self, "错误", str(e))
        else:
            self.close_flag = 0
            self.wifi_open_flag = 0
            self.tool_layout.itemAt(0).widget().deleteLater()
            self.gridLayout.itemAt(0).widget().deleteLater()
            self.wifi_config_pushButton.setText("设置")
            print("断开连接")
            print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))

    def udp_recv(self):
        while self.close_flag:
            try:
                recv_data = self.udp.udpClientSocket.recv(1024)
                recv_data = recv_data.decode('utf-8')
                recv_data = recv_data[:-1]
                recv_data = recv_data.split(',')
                """处理接受的信息"""
                # print(recv_data)
                self.re_text = ''
                for i, data in enumerate(recv_data):
                    if i < 5:
                        if (i == 2):
                            self.pwm_label.setText(data.replace(self.re_item2[i], ''))
                        elif (i == 3):
                            self.ppwm_label.setText(data.replace(self.re_item2[i], ''))
                        elif (i == 4):
                            self.pwmc_label.setText(data.replace(self.re_item2[i], ''))
                    else:
                        if self.signalPlotFlags[i-5]:
                            # self.re_item.append(''.join(re.split(r'[^A-Za-z]', data)))
                            data = data.replace(self.re_item[i-5], '')
                            if self.change_state:
                                self.re_text += '<font color=\"#{1}\">{0}\t</font>'.format(data, self.textColors[i-5])
                            else:
                                self.signalDataArrays[i-5] = np.roll(self.signalDataArrays[i-5], -1)
                                self.signalDataArrays[i-5][-1] = data
                del recv_data
            except socket.timeout:
                print("closing socket")
                print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                self.wifi_config_pushButton_clicked()
        self.closeUDP()

    def update_plot(self):
        if self.wifi_recv_flag:
            if self.change_state:
                self.raw_line.append(self.re_text)
                if self.raw_roll:
                    self.raw_line.moveCursor(QTextCursor.End)
            else:
                for i, plotFlag in enumerate(self.signalPlotFlags):
                    if plotFlag:
                        self.signalPlots[i].setData(self.timeArray, self.signalDataArrays[i])
                        self.signalPlots[i].updateItems()
                        self.signalPlots[i].sigPlotChanged.emit(self.signalPlots[i])

    def closeUDP(self):
        self.variable_init()
        self.close_flag = 0
        self.udp.send_message("START0")
        self.udp.udpClientSocket.close()
        del self.udp

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        print("关闭")
        self.close_flag = 0


class ControlPlotPanel(QtWidgets.QWidget):

    def __init__(self, parent=None, controllerPlotWidget=None):
        '''Constructor for ToolsWidget'''
        super().__init__(parent)
        # 变量

        # 继承主窗口对象
        self.controlledPlot = controllerPlotWidget

        self.horizontalLayout1 = QtWidgets.QHBoxLayout()
        self.horizontalLayout1.setObjectName('horizontalLayout')
        self.setLayout(self.horizontalLayout1)

        self.startStopButton = QtWidgets.QPushButton(self)
        self.startStopButton.setObjectName('Start')
        self.startStopButton.setText('Start')
        self.startStopButton.setIcon(GUIToolKit.getIconByName('start'))
        self.startStopButton.clicked.connect(self.wifi_recv_open_pushButton_clicked)
        self.horizontalLayout1.addWidget(self.startStopButton)

        self.zoomAllButton = QtWidgets.QPushButton(self)
        self.zoomAllButton.setObjectName('zoomAllButton')
        self.zoomAllButton.setText('View all')
        self.zoomAllButton.setIcon(GUIToolKit.getIconByName('zoomall'))
        self.zoomAllButton.clicked.connect(self.zoomAllPlot)
        self.horizontalLayout1.addWidget(self.zoomAllButton)

        self.signalCheckBox = []
        for i in range(len(self.controlledPlot.re_item)):
            checkBox = QtWidgets.QCheckBox(self)
            checkBox.setObjectName('signalCheckBox' + str(i))
            checkBox.setToolTip(self.controlledPlot.re_item[i])
            checkBox.setText(self.controlledPlot.re_item[i])
            checkBox.setIcon(GUIToolKit.getIconByName(self.controlledPlot.signalIcons[i]))
            checkBox.setChecked(True)
            self.signalCheckBox.append(checkBox)
            self.horizontalLayout1.addWidget(checkBox)

    def zoomAllPlot(self):
        if self.controlledPlot.change_state:
            self.controlledPlot.raw_roll = ~self.controlledPlot.raw_roll
        else:
            self.controlledPlot.plotWidget.enableAutoRange()

    def wifi_recv_open_pushButton_clicked(self):
        if self.controlledPlot.wifi_recv_flag == 0:
            # 打开wifi接收
            self.controlledPlot.wifi_recv_flag = 1
            self.startStopButton.setText('Stop')
        else:
            self.controlledPlot.wifi_recv_flag = 0
            self.startStopButton.setText('Start')

if __name__ == '__main__':
    QtCore.QCoreApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling)
    app = QApplication(sys.argv)
    myWin = MyWindow()
    myWin.show()
    sys.exit(app.exec())
