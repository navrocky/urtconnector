
#include <set>
#include <map>

#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/shared_ptr.hpp>


namespace remote {

template<class KeyExtractor1,class KeyExtractor2>
struct key_from_key
{
public:
    typedef typename KeyExtractor1::result_type result_type;

    key_from_key(
        const KeyExtractor1& key1_ = KeyExtractor1(),
        const KeyExtractor2& key2_ = KeyExtractor2()):
        key1(key1_),key2(key2_)
    {}

    template<typename Arg>
    result_type operator()(Arg& arg)const
    {
        return key1(key2(arg));
    }

private:
    KeyExtractor1 key1;
    KeyExtractor2 key2;
};


struct syncro_manager::Pimpl {

    typedef std::set<Service> Services;
    typedef std::set<Storage> Storages;
    typedef std::set<Object>  Objects;

    struct storage_data {
        const Service service;
        const Storage storage;
        const QString storage_uid;

        storage_data(Service se, Storage st, const QString& uid)
            : service(se), storage(st), storage_uid(uid){}
        
        bool operator == (const storage_data& other) const {
            return storage == other.storage;
        }
    };    

    struct objects_data {
        Object object;
        Storage storage;
        
        objects_data(Object o, Storage st) : object(o), storage(st) {}
        
        bool operator == (const objects_data& other) const {
            return object == other.object && storage == other.storage;
        }
    };   

    struct hash {
        std::size_t operator()(storage_data const& data) const {
            return 
                reinterpret_cast<int>(data.service.get()) << 8 |
                reinterpret_cast<int>(data.storage.get()); 
        }
        
        std::size_t operator()(objects_data const& data) const {
            return 
                reinterpret_cast<int>(data.object.get()) << 8 |
                reinterpret_cast<int>(data.storage.get()); 
        }
    };

    typedef boost::multi_index_container<
        storage_data,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<Storage>, BOOST_MULTI_INDEX_MEMBER(storage_data, const Storage, storage)
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<Service>, BOOST_MULTI_INDEX_MEMBER(storage_data, const Service, service)
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<QString>,
                key_from_key<
                    BOOST_MULTI_INDEX_CONST_MEM_FUN(remote::service, const QString&, caption),
                    BOOST_MULTI_INDEX_MEMBER(storage_data, const Service, service)
                >
            >            
        >
    > StorageData;

    typedef boost::multi_index_container<
        objects_data,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<Object>, BOOST_MULTI_INDEX_MEMBER(objects_data, Object, object)
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<Storage>, BOOST_MULTI_INDEX_MEMBER(objects_data, Storage, storage)
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<QString>,
                key_from_key<
                    BOOST_MULTI_INDEX_CONST_MEM_FUN(syncro_manager::object, const QString&, name),
                    BOOST_MULTI_INDEX_MEMBER(objects_data, Object, object)
                >
            >,
            boost::multi_index::hashed_unique<
                boost::multi_index::tag<objects_data>,
                boost::multi_index::identity<objects_data>,
                hash
            >              
        >
    > ObjectsData;

    Services srv_list;
    Objects obj_list;

    StorageData st_data;     
    ObjectsData obj_data;

    /*! metafunction to get iterator type*/
    template <typename Tag>
    struct st_iterator {
        typedef typename StorageData::index<Tag>::type::iterator type;
    };

    /*! metafunction to get iterator type*/
    template <typename Tag>
    struct obj_iterator {
        typedef typename ObjectsData::index<Tag>::type::iterator type;
    };

    //  aliases

    template <typename Tag>
    typename st_iterator<Tag>::type st_begin() { return st_data.get<Tag>().begin(); }

    template <typename Tag>
    typename st_iterator<Tag>::type st_end() { return st_data.get<Tag>().end(); }

    template <typename Tag>
    typename st_iterator<Tag>::type st_find(const Tag& key) { return st_data.get<Tag>().find(key); }

    template <typename Tag>
    typename obj_iterator<Tag>::type obj_begin() { return obj_data.get<Tag>().begin(); }

    template <typename Tag>
    typename obj_iterator<Tag>::type obj_end() { return obj_data.get<Tag>().end(); }

    template <typename Tag>
    typename obj_iterator<Tag>::type obj_find(const Tag& key) { return obj_data.get<Tag>().find(key); }

    template <typename Tag>
    Services services(const Tag& key) const {
        return Services (
            make_transform_iterator(st_data.get<Tag>().lower_bound(key), boost::bind(&storage_data::service, _1)),
            make_transform_iterator(st_data.get<Tag>().upper_bound(key), boost::bind(&storage_data::service, _1))
        );
    }

    Storages storages() const {
        return Storages(
            make_transform_iterator(st_data.begin(), boost::bind(&storage_data::storage, _1)),
            make_transform_iterator(st_data.end(), boost::bind(&storage_data::storage, _1))
        );
    }

    template <typename Tag>
    Storages storages(const Tag& key) const {
        return Storages(
            make_transform_iterator(st_data.get<Tag>().lower_bound(key), boost::bind(&storage_data::storage, _1)),
            make_transform_iterator(st_data.get<Tag>().upper_bound(key), boost::bind(&storage_data::storage, _1))
        );
    }

    Storages storages(const Object& obj) const {
        return Storages(
            make_transform_iterator(obj_data.get<Object>().lower_bound(obj), boost::bind(&objects_data::storage, _1)),
            make_transform_iterator(obj_data.get<Object>().upper_bound(obj), boost::bind(&objects_data::storage, _1))
        );
    }

    std::set<Object> objects(const Storage& storage) const {
        return std::set<Object>(
            boost::make_transform_iterator(obj_data.get<Storage>().lower_bound(storage), boost::bind(&objects_data::object, _1)),
            boost::make_transform_iterator(obj_data.get<Storage>().upper_bound(storage), boost::bind(&objects_data::object, _1))
        );
    }

    /*! UID to access "services" group of syncro_manager settings */
    QString services_uid;

    typedef std::map<Object, boost::shared_ptr<task> > Tasks;
    Tasks active;
};

};
