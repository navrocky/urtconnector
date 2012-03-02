
#ifndef URT_REMOTE_TASK_H
#define URT_REMOTE_TASK_H

#include <QObject>

#include "manager.h"

namespace remote {

class syncro_manager::task : public QObject {
	Q_OBJECT

public:
	virtual void start() {};

Q_SIGNALS:
	void completed(const syncro_manager::Object&, const remote::group&);

private Q_SLOTS:
	virtual void loaded(const remote::group& obj) {};
	virtual void saved() {};
	virtual void error(const QString& error) {};
	virtual void finished() {};
};

struct syncro_manager::task_factory {

	static boost::shared_ptr<syncro_manager::task>
	create_get_task(const syncro_manager::Object& o, const syncro_manager::Storages& st, const remote::group& gr);

	static boost::shared_ptr<syncro_manager::task>
	create_sync_task(const syncro_manager::Object& o, const syncro_manager::Storages& st, const remote::group& gr);

};

} //namespace task

#endif


