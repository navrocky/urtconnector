
#include <common/scoped_tools.h>
#include <remote/manager.h>

#include "sync_settings_form.h"
#include "ui_sync_settings_form.h"
#include "ui_storages.h"

#include <QInputDialog>
#include <QTreeWidget>
#include <QPixmap>

using namespace remote;

typedef syncro_manager::Service Service;
typedef syncro_manager::Storage Storage;
typedef syncro_manager::Object Object;

struct sync_settings_form::Pimpl
{
    Ui_sync_settings_form ui;
    boost::shared_ptr<remote::syncro_manager> sync_man;
};

Q_DECLARE_METATYPE(syncro_manager::Service);
Q_DECLARE_METATYPE(syncro_manager::Storage);
Q_DECLARE_METATYPE(syncro_manager::Object);

sync_settings_form::sync_settings_form(boost::shared_ptr<remote::syncro_manager> sync_man, QWidget* parent)
    : preferences_widget(parent, tr("Synchronization"))
    , p_(new Pimpl)
    , lock_change_(false)
{
    p_->ui.setupUi(this);
    p_->sync_man = sync_man;

    set_icon(QIcon("icons:view-refresh.png"));
    set_header(tr("Server list updating settings"));

    BOOST_FOREACH(Service srv, sync_man->services()){
        QListWidgetItem* item = new QListWidgetItem(srv->caption());
        item->setData(Qt::UserRole, qVariantFromValue(srv));
        p_->ui.srv_list->addItem(item);
    }
    
    BOOST_FOREACH(Object obj, sync_man->objects()){
        QListWidgetItem* item = new QListWidgetItem(obj->name());
        item->setData(Qt::UserRole, qVariantFromValue(obj));
        std::cerr<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
        std::cerr<<"HERE:"<<obj->name().toStdString()<<std::endl;        
        p_->ui.all_objects->addItem(item);
    }
    
    connect(p_->ui.create, SIGNAL(clicked()), SLOT(create()));
    
    connect(p_->ui.erase, SIGNAL(clicked()), SLOT(erase()));
    connect(p_->ui.edit, SIGNAL(clicked()), SLOT(edit()));
    connect(p_->ui.unbind_storage, SIGNAL(clicked()), SLOT(unbind_storage()));
    
    connect(p_->ui.unbind, SIGNAL(clicked()), SLOT(unbind()));
    connect(p_->ui.bind, SIGNAL(clicked()), SLOT(bind()));
    
    connect(p_->ui.unbind_object, SIGNAL(clicked()), SLOT(unbind_object()));
    
    connect(p_->ui.import_btn, SIGNAL(clicked()), SLOT(do_import()));
    connect(p_->ui.export_btn, SIGNAL(clicked()), SLOT(do_export()));
    connect(p_->ui.sync_btn, SIGNAL(clicked()), SLOT(do_sync()));
    
    connect(p_->ui.srv_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(current_srv_changed(QListWidgetItem*,QListWidgetItem*)));
    connect(p_->ui.storage_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(current_st_changed(QListWidgetItem*,QListWidgetItem*)));    
    

    
    connect(p_->sync_man.get(), SIGNAL(storage_changed(remote::syncro_manager::Storage,remote::syncro_manager::Storage)), SLOT(storage_changed(remote::syncro_manager::Storage,remote::syncro_manager::Storage)));
    connect(p_->sync_man.get(), SIGNAL(object_changed(remote::syncro_manager::Object)), SLOT(object_changed(remote::syncro_manager::Object)));
	connect(p_->sync_man.get(), SIGNAL(object_attached(remote::syncro_manager::Object)), SLOT(object_attached(remote::syncro_manager::Object)));
	connect(p_->sync_man.get(), SIGNAL(object_detached(remote::syncro_manager::Object)), SLOT(object_detached(remote::syncro_manager::Object)));

//     connect(p_->ui.geoip_database_choose_button, SIGNAL(clicked()), SLOT(choose_geoip_database()));
//     connect(p_->ui.center_current_row_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
//     connect(p_->ui.clear_offline_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
//     connect(p_->ui.qstat_binary_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
//     connect(p_->ui.qstat_master_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
//     connect(p_->ui.geoip_database_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
//     connect(p_->ui.maxsim_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
//     connect(p_->ui.retry_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
//     connect(p_->ui.interval_spin, SIGNAL(valueChanged(double)), this, SLOT(int_changed()));
//     connect(p_->ui.master_interval_spin, SIGNAL(valueChanged(double)), this, SLOT(int_changed()));
}

