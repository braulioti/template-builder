# Frontend

Aplicação Angular 21 com **Jest** para testes e **SCSS** para estilos.

## Pré-requisitos

- Node.js 20+
- npm 10+

## Instalação

```bash
cd frontend
npm install
```

Se aparecer erro de cache (`only-if-cached`), tente `npm install --prefer-online` ou ajuste o `npm cache` fora do projeto.

## Desenvolvimento

```bash
npm start
```

Acesse [http://localhost:4200](http://localhost:4200).

## Build

```bash
npm run build
```

Artefatos em `dist/frontend`.

## Testes (Jest)

```bash
npm test           # executa os testes
npm run test:watch # modo watch
npm run test:coverage # com cobertura
```

## Estrutura

- `src/app` — componentes e rotas
- `src/styles.scss` — estilos globais (SCSS)
- `setup-jest.ts` — configuração do Jest
- `jest.config.js` — opções do Jest

## Stack

- Angular 21
- Jest + jest-preset-angular
- SCSS
- TypeScript 5.6
