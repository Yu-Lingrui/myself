from start import Ui_Dialog
from PyQt5 import QtWidgets
import start_main_2
import sys

logs={'余翎瑞':'123456'}

class DengLu(QtWidgets.QDialog, Ui_Dialog):
    def __init__(self, parent=None):
        super(DengLu, self).__init__(parent)
        self.setupUi(self)
        self.pushButton_2.clicked.connect(self.login)
        self.pushButton.clicked.connect(self.exit)
    def login(self):
        name = self.lineEdit.text()
        key  = self.lineEdit_2.text()
        if name not in logs:
            self.error()
            self.lineEdit.setText('')
            self.lineEdit.setFocus()
            return
        if logs[name]!=key:
            self.error()
            self.lineEdit_2.setText('')
            self.lineEdit_2.setFocus()
            return
        QtWidgets.QMessageBox.about(self, "正确", "登陆成功")
        self.close()
        ui = start_main_2.MyApp()
        ui.show()




    def exit(self):
        app = QtWidgets.QApplication.instance()  # 实例化APP，获取app的指针
        app.quit()  # 退出应用程序
        #可以直接self.close
    def error(self):
        QtWidgets.QMessageBox.critical(self, "输入错误", "请重新输入！")

    def initUI(self):
        pass


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window1 = DengLu()
    window1.show()
    sys.exit(app.exec_())
