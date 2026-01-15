# Troubleshooting



Este documento descreve **do zero** como instalar e configurar a biblioteca **yaml-cpp** em um projeto C++ utilizando **CMake**, **MSVC** e **vcpkg**, além de resolver os erros mais comuns encontrados durante o processo.

---

## 🎯 Objetivo

Fazer com que o seguinte include compile sem erros:

```cpp
#include <yaml-cpp/yaml.h>
```

---

## 🧠 Conceito Fundamental

Em C++:

* `#include` **não instala bibliotecas**
* É necessário:

    1. Instalar a biblioteca
    2. Configurar o CMake para encontrá-la
    3. Linkar a biblioteca ao target

---

## ✅ Abordagem Recomendada

Utilizar **vcpkg**, o gerenciador de dependências oficial da Microsoft para C++ no Windows.

Benefícios:

* Integração nativa com MSVC
* Compatível com CMake
* Funciona localmente e em CI (GitHub Actions)

---

## 1️⃣ Pré-requisitos

* Windows 10 ou superior
* **Compilador C++** (escolha uma opção):
  * Visual Studio 2022 (com C++ Desktop Development), ou
  * **Visual C++ Build Tools** ([Download aqui](https://visualstudio.microsoft.com/pt-br/visual-cpp-build-tools/)) - Ferramentas independentes de compilação sem o IDE completo
    * ⚠️ **Importante:** Ao instalar, na aba "Componentes individuais", marque:
      * ✅ MSVC v143 – VS 2022 C++ x64/x86 build tools
      * ✅ Windows 10 SDK ou Windows 11 SDK
      * ✅ CMake tools for Windows
* CMake ≥ 3.20 (ou use o CMake tools incluído nas Build Tools)
* Git
* PowerShell

---

## 2️⃣ Instalação do vcpkg

Abra o **PowerShell**:

```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Verifique a instalação:

```powershell
.\vcpkg version
```

---

## 3️⃣ Instalação do yaml-cpp

Ainda dentro da pasta do vcpkg:

```powershell
.\vcpkg install yaml-cpp
```

Isso irá:

* Compilar o yaml-cpp
* Instalar headers e libs para MSVC

Diretório típico após a instalação:

```
vcpkg/installed/x64-windows/
```

---

## 4️⃣ Estrutura Mínima do Projeto

```
seu-projeto/
│
├── CMakeLists.txt
└── src/
    └── main.cpp
```

---

## 5️⃣ Configuração do CMake

### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)
project(seu_projeto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(seu_projeto
    src/main.cpp
)

find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(seu_projeto
    PRIVATE yaml-cpp
)
```

---

## 6️⃣ Código de Teste

### `src/main.cpp`

```cpp
#include <yaml-cpp/yaml.h>
#include <iostream>

int main() {
    YAML::Node node = YAML::Load("key: value");
    std::cout << node["key"].as<std::string>() << std::endl;
    return 0;
}
```

---

## 7️⃣ Gerando o Build (Passo CRÍTICO)

⚠️ **O CMake PRECISA do toolchain do vcpkg**

```powershell
cmake -S . -B build ^
  -DCMAKE_TOOLCHAIN_FILE=C:/caminho/para/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Depois:

```powershell
cmake --build build
```

---

## 8️⃣ Erros Comuns e Soluções

### ❌ `CMake Error: CMAKE_CXX_COMPILER not set` ou `No C++ compiler found`

**Causa:**

* Nenhum compilador C++ está instalado ou não está no PATH do sistema

**Solução:**

1. **Instalar Visual C++ Build Tools** (recomendado para desenvolvimento sem IDE):
   * Download: [Visual C++ Build Tools](https://visualstudio.microsoft.com/pt-br/visual-cpp-build-tools/)
   * Execute o instalador
   * Na aba "Componentes individuais", certifique-se de marcar **TODOS** os seguintes itens obrigatórios:
     * ✅ **MSVC v143 – VS 2022 C++ x64/x86 build tools** (obrigatório)
     * ✅ **Windows 10 SDK** ou **Windows 11 SDK** (obrigatório)
     * ✅ **CMake tools for Windows** (obrigatório)
     * ⬜ Ferramentas de teste C++ (opcional)
   * ⚠️ **ATENÇÃO:** Se faltar qualquer um dos componentes obrigatórios, o erro continuará ocorrendo.
   * Instale e reinicie o terminal

2. **Ou instalar Visual Studio 2022** (IDE completo):
   * Durante a instalação, certifique-se de marcar "Desktop development with C++"

3. **Verificar instalação:**
   ```powershell
   where.exe cl
   ```
   Se retornar um caminho, o compilador está instalado corretamente.

4. **Reconfigure o CMake após instalar:**
   ```powershell
   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/caminho/para/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

---

### ❌ `yaml-cpp/yaml.h: No such file or directory`

**Causa:**

* vcpkg não foi integrado ao CMake

**Solução:**

* Usar `-DCMAKE_TOOLCHAIN_FILE=...` ao rodar o CMake

---

### ❌ `Could not find a package configuration file provided by "yaml-cpp"`

**Causa:**

* yaml-cpp não instalado no vcpkg

**Solução:**

```powershell
.\vcpkg install yaml-cpp
```

---

### ❌ Erros de linker (`unresolved external symbol`)

**Causa:**

* Biblioteca não foi linkada ao target

**Solução:**

```cmake
target_link_libraries(seu_projeto PRIVATE yaml-cpp)
```

---

### ❌ Compila localmente, mas falha no CI

**Causa:**

* vcpkg não configurado no pipeline

**Solução (GitHub Actions):**

```yaml
- name: Install dependencies
  uses: lukka/run-vcpkg@v11
  with:
    vcpkgArguments: yaml-cpp
```

---

## 9️⃣ Checklist Final

* [ ] vcpkg instalado
* [ ] yaml-cpp instalado via vcpkg
* [ ] `find_package(yaml-cpp CONFIG REQUIRED)` no CMake
* [ ] `target_link_libraries(... yaml-cpp)` configurado
* [ ] CMake executado com `CMAKE_TOOLCHAIN_FILE`

---

## ✅ Resultado Esperado

O projeto compila e executa corretamente, imprimindo:


