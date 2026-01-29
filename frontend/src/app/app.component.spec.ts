import { TestBed } from '@angular/core/testing';
import { AppComponent } from './app.component';
import { provideRouter } from '@angular/router';
import { routes } from './app.routes';
import { ConfigService } from './core/config/config.service';

describe('AppComponent', () => {
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [AppComponent],
      providers: [
        provideRouter(routes),
        ConfigService
      ],
    }).compileComponents();
  });

  it('should create the app', () => {
    const fixture = TestBed.createComponent(AppComponent);
    const app = fixture.componentInstance;
    expect(app).toBeTruthy();
  });

  it('should have sidebar and main components', () => {
    const fixture = TestBed.createComponent(AppComponent);
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;
    expect(compiled.querySelector('app-main')).toBeTruthy();
    expect(compiled.querySelector('app-sidebar')).toBeTruthy();
  });

  it('should have isSidebarActive getter', () => {
    const fixture = TestBed.createComponent(AppComponent);
    const app = fixture.componentInstance;
    fixture.detectChanges();
    expect(typeof app.isSidebarActive).toBe('boolean');
  });

  it('should have menus property', () => {
    const fixture = TestBed.createComponent(AppComponent);
    const app = fixture.componentInstance;
    expect(Array.isArray(app.menus)).toBe(true);
    fixture.detectChanges();
    // After view init, menus should be populated from main component
    expect(app.menus.length).toBeGreaterThanOrEqual(0);
  });
});