void sync_settings_form::current_srv_changed(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (current) {
        p_->ui.obj_list->clear();
        p_->ui.storage_list->clear();
        
        Q_ASSERT(srv_current());
        
        BOOST_FOREACH(Storage storage, p_->sync_man->storages(srv_current())) {
            QVariantMap settings = p_->sync_man->settings(storage);
            QListWidgetItem* item = new QListWidgetItem(p_->sync_man->name(storage));
            item->setData(Qt::UserRole, qVariantFromValue(storage));
            p_->ui.storage_list->addItem(item);
        }
    }
}

void sync_settings_form::current_st_changed(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (current) {
        p_->ui.obj_list->clear();
        
        Q_ASSERT(st_current());
        
        BOOST_FOREACH(Object object, p_->sync_man->objects(st_current())) {
            QListWidgetItem* item = new QListWidgetItem(object->name());
            item->setData(Qt::UserRole, qVariantFromValue(object));
            p_->ui.obj_list->addItem(item);
        }
    }
}


void sync_settings_form::create()
{
    Service service = srv_current();
    if (!service) return;
    
    const QString storage_name = QInputDialog::getText(this, "Creating storage", "Name");
    const std::auto_ptr<QVariantMap> settings = service->configure();

    if (!settings.get()) return;
    
    p_->sync_man->create(service, storage_name, *settings);
}

void sync_settings_form::erase()
{
    Storage storage = st_current();
    if (!storage) return;
    
    p_->sync_man->remove(storage);
}


void sync_settings_form::edit()
{
    Service service = srv_current();
    Storage storage = st_current();
    
    if (!service || !storage) return;
    
    const QString storage_name = QInputDialog::getText(this, "Editing storage", "Name", QLineEdit::Normal, p_->sync_man->name(storage));

    const std::auto_ptr<QVariantMap> settings = service->configure(p_->sync_man->settings(storage).value("data").value<QVariantMap>());

    if (!settings.get()) return;
    
    p_->sync_man->remove(storage);
    p_->sync_man->create(service, storage_name, *settings);
}

void sync_settings_form::unbind_storage()
{
    Storage storage = st_current();
    if (!storage) return;
    
    p_->sync_man->unbind(storage);
}

void sync_settings_form::unbind()
{
    Storage storage = st_current();
    Object object = obj_current();    
    
    if (!storage || !object) return;
    
    p_->sync_man->unbind(object, storage);
}

void sync_settings_form::bind()
{
    Storage storage = st_current();
    
    QListWidgetItem* item = p_->ui.all_objects->currentItem();
    Object object = (item) ? item->data(Qt::UserRole).value<Object>() : Object();
    
    if (!storage || !object) return;
    
    p_->sync_man->bind(object, storage);
    std::cerr<<"bbbbb"<<std::endl;
}


void sync_settings_form::unbind_object()
{
    QListWidgetItem* item = p_->ui.all_objects->currentItem();
    Object object = (item) ? item->data(Qt::UserRole).value<Object>() : Object();
    if (!object) return;
    
    p_->sync_man->unbind(object);
}



void sync_settings_form::storage_changed(const syncro_manager::Storage& current, const syncro_manager::Storage& previous)
{
    if (!current)     //storage deleted
    {
        Q_ASSERT(previous.get());
        QList<QListWidgetItem *> items = p_->ui.storage_list->findItems(p_->sync_man->name(previous), Qt::MatchExactly);
        if (items.isEmpty()) return;
        Q_ASSERT(items.size() == 1);
        delete items.front();
    }
    
    if (QListWidgetItem* srv_item = p_->ui.srv_list->currentItem())
    {
        syncro_manager::Service service = srv_item->data(Qt::UserRole).value<syncro_manager::Service>();
        if (previous || p_->sync_man->service(current) != service)
            return;
        
        QVariantMap settings = p_->sync_man->settings(current);
        QListWidgetItem* item = new QListWidgetItem(p_->sync_man->name(current));
        item->setData(Qt::UserRole, qVariantFromValue(current));
        p_->ui.storage_list->addItem(item);
    }
}

void sync_settings_form::object_changed(const remote::syncro_manager::Object& current)
{
    current_st_changed(p_->ui.storage_list->currentItem(), 0);    
}

void sync_settings_form::object_attached(const remote::syncro_manager::Object& obj)
{
	QListWidgetItem* item = new QListWidgetItem(obj->name());
	item->setData(Qt::UserRole, qVariantFromValue(obj));

	p_->ui.all_objects->addItem(item);

    current_st_changed(p_->ui.storage_list->currentItem(), 0);  
}

