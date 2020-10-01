# Windows Merc

### Eng

_Merc_ is one of the brightest [MUD](https://en.wikipedia.org/wiki/MUD) servers of its time, a descendant of [DikuMUD](https://dikumud.com/) and the foundation of [SMAUG](http://realmsofdespair.com/smaug-codebase), ROM and many other derivatives. It's a concise but quite accomplished codebase for a fully-fledged MUD. A very good starting point to dive in the world of MUD making.

_Windows Merc_ is Merc 2.2 ported on Windows OS. Nearly all classic MUD servers had been written under unixes and for unixes. They tried to make ports so that one could compile just one codebase on any system. But this approach leads to certain complications and frustrating difficulties. This project is for those who want to work just on Windows with the code as clean as possible.

The current code has been beautified for comprehensive reading. If needed, native state of the code can be easily obtained anywhere in the web.

The release has been assembled using Dev-C++ 5.11 IDE and successfully run on Windows 7 and 10.

For socket management `libws2_32.a` library has been used.

C11 standard must be set in the compiler setting before assembling. Add `-static-libgcc` for the linker in the compiler configuration.

The current executable is located in the `area` folder.

The code is not without bugs. The main and obvious one is the crash during `spec_mayor()` function execution. For the game not to stop the simplest solution is to just delete `M 3143 spec_mayor` line in `midgaard.are` area file.

### Rus

_Merc_ &ndash; один из самых ярких [мад-серверов](https://ru.wikipedia.org/wiki/Многопользовательский_мир) своего времени, наследник [DikuMUD](https://dikumud.com/), основа [SMAUG](http://realmsofdespair.com/smaug-codebase), ROM и множества других производных кодовых баз. Это довольно лаконичная, но в то же время самодостаточная для полноценного мад-сервера кодовая база. Очень хорошая отправная точка для погружения в мир мад-строительства.

_Windows Merc_ &ndash; это Merc 2.2, портированный на ОС Windows. Практически все классические мад-сервера писались под юниксами и для юниксов. Все порты делались так, чтобы одни и те же исходники можно было скомпилировать на любой системе. Но это создает некоторые осложнения и досадные помехи. Данный проект для тех, кто хочет полноценно работать с максимально чистым кодом под виндой.

В ходе работы код был приведен в современный читабельный вид (beautified). В &laquo;родном&raquo; виде код можно без труда найти на просторах сети.

Настоящий релиз собирался в IDE Dev-C++ 5.11 и благополучно запускался под Windows 7 и 10.

Для работы с сокетами подключалась библиотека `libws2_32.a`.

Перед сборкой в параметрах компилятора следует указать стандарт Си 2011 года `-std=c11`. Компоновщику добавить команду `-static-libgcc`. В Dev-C++ это настраивается в окне &laquo;Параметры компилятора&raquo;.

Исполняемый файл расположен в папке `area`.

Программа не лишена багов. Главный и очевидный &ndash; крэш в ходе работы функции `spec_mayor()`. Чтобы игра не слетала, для начала достаточно просто удалить строку `M 3143 spec_mayor` в файле зоны `midgaard.are`.
