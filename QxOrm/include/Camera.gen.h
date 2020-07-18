/************************************************************************************************
** File created by QxEntityEditor 1.2.5 (2020/07/15 00:46) : please, do NOT modify this file ! **
************************************************************************************************/

#ifndef _LICENTA_CAMERA_H_
#define _LICENTA_CAMERA_H_

class Violation;

class LICENTA_EXPORT Camera
{

   QX_REGISTER_FRIEND_CLASS(Camera)

public:

   typedef QList<QSharedPointer<Violation> > type_ListOfViolation;

protected:

   QUuid m_CameraId;
   QString m_Name;
   QString m_Location;
   QString m_StreamLocation;
   type_ListOfViolation m_ListOfViolation;

public:

   Camera();
   Camera(const QUuid & id);
   virtual ~Camera();

   QUuid getCameraId() const;
   QString getName() const;
   QString getLocation() const;
   QString getStreamLocation() const;
   type_ListOfViolation getListOfViolation() const;
   type_ListOfViolation & ListOfViolation();
   const type_ListOfViolation & ListOfViolation() const;

   void setCameraId(const QUuid & val);
   void setName(const QString & val);
   void setLocation(const QString & val);
   void setStreamLocation(const QString & val);
   void setListOfViolation(const type_ListOfViolation & val);

   type_ListOfViolation getListOfViolation(bool bLoadFromDatabase, const QString & sAppendRelations = QString(), QSqlDatabase * pDatabase = NULL, QSqlError * pDaoError = NULL);
   type_ListOfViolation & ListOfViolation(bool bLoadFromDatabase, const QString & sAppendRelations = QString(), QSqlDatabase * pDatabase = NULL, QSqlError * pDaoError = NULL);

public:

   static QString relation_ListOfViolation(bool key = false) { return (key ? QString("ListOfViolation") : QString("list_of_violation")); }

public:

   static QString column_CameraId(bool key = false) { return (key ? QString("CameraId") : QString("camera_id")); }
   static QString column_Name(bool key = false) { return (key ? QString("Name") : QString("name")); }
   static QString column_Location(bool key = false) { return (key ? QString("Location") : QString("location")); }
   static QString column_StreamLocation(bool key = false) { Q_UNUSED(key); return "StreamLocation"; }

public:

   static QString table_name(bool key = false) { return (key ? QString("Camera") : QString("t_camera")); }

};

typedef std::shared_ptr<Camera> Camera_ptr;
typedef qx::QxCollection<QUuid, Camera_ptr> list_of_Camera;
typedef std::shared_ptr<list_of_Camera> list_of_Camera_ptr;

QX_REGISTER_PRIMARY_KEY(Camera, QUuid)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_LICENTA(Camera, qx::trait::no_base_class_defined, 0, Camera)

#include "../include/Violation.gen.h"

#include "../custom/include/Camera.h"

#endif // _LICENTA_CAMERA_H_
