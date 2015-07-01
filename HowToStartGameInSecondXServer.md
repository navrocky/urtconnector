# Introduction/Введение #

Under Unix/Linux you can start game under other X-session(because of Alt+Tab problem in quake-engine and compositiong slowdown).
This page describes process of starting game under other X-session.

Под Unix/Linux удобно запускать игру во вторых иксах, т. к. не всегда работает Alt+Tab и при включенном композите наблюдаются существенные торможения. Это статья описывает процесс запуска игры во вторых иксах.



# Detail/Подробно #

First of all you can check that starting other X-session availability:
```
>xinit -- :1
```

**1** - number of free X-display. Usualy main X starting in display with number 0.
Change between display possible by **Ctrl+Alt+Fn**, where **n** - 7 + number of session.

If you have seen a new display(usually with xterm) then all is ok.

Now you can use:
```
xinit /usr/games/bin/urbanterror -- :8
```
to play urbanterror under new X-session.

If you use **urtconnector** just set appropriate checkbox in prefferences.

Сперва необходимо убедиться что запуск дополнительных иксов вам разрешен. Для этого исполняем команду:
```
> xinit -- :1
```

**1** - это номер свободного X-дисплея. Обычно первые иксы стартуют в 0-ой сессии. Переключаться между сессиями можно с помощью комбинации клавиш: **Ctrl+Alt+Fn**, где **n** - 7 + номер сессии.

Если вы увидели появившийся экран, заполненный узорчиком и убогим xterm, то значит все хорошо и можно продолжать.

Теперь в urtconnector в настройках программы включаем галочку "Расширенная строка запуска" и в поле вписываем следующую строку:

```
xinit %bin% +name %name% +connect %addr% +password %pwd% +rconpassword %rcon% +set fs_game q3ut4 -- :8
```

Не забываем указать имя бинарника игры в поле "Полное имя файла бинарника Urban Terror".

# Problems/Проблемы #

Sometime you have not permission to start new X-session. Check that suid bit is enabled on /usr/bin/Xorg.

How to enable start new X-session on ubuntu:
```
The trick is to reconfigure x11-common for “Anyone”. This can be done by running the following command in the terminal:

 $ sudo dpkg-reconfigure x11-common

Then, select the option “Anyone” and then run startx.
```

Problems with launching: Ubuntu ( [HowTo](http://forum.ubuntu.ru/index.php?topic=60830.0) ).

Проблемы могут возникнуть с запуском вторых иксов по причине отсутствия прав на это у пользователя. Проверьте установку suid бита на /usr/bin/XOrg (он должен быть установлен). Все новые дистрибутивы этим грешат.

Проблемы с запуском: Ubuntu ( [HowTo](http://forum.ubuntu.ru/index.php?topic=60830.0) ).


Launch tested under: OpenSuse 11.x, FreeBsd, ArchLinux, Gentoo.
Запуск разрешен в следующих ОС: OpenSuse 11.x, FreeBsd, ArchLinux, Gentoo.