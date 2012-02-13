
#include <common/scoped_tools.h>
#include <remote/manager.h>

#include "sync_settings_form.h"
#include "ui_sync_settings_form.h"

#include <QInputDialog>

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
    
    connect(p_->ui.srv_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(current_srv_changed(QListWidgetItem*,QListWidgetItem*)));
    connect(p_->ui.storage_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(current_st_changed(QListWidgetItem*,QListWidgetItem*)));    
    

    
    connect(p_->sync_man.get(), SIGNAL(storage_changed(remote::syncro_manager::Storage,remote::syncro_manager::Storage)), SLOT(storage_changed(remote::syncro_manager::Storage,remote::syncro_manager::Storage)));
    connect(p_->sync_man.get(), SIGNAL(object_changed(remote::syncro_manager::Object,remote::syncro_manager::Object)), SLOT(object_changed(remote::syncro_manager::Object,remote::syncro_manager::Object)));
    
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
            QListWidgetItem* item = new QListWidgetItem(settings["storage_name"].toString());
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
    const QVariantMap settings = service->configure();

    p_->sync_man->create(service, storage_name, settings);
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
    
    const QString storage_name = QInputDialog::getText(this, "Creating storage", "Name", QLineEdit::Normal, storage->name());

    const QVariantMap settings = service->configure(p_->sync_man->settings(storage));

    p_->sync_man->remove(storage);
    p_->sync_man->create(service, storage_name, settings);
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
        QList<QListWidgetItem *> items = p_->ui.storage_list->findItems(previous->name(), Qt::MatchExactly);
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
        QListWidgetItem* item = new QListWidgetItem(current->name());
        item->setData(Qt::UserRole, qVariantFromValue(current));
        p_->ui.storage_list->addItem(item);
    }
}

void sync_settings_form::object_changed(const remote::syncro_manager::Object& current, const remote::syncro_manager::Object& previous)
{
    if (!current)     //Object deleted
    {
        Q_ASSERT(previous.get());
        const QString name = previous->name();
        QList<QListWidgetItem *> items = p_->ui.all_objects->findItems(name, Qt::MatchExactly);
        
        BOOST_FOREACH(QListWidgetItem * item, p_->ui.all_objects->findItems(name, Qt::MatchExactly)) {
            delete item;
        }
    }
    else if (current && !previous)     //Object created
    {
        QListWidgetItem* item = new QListWidgetItem(current->name());
        item->setData(Qt::UserRole, qVariantFromValue(current));
    
        p_->ui.all_objects->addItem(item);
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
    
    p_->sync_man->sync(object);
}

void sync_settings_form::do_export()
{
    Object object = obj_current();
    if (!object) return;
    
    p_->sync_man->put(object);
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



