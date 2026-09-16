DAG STYLE - перенос EnvMap shader hook из JNJ

Добавлено:
  gtasa/GLES.cpp
  gtasa/GLES.h

Изменено:
  main.cpp - подключен gtasa/GLES.h и вызван SetUpGLHooks() после InstallSpecialHooks().
  Android.mk - добавлен gtasa/GLES.cpp в сборку.

Почему НЕ переносился CustomCarEnvMapPipeline:
  В DAG STYLE отсутствуют game/Core/Pool.h и исходники RxPipeline из JNJ.
  При этом в DAG STYLE уже есть нативный CCustomCarEnvMapPipeline внутри libGTASA и код,
  связанный с его destructor callback, поэтому копирование JNJ-класса без адаптации создало
  лишние зависимости и могло сломать сборку.

Что делает перенос:
  Перехватывает RQShader::BuildSource (libGTASA + 0x1A5EB0) и подменяет генерируемые GLSL ES 1.00
  vertex/pixel shaders. В JNJ уже есть EnvMap/refl код: reflection vector, EnvMapCoefficient,
  Out_Refl/Out_Tex1 и смешивание EnvMap с цветом материала.

Важно:
  Адрес 0x1A5EB0 рассчитан на ту же версию/сборку libGTASA, что и исходный JNJ. Если DAG STYLE
  использует другую libGTASA, адрес нужно проверить перед запуском.
