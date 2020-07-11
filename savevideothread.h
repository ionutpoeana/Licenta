#ifndef SAVEVIDEOTHREAD_H
#define SAVEVIDEOTHREAD_H

#include "ocv/proof.h"

#include "widgets/violationwidget.h"

#include "Licenta_all_include.gen.h"

#include <QSqlDatabase>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <queue>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class SaveVideoThread : public QThread
{


    Q_OBJECT
public:
    SaveVideoThread(QSqlDatabase _dataBase, QObject *parent = nullptr);
    ~SaveVideoThread();
    void saveVideo(ViolationProof* violationProof);

signals:
    void videoSaved(const Violation&v);


protected:
    void run() override;

private:
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
    std::queue<ViolationProof*> m_videoQueue;

    QSqlDatabase m_dataBase;

    bool m_abort;
};

#endif // SAVEVIDEOTHREAD_H