void sync_settings_form::object_detached(const remote::syncro_manager::Object& obj)
{
	Q_ASSERT(obj.get());
	const QString name = obj->name();
	QList<QListWidgetItem *> items = p_->ui.all_objects->findItems(name, Qt::MatchExactly);

	BOOST_FOREACH(QListWidgetItem * item, p_->ui.all_objects->findItems(name, Qt::MatchExactly)) {
		delete item;
	}

	current_st_changed(p_->ui.storage_list->currentItem(), 0);  
}



void sync_settings_form::int_changed()
{
    if (!lock_change_)
        emit changed();
}

void sync_settings_form::update_preferences()
{
    SCOPE_COCK_FLAG(lock_change_);

//     app_settings as;
//     p_->ui.center_current_row_check->setChecked(as.center_current_row());
//     p_->ui.clear_offline_check->setChecked(as.clear_offline());
//     p_->ui.geoip_database_edit->setText(as.geoip_database());
// 
//     qstat_options qs;
//     p_->ui.qstat_binary_edit->setText(qs.qstat_path());
//     p_->ui.qstat_master_edit->setText(qs.master_server());
//     p_->ui.maxsim_spin->setValue(qs.max_sim_queries());
//     p_->ui.retry_spin->setValue(qs.retry_number());
//     p_->ui.interval_spin->setValue(qs.retry_interval());
//     p_->ui.master_interval_spin->setValue(qs.retry_master_interval());
}

void sync_settings_form::accept()
{
//     app_settings as;
//     as.center_current_row_set(p_->ui.center_current_row_check->isChecked());
//     as.clear_offline_set(p_->ui.clear_offline_check->isChecked());
//     as.geoip_database_set(p_->ui.geoip_database_edit->text());
// 
//     qstat_options qs;
//     qs.qstat_path_set(p_->ui.qstat_binary_edit->text());
//     qs.master_server_set(p_->ui.qstat_master_edit->text());
//     qs.max_sim_queries_set(p_->ui.maxsim_spin->value());
//     qs.retry_number_set(p_->ui.retry_spin->value());
//     qs.retry_interval_set(p_->ui.interval_spin->value());
//     qs.retry_master_interval_set(p_->ui.master_interval_spin->value());
}

void sync_settings_form::reject()
{
    update_preferences();
}

void sync_settings_form::reset_defaults()
{
//     qstat_options qo;
//     qo.master_server_reset();
//     qo.max_sim_queries_reset();
//     qo.qstat_path_reset();
//     qo.retry_interval_reset();
//     qo.retry_master_interval_reset();
//     qo.retry_number_reset();

    update_preferences();
}


void sync_settings_form::do_import()
{
    Object object = obj_current();
    if (!object) return;
    
    p_->sync_man->get(object);
}

void sync_settings_form::do_export()
{
    Object object = obj_current();
    if (!object) return;
    
    p_->sync_man->put(object);
}

void sync_settings_form::do_sync()
{
    Object object = obj_current();
    if (!object) return;
    
    p_->sync_man->sync(object);
}


syncro_manager::Service sync_settings_form::srv_current() const
{
    QListWidgetItem* item = p_->ui.srv_list->currentItem();
    
    return (item) ? item->data(Qt::UserRole).value<Service>() : Service();
}

syncro_manager::Storage sync_settings_form::st_current() const
{
    QListWidgetItem* item = p_->ui.storage_list->currentItem();
    
    return (item) ? item->data(Qt::UserRole).value<Storage>() : Storage();
}

syncro_manager::Object sync_settings_form::obj_current() const
{
    QListWidgetItem* item = p_->ui.obj_list->currentItem();
    
    return (item) ? item->data(Qt::UserRole).value<Object>() : Object();
}


struct sync_settings_form2::Pimpl
{
    Ui::storages_settings_form2 ui;
    boost::shared_ptr<remote::syncro_manager> sync_man;
};

