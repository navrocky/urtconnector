
///Базовый класс для фильтра.
/*!
    - содержит информацию о фильтре.
    - предрставляет конфигурирование фильтра
*/
class filter
{
public:
    //То что будем фильтровать
    typedef server_info FilteredEntity;
    //Тип функуии в которой проиводится фильтрация
    typedef bool(OperatorType)(const FilteredEntity&);
    //Тип функтора который может обернуть фильтр
    typedef boost::function<OperatorType> FilterType;


    filter();
    virtual ~filter();

    ///return result of filtering
    virtual bool operator()(const FilteredEntity& entity) const ;

    ///Установить id фьлтра
    void set_uid(const std::string& uid);
    const std::string& uid() const;

    ///Установить caption фьлтра
    void set_caption(const std::string& caption);
    const std::string& caption() const;

    ///Установить description фьлтра
    void set_description(const std::string& description);
    const std::string& description() const;

    /*! Save filter settings to QByteArray. */
    virtual QByteArray save() = 0;
    /*! Load filter setings from QByteArray. */
    virtual void load(const QByteArray& ba) = 0;

    QWidget* configure();

protected:
    ///Откопировать внутренности p_
    virtual void fork();

private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};

///Композитный фильтр
/*!
    - позволяет группировать другие фильтры
*/
class composite_filter: public filter
{
public:
    enum Operation{ Or, And, OrNot, AndNot };

    ///Список фильтров
    typedef std::list<filter> FilterList;

    ///Списки фильтров, сортированные по операциям
    typedef std::map<Operation, FilterList> FilterMap;

    ///Добавить операцию и фильтр
    void add(Operation op, const filter& filter);

    ///Получить список всех фильтров
    const FilterMap& list () const;

    ///Получить список фильтров для опрерации
    const FilterList& list (Operation op) const;
    
private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};


///Фабрика фильтровать
class filter_factory
{
public:
    ///Тип функуии которая создает фильтр
    typedef filter(CreatorType)(const QVariant&);
    ///Тип функтора
    typedef boost::function<CreatorType> Creator;

    typedef std::string Id;

    const Id& registrate( const std::string& type, const std::string& desc, Creator );

    filter create( const Id& id, const QVariant& = QVariant() );
};







