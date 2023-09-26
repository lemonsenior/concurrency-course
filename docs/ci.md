# Автоматическое тестирование (CI)

## Просьба

Пожалуйста 🙏, не отправляйте в CI решения, которые не проходят тесты (`clippy test`) локально: в CI запускаются те же самые тесты с помощью той же самой команды.

## Шаги, которые нужно выполнить 1 раз

### Регистрация

#### Шаг 1

Заведите аккаунт на https://gitlab.com/, если у вас его нет.

Не используйте в логине дефис.

#### Шаг 2

Зарегистрируйтесь в приложении [Manytask](http://concurrency-course.cc/signup).

##### Форма регистрации

Заполните поля:

| Поле | Значение |
|---|---|
| _Gitlab Username_ | ваш логин на https://gitlab.com, например, `Lipovsky`. Логин – _case-sensitive_! |
| _First Name_ | ваше имя латиницей (например, `Roman`) |
| _Last Name_ | ваша фамилия латиницей (например, `Lipovsky`) |
| _Group_ | см. телеграм-канал курса |
| _Secret Code_ | его вы узнаете из телеграм-канала курса |

Убедитесь, что прямо сейчас вы залогинены на https://gitlab.com/.

Нажмите `Register`, ожидайте ⌛

#### Шаг 3

Вы попали в приложение, осмотритесь.

В верхней панели вы увидите ссылки на:
- Созданный для вас приватный репозиторий для решений
- Страницу посылок

#### Troubleshooting

В любой непонятной ситуации с приложением поможет одно из двух:
* `/signup` → Заполнить поля формы → Нажать `Register`
* `/` → Нажать `Login`

### Аттач репозитория с решениями

Во время регистрации в приложении был создан репозиторий для ваших решений.

В этот репозиторий вы будете коммитить и пушить решения (только не вручную, а через консольный клиент `clippy`), там же будут запускаться автоматические тесты.

#### Шаг 1

В контейнере перейдите в локальную копию репозитория курса:
```shell
cd /workspace/concurrency-course
```

#### Шаг 2

Прикрепите к репозиторию курса репозиторий с решениями:
```shell
clippy attach {solutions-repo-url}
```
Вместо `{solutions-repo-url}` подставьте url созданного при регистрации репозитория.

Команда будет выглядеть примерно так:
```shell
clippy attach https://gitlab.com/concurrency-course-workspace/2049/solutions/test-y-roman-y-lipovsky-y-lipovsky
```

Команду `attach` можно выполнить повторно: существующая локальная копия сотрется (с вашего явного разрешения), но это не страшно, если вы запушили все ваши коммиты с помощью `clippy push` в remote репозиторий.

##### SSH

Репозиторий решений можно прикрепить иначе:
```shell
clippy attach git@gitlab.com:concurrency-course-workspace/2049/solutions/test-y-roman-y-lipovsky-y-lipovsky
```
Для этого нужно настроить SSH в контейнере, зато потом можно будет не вводить логин и пароль для каждого пуша.

<details>
<summary>Подробнее тут (можно вернуться к этому позже)</summary>

Инструкцию для генерации ключей и привязки их к профилю gitlab можно
найти [здесь](https://docs.gitlab.com/ee/ssh/).

Для работы в контейнере переименуйте приватный ключ в `gitlab-private-key`
и положите его в директорию [`docker/config/keys`](/docker/config/keys),
чтобы не потерять при пересборке контейнера.

**Перелогиньтесь в контейнер** и проверьте, что всё работает:
```shell
$ ssh -T git@gitlab.com
Warning: Permanently added 'gitlab.com,172.65.251.78' (ECDSA) to the list of known hosts.
Welcome to GitLab, @Lipovsky!
```

</details>

#### Шаг 3

Выполните команду
```shell
clippy show-config
```

Вы увидите поля конфига, который нужно заполнить для посылки решений задач.

Часть полей будет заполнена автоматически (например, поля `gitlab.user`, `name.first`, `name.last`). Но все же убедитесь, что они заполнены корректно.

Остальные поля мы заполним на последующих шагах.

#### Шаг 4

На этом шаге вам нужно получить токен для работы с API https://gitlab.com/

Для этого на https://gitlab.com/ зайдите в `Preferences` своего профиля, затем в `Access Tokens`, выберите произвольное имя для вашего токена, поставьте галку напротив `api`, установите expiration date и нажмите `Create personal access token`.

См. [Personal Access Tokens](https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html)

Токен – это полученная случайная строчка из цифр и букв. Сохраните его, иначе он пропадет! Имя токена нам не понадобится. 

#### Шаг 5

Установите полученный токен в `clippy`:
```shell
# Магический разделитель -- нужен на случай дефисов в начале токена
clippy config -- gitlab.token {your-gitlab-token}
```

Здесь `{your-gitlab-token}` – токен, полученный вами на предыдущем шаге.

Токен нужен `clippy` для создания merge request-ов (MR).

#### Шаг 6

Задайте _assignee_ для ваших MR:
```shell
# Подставьте в команду логин своего семинариста на gitlab.com
clippy config assignee Lipovsky
```

#### Шаг 7

Настройте теги для ваших MR:

```shell
# Теги перечисляются через запятую, без лишних пробелов
# Вместо 123 подставьте номер вашей академической группы (если она у вас есть)
clippy config tags 'concurrency,123'
```

## Работа с задачей / репозиторием решений

#### Шаг 1

Переходим в директорию с задачей:
```shell
cd tasks/tutorial/aplusb
```

#### Шаг 2

Решаем задачу.

#### Шаг 3

Коммитим файлы текущей задачи в отдельную ветку локального репозитория решений:
```shell
clippy commit
```

Можно указать комментарий к коммиту:
```shell
clippy commit -m 'solution'
```

#### Шаг 4

Пушим коммиты из локальной ветки задачи в ветку вашего приватного remote-репозитория:
```shell
clippy push
```

#### Шаг 5

Создаем [_merge request_](https://docs.gitlab.com/ee/user/project/merge_requests/) ветки с решением в _master_:
```shell
# Или короче: clippy mr
clippy merge-request
```
После ее выполнения вы получите ссылку на созданный MR. На созданном MR будет запускаться CI.

Команду `merge-request` нужно выполнить всего лишь один раз для каждой задачи.

#### Дальнейшие шаги

Пусть дальше вы нашли и исправили баги в своем коде:

```shell
# ...
clippy commit -m 'fix'
# ...
clippy commit -m 'another fix'
clippy push
```

После `push` новые коммиты попадут в уже существующий MR и перезапустят CI.

## Посылки

Все посылки можно найти во вкладке `Submits` в приложении или в разделе `Merge requests` вашего репозитория с решениями на гитлабе.

Нажав на плашку в колонке `status`, вы получите подробный отчет о тестировании.

## Под капотом

[Картинка](https://gitlab.com/Lipovsky/clippy/-/raw/master/docs/diagram.png)

После регистрации + логина в приложении для вас заводится приватный репозиторий в группе курса на https://gitlab.com/. В этот репозиторий с помощью консольного клиента `clippy` вы будете коммитить / пушить решения задач, на этом же репозитории будут запускаться тесты.

Команда `clippy attach` клонирует этот приватный репозиторий локально в соседнюю с репозиторием курса директорию.

Для каждой задачи в этом репозитории будет создаваться отдельная ветка (например, `tutorial/aplusb`).

Команда `clippy commit` коммитит файлы текущей задачи (см. `submit_files` в `task.json`) _локально_ в ваш репозиторий решений.

Команда `clippy push` пушит коммиты из ветки текущей задачи локального репозитория решений в remote-репозиторий.

Команда `clippy merge-request` создает для текущей задачи [merge request](https://docs.gitlab.com/ee/user/project/merge_requests/) из ветки решения в мастер со всеми запушенными коммитами.

[Документация по командам](https://gitlab.com/Lipovsky/clippy/-/blob/master/docs/commands.md)

В ветку с решением автоматически добавляется файл [gitlab-ci.yml](.grade.gitlab-ci.yml) – это конфиг для запуска тестов (чуть точнее, _джобов_ CI).

Запуск джоба CI на тестирующем сервере состоит из следующих шагов:

1) Скачивается коммит с файлами решения из вашего репозитория решений.
2) Эти файлы накатываются на директорию задачи в репозитории курса с помощью команды `clippy apply`.
3) На задаче в репозитории курса запускаются команды `clippy cmake`, `clippy validate` и `clippy test`, т.е. ровно те же команды, что вы запускаете локально.