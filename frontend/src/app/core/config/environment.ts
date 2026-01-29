/**
 * Environment Configuration
 *
 * Re-exports the environment configuration from the environments folder.
 * This allows the ConfigService to access environment variables in a type-safe way.
 */

import { environment as env } from '../../../environments/environment';

export interface Environment {
  production: boolean;
  appTitle: string;
  appVersion: string;
  apiUrl: string;
  apiTimeout: number;
  enableAnalytics: boolean;
  enableDebug: boolean;
  author: string;
  authorURI: string;
  authorEmail: string;
}

export const environment: Environment = env;
