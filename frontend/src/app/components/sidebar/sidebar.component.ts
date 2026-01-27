import { Component, Input, OnInit, HostListener, ViewEncapsulation } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MenuComponent } from '../menu/menu.component';
import { Menu } from '../../models/menu.interface';

@Component({
  selector: 'app-sidebar',
  standalone: true,
  imports: [CommonModule, MenuComponent],
  templateUrl: './sidebar.component.html',
  styleUrl: './sidebar.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class SidebarComponent implements OnInit {
  @Input() menus: Menu[] = [];
  
  isActive = true; // Sidebar visível por padrão

  toggleSidebar(): void {
    this.isActive = !this.isActive;
  }

  ngOnInit(): void {
    // Em telas menores, o sidebar começa inativo (como no template original)
    this.checkScreenSize();
  }

  @HostListener('window:resize')
  onResize(): void {
    this.checkScreenSize();
  }

  private checkScreenSize(): void {
    // O sidebar sempre começa ativo, independente do tamanho da tela
    // O usuário pode alternar usando o toggle button
    this.isActive = true;
  }
}
