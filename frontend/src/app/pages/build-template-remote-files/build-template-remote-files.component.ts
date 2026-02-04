import { Component, ViewEncapsulation } from '@angular/core';

@Component({
  selector: 'app-build-template-remote-files',
  standalone: true,
  imports: [],
  templateUrl: './build-template-remote-files.component.html',
  styleUrl: './build-template-remote-files.component.scss',
  encapsulation: ViewEncapsulation.None
})
export class BuildTemplateRemoteFilesComponent {
  readonly yamlExample = `version: 0.1

remote:
  - uri: "https://upload.wikimedia.org/wikipedia/commons/2/28/HelloWorld.svg"
    path: "File With Another Name.svg"`;
}
