# Como Executar os Testes

Este documento descreve como executar os testes unitários do Template Builder.

## Pré-requisitos

- CMake 3.10 ou superior
- Compilador C++ com suporte a C++17 (GCC, Clang, ou MSVC)
- Git (para baixar dependências via FetchContent)

## Opção 1: Executar todos os testes com CTest (Recomendado)

### Linux/macOS:

```bash
# 1. Criar diretório de build (se ainda não existe)
mkdir -p build
cd build

# 2. Configurar CMake com testes habilitados
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..

# 3. Compilar o projeto e os testes
cmake --build .

# 4. Executar todos os testes
ctest

# Com mais detalhes:
ctest --output-on-failure

# Com ainda mais verbosidade:
ctest --output-on-failure --verbose
```

### Windows (PowerShell):

```powershell
# 1. Criar diretório de build
mkdir build
cd build

# 2. Configurar CMake com testes habilitados
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..

# 3. Compilar o projeto e os testes
cmake --build . --config Debug

# 4. Executar todos os testes
ctest --output-on-failure -C Debug
```

## Opção 2: Executar testes individuais

### Linux/macOS:

```bash
# Após compilar, você pode executar testes individuais:
cd build

# Executar teste de VariableType
./tests/test_VariableType

# Executar teste de PromptType
./tests/test_PromptType

# Executar teste de FileType
./tests/test_FileType

# Com mais detalhes (mostra todos os casos de teste):
./tests/test_VariableType --gtest_list_tests
./tests/test_VariableType --gtest_color=yes
```

### Windows:

```powershell
cd build

# Executar teste de VariableType
.\tests\Debug\test_VariableType.exe

# Com Visual Studio generator, pode ser:
.\tests\Debug\test_VariableType.exe

# Ou com Ninja/Makefiles:
.\tests\test_VariableType.exe
```

## Opção 3: Executar testes com filtros (Google Test)

Você pode executar apenas testes específicos usando filtros do Google Test:

```bash
# Executar apenas testes que começam com "Default"
./tests/test_VariableType --gtest_filter="VariableTypeTest.Default*"

# Executar todos os testes de uma classe específica
./tests/test_VariableType --gtest_filter="VariableTypeTest.*"

# Executar todos os testes EXCETO um específico
./tests/test_VariableType --gtest_filter="-VariableTypeTest.DefaultConstructor"
```

## Opção 4: Executar testes com coverage

Para gerar relatórios de cobertura de código:

```bash
# Configurar com flags de coverage
mkdir -p build-coverage
cd build-coverage

cmake -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_TESTS=ON \
      -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
      -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
      ..

# Compilar
cmake --build .

# Executar testes
ctest --output-on-failure

# Gerar relatório LCOV
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/_deps/*' --output-file coverage.info
lcov --list coverage.info

# Gerar relatório HTML (se tiver genhtml instalado)
genhtml coverage.info --output-directory coverage-html
```

## Saída Esperada

Ao executar os testes, você deve ver algo como:

```
Running main() from gtest_main.cc
[==========] Running 8 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 8 tests from VariableTypeTest
[ RUN      ] VariableTypeTest.DefaultConstructor
[       OK ] VariableTypeTest.DefaultConstructor (0 ms)
[ RUN      ] VariableTypeTest.ConstructorWithNameAndType
[       OK ] VariableTypeTest.ConstructorWithNameAndType (0 ms)
...
[----------] 8 tests from VariableTypeTest (1 ms total)

[==========] 8 tests from 1 test suite ran. (2 ms total)
[  PASSED  ] 8 tests.
```

## Troubleshooting

### Erro: "Tests were not found"
- Certifique-se de que compilou com `-DBUILD_TESTS=ON`
- Verifique se os executáveis de teste foram criados no diretório `build/tests/`

### Erro: "Google Test not found"
- O CMake deve baixar o Google Test automaticamente via FetchContent
- Se isso falhar, verifique sua conexão com a internet

### Testes falham na compilação
- Verifique se você está usando C++17 ou superior
- Confirme que todas as dependências (yaml-cpp) estão configuradas corretamente

## Integração com IDEs

### CLion
1. Clique com botão direito no arquivo de teste
2. Selecione "Run 'test_VariableType'" ou "Debug 'test_VariableType'"

### Visual Studio Code
1. Instale a extensão "C++ TestMate"
2. Os testes serão detectados automaticamente após compilar

### Visual Studio
1. Abra o "Test Explorer"
2. Os testes serão listados após compilar com `BUILD_TESTS=ON`
