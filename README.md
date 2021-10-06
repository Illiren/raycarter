# RayCaster SDL2

Данный проект является тестовой реализацией многопоточной реализации 2.5D rayсaster рендера на основе SDL2.

![name](./testrun.gif)

_Ray Carter_
## Особенности
 * Распараллеливание рендера кадра с помощью OpenMP
 * Реализация интерфейса для работы с окном, загрузки и отрисовки текстур и обработки ввода
 * отображение миникарты с индикации положения камеры и конуса обзора
   
## Зависимости
1. A C++17 compiler. Following compilers should work:
  * [gcc 7+](https://gcc.gnu.org/)

  * [clang 6+](https://clang.llvm.org/)
  
2. [CMake 2.8+](https://cmake.org/)

3. [SDL2](https://www.libsdl.org/)

4. [OpenMP](https://www.openmp.org/)

### Сборка
Пример сборки из билд директории:
```
 $ cmake -DCMAKE_BUILD_TYPE=%TypeName% -G "%Generator name%" .
 $ cmake --build . --target raycaster
```
## Лицензия
Проект использует MIT License - смотри [LICENSE.md](LICENSE.md) для подробностей.
Внешние библиотеки используют свои, соответствующие лицензии.