sync_settings_form2::sync_settings_form2(boost::shared_ptr< syncro_manager > sync_man, QWidget* parent)
    : preferences_widget(parent, tr("Synchronization"))
    , p_(new Pimpl)
    , lock_change_(false)    
{
    p_->ui.setupUi(this);
    p_->sync_man = sync_man;
    
    set_icon(QIcon("icons:view-refresh.png"));
    set_header(tr("Server list updating settings"));

    p_->ui.storageslst->setHeaderHidden(true);
    p_->ui.storageslst->setIconSize(QSize(25,25));
    p_->ui.objectlst->setIconSize(QSize(25,25));
    
    connect(p_->ui.storageslst, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*))
        , SLOT(current_storage_changed(QTreeWidgetItem*,QTreeWidgetItem*)));
    
    connect(p_->ui.objectlst, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))
        , SLOT(current_object_changed(QListWidgetItem*,QListWidgetItem*)));
    
    connect(p_->sync_man.get(), SIGNAL(storage_changed(remote::syncro_manager::Storage,remote::syncro_manager::Storage))
        , SLOT(update()));
    connect(p_->sync_man.get(), SIGNAL(object_changed(remote::syncro_manager::Object))
        , SLOT(update()));
    connect(p_->sync_man.get(), SIGNAL(object_attached(remote::syncro_manager::Object))
        , SLOT(update()));
    connect(p_->sync_man.get(), SIGNAL(object_detached(remote::syncro_manager::Object))
        , SLOT(update()));
    
    connect(p_->ui.addbtn, SIGNAL(clicked()), SLOT(create()));
    connect(p_->ui.editbtn, SIGNAL(clicked()), SLOT(edit()));
    connect(p_->ui.delbtn, SIGNAL(clicked()), SLOT(destroy()));    
    
    connect(p_->ui.bindbtn, SIGNAL(clicked()), SLOT(bind()));
    connect(p_->ui.unbindbtn, SIGNAL(clicked()), SLOT(unbind()));
    connect(p_->ui.unbindallbtn, SIGNAL(clicked()), SLOT(unbindall()));
    
    connect(p_->ui.importbtn, SIGNAL(clicked()), SLOT(do_import()));
    connect(p_->ui.exportbtn, SIGNAL(clicked()), SLOT(do_export()));
    connect(p_->ui.syncbtn, SIGNAL(clicked()), SLOT(do_sync()));
    
    update();
}

void sync_settings_form2::update()
{
    current_storage_changed(0, 0);
    current_object_changed(0, 0);

    p_->ui.objectlst->clear();
    p_->ui.storageslst->clear();
    
    BOOST_FOREACH (Service srv, p_->sync_man->services()) {
        QTreeWidgetItem* srvitem = new QTreeWidgetItem(QStringList() << srv->caption());
        p_->ui.storageslst->setItemsExpandable(false);
        
        srvitem->setData(0, Qt::UserRole, qVariantFromValue(srv));
        srvitem->setIcon(0, srv->icon());
        p_->ui.storageslst->insertTopLevelItem(0, srvitem);
        
        BOOST_FOREACH (Storage storage, p_->sync_man->storages(srv)) {
            QTreeWidgetItem* stitem = new QTreeWidgetItem(srvitem, QStringList() << p_->sync_man->name(storage));
            stitem->setData(0, Qt::UserRole, qVariantFromValue(storage));
            
            BOOST_FOREACH(Object obj, p_->sync_man->objects(storage)) {
                QTreeWidgetItem* objitem = new QTreeWidgetItem(stitem, QStringList() << obj->name());
                objitem->setData(0, Qt::UserRole, qVariantFromValue(obj));
                objitem->setIcon(0, obj->icon());
            }
            stitem->setExpanded(true);
        }
        srvitem->setExpanded(true);        
    }
    
    BOOST_FOREACH (Object obj, p_->sync_man->objects()) {
        QListWidgetItem* item = new QListWidgetItem(obj->name());
        item->setData(Qt::UserRole, qVariantFromValue(obj));
        item->setIcon(obj->icon());
        p_->ui.objectlst->addItem(item);
    }
}

template <typename T>
T sync_settings_form2::current() const
{
    QTreeWidgetItem* item = p_->ui.storageslst->currentItem();
    
    return (item) ? item->data(0, Qt::UserRole).value<T>() : T();
}

Object sync_settings_form2::current_object() const
{
    QListWidgetItem* item = p_->ui.objectlst->currentItem();
    
    return (item) ? item->data(Qt::UserRole).value<Object>() : Object();
}


void sync_settings_form2::current_storage_changed(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    p_->ui.addbtn->setEnabled(false);
    p_->ui.delbtn->setEnabled(false);
    p_->ui.editbtn->setEnabled(false);
    p_->ui.bindbtn->setEnabled(false);
    p_->ui.unbindbtn->setEnabled(false);
    
    if (current)
    {
        QVariant data = current->data(0, Qt::UserRole);
        
        if (data.canConvert<Service>())
        {
            p_->ui.addbtn->setEnabled(true);
        }
        
        if (data.canConvert<Storage>())
        {
            p_->ui.delbtn->setEnabled(true);
            p_->ui.editbtn->setEnabled(true);
            
            QListWidgetItem* obj = p_->ui.objectlst->currentItem();
            if (obj)
            {
                p_->ui.bindbtn->setEnabled(true);
            }
        }
        
        if (data.canConvert<Object>())
        {
            p_->ui.unbindbtn->setEnabled(true);
        }
    }
}

