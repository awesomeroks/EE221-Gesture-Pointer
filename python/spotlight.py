from PyQt5 import QtCore, QtGui, QtWidgets

class SpotlightWidget(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super(SpotlightWidget, self).__init__(parent, QtCore.Qt.WindowStaysOnTopHint)
        self.setWindowFlags(QtCore.Qt.FramelessWindowHint)
        # self.setAttribute(QtCore.Qt.WA_TranslucentBackground)
        # self.setStyleSheet("background:transparent;")
        self.setAttribute(QtCore.Qt.WA_TransparentForMouseEvents)
        self.setWindowOpacity(0.5)
        self.showFullScreen()
        self.setMouseTracking(True)
        self.center = QtCore.QPoint()

        self.setContextMenuPolicy(QtCore.Qt.ActionsContextMenu)
        self.quitAction = QtWidgets.QAction("Salir", self,shortcut="Ctrl+Q", triggered=self.close)
        self.addAction(self.quitAction)

    def mouseMoveEvent(self, event):
        self.center = event.pos()
        self.update()
        super(SpotlightWidget, self).mouseMoveEvent(event)

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.setPen(QtCore.Qt.red)
        painter.setRenderHint(QtGui.QPainter.Antialiasing)
        # painter.setBrush(QtGui.QColor(108, 119, 125, 255))
        radius = 50
        path = QtGui.QPainterPath()
        if not self.center.isNull():
            path.moveTo(self.center + radius/2*QtCore.QPoint(1, 0))
            path.arcTo(QtCore.QRectF(self.center - radius/2*QtCore.QPointF(1, 1), radius*QtCore.QSizeF(1, 1)), 0, 360)
        # path.addRect(QtCore.QRectF(self.rect()))
        painter.drawPath(path)


if __name__ == '__main__':
    import sys

    app = QtWidgets.QApplication(sys.argv)
    w = SpotlightWidget()
    w.show()
    sys.exit(app.exec_())