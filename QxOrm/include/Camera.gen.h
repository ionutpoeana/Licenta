/************************************************************************************************
** File created by QxEntityEditor 1.2.5 (2020/07/07 22:14) : please, do NOT modify this file ! **
************************************************************************************************/

#ifndef _LICENTA_CAMERA_H_
#define _LICENTA_CAMERA_H_

class Violation;

class LICENTA_EXPORT Camera
{

   QX_REGISTER_FRIEND_CLASS(Camera)

public:

   typedef qx::QxCollection<long, std::shared_ptr<Violation> > type_ListOfViolation;

protected:

   long m_CameraId;
   QString m_Name;
   QString m_Location;
   type_ListOfViolation m_ListOfViolation;

public:

   Camera();
   Camera(const long & id);
   virtual ~Camera();

   long getCameraId() const;
   QString getName() const;
   QString getLocation() const;
   type_ListOfViolation getListOfViolation() const;
   type_ListOfViolation & ListOfViolation();
   const type_ListOfViolation & ListOfViolation() const;

   void setCameraId(const long & val);
   void setName(const QString & val);
   void setLocation(const QString & val);
   void setListOfViolation(const type_ListOfViolation & val);

   type_ListOfViolation getListOfViolation(bool bLoadFromDatabase, const QString & sAppendRelations = QString(), QSqlDatabase * pDatabase = NULL, QSqlError * pDaoError = NULL);
   type_ListOfViolation & ListOfViolation(bool bLoadFromDatabase, const QString & sAppendRelations = QString(), QSqlDatabase * pDatabase = NULL, QSqlError * pDaoError = NULL);

public:

   static QString relation_ListOfViolation(bool key = false) { return (key ? QString("ListOfViolation") : QString("list_of_violation")); }

public:

   static QString column_CameraId(bool key = false) { return (key ? QString("CameraId") : QString("camera_id")); }
   static QString column_Name(bool key = false) { return (key ? QString("Name") : QString("name")); }
   static QString column_Location(bool key = false) { return (key ? QString("Location") : QString("location")); }

public:

   static QString table_name(bool key = false) { return (key ? QString("Camera") : QString("t_camera")); }

};

typedef std::shared_ptr<Camera> Camera_ptr;
typedef qx::QxCollection<long, Camera_ptr> list_of_Camera;
typedef std::shared_ptr<list_of_Camera> list_of_Camera_ptr;

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_LICENTA(Camera, qx::trait::no_base_class_defined, 0, Camera)

#include "../include/Violation.gen.h"

#include "../custom/include/Camera.h"

#endif // _LICENTA_CAMERA_H_
