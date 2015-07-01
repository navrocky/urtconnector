# Plans of development #

This is a sample roadmap:
  * advanced tracking (wait for a friend, wait for a empty slot, and many many more cases) (0.8)
  * game skin (0.8)
  * player profiles (config, nickname)
  * download missing maps
  * store and synchronize player profiles in a cloud (dropbox, ftp, google docs and other)
  * implement game scanning without qstat )
  * urtconnector auto update
  * anything else?
  * ... integrate as plugin with the LeechCraft combine (web-browser, rss, jabber/irc client, etc) %)

# План разработки #

Список того что планируется реализовать в программе в будущем (не столь отдаленном):
  * (Done) система слежения на основе фильтров и автообновления. Позволяет отслеживать определенные события и сигнализировать о них разными способами. Например, можно будет отследить появление игрока или освобождение места на сервере. Также на событие можно назначить и автоматическое действие, например, подключение к серверу.
  * визуальный редактор конфигурации игрока. В него будет входить настройка всех основных параметров с комментариями (имя, моделька, фанстафф-редактор по типу фоторобота), редактор настроек кнопок, биндинги на радио, Также свойства остальных переменных, относящихся к профилю с комментариями к ним.
  * настройка горячих кнопок и тулбаров в программе.
  * реализация нативного протокола обмена с Q3-серверами, отказ от QStat.
  * синхронизация профилей и закладок с онлайн-серверами, например, ftp, или другими.
  * написание инструкции для всего этого в docbook (html, pdf, online) )
  * сборка программы на основных платформах windows, unix (linux, freebsd, solaris etc), mac.
  * список друзей
  * (Partially)универсальный античит. Скриншоты всех игроков матча публикуются на ftp-сервер, просмотр скриншотов, верификация. Помогает от волхака, и возможно от остальных читов, которые как-то проявляют себя на экране. Аимбот обычно видно по демке.
  * (Done)закладка истории подключения к серверам, в ней также можно обновить информацию о текущем состоянии.
  * (Done) гибкая система фильтрации (комбинирование различных фильтров). Предполагаются фильтры по названию сервера, по состоянию, по типу игры, по наличию игроков, по заполненности серверов, по наличию на сервере нескольких игроков одного клана, по имени игроков, по стране, по информационным переменным сервера и т.д.
  * (Done) определение страны нахождения сервера (показ флага страны)
  * (Done) отображение прогресса исполняющихся задач (обновление информации о серверах), возможность их прервать
  * (Done) автообновление списка серверов
  * (Done) наблюдение за буфером обмена, при появлении в нем ифнормации об ip, порте и пароле, автоматическое размещение в полях окна Быстрого соединения. Критерии детекции можно настроить с помощью регулярных выражений. Также можно замигать иконкой в трее и двойной щелчек по ней приведет к запуску игры.
  * (Done) отображение в таблицах и в окне состояния информации о запароленных серверах (выделение их красным цветом или отображение иконки с ключом)
  * (Done) встроенная RCON-консоль для управления сервером, для которого есть RCON-пароль.


Если что-то забыл, пишите каменты ;)