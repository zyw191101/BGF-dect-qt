QT       += core gui widgets network


# 项目名称和生成的可执行文件
TARGET = BackgroundDetect
TEMPLATE = app

#opencv
INCLUDEPATH += D:\3rdParty\opencvNew\opencv\opencvBuild\install\include
LIBS +=D:\3rdParty\opencvNew\opencv\opencvBuild\lib\libopencv_*.a

# 源文件
SOURCES += main.cpp \
    backgroundldetect.cpp \
    imageshift.cpp \
           mainwindow.cpp \


# 头文件
HEADERS += mainwindow.h \
    backgroundldetect.h \
    imageshift.h \



# 界面资源文件（如果有）
# RESOURCES += resources.qrc

# 调试模式下的配置
debug {
    DEFINES += DEBUG_BackgroundDete
}
