/************************************************************************************************
** File created by QxEntityEditor 1.2.5 (2020/07/15 00:46) : please, do NOT modify this file ! **
************************************************************************************************/

#ifndef _LICENTA_VIOLATION_H_
#define _LICENTA_VIOLATION_H_

class Camera;

class LICENTA_EXPORT Violation
{

   QX_REGISTER_FRIEND_CLASS(Violation)

public:

   typedef QSharedPointer<Camera> type_Camera;

protected:

   QUuid m_ViolationId;
   QDateTime m_Time;
   QString m_VideoLocation;
   QString m_PhotoLocation;
   int m_RuleType;
   type_Camera m_Camera;

public:

   Violation();
   Violation(const QUuid & id);
   virtual ~Violation();

   QUuid getViolationId() const;
   QDateTime getTime() const;
   QString getVideoLocation() const;
   QString getPhotoLocation() const;
   int getRuleType() const;
   type_Camera getCamera() const;

   void setViolationId(const QUuid & val);
   void setTime(const QDateTime & val);
   void setVideoLocation(const QString & val);
   void setPhotoLocation(const QString & val);
   void setRuleType(const int & val);
   void setCamera(const type_Camera & val);

   type_Camera getCamera(bool bLoadFromDatabase, const QString & sAppendRelations = QString(), QSqlDatabase * pDatabase = NULL, QSqlError * pDaoError = NULL);

public:

   static QString relation_Camera(bool key = false) { return (key ? QString("Camera") : QString("camera")); }

public:

   static QString column_ViolationId(bool key = false) { return (key ? QString("ViolationId") : QString("violation_id")); }
   static QString column_Time(bool key = false) { return (key ? QString("Time") : QString("time")); }
   static QString column_VideoLocation(bool key = false) { return (key ? QString("VideoLocation") : QString("video_location")); }
   static QString column_PhotoLocation(bool key = false) { return (key ? QString("PhotoLocation") : QString("photo_location")); }
   static QString column_RuleType(bool key = false) { return (key ? QString("RuleType") : QString("rule_type")); }

public:

   static QString table_name(bool key = false) { return (key ? QString("Violation") : QString("t_violation")); }

};

typedef std::shared_ptr<Violation> Violation_ptr;
typedef qx::QxCollection<QUuid, Violation_ptr> list_of_Violation;
typedef std::shared_ptr<list_of_Violation> list_of_Violation_ptr;

QX_REGISTER_PRIMARY_KEY(Violation, QUuid)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_LICENTA(Violation, qx::trait::no_base_class_defined, 0, Violation)

#include "../include/Camera.gen.h"

#include "../custom/include/Violation.h"

#endif // _LICENTA_VIOLATION_H_
