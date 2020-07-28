#include "camerastreamwrapper.h"

#include <QDebug>

CameraStreamWrapper::CameraStreamWrapper(QObject *parent) :
    QObject(parent)
{}

CameraStreamWrapper::~CameraStreamWrapper()
{
    if(cameraStream!=nullptr)
    {
        delete cameraStream;
        cameraStream = nullptr;
    }
}
