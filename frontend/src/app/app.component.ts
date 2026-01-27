import { Component, OnInit, AfterViewInit, ViewChild, ViewEncapsulation } from '@angular/core';
import { SidebarComponent } from './components/sidebar/sidebar.component';
import { MainComponent } from './components/main/main.component';
import { ConfigService } from './core/config/config.service';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [SidebarComponent, MainComponent],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss',
  encapsulation: ViewEncapsulation.None,
})
export class AppComponent implements OnInit, AfterViewInit {
  @ViewChild('sidebar') sidebar!: SidebarComponent;
  @ViewChild('main') main!: MainComponent;

  constructor(private config: ConfigService) {}

  ngOnInit(): void {
    // Log configuration in debug mode
    this.config.logConfig();
  }

  ngAfterViewInit(): void {
    // Remove is-preload class after page loads (like in the original template)
    setTimeout(() => {
      document.body.classList.remove('is-preload');
    }, 100);
  }

  get isSidebarActive(): boolean {
    return this.sidebar ? this.sidebar.isActive : true;
  }

  get menus() {
    return this.main ? this.main.menus : [];
  }
}
