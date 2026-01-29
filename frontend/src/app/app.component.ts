import { Component, OnInit, AfterViewInit, ViewChild, ViewEncapsulation, ChangeDetectorRef } from '@angular/core';
import { SidebarComponent } from './components/sidebar/sidebar.component';
import { MainComponent } from './components/main/main.component';
import { ConfigService } from './core/config/config.service';
import { Menu } from './models/menu.interface';

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

  menus: Menu[] = [];

  constructor(
    private config: ConfigService,
    private cdr: ChangeDetectorRef
  ) {}

  ngOnInit(): void {
    // Log configuration in debug mode
    this.config.logConfig();
  }

  ngAfterViewInit(): void {
    // Initialize menus after view is initialized
    if (this.main) {
      this.menus = this.main.menus;
      this.cdr.detectChanges();
    }

    // Remove is-preload class after page loads (like in the original template)
    setTimeout(() => {
      document.body.classList.remove('is-preload');
    }, 100);
  }

  get isSidebarActive(): boolean {
    return this.sidebar ? this.sidebar.isActive : true;
  }
}
