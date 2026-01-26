/** @type {import('jest').Config} */
const { createCjsPreset } = require('jest-preset-angular/presets');

const config = {
  ...createCjsPreset(),
  setupFilesAfterEnv: ['<rootDir>/setup-jest.ts'],
  testPathIgnorePatterns: ['/node_modules/', '/dist/'],
  collectCoverageFrom: ['src/**/*.ts', '!src/**/*.spec.ts', '!src/main.ts'],
  coverageDirectory: 'coverage',
  coverageReporters: ['html', 'text-summary'],
  moduleNameMapper: {
    '^@app/(.*)$': '<rootDir>/src/app/$1',
  },
  testMatch: ['**/*.spec.ts'],
};

module.exports = config;
