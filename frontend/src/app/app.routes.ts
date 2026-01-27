import { Routes } from '@angular/router';
import { HomeComponent } from './pages/home/home.component';
import { Home2Component } from './pages/home2/home2.component';

export const routes: Routes = [
  {
    path: '',
    redirectTo: '/en',
    pathMatch: 'full'
  },
  {
    path: 'en',
    component: HomeComponent
  },
  {
    path: 'en/home2',
    component: Home2Component
  }
];
