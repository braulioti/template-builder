import { TestBed } from '@angular/core/testing';
import { FooterComponent } from './footer.component';
import { SocialNetwork } from '../../models/social-network.interface';
import { environment } from '../../core/config/environment';

describe('FooterComponent', () => {
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [FooterComponent],
    }).compileComponents();
  });

  it('should create', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    expect(component).toBeTruthy();
  });

  it('should display title when set', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    component.title = 'Get in touch';
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    const h2 = compiled.querySelector('h2');
    expect(h2?.textContent?.trim()).toBe('Get in touch');
  });

  it('should display details when set', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    component.details = 'Some project description.';
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    const section = compiled.querySelector('section');
    const p = section?.querySelector('p');
    expect(p?.textContent?.trim()).toBe('Some project description.');
  });

  it('should display social networks list', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    const networks: SocialNetwork[] = [
      { name: 'Github', faClass: 'fab fa-github', url: 'https://github.com/example' },
      { name: 'Email', faClass: 'fas fa-envelope', url: 'mailto:test@example.com' },
    ];
    component.socialNetworks = networks;
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    const items = compiled.querySelectorAll('.contact li');
    expect(items.length).toBe(2);
    expect(items[0].querySelector('a')?.textContent?.trim()).toBe('Github');
    expect(items[1].querySelector('a')?.textContent?.trim()).toBe('Email');
  });

  it('should render link when social has url', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    component.socialNetworks = [
      { name: 'Github', faClass: 'fab fa-github', url: 'https://github.com/example' },
    ];
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    const link = compiled.querySelector('.contact a');
    expect(link).toBeTruthy();
    expect(link?.getAttribute('href')).toBe('https://github.com/example');
    expect(link?.textContent?.trim()).toBe('Github');
  });

  it('should render span when social has no url', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    component.socialNetworks = [
      { name: 'Label only', faClass: 'fas fa-info' },
    ];
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    const link = compiled.querySelector('.contact a');
    const span = compiled.querySelector('.contact .label');
    expect(link).toBeFalsy();
    expect(span).toBeTruthy();
    expect(span?.textContent?.trim()).toBe('Label only');
  });

  it('should display environment in copyright', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    const copyright = compiled.querySelector('#footer .copyright');
    expect(copyright?.textContent).toContain(environment.appTitle);
    expect(copyright?.textContent).toContain(environment.author);
    const authorLink = copyright?.querySelector('a');
    expect(authorLink?.getAttribute('href')).toBe(environment.authorURI);
  });

  it('should have default empty title, details and socialNetworks', () => {
    const fixture = TestBed.createComponent(FooterComponent);
    const component = fixture.componentInstance;
    expect(component.title).toBe('');
    expect(component.details).toBe('');
    expect(component.socialNetworks).toEqual([]);
  });
});
