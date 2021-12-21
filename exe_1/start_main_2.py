from start import Ui_MainWindow
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import game
import image
import sys


class MyApp(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):  # parent = None代表此QWidget属于最上层的窗口,也就是MainWindows.
        super(MyApp, self).__init__(parent)  # 通过super初始化父类，__init__()函数无self，
        self.setupUi(self)
        self.i = 0
        self.gif = (":/q/hongbao1.gif",
                    ":/q/hongbao2.gif",
                    ":/q/hongbao3.gif",
                    ":/q/hongbao4.gif",
                    ":/q/hongbao5.gif",
                    ":/q/touxiang.jpg",
                    ":/q/886.jpg")
        #'(C:\\Users\\Administrator\\Desktop\\python\\github\\new-start\\Qt副本\\image8\\886.jpg')
        self.button.clicked.connect(self.show_1)
        self.statusShowTime()

    def show_1(self):
        if self.i == 7:
            QtWidgets.QMessageBox.warning(self, "播放完毕", "开始Game时光！")
            self.close()
            game.play()
            #os.system('C:\\Users\\Administrator\\Desktop\python\\Qt - 副本副本\\game.py')
        else:
            self.loading_gif = QMovie(self.gif[self.i])
            self.label.setMovie(self.loading_gif)
            self.loading_gif.start()
            self.i = self.i + 1
            # self.label.setPixmap(QPixmap(self.gif[self.i]))

    def showCurrentTime(self, timeLabel):
        # 获取系统当前时间
        time = QDateTime.currentDateTime()
        # 设置系统时间的显示格式
        timeDisplay = time.toString('yyyy-MM-dd hh:mm:ss dddd')
        # 状态栏显示
        timeLabel.setText(timeDisplay)

    def statusShowTime(self):
        self.timer = QTimer()
        self.timeLabel = QtWidgets.QLabel()
        self.showCurrentTime(self.timeLabel)
        self.statusbar.addPermanentWidget(self.timeLabel, 0)
        self.timer.timeout.connect(lambda: self.showCurrentTime(self.timeLabel))  # 这个通过调用槽函数来刷新时间
        self.timer.start(1000)  # 每隔一秒刷新一次，这里设置为1000ms  即1s


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window2 = MyApp()
    window2.show()
    sys.exit(app.exec_())
