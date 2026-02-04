import { Component, ViewEncapsulation } from '@angular/core';

@Component({
  selector: 'app-build-template-functions',
  standalone: true,
  imports: [],
  templateUrl: './build-template-functions.component.html',
  styleUrl: './build-template-functions.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class BuildTemplateFunctionsComponent {
  readonly functionPlaceholder = '{{upper(variableName)}}';
  readonly upperExample = '{{upper(variableName)}}';
  readonly lowerExample = '{{lower(variableName)}}';
  readonly replaceExample = '{{replace(" ", "_", variableName)}}';
  readonly nestedExample = '{{lower(replace(" ", "_", variableName))}}';
  readonly yamlExample = `version: 0.1

variables:
  - name: projectName
    type: string
    value: "My Project"
  - name: author
    type: string
    value: "John Doe"

files:
  - path: "README.md"
    content: |
      # {{projectName}}
      Author: {{author}}
      Package: {{lower(replace(" ", "_", projectName))}}
      Slug: {{lower(replace(" ", "-", projectName))}}
  - path: "src/constants.js"
    content: |
      const PACKAGE_NAME = "{{lower(replace(" ", "_", projectName))}}";
      const DISPLAY_NAME = "{{upper(projectName)}}";`;
}
