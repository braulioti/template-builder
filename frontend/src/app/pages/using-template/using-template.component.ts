import { Component, ViewEncapsulation } from '@angular/core';
import { RouterLink } from '@angular/router';

@Component({
  selector: 'app-using-template',
  standalone: true,
  imports: [RouterLink],
  templateUrl: './using-template.component.html',
  styleUrl: './using-template.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class UsingTemplateComponent {}
