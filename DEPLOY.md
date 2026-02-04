# Deploy e Release (CI/CD)

Ao criar uma **release** (tag `v*`), a integração contínua:

1. **Gera a imagem Docker** e publica no **Nexus** (registry de imagens).
2. **Publica o docker-compose** no servidor de produção e sobe a aplicação.

## Configuração

### Nexus (imagem Docker)

Configure no repositório (Settings → Secrets and variables → Actions):

| Tipo     | Nome            | Descrição                                    |
|----------|-----------------|----------------------------------------------|
| Variable | `NEXUS_REGISTRY` | Host do Nexus (ex: `nexus.empresa.com:8082`) |
| Secret   | `NEXUS_USERNAME` | Usuário do Nexus                             |
| Secret   | `NEXUS_PASSWORD` | Senha do Nexus                               |

Se `NEXUS_REGISTRY` não for definido, o workflow ainda faz o build da imagem, mas não faz push para o Nexus.

### Servidor de produção (docker-compose)

Para publicar o docker-compose e subir o app em produção:

| Tipo     | Nome                    | Descrição                                                                 |
|----------|-------------------------|---------------------------------------------------------------------------|
| Variable | `PRODUCTION_HOST`       | Host ou IP do servidor (ex: `app.empresa.com`)                            |
| Variable | `PRODUCTION_USER`       | Usuário SSH (ex: `deploy`)                                                |
| Variable | `PRODUCTION_PORT`       | (Opcional) Porta do host para o container (padrão: `80`)                 |
| Variable | `PRODUCTION_DEPLOY_PATH` | (Opcional) Diretório no servidor (padrão: `/opt/template-builder`)       |
| Secret   | `PRODUCTION_SSH_KEY`    | Chave privada SSH para acesso ao servidor                                 |

O job de deploy cria no servidor um arquivo `.env` com a última imagem publicada no Nexus (`IMAGE`) e a porta (`PORT` = `PRODUCTION_PORT` ou 80), e em seguida executa `docker compose up -d`.

No servidor de produção é necessário:

- Docker e Docker Compose (v2) instalados.
- O usuário SSH com permissão para `docker compose` (ex: usuário no grupo `docker`).
- Se o Nexus for privado: fazer `docker login` no servidor ao registry do Nexus (ou configurar `~/.docker/config.json`) para que `docker compose pull` consiga baixar a imagem.

## Uso local do docker-compose

Para rodar com a imagem do Nexus:

```bash
export IMAGE=nexus.empresa.com:8082/template-builder:1.0.0
docker compose up -d
```

Ou defina a porta:

```bash
export IMAGE=nexus.empresa.com:8082/template-builder:1.0.0
export PORT=8080
docker compose up -d
```
