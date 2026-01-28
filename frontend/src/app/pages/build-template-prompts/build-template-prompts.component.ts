import { Component, ViewEncapsulation } from '@angular/core';

@Component({
  selector: 'app-build-template-prompts',
  standalone: true,
  imports: [],
  templateUrl: './build-template-prompts.component.html',
  styleUrl: './build-template-prompts.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class BuildTemplatePromptsComponent {
  readonly variablePlaceholder = '{{variableName}}';
  readonly yamlExample = `version: 0.1

variables:
  - name: projectName
    type: string
    value: "My Project"
  - name: license
    type: string
  - name: features
    type: string

prompts:
  - name: promptConfig
    inputs:
      - variable: projectName
        input: "Enter your project name: "
        type: InputString
      - variable: license
        input: "Select license type: "
        type: Checklist
        options:
          - name: "MIT"
            value: "MIT License"
          - name: "Apache 2.0"
            value: "Apache License 2.0"
          - name: "GPL v3"
            value: "GNU General Public License v3.0"
      - variable: features
        input: "Enter features (one per line): "
        type: ArrayList
    result: |
      Project: {{projectName}}
      License: {{license}}
      Features:
      {{features}}

files:
  - path: "config.txt"
    prompt: promptConfig`;
}
