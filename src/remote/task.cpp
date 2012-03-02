
#include <boost/bind.hpp>

#include "cl/syslog/syslog.h"
#include "common/qt_syslog.h"

#include "task.h"

SYSLOG_MODULE(syncro_manager::task);

using namespace remote;

///const cast pointer to refference(to convert "key" objects to "useful" objects)
template <typename T>
inline T& cast(const boost::shared_ptr<const T>& ptr) {
	return *const_cast<T*>(ptr.get());
}

struct syncro_manager::get_task : public syncro_manager::task {
	Object object;
	Storages storages;
	remote::group group;
	remote::action* action;
	remote::group::Entries entries;

	get_task(const Object& o, const Storages& st, const remote::group& gr)
		: object(o), storages(st), group(gr), action(0){}

	virtual void start() {
		Q_ASSERT(!storages.empty());

		action = cast(*storages.begin()).get(group.type());

		connect(action, SIGNAL(loaded(const remote::group&)), SLOT(loaded(const remote::group&)));
		connect(action, SIGNAL(error(QString)), SLOT(error(QString)));
		connect(action, SIGNAL(finished()), SLOT(finished()));
	}

	virtual void loaded(const remote::group& obj) {
		Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check

		entries = remote::merge(obj.entries(), entries);        
	}

	virtual void error(const QString& error) {
		LOG_ERR << "Error:" << error;
	}

	virtual void finished() {
		Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
		Q_ASSERT(!storages.empty());

		storages.erase(storages.begin());

		if (storages.empty())
		{
			emit completed(object, remote::group(group.type(), entries));
		}
		else
		{
			start();
		}
	}
};

struct syncro_manager::sync_task : public syncro_manager::task {
	Object object;
	Storages get_storages;
	Storages put_storages;
	remote::group group;
	remote::action* action;
	remote::group::Entries entries;

	sync_task(const Object& o, const Storages& st, const remote::group& gr)
		: object(o), get_storages(st), put_storages(st), group(gr), action(0)
		, entries(group.entries())
	{}

	virtual void start() {
		LOG_INFO << "start...";
		start_get();
	}

	void start_get() {
		Q_ASSERT(!get_storages.empty());

		LOG_INFO << " <<< start get...";
		action = cast(*get_storages.begin()).get(group.type());

		connect(action, SIGNAL(loaded(const remote::group&)), SLOT(loaded(const remote::group&)));
		connect(action, SIGNAL(error(QString)), SLOT(error(QString)));
		connect(action, SIGNAL(finished()), SLOT(finished()));
		action->start();
	}

	void start_put() {
		LOG_INFO << "start PUT... >>> ";
		Q_ASSERT(!put_storages.empty());

		action = cast(*put_storages.begin()).put(group);

		connect(action, SIGNAL(saved()), SLOT(saved()));
		connect(action, SIGNAL(error(QString)), SLOT(error(QString)));
		connect(action, SIGNAL(finished()), SLOT(finished()));
		action->start();
	}

	virtual void loaded(const remote::group& obj) {
		Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check

		LOG_INFO << "LAODED";
		entries = remote::merge(obj.entries(), entries);
	}

	virtual void error(const QString& error) {
		LOG_ERR << "Error:" << error;
	}

	virtual void saved() {
		Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
		LOG_INFO << "SAVED >>>";
	}

	virtual void finished() {
		Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check

		LOG_INFO << "finished...";
		(!get_storages.empty())
			? get_finished()
			: put_finished();
	}

	void get_finished() {
		Q_ASSERT(!get_storages.empty());

		LOG_INFO << " <<< finihsed get ...";
		get_storages.erase(get_storages.begin());
		if (get_storages.empty())
		{
			LOG_INFO << " <<< finihsed get ALL";
			remote::group::Entries non_deleted;

			std::remove_copy_if(entries.begin(), entries.end(), std::inserter(non_deleted, non_deleted.end()),
				boost::bind(&remote::intermediate::is_deleted, _1));

			entries.swap(non_deleted);
			group = remote::group(group.type(), entries);
			start_put();
		}
		else
		{
			LOG_INFO << " <<< partial";
			start_get();
		}
	}

	void put_finished() {
		Q_ASSERT(get_storages.empty());
		Q_ASSERT(!put_storages.empty());

		LOG_INFO << " <<< finihsed put ...";

		put_storages.erase(put_storages.begin());
		if (put_storages.empty())
		{
			LOG_INFO << " <<< finihsed put ALL";
			emit completed(object, group);
		}
		else
		{
			LOG_INFO << " <<< partial put";
			start_put();
		}
	}
};


boost::shared_ptr<syncro_manager::task>
syncro_manager::task_factory::create_get_task(const syncro_manager::Object& o, const syncro_manager::Storages& st, const group& gr)
{
	return boost::shared_ptr<syncro_manager::task>(new syncro_manager::get_task(o, st, gr));
}

boost::shared_ptr<syncro_manager::task>
syncro_manager::task_factory::create_sync_task(const syncro_manager::Object& o, const syncro_manager::Storages& st, const group& gr)
{
	return boost::shared_ptr<syncro_manager::task>(new syncro_manager::sync_task(o, st, gr));
}
