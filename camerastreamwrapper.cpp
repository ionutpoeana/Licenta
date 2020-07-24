#include "camerastreamwrapper.h"

#include <QDebug>

CameraStreamWrapper::CameraStreamWrapper(QObject *parent) :
    QObject(parent)
{}

CameraStreamWrapper::~CameraStreamWrapper()
{
    delete cameraStream;
}
