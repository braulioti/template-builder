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
      route: '/en',
      description: 'Home'
    },
    {
      route: '/en/home2',
      description: 'Home2'
    }
  ];
}
