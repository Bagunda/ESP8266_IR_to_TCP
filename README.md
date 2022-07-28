# ESP8266_IR_to_TCP
ESP8266 принимает ИК сигналы и посылает HEX в TCP сокет

<img src="https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgRkEE6NtkB4tAXru2cwJ86OtVhW-V2xFJ2IspgHKLlj79LA2K33yYaXojWLlHu_hAozo214q5rpBUPt_Mnj3XIi1G9gyfQvay2hZoW4oQ_z3d40eIaMX6LasKCf56pj1icaUVwCHZQMkRUYYF6L8qN9uP34zfiavlB7F-jixHsjf5qmLQ9pdF3_0TX/s861/86740657-28b95880-c07a-11ea-9e87-f8f76ac4400f.jpg" width="400"/>
<img src="https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEiJf4ddatoTa0UDqRe1K2ImBmdRNkw6pbhuKLtgVkxgozl-N6JTZzNbHrHkDpaA59dvb4ShLABzvng3Dbi4oPlF3v_NZj5NLxWS9VjLw_G57Uh5dOvRdqJF6DCjUO6PjniAK-bExRF4U44dchmHhQkIteVtTjR4wh9n6VyH8D3mpF9GTiGy5FcGL9Ah/s300/tsop1738_pins.png" width="200"/>

Краткое описание функционала:

Настраиваем подключение ESP к WiFi, поднимаем TCP сокет, принимаем ИК команду и посылаем её в TCP сокет.


Подробнее:

Подключаем наверно любой ИК приёмник, похожий как на фото. 


При помощи библиотеки [IRrecv](https://github.com/crankyoldgit/IRremoteESP8266), принимаем ИК команду с любого пульта и получаем в переменную results.value значение в uint64. И по идее её надо отправить в TCP сокет при помощи команды client.write(value), но эта функция не кушает данные типа long long integer.

    Пробовал по всякому, но наиболее приемлемый способ оказался такой:

        Получаем ИК команду E0E020DF (11100000 11100000 00100000 11011111) - 4 байта.

        Забираем каждые пол байта каждый в свою переменную: 

            1110 0000 1110 0000 0010 0000 1101 1111

            E      0        E      0      2        0       D      F 

        Создаём функцию, где сравниваем полученные пол байта с цифрами 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F. И если равно, то возвращаем результат в char.

        Например: если полученное = 1, то вернуть '1'; если полученное = 13, то вернуть '13'.

        Важно писать в одинарных кавычках, что означает "вернуть значение в char, а не в string".

        Складываем все сконвертированные char'ы в переменную hex3.

        И шлём в TCP сокет client.write(hex3).



Дальше я в Крестрон всё это пробрасывал.

В Crestron настраиваем TCP клиент на IP адрес ESP, на порт, указанный в скетче.

Сравниваем полученную команду с той которая нам нужна. Например E0E020DF - кнопка "1" пульта от Samsung телевизора. И если равно, то переключаем одно из реле процессора.

