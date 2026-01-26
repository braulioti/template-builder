import { Component, Input, ViewEncapsulation } from '@angular/core';
import { RouterModule } from '@angular/router';
import { environment } from '../../core/config/environment';
import { SocialNetwork } from '../../models/social-network.interface';

@Component({
  selector: 'app-header',
  standalone: true,
  imports: [RouterModule],
  templateUrl: './header.component.html',
  styleUrl: './header.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class HeaderComponent {
  title = environment.appTitle;

  @Input() socialNetworks: SocialNetwork[] = [];
}
