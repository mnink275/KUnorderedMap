# Unordered Map

- My implementation of the unordered_map. It was created for learning, practicing and better understanding of C++.

- The executable file was checked under Valgrind with flags:
  ```shell
  --leak-check=yes --track-origins=yes
  ```
- Debugging was performed in Visual Studio 17 2022.

## Building
- Visual Studio 17 2022:
  ```shell
  git clone https://github.com/mnink275/MyUnorderedMap
  cd MyUnorderedMap
  cmake -S . -B build -G "Visual Studio 17 2022"
  cmake --build build
  cmake --open build
  ```

- Linux:
  ```shell
  git clone https://github.com/mnink275/MyUnorderedMap
  cd MyUnorderedMap
  cmake -S . -B build -G "Unix Makefiles"
  cmake --build build
  ./build/UnorderedMap
  ```
