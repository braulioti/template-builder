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

  openSubmenus: Record<number, boolean> = {};

  toggleSubmenu(index: number, event: Event): void {
    event.preventDefault();
    this.openSubmenus[index] = !this.openSubmenus[index];
  }

  isSubmenuOpen(index: number): boolean {
    return this.openSubmenus[index] ?? false;
  }

  hasChildren(menu: Menu): boolean {
    return menu.childrens !== undefined && menu.childrens.length > 0;
  }
}
