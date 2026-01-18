# Executar Testes no CLion (Windows)

## Problema: "No tests were found"

Este problema acontece quando o CMake não foi configurado com `BUILD_TESTS=ON` ou quando o CTest não encontra os executáveis de teste.

## Solução Passo a Passo

### 1. Configurar o CMake no CLion

1. Abra o CLion
2. Vá em **File → Settings** (ou `Ctrl+Alt+S`)
3. Navegue até **Build, Execution, Deployment → CMake**
4. Selecione o seu perfil (geralmente "Debug" ou "RelWithDebInfo")
5. No campo **CMake options**, adicione:
   ```
   -DBUILD_TESTS=ON
   ```
   Ou você pode adicionar manualmente editando o campo.

6. Clique em **Apply** e depois **OK**

### 2. Recarregar o CMake

- No CLion, clique em **File → Reload CMake Project** (ou `Ctrl+Shift+O`)
- Ou clique no ícone de "reload" ao lado do nome do projeto no canto superior direito

### 3. Compilar o Projeto

- Pressione `Ctrl+F9` para compilar
- Ou vá em **Build → Build Project**
- Verifique se os executáveis de teste foram criados em `cmake-build-debug/tests/` ou `cmake-build-debug/Debug/tests/`

### 4. Executar os Testes

Você tem **3 opções**:

#### Opção A: Via CLion Test Runner (Recomendado)

1. O CLion deve detectar automaticamente os testes do Google Test
2. Procure por ícones verdes ▶️ ao lado das funções `TEST()` nos arquivos de teste
3. Clique com botão direito e selecione **Run 'test_VariableType'**
4. Ou use o **Test Runner** (aba na parte inferior do CLion)

#### Opção B: Via CTest no Terminal do CLion

1. Abra o terminal integrado do CLion
2. Navegue até o diretório de build:
   ```powershell
   cd cmake-build-debug
   ```
3. Execute o CTest:
   ```powershell
   ctest --output-on-failure -C Debug
   ```
   
   **Nota**: O `-C Debug` é necessário quando você usa o gerador Visual Studio (multi-config)

#### Opção C: Executar Testes Individuais

No terminal do CLion, execute diretamente:

```powershell
cd cmake-build-debug
.\tests\Debug\test_VariableType.exe
# ou
.\tests\Debug\test_PromptType.exe
.\tests\Debug\test_FileType.exe
```

## Verificar se está Funcionando

Execute este comando no terminal do CLion (dentro do diretório `cmake-build-debug`):

```powershell
# Listar testes disponíveis
ctest -N -C Debug

# Deve mostrar algo como:
# Test #1: test_VariableType
# Test #2: test_PromptType
# Test #3: test_FileType
```

## Troubleshooting

### Se ainda aparecer "No tests were found":

1. **Verifique se BUILD_TESTS está ativo:**
   - No CLion, vá em **Tools → CMake → Reset Cache and Reload Project**
   - Confirme que `BUILD_TESTS` está marcado nas opções

2. **Verifique se os executáveis foram criados:**
   ```powershell
   dir cmake-build-debug\tests\Debug\*.exe
   ```
   Deve mostrar `test_VariableType.exe`, `test_PromptType.exe`, etc.

3. **Tente executar manualmente um teste:**
   ```powershell
   cd cmake-build-debug
   .\tests\Debug\test_VariableType.exe
   ```
   Se funcionar manualmente mas não no CTest, é problema de configuração do CMake.

4. **Limpe e reconstrua:**
   - No CLion: **Build → Clean**
   - Depois: **File → Reload CMake Project**
   - Compile novamente: `Ctrl+F9`

### Se der erro de compilação:

- Certifique-se de que tem C++17 habilitado
- Verifique se as dependências (yaml-cpp, Google Test) foram baixadas corretamente
- Veja os logs de build para erros específicos

## Configuração Permanente

Para sempre ter os testes habilitados, você pode editar o `CMakeLists.txt` principal:

```cmake
# Em vez de:
option(BUILD_TESTS "Build unit tests" OFF)

# Use:
option(BUILD_TESTS "Build unit tests" ON)
```

Isso fará com que os testes sejam compilados por padrão.

## Alternativa: Usar o Profile de Debug com BUILD_TESTS=ON

1. **File → Settings → Build, Execution, Deployment → CMake**
2. Clique no **+** para adicionar um novo profile chamado "Debug with Tests"
3. Configure:
   - **Name**: Debug with Tests
   - **Build type**: Debug
   - **CMake options**: `-DBUILD_TESTS=ON`
4. Selecione este profile quando quiser trabalhar com testes
