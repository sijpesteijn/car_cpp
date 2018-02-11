/**
 * Angular 2 decorators and services
 */
import {
    Component,
    OnInit,
    ViewEncapsulation
} from '@angular/core';
import {AppState} from './app.service';
import {Config} from './app.config';
import {Car, CarService} from "./car.service";
import { NavigationStart, Router } from "@angular/router";

/**
 * App Component
 * Top Level Component
 */
@Component({
    selector: 'app',
    encapsulation: ViewEncapsulation.None,
    styles: [require('./app.component.css')],
    template: require('./app.html')
})
export class AppComponent {
    private car: Car;

    constructor(private carService: CarService,
                private router: Router) {
    }

    ngOnInit() {
        this.carService.getCar().subscribe((car) => this.car = car);
        this.router.events.subscribe(evt => {
            if (this.car && evt instanceof NavigationStart) {
                if (evt.url.endsWith('off')) {
                    this.car.mode = 0;
                } else if (evt.url.endsWith('manual')) {
                    this.car.mode = 1;
                } else if (evt.url.endsWith('race')) {
                    this.car.mode = 2;
                }
                this.carService.saveCar(this.car).subscribe();
            }
        })
    }
}

/**
 * Please review the https://github.com/AngularClass/angular2-examples/ repo for
 * more angular app examples that you may copy/paste
 * (The examples may not be updated as quickly. Please open an issue on github for us to update it)
 * For help or questions please contact us at @AngularClass on twitter
 * or our chat on Slack at https://AngularClass.com/slack-join
 */
