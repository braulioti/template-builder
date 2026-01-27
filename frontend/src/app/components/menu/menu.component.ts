import { Component, Input, ViewEncapsulation } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule } from '@angular/router';
import { Menu } from '../../models/menu.interface';

@Component({
  selector: 'app-menu',
  standalone: true,
  imports: [CommonModule, RouterModule],
  templateUrl: './menu.component.html',
  styleUrl: './menu.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class MenuComponent {
  @Input() menus: Menu[] = [];
  
  openSubmenus: { [key: string]: boolean } = {};

  toggleSubmenu(route: string, event: Event): void {
    event.preventDefault();
    this.openSubmenus[route] = !this.openSubmenus[route];
  }

  isSubmenuOpen(route: string): boolean {
    return this.openSubmenus[route] || false;
  }

  hasChildren(menu: Menu): boolean {
    return menu.childrens !== undefined && menu.childrens.length > 0;
  }
}
