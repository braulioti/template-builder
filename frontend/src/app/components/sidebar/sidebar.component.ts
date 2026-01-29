import { Component, Input, OnInit, HostListener, ViewEncapsulation } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MenuComponent } from '../menu/menu.component';
import { FooterComponent } from '../footer/footer.component';
import { Menu } from '../../models/menu.interface';
import { SocialNetwork } from '../../models/social-network.interface';
import {environment} from "../../core/config/environment";

@Component({
  selector: 'app-sidebar',
  standalone: true,
  imports: [CommonModule, MenuComponent, FooterComponent],
  templateUrl: './sidebar.component.html',
  styleUrl: './sidebar.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class SidebarComponent implements OnInit {
  @Input() menus: Menu[] = [];
  @Input() footerTitle = 'About this project';
  @Input() footerDetails = 'Template Builder is a CLI tool that generates project templates from YAML files. Define variables, prompts, files, folders, and remote downloads in a single template file and run it to scaffold new projects with interactive prompts and variable substitution.';
  @Input() footerSocialNetworks: SocialNetwork[] = [];

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
