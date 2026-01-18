# Como Executar Todos os Testes de Uma Vez

## Opção 1: Usar CTest (Recomendado)

O CTest já está configurado e registra todos os testes automaticamente.

### No Terminal:

```bash
# No diretório de build
cd build
# ou
cd cmake-build-debug  # se estiver usando CLion

# Executar todos os testes
ctest

# Com mais detalhes
ctest --output-on-failure

# Com ainda mais verbosidade
ctest --output-on-failure --verbose

# Listar todos os testes disponíveis (sem executar)
ctest -N

# Executar testes específicos por padrão de nome
ctest -R "VariableType"  # Executa apenas testes que contêm "VariableType"
```

### No Windows (PowerShell):

```powershell
cd build
# ou
cd cmake-build-debug

# Executar todos os testes
ctest

# Com configuração Debug (se estiver usando Visual Studio generator)
ctest -C Debug

# Com detalhes
ctest --output-on-failure -C Debug
```

## Opção 2: Script para Executar Todos os Testes Manualmente

Você pode criar um script simples para executar todos os executáveis de teste:

### Windows (PowerShell):

```powershell
# No diretório de build
cd build

# Executar todos os testes
Get-ChildItem -Path . -Recurse -Filter "test_*.exe" | ForEach-Object {
    Write-Host "Running: $($_.FullName)"
    & $_.FullName
    Write-Host ""
}
```

### Linux/macOS:

```bash
# No diretório de build
cd build

# Executar todos os testes
find . -type f -executable -name "test_*" -exec {} \;

# Ou com mais informações
find . -type f -executable -name "test_*" | while read test; do
    echo "Running: $test"
    "$test"
    echo ""
done
```

## Opção 3: No CLion

1. **Via Test Runner:**
   - Clique com botão direito na pasta `tests/`
   - Selecione "Run 'All Tests'"
   - Ou use o ícone de "Run All Tests" na barra de ferramentas

2. **Via CTest:**
   - Abra o terminal integrado do CLion
   - Execute: `cd cmake-build-debug && ctest --output-on-failure`

3. **Via Run Configuration:**
   - Criar uma nova configuração "CTest"
   - Programa: `ctest`
   - Argumentos: `--output-on-failure`

## Opção 4: Criar um Target Custom no CMake (Avançado)

Você pode adicionar um target custom no CMakeLists.txt:

```cmake
# No CMakeLists.txt principal, após a seção de testes
if(BUILD_TESTS)
    add_custom_target(run_all_tests
        COMMAND ctest --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS test_VariableType test_PromptType test_FileType test_template-builder
        COMMENT "Running all unit tests"
    )
endif()
```

Depois você pode executar:
```bash
cmake --build build --target run_all_tests
```

## Resumo Rápido

**Comando mais simples:**
```bash
cd build
ctest --output-on-failure
```

**No Windows com Visual Studio generator:**
```powershell
cd build
ctest --output-on-failure -C Debug
```

## Verificar se os Testes Estão Registrados

Para ver todos os testes que o CTest encontrou:
```bash
ctest -N
```

Isso mostrará algo como:
```
Test #1: test_VariableType
Test #2: test_PromptType
Test #3: test_FileType
Test #4: test_template-builder
```