void sync_settings_form2::current_object_changed(QListWidgetItem* current, QListWidgetItem* previous)
{
    p_->ui.bindbtn->setEnabled(false);
    p_->ui.unbindallbtn->setEnabled(false);
    
    if (current)
    {
        p_->ui.unbindallbtn->setEnabled(true);
        
        if (QTreeWidgetItem* storage = p_->ui.storageslst->currentItem())
        {
            QVariant data = storage->data(0, Qt::UserRole);
            
            if (data.canConvert<Storage>())
            {
                p_->ui.bindbtn->setEnabled(true);
            }
        }
    }
}

template <typename T>
QTreeWidgetItem * find_item(QTreeWidgetItem *parent, const T& t)
{
    if (parent->data(0, Qt::UserRole).value<T>() == t) {
        return parent;
    }
        
    for (int i = 0; i< parent->childCount(); ++i) {
        if (QTreeWidgetItem * item = find_item(parent->child(i), t)) {
            return item;
        }
    }
    return 0;
}

template <typename T>
QTreeWidgetItem * find_item(QTreeWidget* tree, const T& t)
{
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        if (QTreeWidgetItem * item = find_item(tree->topLevelItem(i, t))) {
            return item;
        }
    }
    return 0;
}

void sync_settings_form2::create()
{
    Service service = current<Service>();
    Q_ASSERT(service);
    
    const QString storage_name = QInputDialog::getText(this, "Creating storage", "Name");
    if (storage_name.isEmpty()) return;
    
    const std::auto_ptr<QVariantMap> settings = service->configure();
    if (!settings.get()) return;
    
    p_->sync_man->create(service, storage_name, *settings);
}

void sync_settings_form2::edit()
{
    Storage storage = current<Storage>();
    Q_ASSERT(storage);    
    
    Service service = p_->sync_man->service(storage);
    Q_ASSERT(service);    
    
    const QString storage_name = QInputDialog::getText(this, "Editing storage", "Name", QLineEdit::Normal, p_->sync_man->name(storage));
    if (storage_name.isEmpty()) return;    

    const std::auto_ptr<QVariantMap> settings = service->configure(p_->sync_man->settings(storage).value("data").value<QVariantMap>());
    if (!settings.get()) return;
    
    p_->sync_man->remove(storage);
    p_->sync_man->create(service, storage_name, *settings);
}

void sync_settings_form2::destroy()
{
    Storage storage = current<Storage>();
    Q_ASSERT(storage);  
    
    p_->sync_man->remove(storage);
}

void sync_settings_form2::bind()
{
    Storage storage = current<Storage>();
    Q_ASSERT(storage);
    
    Object object = current_object();
    Q_ASSERT(object);
    
    p_->sync_man->bind(object, storage);
}

void sync_settings_form2::unbind()
{
    Object object = current<Object>();  
    Q_ASSERT(object);

    Storage storage = p_->ui.storageslst->currentItem()->parent()->data(0, Qt::UserRole).value<Storage>();
    Q_ASSERT(storage);
    
    p_->sync_man->unbind(object, storage);
}

void sync_settings_form2::unbindall()
{
    Object object = current_object();
    Q_ASSERT(object);
    
    p_->sync_man->unbind(object);
}

void sync_settings_form2::do_import()
{
    BOOST_FOREACH(Object o, p_->sync_man->objects()) {
        p_->sync_man->get(o);
    }
}

void sync_settings_form2::do_export()
{
    BOOST_FOREACH(Object o, p_->sync_man->objects()) {
        p_->sync_man->put(o);
    }
}

void sync_settings_form2::do_sync()
{
    BOOST_FOREACH(Object o, p_->sync_man->objects()) {
        if (!p_->sync_man->storages(o).empty())
            p_->sync_man->sync(o);
    }
}



void sync_settings_form2::accept()
{
    preferences_widget::accept();
}


void sync_settings_form2::reject()
{
    preferences_widget::reject();
}

void sync_settings_form2::reset_defaults()
{
    preferences_widget::reset_defaults();
}

void sync_settings_form2::update_preferences()
{
    update();
    preferences_widget::update_preferences();
}


