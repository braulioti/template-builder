import { Component, OnInit, HostListener, ViewEncapsulation } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule } from '@angular/router';

@Component({
  selector: 'app-sidebar',
  standalone: true,
  imports: [CommonModule, RouterModule],
  templateUrl: './sidebar.component.html',
  styleUrl: './sidebar.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class SidebarComponent implements OnInit {
  isActive = true; // Sidebar visível por padrão

  ngOnInit(): void {
    // Em telas menores, o sidebar começa inativo (como no template original)
    this.checkScreenSize();
  }

  @HostListener('window:resize')
  onResize(): void {
    this.checkScreenSize();
  }

  private checkScreenSize(): void {
    // Em telas <= 980px, o sidebar começa inativo
    if (window.innerWidth <= 980) {
      this.isActive = false;
    } else {
      this.isActive = true;
    }
  }

  toggleSidebar(): void {
    this.isActive = !this.isActive;
  }
}
