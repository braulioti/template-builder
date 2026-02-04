import { Routes } from '@angular/router';
import { HomeComponent } from './pages/home/home.component';
import { InstallationComponent } from './pages/installation/installation.component';
import { UsingTemplateComponent } from './pages/using-template/using-template.component';
import { ChangelogComponent } from './pages/changelog/changelog.component';
import { TermsOfUseComponent } from './pages/terms-of-use/terms-of-use.component';
import { BuildTemplateIntroductionComponent } from './pages/build-template-introduction/build-template-introduction.component';
import { BuildTemplateVariablesComponent } from './pages/build-template-variables/build-template-variables.component';
import { BuildTemplateRemoteFilesComponent } from './pages/build-template-remote-files/build-template-remote-files.component';
import { BuildTemplateFilesComponent } from './pages/build-template-files/build-template-files.component';
import { BuildTemplatePromptsComponent } from './pages/build-template-prompts/build-template-prompts.component';
import { BuildTemplateFunctionsComponent } from './pages/build-template-functions/build-template-functions.component';

export const routes: Routes = [
  {
    path: '',
    redirectTo: '/en',
    pathMatch: 'full',
  },
  {
    path: 'en',
    children: [
      { path: '', component: HomeComponent },
      { path: 'installation', component: InstallationComponent },
      { path: 'using-template', component: UsingTemplateComponent },
      { path: 'changelog', component: ChangelogComponent },
      { path: 'terms-of-use', component: TermsOfUseComponent },
      {
        path: 'build-template',
        children: [
          { path: '', pathMatch: 'full', redirectTo: 'introduction' },
          { path: 'introduction', component: BuildTemplateIntroductionComponent },
          { path: 'variables', component: BuildTemplateVariablesComponent },
          { path: 'remote-files', component: BuildTemplateRemoteFilesComponent },
          { path: 'files', component: BuildTemplateFilesComponent },
          { path: 'prompts', component: BuildTemplatePromptsComponent },
          { path: 'functions', component: BuildTemplateFunctionsComponent },
        ],
      },
    ],
  },
];
