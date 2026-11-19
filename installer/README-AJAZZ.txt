Wireless Battery - Windows Style 1.0.4.4
https://github.com/unioneon/WirelessBattery-Windows-Style

Независимый форк Wireless Device Battery Monitor автора krisgfx9,
основанный на оригинальной версии v1.0.4:
https://github.com/krisgfx9/Wireless-Device-Battery-Monitor
Лицензия GPL-3.0-or-later сохранена. Это не официальный релиз оригинала.

Изменения Windows Style:
- Мягкие цвета и сглаживание иконок/текста панели задач.
- Векторная иконка геймпада: округлый корпус, крестовина и одно крупное кольцо.
- Пропуск ответа о батарее больше не создаёт ложный статус asleep для мыши.
- Протокол приёмника Lofree Flow Lite84: процент, зарядка и напряжение.
- AJAZZ AK820 Pro: запросы USB/приёмника, правильное имя и иконка по Bluetooth,
  единая запись при разных способах подключения.
- Регрессионные тесты и диагностические инструменты в исходном репозитории.

Запуск:
1. Распакуйте переносной архив целиком в постоянную папку или используйте Setup.
2. Закройте предыдущую работающую программу через её меню Exit.
3. Запустите WirelessBattery.exe.
4. Для строки иконок выберите Show on the taskbar (icon + percentage).

Проверенные подключения:
AJAZZ AK820 Pro - Bluetooth; Lofree Flow Lite84 - приёмник 2,4 ГГц;
Logitech MX Master 3S - Logitech receiver; Sony WH-1000XM4 и Xbox Controller - Bluetooth.
Проценты меняются по мере разряда/зарядки; исходники содержат результаты проверки.

Ограничение AJAZZ:
На проверенном экземпляре реальный процент доступен через Bluetooth.
Радиоприёмник отвечает, но не передаёт числовой заряд. По кабелю клавиатура
возвращает специальный признак FF: он отображается как USB, а не как 100%.
Программа не подставляет измерение, которого нет в ответе устройства.

Если Lofree спит, нажмите на ней несколько клавиш и выберите Refresh now.
После успешного чтения последнее значение сохраняется до 30 минут при пропусках
ответов и отмечается в меню как last known. Пропуск ответа сам по себе не доказывает сон.

Мыши, гарнитуры и контроллеры поддерживаются драйверами оригинального проекта.
HeadsetControl 4.3.0 включён для дополнительных гарнитур. Настройки устройств,
раскладки, подсветка и прошивки не изменяются этой доработкой.

Исходники доступны в репозитории и по тегу релиза. Соответствующие исходники
HeadsetControl приложены к релизу отдельно. См. LICENSE.txt и THIRD_PARTY_NOTICES.txt.

English: Independent GPL-3.0-or-later fork of krisgfx9's v1.0.4. Softer taskbar
rendering, clearer controller icon, corrected cached/sleep state, Lofree receiver
support and AJAZZ Bluetooth identity normalization. On the tested AJAZZ unit,
use Bluetooth for a percentage; receiver/USB reporting remains limited.
