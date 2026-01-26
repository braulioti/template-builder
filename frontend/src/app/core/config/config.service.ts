import { Injectable } from '@angular/core';
import { environment } from './environment';

/**
 * Configuration Service
 * 
 * Provides access to application configuration and environment variables
 */
@Injectable({
  providedIn: 'root'
})
export class ConfigService {
  private config = environment;

  /**
   * Get the full environment configuration
   */
  getEnvironment(): typeof environment {
    return this.config;
  }

  /**
   * Check if running in production mode
   */
  isProduction(): boolean {
    return this.config.production;
  }

  /**
   * Get application title
   */
  getAppTitle(): string {
    return this.config.appTitle;
  }

  /**
   * Get application version
   */
  getAppVersion(): string {
    return this.config.appVersion;
  }

  /**
   * Get API base URL
   */
  getApiUrl(): string {
    return this.config.apiUrl || 'http://localhost:3000/api';
  }

  /**
   * Get API timeout in milliseconds
   */
  getApiTimeout(): number {
    return this.config.apiTimeout;
  }

  /**
   * Check if analytics is enabled
   */
  isAnalyticsEnabled(): boolean {
    return this.config.enableAnalytics;
  }

  /**
   * Check if debug mode is enabled
   */
  isDebugEnabled(): boolean {
    return this.config.enableDebug;
  }

  /**
   * Log configuration (only in debug mode)
   */
  logConfig(): void {
    if (this.config.enableDebug) {
      console.log('Application Configuration:', this.config);
    }
  }
}
