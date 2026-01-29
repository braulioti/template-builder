import { Component, ViewEncapsulation } from '@angular/core';
import { RouterLink } from '@angular/router';

@Component({
  selector: 'app-build-template-variables',
  standalone: true,
  imports: [RouterLink],
  templateUrl: './build-template-variables.component.html',
  styleUrl: './build-template-variables.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class BuildTemplateVariablesComponent {
  readonly variableSyntax = '{{variableName}}';
  readonly upperSyntax = '{{upper(variableName)}}';
  readonly yamlExample = `version: 0.1

variables:
  - name: message
    type: string
    value: "Hello World"

files:
  - path: "output.txt"
    content: "{{message}}"`;
}
