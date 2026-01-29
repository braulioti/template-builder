import { Component, ViewEncapsulation } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { HeaderComponent } from '../header/header.component';
import { SocialNetwork } from '../../models/social-network.interface';
import { Menu } from '../../models/menu.interface';

@Component({
  selector: 'app-main',
  standalone: true,
  imports: [RouterOutlet, HeaderComponent],
  templateUrl: './main.component.html',
  styleUrl: './main.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class MainComponent {
  socialNetworks: SocialNetwork[] = [
    { name: 'Github', faClass: 'fab fa-github', url: 'https://github.com/braulioti/template-builder' }
  ];

  menus: Menu[] = [
    {
      route: '',
      description: 'Quick Start',
      childrens: [
        { route: '/en', description: 'About Template Builder' },
        { route: '/en/installation', description: 'Installation' },
        { route: '/en/using-template', description: 'Using a Template' },
        { route: '/en/changelog', description: 'Changelog' },
        { route: '/en/terms-of-use', description: 'Terms of Use' },
      ],
    },
    {
      route: '',
      description: 'Build Template',
      childrens: [
        { route: '/en/build-template/introduction', description: 'Introduction' },
        { route: '/en/build-template/variables', description: 'Variables' },
        { route: '/en/build-template/remote-files', description: 'Remote Files' },
        { route: '/en/build-template/files', description: 'Files' },
        { route: '/en/build-template/prompts', description: 'Prompts' },
        { route: '/en/build-template/functions', description: 'Functions' },
      ],
    },
  ];
}
