import { Component, Input, ViewEncapsulation } from '@angular/core';
import { environment } from '../../core/config/environment';
import { SocialNetwork } from '../../models/social-network.interface';

@Component({
  selector: 'app-footer',
  standalone: true,
  imports: [],
  templateUrl: './footer.component.html',
  encapsulation: ViewEncapsulation.None
})
export class FooterComponent {
  @Input() title = '';
  @Input() details = '';
  @Input() socialNetworks: SocialNetwork[] = [];
  protected readonly environment = environment;
}
