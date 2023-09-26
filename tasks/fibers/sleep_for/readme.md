# SleepFor

## Пререквизиты

- [fibers/yield](/tasks/fibers/yield)

---

Пока файберы из `exe` умеют лишь запускаться и перепланироваться. 

В этой задаче мы научим файберы спать.

## Event Loop

Мы поменяем планировщик: теперь это будет не пул потоков, а `asio::io_context` – [_event loop_](http://latentflip.com/loupe/) из библиотеки [`asio`](https://think-async.com/).

`io_context` планирует таймеры и операции ввода-вывода над сокетами + пользовательские _обработчики_ (_handler_) или _коллбэки_ (_callback_).

### `post` 

С помощью свободной функции `post` можно запланировать в `io_context` исполнение произвольной лямбды: 

```cpp
#include <asio/io_context.hpp>
#include <asio/post.hpp>

void PostExample() {
  // Планировщик I/O
  asio::io_context io;
    
  // Планируем задачу на исполнение
  asio::post(io, [] {
    // Эта лямбда будет запущена из цикла io_context::run
    // NB: помимо `post`, есть и другие способы запланировать задачу: `dispatch`, `defer`
    fmt::println("Running on io_context");
  });
  
  // Планировщик "работает" в вызове `run`:
  // в нем происходит поллинг системных очередей событий и 
  // запуск пользовательских коллбэков
  
  // Вызов `run` вернет управление тогда, когда в планировщике
  // не останется ни пользовательских обработчиков, 
  // ни outstanding асинхронных операций / таймеров
  
  // Метод `run` на `io_context` можно вызывать из нескольких потоков: 
  // тогда пользовательские обработчики будут запускаться параллельно
  
  io.run();
  // <-- На экране напечатано `Running on io_context`
  
  // Планировщик запускает единственную задачу, работа в нем 
  // заканчивается, он автоматически останавливается
}
```

#### Executors

В `asio` интегрирован целый фреймворк _экзекуторов_ (_executors_):

[Executors and Asynchronous Operations](http://chriskohlhoff.github.io/executors/)


### Таймеры

[Using a timer asynchronously](https://think-async.com/Asio/asio-1.22.1/doc/asio/tutorial/tuttimer2.html)

```cpp
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

void TimerExample() {
  // Event loop, планировщик I/O
  asio::io_context io;
  
  // Таймер – I/O object
  // Привязываем его к планировщику
  asio::steady_timer timer{io};
  
  // Заводим таймер
  timer.expires_after(3s);
  
  // "Подвешиваем" к таймеру обработчик
  // NB: таймер должен "дожить" до завершения асинхронной операции!
  timer.async_wait([](std::error_code /*ignore?*/) {
    // Обработчик будет вызван из цикла io_context::run
    fmt::println("Ding dong");
  });
  
  // Прямо сейчас исполнять нечего, но в планировщике есть 
  // заведенный таймер
  // Поток блокируется в вызове `run` на 3 секунды, 
  // затем срабатывает таймер, который приводит 
  // к запуску обработчика
  io.run();
  // <-- На экране напечатано `Ding dong`
  
  // После срабатывания таймера и запуска обработчика
  // планировщик останавливается, в нем больше не осталось работы
}
```

## Пул потоков / event loop

- В пуле потоков воркеры блокируются на общей очереди задач.
- В `io_context` потоки блокируются на системной очереди событий (например, на `epoll`).

## `SleepFor`

Научите файберы спать / трансформируйте с помощью файберов асинхронные таймеры из `asio` в синхронный `SleepFor`:

```cpp
void SleepForExample() {
  using namespace exe;
  
  // Планировщик для файберов
  asio::io_context scheduler;
  
  fibers::Go(scheduler, [] {
    for (size_t i = 0; i < 10; ++i) {
      fmt::println("Step {}", i);
      // Файбер останавливается на 1 секунду, но
      // не блокирует при этом цикл планировщика 
      fibers::SleepFor(1s);
    }
  });
  
  // Запускаем планировщик
  // Вызов `run` завершится через 10 секунд
  scheduler.run();
}
```

### Требования к реализации

- Спящий файбер не должен блокировать поток планировщика.

- Спящий файбер не должен постоянно получать управление и перепланироваться, планировщик не должен тратить на него процессорное время.

- Если в планировщике нет готовых исполняться файберов, но есть спящие файберы, то планировщик должен блокировать поток, в котором он (планировщик) запущен, до пробуждения первого файбера.

## Задание

1) Адаптируйте файберы к новому планировщику
2) Реализуйте `SleepFor`

## Замечания по реализации

### Гонка

В наивной реализации `Yield` / `SleepFor` возможна гонка между остановкой и возобновлением файбера. 

Разрешите ее средствами `asio`, без блокирующего ожидания / взаимного исключения внутри файбера.

Обратите внимание на `asio::defer`.

### `SleepFor`

Работа с таймерами должна быть заключена в функции `SleepFor`. Класс `Fiber` не должен знать про таймеры.

Подумайте над тем, где в памяти разместить `asio::steady_timer` для `SleepFor`.

### `Yield`

Реализуйте `Yield` подобно `SleepFor`.

## Asio

* [Github](https://github.com/chriskohlhoff/asio/)
* [Basic Anatomy](https://think-async.com/Asio/asio-1.22.1/doc/asio/overview/basics.html)
* [Overview](https://think-async.com/Asio/asio-1.22.1/doc/asio/overview.html)
* [Tutorial](https://think-async.com/Asio/asio-1.22.1/doc/asio/tutorial.html)