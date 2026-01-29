import { Component, ViewEncapsulation } from '@angular/core';
import { RouterLink } from '@angular/router';

@Component({
  selector: 'app-build-template-files',
  standalone: true,
  imports: [RouterLink],
  templateUrl: './build-template-files.component.html',
  styleUrl: './build-template-files.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class BuildTemplateFilesComponent {
  readonly variablePlaceholder = '{{variableName}}';
  readonly yamlExample = `version: 0.1

variables:
  - name: projectName
    type: string
    value: "My Project"

files:
  - path: "README.md"
    content: "Project: {{projectName}}"
  - path: ".gitignore"
  - path: "LICENSE"
    content: "MIT License"
  - path: "src/main.js"
    content: "console.log('Hello World');"`;
}
