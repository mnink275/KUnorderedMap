# MyUnorderedMap

- My implementation of the unordered_map. It was created for learning and for a better figuring out of C++.

- The executable file was obtained and run under Valgrind by the command:
  ```shell
  g++ *.cpp -std=c++14 -Wall -o ./a.out && valgrind ./a.out --leak-check=yes --track-origins=yes
  ```
- Debugging was performed in Visual Studio 17 2022.

- Building for Visual Studio 17 2022:
  ```shell
  git clone https://github.com/mnink275/MyUnorderedMap
  cd MyUnorderedMap
  mkdir build
  cd build
  cmake ..
  cmake --build .
  cmake --open .
  ```