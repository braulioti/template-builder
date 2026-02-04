# Build stage: Angular
FROM node:20-alpine AS builder

WORKDIR /app

COPY frontend/ ./
RUN npm ci && npm run build

# Runtime stage: nginx
FROM nginx:alpine

# Remove default static content and use our app
RUN rm -rf /usr/share/nginx/html/*

# Angular application builder outputs to dist/frontend/browser
COPY --from=builder /app/dist/frontend/browser /usr/share/nginx/html

COPY docker/nginx-default.conf /etc/nginx/conf.d/default.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
