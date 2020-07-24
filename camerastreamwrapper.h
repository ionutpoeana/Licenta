#ifndef WORKER_H
#define WORKER_H

#include "ocv/camerastream.h"

#include <QObject>
#include <QTimer>

class CameraStreamWrapper : public QObject
{
    Q_OBJECT
public:
    explicit CameraStreamWrapper(QObject *parent = 0);
    virtual ~CameraStreamWrapper();
    cv::CameraStream *cameraStream;

};

#endif // WORKER_H
